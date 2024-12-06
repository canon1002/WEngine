#include "BossEnemy.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "App/Player/Player.h"
#include "GameEngine/Append/Collider/AABBCollider.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "App/AI/BehaviorTree/BTMoveAction.h"
#include "App/AI/BehaviorTree/BTAttackAction.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"

#include "GameEngine/GameMaster/Framerate.h"
#include "App/Enemy/Action/ActionList.h"

void BossEnemy::Init() {
	
	// モデルの読み込み
	//ModelManager::GetInstance()->LoadModel("Boss", "boss.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "Idle.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "Run.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "backStep.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "death.gltf");

	ModelManager::GetInstance()->LoadModel("Boss", "Slash.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "SlashR.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "SlashJamp.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "SlashDash.gltf");
	
	ModelManager::GetInstance()->LoadModel("Boss", "Thrust.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "magic.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "kick.gltf");

	mObject = std::make_unique<Object3d>();
	mObject->Init("BossEnemyObj");
	mObject->mWorldTransform->scale = { 1.0f,1.0f,1.0f };
	mObject->mWorldTransform->translation = { 0.0f,0.0f,20.0f };

	// モデルを設定
	mObject->SetModel("boss.gltf");
	mObject->GetModel()->mMaterialData->color = { 1.0f,0.7f,0.7f,1.0f };
	// スキニングアニメーションの生成
	mObject->mSkinning = new Skinning();
	mObject->mSkinning->Init("Boss", "Idle.gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->SetMotionBlendingInterval(30.0f);
	// 使用するアニメーションを登録しておく
	mObject->mSkinning->CreateSkinningData("Boss", "Idle", ".gltf", mObject->GetModel()->modelData, true);
	mObject->mSkinning->CreateSkinningData("Boss", "Run", ".gltf", mObject->GetModel()->modelData, true);
	mObject->mSkinning->CreateSkinningData("Boss", "backStep", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "death", ".gltf", mObject->GetModel()->modelData);
	
	mObject->mSkinning->CreateSkinningData("Boss", "Slash", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "SlashR", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "SlashDash", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "SlashJamp", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "Thrust", ".gltf", mObject->GetModel()->modelData);

	mObject->mSkinning->CreateSkinningData("Boss", "magic", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "kick", ".gltf", mObject->GetModel()->modelData);
	
	// アニメーションの再生速度を1.5倍速に変更
	mObject->mSkinning->SetAnimationPlaySpeed(1.5f);

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

	// ステータス取得
	mStatus = StatusManager::GetInstance()->GetBossStatus();

	// コライダーの宣言
	mObject->mCollider = new SphereCollider(mObject->mWorldTransform, mObject->GetWorldTransform()->scale.x);
	mObject->mCollider->Init();
	mObject->mCollider->SetAddTranslation(Vector3(0.0f, mObject->GetWorldTransform()->scale.y, 0.0f));
	mObject->mCollider->SetCollisionAttribute(kCollisionAttributeEnemy);
	mObject->mCollider->SetCollisionMask(kCollisionAttributePlayerBullet);

	mRightHandWorldMat = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
	mRightHandsWorldMat = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });

	mWeapon = std::make_unique<Object3d>();
	mWeapon->Init("Weapon");
	mWeapon->SetModel("sword.gltf");
	mWeapon->mSkinning = new Skinning();
	mWeapon->GetModel()->SetCubeTexture(Skybox::GetInstance()->mTextureHandle);
	mWeapon->mSkinning->Init("Weapons", "sword.gltf",
		mWeapon->GetModel()->modelData);
	mWeapon->mSkinning->IsInactive();
	mWeapon->mSkeleton = Skeleton::Create(mWeapon->GetModel()->modelData.rootNode);
	// 拡大率を変更
	mWeapon->mWorldTransform->scale = { 3.0f,3.0f,12.0f };

	// ペアレント
	mWeapon->mWorldTransform->SetParent(mRightHandWorldMat);

	// 武器にコライダーをセットする
	for (int32_t i = 0; i < 5; i++) {
		mWeaponWorldMat[i] = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
		// コライダー 宣言
		SphereCollider* newCollider = new SphereCollider(new WorldTransform(), 0.2f);
		// 初期化
		newCollider->Init();
		newCollider->SetCollisionAttribute(kCollisionAttributeEnemyBullet);
		newCollider->SetCollisionMask(kCollisionAttributePlayer);
		// 武器に設定したボーンのワールド座標をセット
		newCollider->GetWorld()->SetParent(mWeaponWorldMat[i]);
		//// 配列にプッシュする
		mWeaponColliders.push_back(newCollider);
	}

	mReloadBTCount = 10.0f;

	// -- エフェクト関係 -- //

	// 剣先と根本のワールド座標
	for (size_t i = 0; i < mWorldTransformSword.size(); i++) {
		mWorldTransformSword[i] = new WorldTransform();
		mWorldTransformSword[i]->Init();
	}
	// ペアレントを設定(後にワールド座標を取得する)
	mWorldTransformSword[0]->SetParent(mWeaponWorldMat[0]);
	mWorldTransformSword[1]->SetParent(mWeaponWorldMat[4]);

}

