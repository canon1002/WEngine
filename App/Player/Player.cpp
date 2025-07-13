#include "Player.h"
#include "App/Bullet/PlayerBullet.h"
#include "GameEngine/Resource/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Component/Collider/SphereCollider.h"
#include "GameEngine/Component/Collider/AABBCollider.h"
#include "GameEngine/Component/Collider/CollisionManager.h"
#include "App/Enemy/BossEnemy.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Editor/BlackBoard.h"

Player::Player()
{
}

Player::~Player() {

}

void Player::Init() {

	// 初期化項目細分化のために オブジェクト・ワーク部分を
	// 分けて初期化する

	// 名前の設定
	mName = "Player";

	// オブジェクト部分 初期化
	InitObject();
	
	// ワーク部分初期化
	InitWorks();

	// 最初は通常状態から始める
	mWorks->mBehavior = Behavior::kRoot;

	// 移動量を初期化
	mVelocity = { 0.0f,0.0f ,0.0f };
	// 重力の影響を受ける
	mIsGravity = true;
	// ジャンプ回数のセット
	mJumpCount = kJumpCountMax;
	// レティクルの初期化
	mReticle = std::make_unique<Reticle3D>();
	mReticle->Init();
	// 構え
	mIsAimMode = false;
	// ステータス取得
	mStatus = GameManager::GetInstance()->GetStatus("Player");
	// 方向指定(とりあえず奥を向く)
	mDirection = { 0.0f,0.0f,1.0f };
	mDirectionForInput = { 0.0f,0.0f,0.0f };
	// ターゲットロックオンの有無
	mIsLockOnToTarget = false;

}

