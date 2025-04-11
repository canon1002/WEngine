#include "Player.h"
#include "App/Bullet/PlayerBullet.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Append/Collider/AABBCollider.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "App/Enemy/BossEnemy.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "App/BlackBoard.h"

Player::Player()
{
}

Player::~Player() {

}

void Player::Init() {

	// モデル読み込み
	ModelManager::GetInstance()->LoadModel("player", "idle.gltf");
	ModelManager::GetInstance()->LoadModel("player", "run.gltf");
	ModelManager::GetInstance()->LoadModel("player", "gatotu0.gltf");
	ModelManager::GetInstance()->LoadModel("player", "fastSlash.gltf");
	ModelManager::GetInstance()->LoadModel("player", "slashR.gltf");
	ModelManager::GetInstance()->LoadModel("player", "slashL.gltf");
	ModelManager::GetInstance()->LoadModel("player", "slashEnd.gltf");
	ModelManager::GetInstance()->LoadModel("player", "avoid.gltf");
	ModelManager::GetInstance()->LoadModel("player", "backStep.gltf");
	ModelManager::GetInstance()->LoadModel("player", "thrust.gltf");

	mObject = std::make_unique<Object3d>();
	mObject->Init("PlayerObj");
	mObject->SetScale({ 2.5f,2.5f,2.5f });
	mObject->SetTranslate({ 1.0f,1.0f,7.0f });
	
	// モデルを設定
	mObject->SetModel("idle.gltf");
	// スキニングアニメーションを生成
	mObject->mSkinning = make_unique<Skinning>();
	mObject->mSkinning->Init("player", "idle.gltf", mObject->GetModel()->modelData);
	// モーションブレンド速度
	mObject->mSkinning->SetMotionBlendingInterval(2.0f);
	// アニメーション再生速度
	mObject->mSkinning->SetAnimationPlaySpeed(1.0f);
	// 使用するアニメーションを登録しておく
	mObject->mSkinning->CreateSkinningData("player", "idle", ".gltf", mObject->GetModel()->modelData, true);
	mObject->mSkinning->CreateSkinningData("player", "prepare", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "gatotu0", ".gltf", mObject->GetModel()->modelData);

	mObject->mSkinning->CreateSkinningData("player", "fastSlash", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "slashR", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "slashL", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "slashEnd", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "thrust", ".gltf", mObject->GetModel()->modelData);
	
	mObject->mSkinning->CreateSkinningData("player", "avoid", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "backStep", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "walk", ".gltf", mObject->GetModel()->modelData, true);
	mObject->mSkinning->CreateSkinningData("player", "run", ".gltf", mObject->GetModel()->modelData, true);



	// 最初は通常状態から始める
	mBehavior = Behavior::kRoot;

	// 移動量を初期化
	mVelocity = { 0.0f,0.0f ,0.0f };

	// 重力の影響を受ける
	mIsGravity = true;

	// ジャンプ回数のセット
	mJumpCount = kJumpCountMax;

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

	// レティクルの初期化
	mReticle = std::make_unique<Reticle3D>();
	mReticle->Init();

	// 構え
	mIsAimMode = false;

	// 回避行動リクエスト
	mAvoidStatus.mIsAvoidRequest = false;
	// 回避中か
	mAvoidStatus.mIsAvoid = false;
	// 回避時の無敵状態であるか
	mAvoidStatus.mIsAvoidInvincible = false;
	// 回避距離
	mAvoidStatus.mAvoidRange = 5.0f;
	// 回避速度
	mAvoidStatus.mAvoidSpeed = 0.0f;
	// 回避経過時間( 0.0f ~ 1.0f )
	mAvoidStatus.mAvoidTime = 0.0f;
	// 回避開始地点
	mAvoidStatus.mAvoidMoveStartPos = { 0.0f,0.0f,0.0f };
	// 回避終了地点
	mAvoidStatus.mAvoidMoveEndPos = { 0.0f,0.0f,0.0f };

	// 攻撃関連パラメータ
	mAttackStatus.sword = std::make_unique<Object3d>();
	mAttackStatus.sword->Init("sword");
	mAttackStatus.sword->SetModel("sword.gltf");
	mAttackStatus.sword->mWorldTransform->scale = { 0.1f,0.1f,0.175f };
	mAttackStatus.sword->mWorldTransform->rotation = { 2.0f,-0.6f,1.4f };
	mAttackStatus.sword->mWorldTransform->translation = { 0.05f,0.0f,0.05f };
	mAttackStatus.sword->mSkinning = make_unique<Skinning>();
	mAttackStatus.sword->mSkinning->Init("Weapons", "sword.gltf",
		mAttackStatus.sword->GetModel()->modelData);
	mAttackStatus.sword->mSkinning->IsInactive();

	mAttackStatus.pos = { 1000.0f,900.0f,0.0f };// 初期位置
	mAttackStatus.normalRot = { 1.7f,-0.3f,-0.2f };// 初期回転量
	mAttackStatus.endRot = { 3.8f,-0.3f,1.0f };// 最終回転量
	// ペアレント
	mAttackStatus.weaponParentMat = MakeIdentity();
	mAttackStatus.sword->mWorldTransform->SetParent(mAttackStatus.weaponParentMat);

	// 武器にコライダーをセットする
	for (int32_t i = 0; i < 5; i++) {
		mAttackStatus.swordWorldMat[i] = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
		// コライダー 宣言
		std::shared_ptr<GameCollider> newCollider = std::make_shared<GameCollider>();
		newCollider->collider = std::make_unique<SphereCollider>(new WorldTransform(), 0.25f);
		// 初期化
		newCollider->collider->Init();
		newCollider->collider->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kPlayerWeapon));
		// 武器に設定したボーンのワールド座標をセット
		newCollider->collider->GetWorld()->SetParent(mAttackStatus.swordWorldMat[i]);
		//// 配列にプッシュする
		mAttackStatus.swordColliders.push_back(std::move(newCollider));
	}

	// ステータス取得
	mStatus = std::make_shared<Status>();
	StatusManager::GetInstance()->GetPlayerStatus(*mStatus);

	// -- エフェクト関係 -- //

	// 剣先と根本のワールド座標
	for (size_t i = 0; i < mWorldTransformSword.size(); i++) {
		mWorldTransformSword[i] = make_unique<WorldTransform>();
		mWorldTransformSword[i]->Init();
	}
	// ペアレントを設定(後にワールド座標を取得する)
	mWorldTransformSword[0]->SetParent(mAttackStatus.swordWorldMat[0]);
	mWorldTransformSword[1]->SetParent(mAttackStatus.swordWorldMat[4]);

	// 方向指定(とりあえず奥を向く)
	mDirection = { 0.0f,0.0f,1.0f };
	mDirectionForInput = { 0.0f,0.0f,0.0f };

	// ターゲットロックオンの有無
	mIsLockOnToTarget = false;

}