void BossEnemy::InitBehavior() {

	// ボスのState番号の配列(実行中に増減しないため、長さ固定の配列を宣言)
	mStateArr[VITALITY] = std::make_unique<VitalityBossState>();
	// 初期シーンの初期化
	mStateArr[VITALITY]->Init(this);

	// Behavior Treeを構築する 
	mRoot = std::make_unique<BT::Sequence>();

	// 接近 -> 近接攻撃
	BT::Sequence* newSequence = new BT::Sequence();
	BT::Selector* ReafOneSelector = new BT::Selector();


	newSequence->SetChild(new BT::Condition(std::bind(&BossEnemy::InvokeFarDistance, this)));
	newSequence->SetChild(new BT::MoveToPlayer(this));// 接近
	newSequence->SetChild(new BT::AttackClose(this));// 近接攻撃

	// ジャンプ攻撃orダッシュ攻撃
	BT::Selector* atkSelector = new BT::Selector();
	atkSelector->SetChild(new BT::Decorator(std::bind(&BossEnemy::InvokeFarDistance, this), new BT::AttackDash(this))); // Playerが遠い場合、ダッシュ攻撃
	atkSelector->SetChild(new BT::AttackJump(this));
	

	// 構築
	newSequence->SetChild(atkSelector);
	ReafOneSelector->SetChild(newSequence);


	// 接近状態だったら
	BT::Sequence* startNear = new BT::Sequence();
	startNear->SetChild(new BT::AttackThrust(this));	// 刺突
	startNear->SetChild(new BT::Condition(std::bind(&BossEnemy::InvokeNearDistance, this))); // プレイヤーが近くにいたら下の行動を実行
	startNear->SetChild(new BT::BackStep(this));		// 後退
	ReafOneSelector->SetChild(startNear);


	mRoot->SetChild(ReafOneSelector);

}

void BossEnemy::InitActions()
{
	// 各行動をmap配列に追加していく

	// 接近
	mActions["MoveToPlayer"] = new ACT::MoveToPlayer();
	mActions["MoveToPlayer"]->Init(this);

	// 後退
	mActions["BackStep"] = new ACT::BackStep();
	mActions["BackStep"]->Init(this);

	// 近接攻撃
	mActions["AttackClose"] = new ACT::AttackClose();
	mActions["AttackClose"]->Init(this);

	// 刺突攻撃
	mActions["AttackThrust"] = new ACT::AttackThrust();
	mActions["AttackThrust"]->Init(this);

	// ダッシュ攻撃
	mActions["AttackDash"] = new ACT::AttackDash();
	mActions["AttackDash"]->Init(this);

	// ジャンプ攻撃
	mActions["AttackJump"] = new ACT::AttackJump();
	mActions["AttackJump"]->Init(this);

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
		mVelocity.y -= 9.8f * (1.0f / 720.0f);
	}
	else if (mObject->mWorldTransform->translation.y < 0.0f) {
		mObject->mWorldTransform->translation.y = 0.0f;
		// 移動量修正
		mVelocity.y = 0.0f;
	}

	if (mStatus->HP > 0.0f) {

		// ステートの更新処理を行う
		this->UpdateState();

		// BehaviorTreeの更新処理を行う
		this->UpdateBehaviorTree();

		
	}

	// オブジェクト更新
	UpdateObject();

}

