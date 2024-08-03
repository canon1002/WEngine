#include "BossEnemy.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/GameBase/Player/Player.h"
#include "GameEngine/Append/Collider/AABBCollider.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/GameBase/AI/BehaviorTree/BTMoveAction.h"
#include "GameEngine/GameBase/AI/BehaviorTree/BTAttackAction.h"

void BossEnemy::Init() {
	mObject = std::make_unique<Object3d>();
	mObject->Init("BossEnemyObj");
	mObject->SetModel("boss.gltf");
	mObject->mSkinning = new Skinnig();
	mObject->mSkinning->Init("boss", "Idle.gltf",
		mObject->GetModel()->modelData);
	mObject->mSkeleton = Skeleton::Create(mObject->GetModel()->modelData.rootNode);
	mObject->SetTranslate({ 3.0f,1.0f,7.0f });
	mObject->GetModel()->materialData_->color = { 1.0f,0.7f,0.7f,1.0f };
	
	// ワールドトランスフォーム
	mObject->mWorldTransform->scale= { 1.5f,1.5f,1.5f};
	mObject->mWorldTransform->translation = { 0.0f,0.0f,20.0f };

	// 移動量を初期化
	mVelocity = { 0.0f,0.0f ,0.002f };
	// 重力の影響を受ける
	mIsGravity = true;
	// ジャンプ回数のセット
	mJumpCount = kJumpCountMax;

	// ビヘイビアツリーの初期化
	this->InitBehavior();
	// 各行動クラスの初期化
	this->InitActions();

	// コライダーの宣言
	mObject->mCollider = new SphereCollider(mObject->mWorldTransform, mObject->GetWorldTransform()->scale.x);
	mObject->mCollider->Init();
	mObject->mCollider->SetAddtranslation(Vector3(0.0f, mObject->GetWorldTransform()->scale.y, 0.0f));
	mObject->mCollider->SetCollisionAttribute(kCollisionAttributeEnemy);
	mObject->mCollider->SetCollisionMask(kCollisionAttributePlayerBullet);

}

void BossEnemy::InitBehavior() {

	// ボスのState番号の配列(実行中に増減しないため、長さ固定の配列を宣言)
	mStateArr[VITALITY] = std::make_unique<VitalityBossState>();
	// 初期シーンの初期化
	mStateArr[VITALITY]->Init(this);

	// Behavior Treeを構築する 
	mRoot = std::make_unique<BT::Sequence>();
	mRoot->SetChild(new BT::Condition(std::bind(&BossEnemy::InvokeFarDistance,this)));
	mRoot->SetChild(new BT::MoveToPlayer(this));
	mRoot->SetChild(new BT::AttackClose(this));
	
}

void BossEnemy::InitActions()
{
	// 各行動をmap配列に追加していく
	
	// 接近
	mActions["MoveToPlayer"] = new ACT::MoveToPlayer();
	mActions["MoveToPlayer"]->Init(this);
	// 近接攻撃
	mActions["AttackClose"] = new ACT::AttackClose();
	mActions["AttackClose"]->Init(this);
	// 初期は行動しない
	mActiveAction = nullptr;

}

// 関数ポインタテーブルの実体
void (BossEnemy::* BossEnemy::CommandTable[])() = {
	&BossEnemy::MoveForward,
	&BossEnemy::BackStep,
};


void BossEnemy::Update() {

	// テーブルから関数を呼び出す
	//(this->*CommandTable[0])();


	// 一旦ここに落下処理をつくる
	if (mObject->mWorldTransform->translation.y > 0.0f) {
		// 移動量を加算
		mObject->mWorldTransform->translation.y += mVelocity.y;
		mVelocity.y -= 9.8f * (1.0f / 360.0f);
	}
	else if (mObject->mWorldTransform->translation.y < 0.0f) {
		mObject->mWorldTransform->translation.y = 0.0f;
		// 移動量修正
		mVelocity.y = 0.0f;
	}

	// ステートの更新処理を行う
	this->UpdateState();

	// BehaviorTreeの更新処理を行う
	mBTStatus = mRoot->Tick();
	if (mBTStatus == BT::NodeStatus::SUCCESS || mBTStatus == BT::NodeStatus::FAILURE) {
		// 結果が帰ってきたら初期化処理
		mRoot->Reset();

		// 各アクションの初期化もしておく
		for (auto& action : mActions) {
			action.second->End();
			action.second->Reset();
		}
		// nullを代入しておく
		mActiveAction = nullptr;
	}

	// オブジェクト更新
	mObject->Update();
	mObject->mCollider->Update();

	if (mActiveAction != nullptr) {
		mActiveAction->Update();
	}

}