void Player::Update() {

	if (mStatus->HP > 0.0f) {

		mVelocity = { 0.0f,0.0f,0.0f };

		// 方向入力
		InputDirection();
		// 落下処理
		Fall();
		// 移動処理
		Move();
		// 回避関連処理
		Avoid();
		// 攻撃関連処理
		Attack();
		// 方向修正
		AdJustDirection();
		
		switch (mBehavior)
		{
		case Behavior::kRoot:

			// コンボ回数リセット
			mAttackStatus.comboCount = 0;

			if (mObject->mSkinning->GetNowSkinCluster()->name != "idle" &&
				!mObject->mSkinning->SearchToWaitingSkinCluster("idle"))
			{
				mObject->mSkinning->SetNextAnimation("idle");
			}

			break;
		case Behavior::kMove:

			if (mObject->mSkinning->GetNowSkinCluster()->name != "walk" &&
				!mObject->mSkinning->SearchToWaitingSkinCluster("walk"))
			{
				mObject->mSkinning->SetNextAnimation("walk");
			}

			break;
		case Behavior::kDash:

			if (mObject->mSkinning->GetNowSkinCluster()->name != "run" &&
				!mObject->mSkinning->SearchToWaitingSkinCluster("run"))
			{
				mObject->mSkinning->SetNextAnimation("run");
			}

			break;

		case Behavior::kAttack:
			break;
		case Behavior::kJump:
			break;
		case Behavior::kCharge:
			break;
		case Behavior::kChargeAttack:
			break;
		case Behavior::kAvoid:
			break;
		case Behavior::kGuard:
			break;
		default:
			break;
		}

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

	// UI更新
	mStatus->Update();
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
	mObject->mWorldTransform->translation.x = std::clamp(mObject->mWorldTransform->translation.x, -20.0f, 20.0f);
	mObject->mWorldTransform->translation.z = std::clamp(mObject->mWorldTransform->translation.z, -20.0f, 20.0f);

	// オブジェクト更新
	mObject->Update();
	mObject->mSkinning->GetSkeleton().joints;
	mObject->mCollider->Update();

	// 身体の部位のワールド行列を更新
	UpdateBodyCollider();

	// 右手のワールド行列を更新
	mAttackStatus.weaponParentMat = Multiply(
		GetObject3D()->mSkinning->GetSkeleton().joints[GetObject3D()->mSkinning->GetSkeleton().jointMap["mixamorig:RightHand"]
		].skeletonSpaceMatrix, GetObject3D()->GetWorldTransform()->GetWorldMatrix());

	// 剣
	mAttackStatus.sword->Update();

	// ワールド座標更新
	mAttackStatus.swordWorldMat[0] = Multiply(
		mAttackStatus.sword->mSkinning->GetBoneMatrix("Blade0"),
		mAttackStatus.sword->GetWorldTransform()->GetWorldMatrix());
	
	mAttackStatus.swordWorldMat[1] = Multiply(
		mAttackStatus.sword->mSkinning->GetSkeleton().joints[mAttackStatus.sword->mSkinning->GetSkeleton().jointMap["Blade1"]
		].skeletonSpaceMatrix, mAttackStatus.sword->GetWorldTransform()->GetWorldMatrix());
	mAttackStatus.swordWorldMat[2] = Multiply(
		mAttackStatus.sword->mSkinning->GetSkeleton().joints[mAttackStatus.sword->mSkinning->GetSkeleton().jointMap["Blade2"]
		].skeletonSpaceMatrix, mAttackStatus.sword->GetWorldTransform()->GetWorldMatrix());
	mAttackStatus.swordWorldMat[3] = Multiply(
		mAttackStatus.sword->mSkinning->GetSkeleton().joints[mAttackStatus.sword->mSkinning->GetSkeleton().jointMap["Blade3"]
		].skeletonSpaceMatrix, mAttackStatus.sword->GetWorldTransform()->GetWorldMatrix());
	mAttackStatus.swordWorldMat[4] = Multiply(
		mAttackStatus.sword->mSkinning->GetSkeleton().joints[mAttackStatus.sword->mSkinning->GetSkeleton().jointMap["Blade4"]
		].skeletonSpaceMatrix, mAttackStatus.sword->GetWorldTransform()->GetWorldMatrix());

	// 弾 更新
	for (auto& bullet : mBullets) {
		bullet->Update();
	}

	// 武器のコライダー 更新
	for (auto& collider : mAttackStatus.swordColliders) {
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
		"kRoot",   // 通常状態
		"kMove",   // 移動状態
		"kAttack", // 攻撃中
		"kJump",	 // ジャンプ中
		"kCharge", // 溜め動作中
		"kChargeAttack", // 溜め攻撃
		"kAvoid",  // 回避行動
		"kGuard",  // 防御行動
	};
	int32_t behaviorNumber = (int32_t)mBehavior;

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
		ImGui::DragFloat("AvoidRange", &mAvoidStatus.mAvoidRange);
		ImGui::DragFloat("AvoidSpeed", &mAvoidStatus.mAvoidSpeed);
		ImGui::DragFloat("AvoidTime", &mAvoidStatus.mAvoidTime);
		ImGui::DragFloat3("Avoid Start", &mAvoidStatus.mAvoidMoveStartPos.x);
		ImGui::DragFloat3("Avoid End", &mAvoidStatus.mAvoidMoveEndPos.x);

	}

	// 攻撃パラメータ
	if (ImGui::CollapsingHeader("Attack")) {
		std::string timeStr = "Time : " + std::to_string(mAttackStatus.motionTime);
		ImGui::ProgressBar(mAttackStatus.motionTime, ImVec2(-1.0f, 0.0f), timeStr.c_str());
		ImGui::DragInt("ComboCount", &mAttackStatus.comboCount, 0);
		std::string inputTimeStr = "inputWait : " + std::to_string(mAttackStatus.inputWaitingTime);
		ImGui::ProgressBar(mAttackStatus.inputWaitingTime, ImVec2(-1.0f, 0.0f), inputTimeStr.c_str());

		if (mAttackStatus.isComboRequest)
		{
			ImGui::Text("ComboRequest: [ON]");
		}
		else
		{
			ImGui::Text("ComboRequest: [OFF]");
		}
		mAttackStatus.sword->DrawGuiTree();

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


	if (mBehavior == Behavior::kAttack || mBehavior == Behavior::kChargeAttack){
		if (mAttackStatus.isOperating){
			// 武器のコライダー 描画
			for (auto& collider : mAttackStatus.swordColliders) {
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
	mAttackStatus.sword->Draw();

}

void Player::SetColliderList(){

	// プレイヤー本体のコライダー
	for (auto& collider : mBodyPartColliders) {
		// ゲームマネージャのリストに追加
		GameManager::GetInstance()->SetCollider(collider.second);
	}


	// プレイヤーの武器コライダー

	// 攻撃中かつ命中前であればコライダーをリストに追加する
	if (mAttackStatus.isOperating && !mAttackStatus.isHit) {
		for (auto& collider : mAttackStatus.swordColliders) {

			// ゲームマネージャのリストに追加
			GameManager::GetInstance()->SetCollider(collider);
		}
	}
}

bool Player::GetIsOperating() const {
	// 攻撃ステータスから攻撃中であるか取得
	return mAttackStatus.isOperating;
}

void Player::Avoid()
{
	//  非回避状態で Aボタンで回避
	if (mBehavior != Behavior::kAvoid &&
		InputManager::GetInstance()->GetPused(Gamepad::Button::A)) {

		// パラメータの補正

		// 回避距離
		mAvoidStatus.mAvoidRange = 4.0f;
		// 回避速度
		mAvoidStatus.mAvoidSpeed = 2.0f;
		// 回避経過時間( 0.0f ~ 1.0f )
		mAvoidStatus.mAvoidTime = 0.0f;
		// 回避開始地点に現在の座標を代入
		mAvoidStatus.mAvoidMoveStartPos = mObject->GetWorldTransform()->translation;


		// スティック入力の量
		const static int stickValue = 6000;
		// いずれかの数値が、指定した値以上(以下)であれば移動回避を行う
		if (InputManager::GetInstance()->GetStick(Gamepad::Stick::LEFT_X) < -stickValue || // 左 
			InputManager::GetInstance()->GetStick(Gamepad::Stick::LEFT_X) > stickValue || // 右
			InputManager::GetInstance()->GetStick(Gamepad::Stick::LEFT_Y) < -stickValue || // 上
			InputManager::GetInstance()->GetStick(Gamepad::Stick::LEFT_Y) > stickValue	  // 下
			) {

			// スティックの入力方向を計算
			Vector3 direction = {
				(float)InputManager::GetInstance()->GetStick(Gamepad::Stick::LEFT_X) ,
				0.0f,
				(float)InputManager::GetInstance()->GetStick(Gamepad::Stick::LEFT_Y)
			};
			// 正規化
			direction = Normalize(direction);
			// カメラの回転量を反映
			direction = TransformNomal(direction, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix());
			// y座標は移動しない
			direction.y = 0.0f;

			mAvoidStatus.mAvoidMoveEndPos = mObject->mWorldTransform->translation + Scalar(mAvoidStatus.mAvoidRange, direction);

			// 回避状態に変更
			mBehavior = Behavior::kAvoid;

			// アニメーション変更
			mObject->mSkinning->SetNextAnimation("avoid");
			mObject->mSkinning->SetAnimationPlaySpeed(mAvoidStatus.mAvoidSpeed);

		}
		// スティック入力がなければバックステップを行う
		else {
			
			// 移動方向をプレイヤーの向きの反対方向にを終点に設定
			mAvoidStatus.mAvoidMoveEndPos = mObject->mWorldTransform->translation + Scalar(-mAvoidStatus.mAvoidRange, mDirection);
			// 回避状態に変更
			mBehavior = Behavior::kAvoid;
			// アニメーション変更
			mObject->mSkinning->SetNextAnimation("backStep");
			mObject->mSkinning->SetAnimationPlaySpeed(mAvoidStatus.mAvoidSpeed);
		}

	}

	// 回避中の処理
	if (mBehavior == Behavior::kAvoid) {

		// 線形補間を利用して座標を移動させる
		if (mAvoidStatus.mAvoidTime < 1.0f) {
			// 回避時の移動速度/フレームレートで加算する
			mAvoidStatus.mAvoidTime += (mAvoidStatus.mAvoidSpeed / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();
		}
		else if (mAvoidStatus.mAvoidTime >= 1.0f) {
			// タイムを補正
			mAvoidStatus.mAvoidTime = 1.0f;

			// 移動終了
			mBehavior = Behavior::kRoot;
			mObject->mSkinning->SetAnimationPlaySpeed(1.0f);
		}

		// 移動
		mObject->mWorldTransform->translation.x = (1.0f - mAvoidStatus.mAvoidTime) * mAvoidStatus.mAvoidMoveStartPos.x + mAvoidStatus.mAvoidTime * mAvoidStatus.mAvoidMoveEndPos.x;
		mObject->mWorldTransform->translation.z = (1.0f - mAvoidStatus.mAvoidTime) * mAvoidStatus.mAvoidMoveStartPos.z + mAvoidStatus.mAvoidTime * mAvoidStatus.mAvoidMoveEndPos.z;

	}


}

void Player::Attack()
{
	// コンボ上限に達していない
	if (mAttackStatus.comboCount < kComboCountMax) {
		// Bボタン Triggerで攻撃
		if (InputManager::GetInstance()->GetPused(Gamepad::Button::B) || InputManager::GetInstance()->GetTriggerKey(DIK_RETURN))
		{
			// 初期動作
			if (mBehavior == Behavior::kRoot || mBehavior == Behavior::kMove)
			{
				mBehavior = Behavior::kAttack;
				mAttackStatus.motionTime = 1.0f;
			}

			// コンボ有効
			mAttackStatus.isComboRequest = true;
		}
	}
	if (mBehavior != Behavior::kAttack) { return; }
	if (mAttackStatus.motionTime >= 1.0f)
	{
		// コンボ
		if (mAttackStatus.isComboRequest)
		{
			// コンボ回数 増加	
			mAttackStatus.comboCount++;
			// リクエスト解除
			mAttackStatus.isComboRequest = false;
			// タイマーをリセット
			mAttackStatus.motionTime = 0.0f;

			mAttackStatus.isOperating = false;
			mAttackStatus.isHit = false;

			// コンボ段階に応じたモーションに切り替える
			// 攻撃コンボの段階に応じた処理
			switch (mAttackStatus.comboCount)
			{
			case 1:
				mObject->mSkinning->SetNextAnimation("slashR");
				break;

			case 2:
				mObject->mSkinning->SetNextAnimation("slashL");
				break;

			case 3:
				mObject->mSkinning->SetNextAnimation("slashEnd");
				break;

			default:
				break;
			}

		}
		// コンボを継続しない場合、通常状態に戻る
		else{
			if (mAttackStatus.motionTime >= 1.5f) {

				mAttackStatus.motionTime = 0.0f;
				mAttackStatus.isOperating = false;
				mAttackStatus.isHit = false;
				mAttackStatus.isUp = false;
				//mObject->mSkinning->SetNextAnimation("idle");
				// コンボ回数リセット
				mAttackStatus.comboCount = 0;
				mBehavior = Behavior::kRoot;
				
			}
		}
	}
	
	if (mAttackStatus.motionTime <= 1.5f)
	{
		mAttackStatus.motionTime += BlackBoard::CombertBattleFPS(1.5f);

		// 攻撃が命中していない場合
		if (mAttackStatus.motionTime > 0.4f && !mAttackStatus.isHit && !mAttackStatus.isOperating) {
			
			// 攻撃フラグをtrueにする
			mAttackStatus.isOperating = true;

			// 射撃(斬撃を飛ばす)
			Shot();

		}

	}



}

void Player::Shot(){

	// 弾のパラメータを設定する
	BulletStatus bulletStatus;
	bulletStatus.pos = mAttackStatus.sword->GetWorldTransform()->GetWorldPosition();
	bulletStatus.radius = BlackBoard::GetGlobalVariables()->GetFloatValue("PlayerBullet", "Radius");
	bulletStatus.moveSpeed = BlackBoard::GetGlobalVariables()->GetFloatValue("PlayerBullet", "MoveSpeed");

	// 方向
	if (Length(mBoss->GetBodyPos() - bulletStatus.pos) != 0.0f) {

		// ターゲットの座標に向かって放つ
		bulletStatus.direction = Normalize(mBoss->GetBodyPos() - bulletStatus.pos);
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
	// 通常/防御時に有効
	if (mBehavior == Behavior::kRoot || mBehavior == Behavior::kMove || mBehavior == Behavior::kDash) {

		// いずれかの数値が、以上(以下)であれば移動処理を行う
		if (Length(mDirectionForInput) != 0.0f) {

			// 移動速度を設定
			float moveSpeed = 0.10f;

			// RBボタン長押しでダッシュ状態にする
			if (InputManager::GetInstance()->GetLongPush(Gamepad::Button::RIGHT_SHOULDER)) {

				moveSpeed *= 2.0f;

				// ダッシュ状態に変更
				if (mBehavior == Behavior::kRoot || mBehavior == Behavior::kMove) {
					mBehavior = Behavior::kDash;
				}

			}
			// 押されていない場合は通常の移動にする
			else {

				// 移動状態に変更
				if (mBehavior == Behavior::kRoot || mBehavior == Behavior::kDash) {
					mBehavior = Behavior::kMove;
				}

			}

			// 平行移動を行う
			mVelocity = mDirectionForInput * moveSpeed;

		}

		// 入力がなければ
		if (mDirectionInputCount <= 0.0f) {

			if (mBehavior == Behavior::kMove || mBehavior == Behavior::kDash) {
				// 通常状態に戻す
				mBehavior = Behavior::kRoot;
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

	// ロックオン中のカメラ処理
	if (mIsLockOnToTarget) {

		// ターゲット方向にカメラを向ける
		MainCamera::GetInstance()->SetCameraRotarionToSearchTarget();
		// ターゲット方向に体を向ける
		mDirection = TransformNomal(Vector3(0.0f,0.0f,1.0f), MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix());
	}
	// 非ロックオン時
	else {

		// 入力方向を向くようにする
		if (Length(mDirectionForInput) != 0.0f) {
			mDirection = mDirectionForInput;
		}
	}

}

void Player::Save()
{
}

void Player::Load()
{
}