void BossEnemy::UpdateBehaviorTree(){

	// リロードカウントを減らす
	mReloadBTCount -= (6.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();
	
	// リロードカウントが0以下になったら行動を実行
	if (mReloadBTCount <= 0.0f) {

		// リロードカウントをリセット
		mReloadBTCount = 1.0f;

		// ビヘイビアツリーの実行
		mBTStatus = mRoot->Tick();

		// ビヘイビアツリーの実行結果が成功か失敗だったら
		if (mBTStatus == BT::NodeStatus::SUCCESS || mBTStatus == BT::NodeStatus::FAILURE) {

			// ビヘイビアツリーをリセット
			mRoot->Reset();

			// 各アクションの初期化もしておく
			for (auto& action : mActions) {
				action.second->End();
				action.second->Reset();
			}
			
			// 行動クラスのポインタをnullptrにする
			mActiveAction = nullptr;
		}
	}
}

void BossEnemy::UpdateObject(){

	if (mActiveAction != nullptr) {
		mActiveAction->Update();
	}


	// ステージ限界値に合わせた座標の補正
	mObject->mWorldTransform->translation.x = std::clamp(mObject->mWorldTransform->translation.x, -20.0f, 20.0f);
	mObject->mWorldTransform->translation.z = std::clamp(mObject->mWorldTransform->translation.z, -20.0f, 20.0f);

	// 右手のワールド行列を更新
	mRightHandWorldMat = Multiply(
		GetObject3D()->mSkinning->GetSkeleton().joints[GetObject3D()->mSkinning->GetSkeleton().jointMap["mixamorig:RightHandThumb1"]
		].skeletonSpaceMatrix, GetObject3D()->GetWorldTransform()->GetWorldMatrix());

	mRightHandsWorldMat = Multiply(
		GetObject3D()->mSkinning->GetSkeleton().joints[GetObject3D()->mSkinning->GetSkeleton().jointMap["mixamorig:RightHandMiddle1"]
		].skeletonSpaceMatrix, GetObject3D()->GetWorldTransform()->GetWorldMatrix());

	mWeapon->Update();


	// ワールド座標更新
	mWeaponWorldMat[0] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade0"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[1] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade1"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[2] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade2"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[3] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade3"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[4] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade4"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());

	// 武器のコライダー 更新
	for (Collider* collider : mWeaponColliders) {
		collider->Update();
	}

	// オブジェクト更新
	mObject->Update();
	mObject->mCollider->Update();

	// UI更新
	mStatus->Update(mObject->GetWorldTransform());

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

	// 武器のワールド行列を取得
	Matrix4x4 matrix = mWeapon->GetWorldTransform()->GetWorldMatrix();

	// スケールを抽出
	Vector3 col0(matrix.m[0][0], matrix.m[1][0], matrix.m[2][0]);
	Vector3 col1(matrix.m[0][1], matrix.m[1][1], matrix.m[2][1]);
	Vector3 col2(matrix.m[0][2], matrix.m[1][2], matrix.m[2][2]);

	Vector3 scale = { 1.0f,1.0f,1.0f };
	scale.x = Length(col0);
	scale.y = Length(col1);
	scale.z = Length(col2);

	// スケール成分を取り除いて回転行列を正規化
	col0 = Normalize(col0);
	col1 = Normalize(col1);
	col2 = Normalize(col2);

	// 回転行列からEuler角を計算
	Vector3 rotation = { 0.0f,0.0f,0.0f };
	rotation.y = std::asin(-col0.z);
	if (std::cos(rotation.y) != 0) {
		rotation.x = std::atan2(col1.z, col2.z);
		rotation.z = std::atan2(col0.y, col0.x);
	}
	else {
		rotation.x = std::atan2(-col2.x, col1.y);
		rotation.z = 0;
	}

	ImGui::Begin("BossEnemy");
	if (ImGui::CollapsingHeader("Animation")) {

		std::string strAnimeT = "AnimationTime : " + std::to_string(mObject->mSkinning->GetNowSkinCluster()->animationTime/ mObject->mSkinning->GetDurationTime());
		ImGui::ProgressBar(mObject->mSkinning->GetNowSkinCluster()->animationTime / mObject->mSkinning->GetDurationTime(), ImVec2(-1.0f, 0.0f), strAnimeT.c_str());

		std::string strNormalT = "MotionBlendingTime : " + std::to_string(mObject->mSkinning->GetMotionBlendingTime());
		ImGui::ProgressBar(mObject->mSkinning->GetMotionBlendingTime(), ImVec2(-1.0f, 0.0f), strNormalT.c_str());

		// Skinning
		mObject->mSkinning->DrawGui();
	}
	ImGui::ListBox("State", &currentItem, behaviorState, IM_ARRAYSIZE(behaviorState), 3);
	ImGui::DragInt("HP", &mStatus->HP, 1.0f, 0, 100);
	ImGui::DragInt("STR", &mStatus->STR, 1.0f, 0, 100);
	ImGui::DragInt("VIT", &mStatus->VIT, 1.0f, 0, 100);
	ImGui::DragInt("AGI", &mStatus->AGI, 1.0f, 0, 100);
	mObject->DrawGuiTree();
	mWeapon->DrawGuiTree();
	ImGui::DragFloat3("Scale", &scale.x);
	ImGui::DragFloat3("Rotation", &rotation.x);
	for (int i = 0; i < 4; ++i) {
		// Floatの4x4行列内の数値を表示
		ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), mWeapon->GetWorldTransform()->GetWorldMatrix().m[i]);
	}


	ImGui::End();
