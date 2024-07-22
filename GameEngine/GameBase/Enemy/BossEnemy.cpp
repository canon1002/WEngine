#include "BossEnemy.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/GameBase/Player/Player.h"
#include "GameEngine/Append/Collider/AABBCollider.h"

void BossEnemy::Init() {
	mObject = std::make_unique<Object3d>();
	mObject->Init("BossEnemyObj");
	mObject->SetModel("walk.gltf");
	mObject->mSkinning = new Skinnig();
	mObject->mSkinning->Init("human", "walk.gltf",
		mObject->GetModel()->modelData);
	mObject->mSkeleton = Skeleton::Create(mObject->GetModel()->modelData.rootNode);
	mObject->SetTranslate({ 3.0f,1.0f,7.0f });
	mObject->GetModel()->materialData_->color = { 1.0f,0.7f,0.7f,1.0f };

	// 移動量を初期化
	mVelocity = { 0.0f,0.0f ,0.002f };
	// 重力の影響を受ける
	mIsGravity = true;
	// ジャンプ回数のセット
	mJumpCount = kJumpCountMax;

	// ビヘイビアツリーの初期化
	this->InitBehavior();

	// コライダーの宣言
	mObject->mCollider = new AABBCollider(mObject->mWorldTransform, Vector3(0.5f, 0.5f, 0.5f));
	mObject->mCollider->Init();

}

void BossEnemy::InitBehavior() {

	// ボスのState番号の配列(実行中に増減しないため、長さ固定の配列を宣言)
	mStateArr[VITALITY] = std::make_unique<VitalityBossState>();
	// 初期シーンの初期化
	mStateArr[VITALITY]->Init(this);
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
	//this->UpdateState();

	// オブジェクト更新
	mObject->Update();
	mObject->mCollider->Update();
}

void BossEnemy::Draw() {
	mObject->Draw();
}

void BossEnemy::DrawGUI() {
	mObject->DrawGUI();
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

Vector3 BossEnemy::GetWorldPosForTarget() {
	return pPlayer->GetWorldPos();
}

bool BossEnemy::invokeNearDistance() {
	//	距離が近い場合のみ実行
	if (Length(Vector3(
		GetWorldPos().x - GetWorldPosForTarget().x,
		GetWorldPos().y - GetWorldPosForTarget().y,
		GetWorldPos().z - GetWorldPosForTarget().z)) 
		<= 1.0f) {
		return true;
	}
	return false;
}

bool BossEnemy::invokeFarDistance(){
	//	距離が遠い場合のみ実行
	if (Length(Vector3(
		GetWorldPos().x - GetWorldPosForTarget().x,
		GetWorldPos().y - GetWorldPosForTarget().y,
		GetWorldPos().z - GetWorldPosForTarget().z))
		>= 1.0f) {
		return true;
	}
	return false;
}

void BossEnemy::ColliderDraw() {
	mObject->mCollider->Draw();
}