void BossEnemy::Draw() {
	mObject->Draw();
}

void BossEnemy::DrawGUI() {
#ifdef _DEBUG
	const char* behaviorState[] = { 
		"SUCCESS", // 成功
		"FAILURE", // 失敗
		"RUNNING", // 実行中 
	};
	int32_t currentItem = (int32_t)mBTStatus;

	ImGui::Begin("BossEnemy");
	ImGui::ListBox("State", &currentItem, behaviorState, IM_ARRAYSIZE(behaviorState), 3);
	mObject->DrawGuiTree();
	ImGui::End();
#endif // _DEBUG
}

void BossEnemy::SetCollider(CollisionManager* cManager)
{
	mActions["AttackClose"]->SetCollider(cManager);

}

void BossEnemy::UpdateState() {

	// ステートのチェック
	mPrevStateNo = mCurrentStateNo;
	mCurrentStateNo = mStateArr[mCurrentStateNo]->GetStateNo();

	// ステートが変更されているかの確認
	if (mPrevStateNo != mCurrentStateNo) {

		// ステートが変更されたら初期化を行う
		mStateArr[mCurrentStateNo]->Init(this);
	}

	// ステート 更新処理
	mStateArr[mCurrentStateNo]->Update();

}

//
void BossEnemy::SetNextAction(const std::string& key)
{
	// マップからポインタを取得
	mActiveAction = mActions[key];
}

ACT::Condition BossEnemy::GetActionCondition(const std::string& key)
{
	// 引数で指定した行動クラスの状態を取得する
	return mActions[key]->GetCondition();

}

ACT::IAction* BossEnemy::GetActionClass(const std::string& key){
	// 引数で指定した行動クラスの状態を取得する
	return mActions[key];
}

void BossEnemy::AttackLong()
{

}

void BossEnemy::AttackClose()
{
	//// 前回の行動を終了
	//if (mActiveAction != nullptr) {
	//	mActiveAction->End();
	//}
	// 現行アクションを設定
	mActiveAction = mActions["AttackClose"];
	mActiveAction->Start();
}

void BossEnemy::MoveToPlayer()
{
	//// 前回の行動を終了
	//if (mActiveAction != nullptr) {
	//	mActiveAction->End();
	//}
	// 現行アクションを設定
	mActiveAction = mActions["MoveToPlayer"];
	mActiveAction->Start();
}

void BossEnemy::EscapeToPlayer()
{


}

Vector3 BossEnemy::GetWorldPos()
{
	return mObject->GetWorldTransform()->translation;
}

Vector3 BossEnemy::GetWorldPosForTarget() {
	return pPlayer->GetWorldPos();
}

bool BossEnemy::InvokeNearDistance() {
	//	距離が近い場合のみ実行
	if (Length(Vector3(
		GetWorldPos().x - GetWorldPosForTarget().x,
		GetWorldPos().y - GetWorldPosForTarget().y,
		GetWorldPos().z - GetWorldPosForTarget().z))
		<= (mObject->mWorldTransform->scale.x + mObject->mWorldTransform->scale.z) / 1.0f) {
		return true;
	}
	return false;
}

bool BossEnemy::InvokeFarDistance(){
	//	距離が遠い場合のみ実行
	if (Length(Vector3(
		GetWorldPos().x - GetWorldPosForTarget().x,
		GetWorldPos().y - GetWorldPosForTarget().y,
		GetWorldPos().z - GetWorldPosForTarget().z))
		>= 5.0f) {
		return true;
	}
	return false;
}

void BossEnemy::ColliderDraw() {
	if (mActiveAction != nullptr) {
		mActiveAction->Draw();
	}
#ifdef _DEBUG
	mObject->mCollider->Draw();
#endif // _DEBUG
}