void Player::InitObject(){

	mObject = std::make_unique<Object3d>();
	mObject->Init("PlayerObj");
	mObject->SetScale({ 2.5f,2.5f,2.5f });
	mObject->SetTranslate({ 1.0f,1.0f,7.0f });

	// モデルを設定
	mObject->SetModelFullPath("Actor", "Actor.gltf");
	// スキニングアニメーションを生成
	mObject->mSkinning = make_unique<Skinning>();
	mObject->mSkinning->Init("Actor", "Actor_Idle.gltf", mObject->GetModel()->mModelData);
	// モーションブレンド速度
	mObject->mSkinning->SetMotionBlendingInterval(15.0f);
	// アニメーション再生速度
	mObject->mSkinning->SetAnimationPlaySpeed(1.0f);
	// 使用するアニメーションを登録しておく
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Idle", ".gltf", mObject->GetModel()->mModelData, true);

	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Avoid", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_BackStep", ".gltf", mObject->GetModel()->mModelData);

	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Walk", ".gltf", mObject->GetModel()->mModelData, true);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Run", ".gltf", mObject->GetModel()->mModelData, true);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_BackRun", ".gltf", mObject->GetModel()->mModelData, true);

	mObject->mSkinning->CreateSkinningData("Actor", "Actor_S0", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_S1", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_S2", ".gltf", mObject->GetModel()->mModelData);

	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Knockback", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Death", ".gltf", mObject->GetModel()->mModelData);

	mObject->mSkinning->CreateSkinningData("Actor", "Actor_MagicCastLong", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_MagicCast", ".gltf", mObject->GetModel()->mModelData);


	// コライダーの宣言
	mObject->mCollider = std::make_unique<SphereCollider>(mObject->mWorldTransform.get(), 0.5f);
	mObject->mCollider->Init();
	mObject->mCollider->SetAddTranslation(Vector3(0.0f, 0.55f, -0.1f));
	mObject->mCollider->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kPlayer));

	// 身体の部位に合わせたコライダーを生成
	CreateBodyPartCollider("Head", 0.2f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	CreateBodyPartCollider("Hips", 0.15f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	CreateBodyPartCollider("LeftLeg", 0.1f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	CreateBodyPartCollider("LeftFoot", 0.1f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	CreateBodyPartCollider("RightLeg", 0.1f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	CreateBodyPartCollider("RightFoot", 0.1f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	CreateBodyPartCollider("LeftForeArm", 0.1f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	CreateBodyPartCollider("LeftHand", 0.1f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	CreateBodyPartCollider("RightForeArm", 0.1f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	CreateBodyPartCollider("RightHand", 0.1f, static_cast<uint32_t>(CollisionTypeId::kPlayer));
	// 頭部のコライダーの座標を調整
	mBodyPartColliders["Head"]->collider->SetAddTranslation({ 0.0f,0.05f,0.0f });

	// -- 攻撃関連パラメータ -- //

	sword = std::make_unique<Object3d>();
	sword->Init("ASword");
	sword->SetModelFullPath("Weapons", "ASword.gltf");
	sword->mWorldTransform->scale = { 1.0f,1.0f,1.0f };
	sword->mSkinning = make_unique<Skinning>();
	sword->mSkinning->Init("Weapons", "ASword.gltf",
		sword->GetModel()->mModelData);
	sword->mSkinning->IsInactive();

	// ペアレント
	weaponParentMat = MakeIdentity();
	sword->mWorldTransform->SetParent(weaponParentMat);

	// 武器にコライダーをセットする
	for (int32_t i = 0; i < 5; i++) {
		swordWorldMat[i] = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
		// コライダー 宣言
		std::shared_ptr<GameCollider> newCollider = std::make_shared<GameCollider>();
		newCollider->collider = std::make_unique<SphereCollider>(new WorldTransform(), 0.35f);
		// 初期化
		newCollider->collider->Init();
		newCollider->collider->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kPlayerWeapon));

		// 武器に設定したボーンのワールド座標をセット
		newCollider->collider->GetWorld()->SetParent(swordWorldMat[i]);
		//// 配列にプッシュする
		swordColliders.push_back(std::move(newCollider));
	}


	// -- エフェクト関係 -- //

// 剣先と根本のワールド座標
	for (size_t i = 0; i < mWorldTransformSword.size(); i++) {
		mWorldTransformSword[i] = make_unique<WorldTransform>();
		mWorldTransformSword[i]->Init();
	}
	// ペアレントを設定(後にワールド座標を取得する)
	mWorldTransformSword[0]->SetParent(swordWorldMat[0]);
	mWorldTransformSword[1]->SetParent(swordWorldMat[4]);

}

void Player::InitWorks(){

	
	// ワーククラスの初期化
	mWorks = std::make_unique<ActorWorks>();
	mWorks->Init("player");


	// -- 攻撃 -- //

	// 各攻撃の定数を取得
	kConstAttacks[0].offence = 1.0f;
	kConstAttacks[0].operationTime = 0.5f;
	kConstAttacks[0].afterTime = 0.8f;
	kConstAttacks[0].motionTimeMax = 1.0f;
	kConstAttacks[0].actionSpeed = 1.5f;
	
	kConstAttacks[1].offence = 1.0f;
	kConstAttacks[1].operationTime = 0.5f;
	kConstAttacks[1].afterTime = 0.8f;
	kConstAttacks[1].motionTimeMax = 1.0f;
	kConstAttacks[1].actionSpeed = 1.5f;

	kConstAttacks[2].offence = 1.0f;
	kConstAttacks[2].operationTime = 0.1f;
	kConstAttacks[2].afterTime = 0.45f;
	kConstAttacks[2].motionTimeMax = 1.5f;
	kConstAttacks[2].actionSpeed = 1.5f;

	//kConstAttacks[3].offence = 1.0f;
	//kConstAttacks[3].operationTime = 0.1f;
	//kConstAttacks[3].afterTime = 0.45f;
	//kConstAttacks[3].motionTimeMax = 0.6f;
	//kConstAttacks[3].actionSpeed = 1.0f;

	//kConstAttacks[2].offence = 1.0f;
	//kConstAttacks[2].operationTime = 0.1f;
	//kConstAttacks[2].afterTime = 0.45f;
	//kConstAttacks[2].motionTimeMax = 0.6f;
	//kConstAttacks[2].actionSpeed = 1.0f;

	//kConstAttacks[3].offence = 1.0f;
	//kConstAttacks[3].operationTime = 0.05f;
	//kConstAttacks[3].afterTime = 0.4f;
	//kConstAttacks[3].motionTimeMax = 0.8f;
	//kConstAttacks[3].actionSpeed = 1.0f;

}

void Player::Update() {

	// -- 入力処理 -- //

	// 移動量の初期化
	mVelocity = { 0.0f,0.0f,0.0f };

	// ワーク切り替え処理(入力関係)
	Input();
	// 方向入力
	InputDirection();

	// 各ワークごとの処理を関数ポインタで実行する
	if (mWorkFunc) {
		(this->*mWorkFunc)();
	}


	// -- 物理処理 -- //
	
	// 落下処理
	Fall();
	// 方向修正
	AdJustDirection();
	// 移動処理
	Move();

	// -- その他処理 -- //

	// レティクル・ロックオン処理
	LockOn();

	// ステータス更新
	mStatus->Update();

	if (mStatus->HP > 0.0f) {

		
		switch (mWorks->mBehavior)
		{
		case Behavior::kRoot:

			if (mObject->mSkinning->GetNowSkinCluster()->name != "Actor_Idle" &&
				!mObject->mSkinning->SearchToWaitingSkinCluster("Actor_Idle"))
			{
				mObject->mSkinning->SetNextAnimation("Actor_Idle");
			}

			break;
		case Behavior::kMove:

			if (mObject->mSkinning->GetNowSkinCluster()->name != "Actor_Walk" &&
				!mObject->mSkinning->SearchToWaitingSkinCluster("Actor_Walk"))
			{
				mObject->mSkinning->SetNextAnimation("Actor_Walk");
			}

			break;
		case Behavior::kDash:

			if (mObject->mSkinning->GetNowSkinCluster()->name != "Actor_Run" &&
				!mObject->mSkinning->SearchToWaitingSkinCluster("Actor_Run"))
			{
				mObject->mSkinning->SetNextAnimation("Actor_Run");
			}

			break;

		case Behavior::kAttack:
			break;
		case Behavior::kJump:
			break;
		case Behavior::kCharge:
			break;
		case Behavior::kAvoid:
			break;
		case Behavior::kGuard:
			break;
		default:
			break;
		}

		

	}

}

void Player::UpdateObject(){

	// 移動量に応じた移動を行う
	mObject->mWorldTransform->translation += mVelocity;
	// 無敵時間時の処理
	InvincibleObjectUpdate();

	// 補間数値
	static float t = 0.0f;
	// 勾配
	static float k = 1.0f;
	// 始点と終点
	static Vector3 s = { 0.0f,0.5f,-8.0f };
	static Vector3 e = { 0.0f,1.8f,-26.0f };

	// 補間後の数値を計算
	Vector3 cVel = ExponentialInterpolation(s, e, t, k);

	// メインカメラに追加の平行移動値を与える
	MainCamera::GetInstance()->SetAddTranslation(TransformNomal(cVel, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix()));

	// ステージ限界値に合わせた座標の補正
	mObject->mWorldTransform->translation.x = std::clamp(mObject->mWorldTransform->translation.x,
		-BlackBoard::GetFieldRadius().x, BlackBoard::GetFieldRadius().x);
	mObject->mWorldTransform->translation.z = std::clamp(mObject->mWorldTransform->translation.z,
		-BlackBoard::GetFieldRadius().z, BlackBoard::GetFieldRadius().z);

	// オブジェクト更新
	mObject->Update();
	mObject->mSkinning->GetSkeleton().joints;
	mObject->mCollider->Update();

	// 身体の部位のワールド行列を更新
	UpdateBodyCollider();

	// 右手のワールド行列を更新
	weaponParentMat = Multiply(
		GetObject3D()->mSkinning->GetBoneMatrix("mixamorig:Sword_joint"),
		GetObject3D()->GetWorldTransform()->GetWorldMatrix());

	// 剣
	sword->Update();

	// ワールド座標更新
	for(int32_t i = 0; i < 5; i++) {

		// ボーン名
		std::string boneName = "Blade" + std::to_string(i);

		// 剣の各ボーンのワールド座標を更新
		swordWorldMat [i] = Multiply(
			sword->mSkinning->GetBoneMatrix(boneName),
			sword->GetWorldTransform()->GetWorldMatrix());
	}

	// 弾 更新
	for (auto& bullet : mBullets) {
		bullet->Update();
	}

	// 武器のコライダー 更新
	for (auto& collider : swordColliders) {
		collider->collider->Update();
	}

}

void Player::Draw() {
	mObject->Draw();
}

void Player::DrawGUI() {
#ifdef _DEBUG

	// メニューバーを表示する
	ImGui::Begin("Player");

	mObject->DrawGuiTree();

	ImGui::DragFloat3("DirectionInuut", &mDirectionForInput.x);
	ImGui::DragFloat3("Direction", &mDirection.x);

	const char* str[] = {
		"kRoot",  // 通常状態
		"kMove",  // 移動状態
		"kDash",// ダッシュ状態
		"kAttack",// 攻撃中
		"kJump",// ジャンプ中
		"kCharge",// 溜め動作中
		"kAvoid", // 回避行動
		"kGuard", // 防御行動
		"kParry",// パリィ中
		"kBreak", // ブレイク中
	};
	int32_t behaviorNumber = (int32_t)mWorks->mBehavior;

	ImGui::ListBox("Current State", &behaviorNumber, str, IM_ARRAYSIZE(str));

	// コライダー
	if (ImGui::CollapsingHeader("Collider")) {

		// コライダーのデバッグ表示
		for (auto& collider : mBodyPartColliders) {

			if (ImGui::TreeNode(collider.first.c_str())) {

				// ワールドトランスフォームのデバッグ表示
				ImGui::DragFloat3("Scale", &collider.second->collider->GetWorld()->scale.x);
				ImGui::DragFloat3("Rotate", &collider.second->collider->GetWorld()->rotation.x);
				ImGui::DragFloat3("Translate", &collider.second->collider->GetWorld()->translation.x);
				
				// ワールド行列の表示
				for (int i = 0; i < 4; ++i) {
					// Floatの4x4行列内の数値を表示
					ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), collider.second->collider->GetWorld()->GetWorldMatrix().m[i]);
				}
				
				ImGui::TreePop();
			}
		}		
	}

	// プレイヤー ステータス
	if (ImGui::CollapsingHeader("Status")) {
		ImGui::DragInt("HP", &mStatus->HP, 1.0f, 0, 100);
		ImGui::DragInt("STR", &mStatus->STR, 1.0f, 0, 100);
		ImGui::DragInt("VIT", &mStatus->VIT, 1.0f, 0, 100);
		ImGui::DragInt("AGI", &mStatus->AGI, 1.0f, 0, 100);

	}

	// 回避パラメータ
	if (ImGui::CollapsingHeader("Avoid")) {
		ImGui::DragFloat("AvoidRange", &mWorks->mWorkAvoid.avoidRange);
		ImGui::DragFloat("AvoidSpeed", &mWorks->mWorkAvoid.avoidSpeed);
		ImGui::DragFloat("AvoidTime", &mWorks->mWorkAvoid.elapsedTime);

		// 回避動作時間の表示
		std::string timeStr = "Time : " + std::to_string(mWorks->mWorkAvoid.elapsedTime);
		ImGui::ProgressBar(mWorks->mWorkAttack.elapsedTime, ImVec2(-1.0f, 0.0f), timeStr.c_str());

	}

	// 攻撃パラメータ
	if (ImGui::CollapsingHeader("Attack")) {

		
		int32_t constAttackIndex = 0;
		for (auto& constAttack : kConstAttacks) {
			
			std::string attackName = "constAttack"+ std::to_string(constAttackIndex);

			if(ImGui::TreeNode(attackName.c_str())) {
				ImGui::DragFloat("Offence", &constAttack.offence, 0.01f, 0.0f, 10.0f);
				ImGui::DragFloat("OperationTime", &constAttack.operationTime, 0.01f, 0.0f, 10.0f);
				ImGui::DragFloat("AfterTime", &constAttack.afterTime, 0.01f, 0.0f, 10.0f);
				ImGui::DragFloat("MotionTimeMax", &constAttack.motionTimeMax, 0.01f, 0.0f, 10.0f);
				ImGui::DragFloat("ActionSpeed", &constAttack.actionSpeed, 0.01f, 0.0f, 10.0f);
				ImGui::TreePop();
			}
			
			constAttackIndex++;
		}

		// 攻撃動作時間の表示
		std::string timeStr = "Time : " + std::to_string(mWorks->mWorkAttack.elapsedTime);
		ImGui::ProgressBar(mWorks->mWorkAttack.elapsedTime, ImVec2(-1.0f, 0.0f), timeStr.c_str());
		ImGui::DragInt("ComboCount", &mWorks->mWorkAttack.comboCount, 0);
		
		// 入力待ち時間の表示
		std::string inputTimeStr = "inputWait : " + std::to_string(mWorks->mWorkAttack.inputWaitingTime);
		ImGui::ProgressBar(mWorks->mWorkAttack.inputWaitingTime, ImVec2(-1.0f, 0.0f), inputTimeStr.c_str());

		if (mWorks->mWorkAttack.isComboRequest)
		{
			ImGui::Text("ComboRequest: [ON]");
		}
		else
		{
			ImGui::Text("ComboRequest: [OFF]");
		}
		sword->DrawGuiTree();

	}

	ImGui::End();




#endif // _DEBUG


}

void Player::ColliderDraw() {
#ifdef _DEBUG

	//mObject->mCollider->Draw();

	// 身体の部位に合わせたコライダーを描画
	for (auto& collider : mBodyPartColliders) {
		collider.second->collider->Draw();
	}

	
	if (mWorks->mBehavior == Behavior::kAttack){
		if (mWorks->mWorkAttack.isOperating){
			// 武器のコライダー 描画
			for (auto& collider : swordColliders) {
				collider->collider->Draw();
			}
		}
	}

#endif // _DEBUG
	mReticle->Draw3DReticle();

	// 弾
	for (auto& bullet : mBullets) {
		bullet->Draw();
	}

	// 剣
	sword->Draw();

}

void Player::SetColliderList(){

	// プレイヤー本体のコライダー
	for (auto& collider : mBodyPartColliders) {
		// ゲームマネージャのリストに追加
		GameManager::GetInstance()->SetCollider(collider.second);
	}


	// プレイヤーの武器コライダー

	// 攻撃中かつ命中前であればコライダーをリストに追加する
	if (mWorks->mWorkAttack.isOperating && !mWorks->mWorkAttack.isHit) {
		for (auto& collider : swordColliders) {

			// ゲームマネージャのリストに追加
			GameManager::GetInstance()->SetCollider(collider);
		}
	}

	// 弾のコライダーを登録
	/*for (auto& bullet : mBullets) {
		GameManager::GetInstance()->SetCollider(bullet->GetCollider());
	}*/


}

bool Player::GetIsOperating() const {
	// 攻撃ステータスから攻撃中であるか取得
	if (mWorks->mWorkAttack.attackPhase == InAttack) {
		return true;
	}
	return false;
}


void Player::Input() {

	// 行動中は入力を受け付けない
	if (!(mWorks->mBehavior == Behavior::kRoot ||
		mWorks->mBehavior == Behavior::kMove ||
		mWorks->mBehavior == Behavior::kDash)
		) {
		return;
	}

	// コントローラー及びキー入力(各行動のリクエスト)を受け取る
	auto buffered = InputManager::GetInstance()->ConsumeBufferedInput();
	if (buffered.has_value()) {
		switch (*buffered) {
		case BufferedInput::Attack:
			mWorks->mWorkAttack.isComboRequest = true;
			mWorks->mWorkAvoid.isAvoidRequest = false;
			mWorks->mWorkGuard.isGuardRequest = false;
			break;
		case BufferedInput::Avoid:
			mWorks->mWorkAvoid.isAvoidRequest = true;
			mWorks->mWorkAttack.isComboRequest = false;
			mWorks->mWorkGuard.isGuardRequest = false;
			break;
		case BufferedInput::Guard:
			mWorks->mWorkGuard.isGuardRequest = true;
			mWorks->mWorkAttack.isComboRequest = false;
			mWorks->mWorkAvoid.isAvoidRequest = false;
			break;
		default: break;
		}
	}

	// リクエストに応じたワーク関数のセット
	if (mWorks->mWorkAttack.isComboRequest) {
		mWorkFunc = &Player::Attack;
	}

	if (mWorks->mWorkAvoid.isAvoidRequest) {
		mWorkFunc = &Player::Avoid;
	}
	
	if (mWorks->mWorkGuard.isGuardRequest) {
		mWorkFunc = &Player::Guard;
	}


}

void Player::Avoid(){

	// ワーク構造体を取得
	auto& work = mWorks->mWorkAvoid;

	
	// 初回回避処理
	if (work.isAvoidRequest && !work.isOperating) {
		work.isOperating = true;
		work.elapsedTime = 0.0f;
		
		
		// 回避方向の設定
		Vector3 dir = mDirectionForInput;
		// アニメ再生
		mObject->mSkinning->SetNextAnimation("Actor_Avoid");
		
		// 入力がないときは後方ステップ
		if (Length(dir) == 0.0f) {

			// 移動方向を画面手前に設定
			dir = Vector3{ 0, 0, -1 };

			// アニメーションを後方ステップに変更
			mObject->mSkinning->SetNextAnimation("Actor_BackStep");

		}
		// 回避方向の正規化
		work.avoidDirection = Normalize(dir);

		// アニメーション再生速度の設定(回避速度依存)
		mObject->mSkinning->SetAnimationPlaySpeed(work.avoidSpeed);

		// 状態変化
		mWorks->mBehavior = Behavior::kAvoid;

	}

	// 回避中の処理
	if (mWorks->mBehavior == Behavior::kAvoid) {

		// 回避時間更新
		work.elapsedTime += BlackBoard::CombertBattleFPS(mWorks->mWorkAvoid.avoidSpeed);

		// 移動ベクトルに加算（距離制御も可能）
		Vector3 avoidVelocity = work.avoidDirection * (BlackBoard::CombertBattleFPS(work.avoidRange));
		mObject->mWorldTransform->translation += avoidVelocity;

		// 回避完了判定
		const float avoidDuration = mObject->mSkinning->
			GetSkinCluster("Actor_Avoid")->animation.duration; // 避けモーション再生時間と一致させる

		if (work.elapsedTime >= avoidDuration) {
			work.elapsedTime = 0.0f;
			work.isOperating = false;
			work.isAvoidRequest = false;

			mWorks->mBehavior = Behavior::kRoot;
			mWorkFunc = nullptr;

			// アニメーション再生速度の設定(回避速度依存)
			mObject->mSkinning->SetAnimationPlaySpeed(1.0f);
		}

	}



}

void Player::Guard()
{
}

void Player::JustAction(){
}


void Player::Attack()
{
	// ワーク構造体を取得
	auto& work = mWorks->mWorkAttack;
	// 各攻撃段階のデータを取得
	const ConstAttack& attackData = kConstAttacks[work.comboCount];

	// 各状況に応じて攻撃処理を行う
	switch (mWorks->mWorkAttack.attackPhase)
	{
	case Default:

		// 初回実行時
		if (work.isComboRequest) {
			work.isComboRequest = false;
			work.isOperating = true;
			work.isHit = false;
			work.comboCount = 0;
			work.elapsedTime = 0.0f;

			// 初段のアニメーション再生
			mObject->mSkinning->SetNextAnimation("Actor_S0");
			// アニメーションの再生速度を設定
			mObject->mSkinning->SetAnimationPlaySpeed(attackData.actionSpeed);

			// 振る舞いの変更
			mWorks->mBehavior = Behavior::kAttack;
			mWorks->mWorkAttack.attackPhase = OperatingExtra;
		}

		break;
	case OperatingExtra:

		// 予備動作処理
		// この段階では衝突判定(=コライダーの発生)を行わない

		// 動作時間加算
		work.elapsedTime += BlackBoard::CombertBattleFPS(mObject->mSkinning->GetAnimationPlaySpeed());

		// 一定時間が経過したら、攻撃状態に移行
		if (work.elapsedTime > attackData.operationTime) {
			mWorks->mWorkAttack.attackPhase = InAttack;
		}

		break;
	case InAttack:

		// 攻撃動作処理
		// この段階では衝突判定を行う

		// 動作時間加算
		work.elapsedTime += BlackBoard::CombertBattleFPS(mObject->mSkinning->GetAnimationPlaySpeed());

		// 一定時間が経過したら、攻撃後動作状態に移行
		if (work.elapsedTime > attackData.afterTime) {
			mWorks->mWorkAttack.attackPhase = After;
		}

		break;
	case After:
		
		// 攻撃後動作
		// この段階では衝突判定(=コライダーの発生)を行わない
		// また、攻撃リクエストとカウント状況に応じて動作を変える

		// 動作時間加算
		work.elapsedTime += BlackBoard::CombertBattleFPS(mObject->mSkinning->GetAnimationPlaySpeed());

		// 入力受付時間 → バッファ確認
		if (!work.isComboRequest) {

			auto buffered = InputManager::GetInstance()->ConsumeBufferedInput();
			if (buffered.has_value() && *buffered == BufferedInput::Attack) {
				work.isComboRequest = true;
			}
		}

		// 攻撃モーション終了 + 受付あり → 次段コンボ
		if (work.isComboRequest && work.elapsedTime >= attackData.motionTimeMax) {

			// 次の段が存在する場合のみ進行
			if (work.comboCount + 1 < kComboCountMax) {
				work.comboCount++;
				work.elapsedTime = 0.0f;
				work.isComboRequest = false;
				work.isHit = false;

				std::string nextAnimName = "Actor_S" + std::to_string(work.comboCount);
				mObject->mSkinning->SetNextAnimation(nextAnimName); // 必要に応じて "S1", "S2" などに変更
				// アニメーションの再生速度を設定
				mObject->mSkinning->SetAnimationPlaySpeed(kConstAttacks[work.comboCount].actionSpeed);

				// 振る舞いの変更
				mWorks->mWorkAttack.attackPhase = OperatingExtra;

				return;
			}
		}

		// 全体終了処理
		if (work.elapsedTime >= attackData.motionTimeMax) {
			work.isOperating = false;
			work.isComboRequest = false;
			work.elapsedTime = 0.0f;
			work.comboCount = 0;

			// 振る舞いの変更
			mWorks->mBehavior = Behavior::kRoot;
			mWorks->mWorkAttack.attackPhase = Default;
			// ワーク関数のリセット
			mWorkFunc = nullptr;
			// アニメーションの再生速度を設定
			mObject->mSkinning->SetAnimationPlaySpeed(1.0f);
		}
		
		break;
	default:
		break;
	}

}

void Player::Shot(){

	// 弾のパラメータを設定する
	BulletStatus bulletStatus;
	bulletStatus.pos = sword->GetWorldTransform()->GetWorldPosition();
	bulletStatus.radius = BlackBoard::GetGlobalVariables()->GetFloatValue("PlayerBullet", "Radius");
	bulletStatus.moveSpeed = BlackBoard::GetGlobalVariables()->GetFloatValue("PlayerBullet", "MoveSpeed");

	// 方向
	if (Length(mBoss->GetBodyPos() - bulletStatus.pos) != 0.0f) {

		// ターゲットの座標に向かって放つ
		bulletStatus.direction = Normalize(mBoss->GetBodyPos() - bulletStatus.pos);
		// ただし、Y座標は動かさず、正面に飛ばす
		bulletStatus.direction.y = 0.0f;
	}
	else {
		// 重なっていたら真下に発射する
		bulletStatus.direction = { 0.0f,-1.0f,0.0f };
	}
	// 生存時間
	bulletStatus.lifeTime = BlackBoard::GetGlobalVariables()->GetFloatValue("PlayerBullet", "LifeTime");
	// 威力
	bulletStatus.power = BlackBoard::GetGlobalVariables()->GetFloatValue("PlayerBullet", "Power");
	// ノックバックは1を指定されていた場合のみ発生する
	int32_t enableKnockback = BlackBoard::GetGlobalVariables()->GetIntValue("PlayerBullet", "KnockBack");
	if (enableKnockback == 1) {
		bulletStatus.isHitKnockBack = true;
	}
	// 衝突属性ID
	bulletStatus.collisionAttribute = static_cast<uint32_t>(CollisionTypeId::kPlayerBullet);

	// 弾の作成
	std::unique_ptr<PlayerBullet>bullet = std::make_unique<PlayerBullet>(bulletStatus);
	// リストへの追加
	mBullets.push_back(std::move(bullet));
}

void Player::Move()
{
	// 通常/移動時に有効
	if (mWorks->mBehavior == Behavior::kRoot || mWorks->mBehavior == Behavior::kMove || mWorks->mBehavior == Behavior::kDash) {

		// いずれかの数値が、以上(以下)であれば移動処理を行う
		if (Length(mDirectionForInput) != 0.0f) {

			// 移動速度を設定(AGI/100)
			float moveSpeed = mStatus->AGI * 0.01f;
			
			// RBボタン長押しでダッシュ状態にする
			if (InputManager::GetInstance()->GetLongPush(Gamepad::Button::RIGHT_SHOULDER)) {

				moveSpeed *= 2.0f;

				// ダッシュ状態に変更
				if (mWorks->mBehavior == Behavior::kRoot || mWorks->mBehavior == Behavior::kMove) {
					mWorks->mBehavior = Behavior::kDash;
				}

			}
			// 押されていない場合は通常の移動にする
			else {

				// 移動状態に変更
				if (mWorks->mBehavior == Behavior::kRoot || mWorks->mBehavior == Behavior::kDash) {
					mWorks->mBehavior = Behavior::kMove;
				}

			}

			// 平行移動を行う
			mVelocity = mDirectionForInput * moveSpeed;

		}

		// 入力がなければ
		if (mDirectionInputCount <= 0.0f) {

			if (mWorks->mBehavior == Behavior::kMove || mWorks->mBehavior == Behavior::kDash) {
				// 通常状態に戻す
				mWorks->mBehavior = Behavior::kRoot;
			}
		}

	}

}

void Player::Fall()
{
	// 落下処理
	if (mObject->mWorldTransform->translation.y > 0.0f) {

		// 移動量を加算
		mObject->mWorldTransform->translation.y += mVelocity.y;
		mVelocity.y -= BlackBoard::CombertBattleFPS(0.98f);

	}
	// 地面に到達したら
	else if (mObject->mWorldTransform->translation.y < 0.0f) {

		// 地面に固定
		mObject->mWorldTransform->translation.y = 0.0f;

		// 移動量修正
		mVelocity.y = 0.0f;

		// ジャンプ回数のリセット
		mJumpCount = kJumpCountMax;
	}
}

void Player::InputDirection(){

	// 通常・移動時に有効
	if (mWorks->mBehavior == Behavior::kRoot || mWorks->mBehavior == Behavior::kMove || mWorks->mBehavior == Behavior::kDash) {

		// 入力方向の初期化
		mDirectionForInput = { 0.0f ,0.0f ,0.0f };

		// スティック入力の量
		const static int stickValue = 8000;
		// いずれかの数値が、以上(以下)であれば移動処理を行う
		if (InputManager::GetInstance()->GetStickRatio(Gamepad::Stick::LEFT_X, stickValue) != 0.0f ||
			InputManager::GetInstance()->GetStickRatio(Gamepad::Stick::LEFT_Y, stickValue) != 0.0f) {

			// カウントの増加
			if (mDirectionInputCount < 1.0f) {
				mDirectionInputCount += BlackBoard::GetBattleFPS();
				if (mDirectionInputCount > 1.0f) {
					mDirectionInputCount = 1.0f;
				}
			}

			// Xの移動量とZの移動量を設定する
			mDirectionForInput = {
				(float)InputManager::GetInstance()->GetStick(Gamepad::Stick::LEFT_X) ,
				0.0f,
				(float)InputManager::GetInstance()->GetStick(Gamepad::Stick::LEFT_Y)
			};

			// 正規化
			if (Length(mDirectionForInput) != 0.0f) {
				mDirectionForInput = Normalize(mDirectionForInput);
			}

			// カメラの回転量を反映
			mDirectionForInput = TransformNomal(mDirectionForInput, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix());
			// y座標は移動しない
			mDirectionForInput.y = 0.0f;

		}
		else {
			// 入力がなければカウントを0に戻す
			mDirectionInputCount = 0.0f;
		}

		//	ターゲットロックオンの切り替え
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::X)) {

			// ターゲットロックオン状態を解除する
			if (mIsLockOnToTarget) {
				MainCamera::GetInstance()->SetCameraRotateControll(true);
				MainCamera::GetInstance()->EndSearchTarget();
				mIsLockOnToTarget = false;
			}

			// ターゲットロックオン状態にする
			else if (mReticle->IsLockOn(mBoss->GetBodyPos())) {
				MainCamera::GetInstance()->SetCameraRotateControll(false);
				mIsLockOnToTarget = true;
			}

		}

		// ターゲット方向にカメラを向ける
		MainCamera::GetInstance()->SetCameraRotarionToSearchTarget();

		// プレイヤーは入力方向を向くようにする
		if (Length(mDirectionForInput) != 0.0f) {
			mDirection = mDirectionForInput;
		}

	}

}

void Player::LockOn(){

	// ロックオン時、レティクルをターゲットに向ける

	// ターゲット(ボスキャラ)の座標を取得
	Vector3 newReticlePos = mBoss->GetBodyPos();
	mReticle->SetWorldPos(newReticlePos);

	if (mIsLockOnToTarget) {
		mReticle->SetReticleColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else {
		if (mReticle->IsLockOn(newReticlePos)) {
			mReticle->SetReticleColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else {
			mReticle->SetReticleColor(Color(0.2f, 0.2f, 0.2f, 1.0f));
		}
	}

	// レティクル 更新
	mReticle->Update();

}