#endif // _DEBUG
}

void BossEnemy::SetCollider(CollisionManager* cManager)
{
	mActions["AttackClose"]->SetCollider(cManager);
	mActions["AttackThrust"]->SetCollider(cManager);
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

ACT::IAction* BossEnemy::GetActionClass(const std::string& key) {
	// 引数で指定した行動クラスの状態を取得する
	return mActions[key];
}

void BossEnemy::ReciveDamageTolayer(float power)
{
	// プレイヤーにダメージを与える
	StatusManager::GetInstance()->ReceiveDamage(mStatus, power, pPlayer->GetStatus());
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

void BossEnemy::AttackThrust()
{
	// 現行アクションを設定
	mActiveAction = mActions["AttackThrust"];
	mActiveAction->Start();
}

void BossEnemy::AttackDash(){
	// 現行アクションを設定
	mActiveAction = mActions["AttackDash"];
	mActiveAction->Start();
}

void BossEnemy::AttackJump(){
	// 現行アクションを設定
	mActiveAction = mActions["AttackJump"];
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

void BossEnemy::BackStep()
{
	// 現行アクションを設定
	mActiveAction = mActions["BackStep"];
	mActiveAction->Start();
}

void BossEnemy::Jump(float JumpPower)
{
	// 移動量を加算
	mObject->mWorldTransform->translation.y += JumpPower;
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
		<= (mObject->mWorldTransform->scale.x + mObject->mWorldTransform->scale.z) / 0.8f) {
		return true;
	}
	return false;
}

bool BossEnemy::InvokeFarDistance() {
	//	距離が遠い場合のみ実行
	if (Length(Vector3(
		GetWorldPos().x - GetWorldPosForTarget().x,
		GetWorldPos().y - GetWorldPosForTarget().y,
		GetWorldPos().z - GetWorldPosForTarget().z))
		> (mObject->mWorldTransform->scale.x + mObject->mWorldTransform->scale.z) / 0.8f) {
		return true;
	}
	return false;
}

void BossEnemy::ColliderDraw() {
	mWeapon->Draw();

#ifdef _DEBUG

	if (mActiveAction != nullptr) {
		mActiveAction->Draw();
	}

	mObject->mCollider->Draw();
	
#endif // _DEBUG
}