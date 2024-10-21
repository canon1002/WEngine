#include "Player.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Append/Collider/AABBCollider.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "App/Enemy/BossEnemy.h"
#include "App/Reaction/DamageReaction.h"


Player::~Player() {

	for (auto& arrow : mArrows) {
		delete arrow;
	}
}

void Player::Init() {

	// モデル読み込み
	ModelManager::GetInstance()->LoadModel("player", "idle.gltf");
	ModelManager::GetInstance()->LoadModel("player", "gatotu0.gltf");
	ModelManager::GetInstance()->LoadModel("player", "slash.gltf");
	ModelManager::GetInstance()->LoadModel("player", "walk.gltf");

	mObject = std::make_unique<Object3d>();
	mObject->Init("PlayerObj");
	mObject->SetTranslate({ 1.0f,1.0f,7.0f });
	// モデルを設定
	mObject->SetModel("idle.gltf");
	// スキニングアニメーションを生成
	mObject->mSkinning = new Skinnig();
	mObject->mSkinning->Init("player", "idle.gltf", mObject->GetModel()->modelData);
	// 使用するアニメーションを登録しておく
	mObject->mSkinning->CreateSkinningData("player", "idle", ".gltf", mObject->GetModel()->modelData, true);
	mObject->mSkinning->CreateSkinningData("player", "prepare", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "gatotu0", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "slash", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "slashUp", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("player", "walk", ".gltf", mObject->GetModel()->modelData, true);


	// メインカメラをフォローカメラ仕様にする
	//MainCamera::GetInstance()->SetTarget(mObject->GetWorldTransform());

	// 最初は通常状態から始める
	mBehavior = Behavior::kRoot;

	// 移動量を初期化
	mVelocity = { 0.0f,0.0f ,0.0f };

	// 重力の影響を受ける
	mIsGravity = true;

	// ジャンプ回数のセット
	mJumpCount = kJumpCountMax;

	// コライダーの宣言
	mObject->mCollider = new SphereCollider(mObject->mWorldTransform, 1.0f);
	mObject->mCollider->Init();
	mObject->mCollider->SetAddtranslation(Vector3(0.0f, 0.55f, -0.1f));
	mObject->mCollider->SetCollisionAttribute(kCollisionAttributePlayer);
	mObject->mCollider->SetCollisionMask(kCollisionAttributeEnemyBullet);

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
	mAvoidStatus.mAvoidRange = 10.0f;
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
	mAttackStatus.sword->mWorldTransform->scale = { 0.5f,0.5f,0.5f };
	mAttackStatus.sword->mWorldTransform->rotation = { 1.7f,-0.3f,0.0f };
	mAttackStatus.sword->mSkinning = new Skinnig();
	mAttackStatus.sword->mSkinning->Init("Weapons", "sword.gltf",
		mAttackStatus.sword->GetModel()->modelData);
	mAttackStatus.sword->mSkinning->IsInactive();
	mAttackStatus.sword->mSkeleton = Skeleton::Create(mAttackStatus.sword->GetModel()->modelData.rootNode);
	//mAttackStatus.sword->mWorldTransform->translation = mObject->GetWorldTransform()->translation;
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
		SphereCollider* newCollider = new SphereCollider(new WorldTransform(), 0.1f);
		// 初期化
		newCollider->Init();
		newCollider->SetCollisionAttribute(kCollisionAttributeEnemyBullet);
		newCollider->SetCollisionMask(kCollisionAttributePlayer);
		// 武器に設定したボーンのワールド座標をセット
		newCollider->
			GetWorld()->SetParent(mAttackStatus.swordWorldMat[i]);
		//// 配列にプッシュする
		mAttackStatus.swordColliders.push_back(newCollider);
	}

	// 防御関連パラメータ
	mGuardStatus.shield = std::make_unique<Object3d>();
	mGuardStatus.shield->Init("shield");
	mGuardStatus.shield->SetModel("Shield.gltf");
	mGuardStatus.shield->mWorldTransform->scale = { 0.4f,0.4f,0.4f };
	mGuardStatus.shield->mWorldTransform->rotation = { 1.5f,0.0f,0.0f };
	mGuardStatus.shield->mWorldTransform->translation = mObject->GetWorldTransform()->translation;
	mGuardStatus.normalPos = { -150.0f,900.0f,0.0f };// 初期位置
	mGuardStatus.guardPos = { 170.0f,720.0f,0.0f };// 構え位置

	// ステータス取得
	mStatus = new Status();
	StatusManager::GetInstance()->GetPlayerStatus(*mStatus);

}

void Player::Update() {

	if (mStatus->HP > 0.0f) {

		// 落下処理
		Fall();
		// 移動処理
		Move();

		// 回避関連処理
		//Avoid();
		// 防御関連処理
		//Guard();

		// 突撃
		SpecialAtkRB();
		// 攻撃関連処理
		Attack();

		// デバッグ操作
#ifdef _DEBUG
		DebagCtr();
#endif // _DEBUG

		switch (mBehavior)
		{
		case Behavior::kRoot:

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
		

		// レティクル 更新
		mReticle->Update();

	}

	// オブジェクト更新
	UpdateObject();

	// シールド
	mGuardStatus.shield->Update();

	// UI更新
	mStatus->Update();

}

void Player::UpdateObject()
{
	// オブジェクト更新
	mObject->Update();
	mObject->mSkinning->GetSkeleton().joints;
	mObject->mCollider->Update();

	// 右手のワールド行列を更新
	mAttackStatus.weaponParentMat = Multiply(
		GetObject3D()->mSkinning->GetSkeleton().joints[GetObject3D()->mSkinning->GetSkeleton().jointMap["weaponM"]
		].skeletonSpaceMatrix, GetObject3D()->GetWorldTransform()->GetWorldMatrix());

	// 剣
	mAttackStatus.sword->Update();

	// ワールド座標更新
	mAttackStatus.swordWorldMat[0] = Multiply(
		mAttackStatus.sword->mSkinning->GetSkeleton().joints[mAttackStatus.sword->mSkinning->GetSkeleton().jointMap["Blade0"]
		].skeletonSpaceMatrix, mAttackStatus.sword->GetWorldTransform()->GetWorldMatrix());
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

	// 武器のコライダー 更新
	for (Collider* collider : mAttackStatus.swordColliders) {
		collider->Update();
	}

	// 衝突時の処理
	if (mBehavior == Behavior::kAttack || mBehavior == Behavior::kChargeAttack) {
		if ((mAttackStatus.isOperating == true || mChargeStatus.isCharge == true) && mAttackStatus.isHit == false)
		{
			for (Collider* collider : mAttackStatus.swordColliders)
			{
				if (collider->GetOnCollisionFlag() == false)
				{
					continue;
				}

				// 次のフレームで消す
				mAttackStatus.isHit = true;

				// 敵にダメージを与える
				ReciveDamageToBoss(1.2f);

				// ダメージ表示
				int32_t damage = static_cast<int32_t>(static_cast<int32_t>(mStatus->STR / 2.0f) * 1.0f) - static_cast<int32_t>(mBoss->GetStatus()->VIT / 4.0f);;
				DamageReaction::GetInstance()->Reaction(mReticle->GetWorld3D(), damage, MainCamera::GetInstance());
			}
		}
	}

}

void Player::Draw() {
	mObject->Draw();
}

void Player::DrawGUI() {
#ifdef _DEBUG

	// メニューバーを表示する
	ImGui::Begin("Player");

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

	ImGui::ListBox("Current State",&behaviorNumber, str, IM_ARRAYSIZE(str));

	if (ImGui::CollapsingHeader("Animation")) {

		std::string strNormalT = "MotionBlendingTime : " + std::to_string(mObject->mSkinning->GetMotionBlendingTime());
		ImGui::ProgressBar(mObject->mSkinning->GetMotionBlendingTime(), ImVec2(-1.0f, 0.0f), strNormalT.c_str());

		// Skinning
		mObject->mSkinning->DrawGui();
	}

	//mObject->DrawGuiTree();
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

	// 防御パラメータ
	if (ImGui::CollapsingHeader("Guard")) {
		ImGui::DragFloat3("NormalPos", &mGuardStatus.normalPos.x, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("GuradPos", &mGuardStatus.guardPos.x, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Pos", &mGuardStatus.pos.x, 0.05f, -10.0f, 10.0f);
		mGuardStatus.shield->DrawGuiTree();

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
#endif // _DEBUG
	mReticle->Draw3DReticle();

	// 剣
	mAttackStatus.sword->Draw();
	if (mBehavior == Behavior::kAttack || mBehavior == Behavior::kChargeAttack)
	{
		if (mAttackStatus.isOperating|| mChargeStatus.isCharge)
		{
			// 武器のコライダー 描画
			for (Collider* collider : mAttackStatus.swordColliders) {
				collider->Draw();
			}
		}
	}

	for (const auto& arrow : mArrows) {
		arrow->GetCollider()->Draw();
	}
}

void Player::SetColliderListForArrow(CollisionManager* cManager)
{
	// アロークラスのコライダーをリストに追加する
	auto arrowIt = mArrows.begin();  // イテレータを初期化
	while (arrowIt != mArrows.end()) {
		const auto& arrow = *arrowIt;
		cManager->SetCollider(arrow->GetCollider());
		++arrowIt;  // イテレータを次に進める
	}

}

void Player::SetColliderList(CollisionManager* cManager)
{
	// 攻撃中かつ命中前であればコライダーをリストに追加する
	if (!mAttackStatus.isHit && mAttackStatus.isOperating) {
		for (Collider* collider : mAttackStatus.swordColliders) {
			cManager->SetCollider(collider);
		}
	}
}

void Player::Avoid()
{
	//  非回避状態で Aボタンで回避
	if (mBehavior != Behavior::kAvoid &&
		mInput->GetLongPush(Gamepad::Button::A)) {

		// パラメータの補正

		// 回避距離
		mAvoidStatus.mAvoidRange = 10.0f;
		// 回避速度
		mAvoidStatus.mAvoidSpeed = 2.0f;
		// 回避経過時間( 0.0f ~ 1.0f )
		mAvoidStatus.mAvoidTime = 0.0f;
		// 回避開始地点に現在の座標を代入
		mAvoidStatus.mAvoidMoveStartPos = mObject->GetWorldTransform()->translation;


		// スティック入力の量
		const static int stickValue = 6000;
		// いずれかの数値が、指定した値以上(以下)であれば移動回避を行う
		if (mInput->GetStick(Gamepad::Stick::LEFT_X) < -stickValue || // 左 
			mInput->GetStick(Gamepad::Stick::LEFT_X) > stickValue || // 右
			mInput->GetStick(Gamepad::Stick::LEFT_Y) < -stickValue || // 上
			mInput->GetStick(Gamepad::Stick::LEFT_Y) > stickValue	  // 下
			) {

			// スティックの入力方向を計算
			Vector3 direction = {
				(float)mInput->GetStick(Gamepad::Stick::LEFT_X) ,
				0.0f,
				(float)mInput->GetStick(Gamepad::Stick::LEFT_Y)
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

		}
		// スティック入力がなければバックステップを行う
		else {

			Vector3 direction = TransformNomal(Vector3(0.0f, 0.0f, -1.0f), MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix());
			// y座標は移動しない
			direction.y = 0.0f;
			mAvoidStatus.mAvoidMoveEndPos = mObject->mWorldTransform->translation + Scalar(mAvoidStatus.mAvoidRange, direction);
			// 回避状態に変更
			mBehavior = Behavior::kAvoid;

		}

	}

	// 回避中の処理
	if (mBehavior == Behavior::kAvoid) {

		// 線形補間を利用して座標を移動させる
		if (mAvoidStatus.mAvoidTime < 1.0f) {
			// 回避時の移動速度/フレームレートで加算する
			mAvoidStatus.mAvoidTime += mAvoidStatus.mAvoidSpeed / 60.0f;
		}
		else if (mAvoidStatus.mAvoidTime >= 1.0f) {
			// タイムを補正
			mAvoidStatus.mAvoidTime = 1.0f;

			// 移動終了
			mBehavior = Behavior::kRoot;
		}

		// 移動
		mObject->mWorldTransform->translation.x = (1.0f - mAvoidStatus.mAvoidTime) * mAvoidStatus.mAvoidMoveStartPos.x + mAvoidStatus.mAvoidTime * mAvoidStatus.mAvoidMoveEndPos.x;
		mObject->mWorldTransform->translation.z = (1.0f - mAvoidStatus.mAvoidTime) * mAvoidStatus.mAvoidMoveStartPos.z + mAvoidStatus.mAvoidTime * mAvoidStatus.mAvoidMoveEndPos.z;

	}


}

void Player::Guard()
{
	// キー入力取得
	if (mInput->GetLongPush(Gamepad::Button::RIGHT_SHOULDER)) {

		// 入力中 ガードへ移行

		// tを増加させ、座標を移動
		if (mGuardStatus.t < 1.0f) {
			mGuardStatus.t += 5.0f / 60.0f;
		}
		else if (mGuardStatus.t > 1.0f) {
			mGuardStatus.t = 1.0f;
		}

	}
	// キー入力がなければtを減少
	else {
		if (mGuardStatus.t > 0.0f) {
			mGuardStatus.t -= 5.0f / 60.0f;
		}
		else if (mGuardStatus.t < 0.0f) {
			mGuardStatus.t = 0.0f;
		}
	}

	// 補間後の数値を計算
	mGuardStatus.pos = ExponentialInterpolation(mGuardStatus.normalPos, mGuardStatus.guardPos, mGuardStatus.t, 1.0f);

	// シールドの位置を更新(2D配置→ワールド座標に変換)
	mGuardStatus.shield->mWorldTransform->translation = DamageReaction::GetWorldPosForScreen(Vector2(mGuardStatus.pos.x, mGuardStatus.pos.y), 1.0f, MainCamera::GetInstance());
	// 回転量をカメラから取得
	mGuardStatus.shield->mWorldTransform->rotation.y = MainCamera::GetInstance()->GetRotate().y;


	// tの値に応じてflagをon/offする
	if (mGuardStatus.t > 0.9f) {
		mGuardStatus.flag = true;
		mBehavior = Behavior::kGuard;

	}
	else {
		mGuardStatus.flag = false;
		mBehavior = Behavior::kRoot;
	}

	// flagがtrueのとき、ダメージ時軽減を行う
	if (mGuardStatus.flag) {
		// 防御中は守備力を倍にする
		mStatus->VIT = StatusManager::GetInstance()->GetPlayerStatus()->VIT * 2;
	}
	else {
		mStatus->VIT = StatusManager::GetInstance()->GetPlayerStatus()->VIT;
	}
}

void Player::Attack()
{
	// コンボ上限に達していない
	if (mAttackStatus.comboCount < kComboCountMax) {
		// Bボタン Triggerで攻撃
		if (mInput->GetPused(Gamepad::Button::B) || mInput->GetTriggerKey(DIK_RETURN))
		{
			// 初期動作
			if (mBehavior == Behavior::kRoot)
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

				mObject->mSkinning->SetNextAnimation("slash");

				break;

			case 2:

				mObject->mSkinning->SetMotionBlendingInterval(4.0f);
				mObject->mSkinning->SetNextAnimation("slashUp");

				break;

			case 3:

				mObject->mSkinning->SetMotionBlendingInterval(10.0f);
				mObject->mSkinning->SetNextAnimation("prepare");

				break;

			default:
				break;
			}

		}
		// コンボを継続しない場合、通常状態に戻る
		else
		{
			mAttackStatus.motionTime = 0.0f;
			mAttackStatus.isOperating = false;
			mAttackStatus.isHit = false;
			mAttackStatus.isUp = false;
			mObject->mSkinning->SetMotionBlendingInterval(2.0f);
			//mObject->mSkinning->SetNextAnimation("idle");
			// コンボ回数リセット
			mAttackStatus.comboCount = 0;
			mBehavior = Behavior::kRoot;
		}
	}
	else
	{
		mAttackStatus.motionTime += (1.0f / 60.0f);

		if (mAttackStatus.comboCount == 3)
		{
			if (mAttackStatus.motionTime >= 0.1f &&
				mObject->mSkinning->GetNowSkinCluster()->name != "gatotu0" &&
				!mObject->mSkinning->SearchToWaitingSkinCluster("gatotu0"))
			{
				mObject->mSkinning->SetNextAnimation("gatotu0");
				mAttackStatus.isOperating = true;
			}
		}
		else {
			
			// 攻撃が命中していない かつ tが0.1f以上であれば攻撃フラグをtrueにする
			if (mAttackStatus.motionTime >= 0.6f && !mAttackStatus.isHit && !mAttackStatus.isOperating)
			{
				mAttackStatus.isOperating = true;
			}

		}
	}



}

void Player::Move()
{
	// 通常/防御時に有効
	if (mBehavior == Behavior::kRoot || mBehavior == Behavior::kMove || mBehavior == Behavior::kGuard) {

		if (mInput == nullptr) {
			mInput = InputManager::GetInstance();
		}

		// 上下移動の可否
		if (mInput->GetPushKey(DIK_W)) {
			mDirection.z += 1.0f;
		}
		if (mInput->GetPushKey(DIK_S)) {
			mDirection.z -= 1.0f;
		}
		// 左右移動の可否
		if (mInput->GetPushKey(DIK_A)) {
			mDirection.x -= 1.0f;
		}
		if (mInput->GetPushKey(DIK_D)) {
			mDirection.x += 1.0f;
		}

		// スティック入力の量
		const static int stickValue = 6000;
		// いずれかの数値が、以上(以下)であれば移動処理を行う
		if (mInput->GetStick(Gamepad::Stick::LEFT_X) < -stickValue || // 左 
			mInput->GetStick(Gamepad::Stick::LEFT_X) > stickValue || // 右
			mInput->GetStick(Gamepad::Stick::LEFT_Y) < -stickValue || // 上
			mInput->GetStick(Gamepad::Stick::LEFT_Y) > stickValue	  // 下
			) {

			// Xの移動量とYの移動量を設定する
			mDirection = {
				(float)mInput->GetStick(Gamepad::Stick::LEFT_X) ,
				0.0f,
				(float)mInput->GetStick(Gamepad::Stick::LEFT_Y)
			};

			// 正規化
			if (Length(mDirection) != 0.0f) {
				mDirection = Normalize(mDirection);
			}

			// 移動速度を設定
			float moveSpeed = 0.10f;
			// ガード中は移動速度を減少
			if (mBehavior == Behavior::kGuard) {
				moveSpeed = 0.03f;
			}

			// カメラの回転量を反映
			mDirection = TransformNomal(mDirection, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix());
			// y座標は移動しない
			mDirection.y = 0.0f;

			// 平行移動を行う
			mObject->mWorldTransform->translation += mDirection * moveSpeed;

			// ここから回転処理
			const float PI = 3.14f;
			float rotateY = std::atan2f(mDirection.x, mDirection.z);
			//rotateY = std::fmodf(rotateY, 2.0f * PI);
			if (rotateY > PI) {
				rotateY -= 2.0f * PI;
			}
			if (rotateY < -PI) {
				rotateY += 2.0f * PI;
			}
			mObject->mWorldTransform->rotation.y = rotateY;

			// 移動状態に変更
			if (mBehavior == Behavior::kRoot) {
				mBehavior = Behavior::kMove;
				
			}

		}

		if (mInput->GetStick(Gamepad::Stick::LEFT_X) >= -stickValue && // 左 
			mInput->GetStick(Gamepad::Stick::LEFT_X) <= stickValue && // 右
			mInput->GetStick(Gamepad::Stick::LEFT_Y) >= -stickValue && // 上
			mInput->GetStick(Gamepad::Stick::LEFT_Y) <= stickValue	  // 下
			) {

			if (mBehavior == Behavior::kMove) {
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
		mVelocity.y -= 9.8f * (1.0f / 360.0f);
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

void Player::DebagCtr()
{
	// 補間数値
	static float t = 0.0f;
	// 勾配
	static float k = 1.0f;
	// 始点と終点
	static Vector3 s = { 0.0f,0.5f,-8.0f };
	static Vector3 e = { 0.0f,1.8f,-26.0f };

	// カメラの回転量を保持
	if (mInput->GetPused(Gamepad::Button::LEFT_SHOULDER) && t == 0.0f) {
		mCameraPreDir = MainCamera::GetInstance()->GetRotate();
	}

	// 狙えるようにカメラの移動 
	if (mInput->GetLongPush(Gamepad::Button::LEFT_SHOULDER)) {

		mIsAimMode = true;
	}
	else {
		mIsAimMode = false;

	}
	if (mIsAimMode) {
		if (t < 1.0f) {
			t += 2.0f / 60.0f;
		}
		else if (t > 1.0f) {
			t = 1.0f;
		}
	}
	else {
		if (t > 0.0f) {
			t -= 3.0f / 60.0f;
		}
		else if (t < 0.0f) {
			t = 0.0f;
		}
	}

	if (t > 0.0f) {
		mIsCameraRotateLock = true;
	}
	else {
		mIsCameraRotateLock = false;
	}

	if (mIsCameraRotateLock) {
		MainCamera::GetInstance()->SetCameraRotateControll(false);
		MainCamera::GetInstance()->SetRotate(Vector3(ExponentialInterpolation(mCameraPreDir.x, 0.55f, t, k), mCameraPreDir.y, mCameraPreDir.z));
	}
	else {
		MainCamera::GetInstance()->SetCameraRotateControll(true);
	}

	// 補間後の数値を計算
	Vector3 cVel = ExponentialInterpolation(s, e, t, k);

	// メインカメラに追加の平行移動値を与える
	MainCamera::GetInstance()->SetAddTranslation(TransformNomal(cVel, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix()));

}

void Player::SpecialAtkRB()
{
	// 入力段階
	if (mBehavior == Behavior::kRoot || mBehavior == Behavior::kMove || mBehavior == Behavior::kCharge) {

		// 突進攻撃(仮)
		if (mInput->GetGamepad()->getButton(Gamepad::Button::RIGHT_SHOULDER)) {
			mChargeStatus.pushingFrame += 1.0f / 60.0f;

			// 入力時間が一定まで達していたら 攻撃方向を指定できるようにする
			if (mChargeStatus.pushingFrame >= 1.0f)
			{
				if (mBehavior == Behavior::kRoot || mBehavior == Behavior::kMove)
				{
					mBehavior = Behavior::kCharge;
				}
				
				if (mBehavior == Behavior::kCharge) {
					if (mObject->mSkinning->GetNowSkinCluster()->name != "prepare" &&
						!mObject->mSkinning->SearchToWaitingSkinCluster("prepare")) {
						// 溜めモーションを切り替える
						mObject->mSkinning->SetMotionBlendingInterval(10.0f);
						mObject->mSkinning->SetNextAnimation("prepare");
					}
				}
				// 溜めフラグをtrueに
				mChargeStatus.isCastMode = true;

				// 右スティックで方向を指定
				const static int stickValue = 6000;
				// いずれかの数値が、以上(以下)であれば移動処理を行う
				if (mInput->GetStick(Gamepad::Stick::RIGHT_X) < -stickValue || // 左 
					mInput->GetStick(Gamepad::Stick::RIGHT_X) > stickValue || // 右
					mInput->GetStick(Gamepad::Stick::RIGHT_Y) < -stickValue || // 上
					mInput->GetStick(Gamepad::Stick::RIGHT_Y) > stickValue	  // 下
					) {

					// Xの移動量とYの移動量を設定する
					mDirection = {
						(float)mInput->GetStick(Gamepad::Stick::RIGHT_X) ,
						0.0f,
						(float)mInput->GetStick(Gamepad::Stick::RIGHT_Y)
					};

					// 正規化
					if (Length(mDirection) != 0.0f) {
						mDirection = Normalize(mDirection);
					}

					// カメラの回転量を反映
					//mDirection = TransformNomal(mDirection, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix());
					// y座標は移動しない
					mDirection.y = 0.0f;

					// ここから回転処理
					const float PI = 3.14f;
					float rotateY = std::atan2f(mDirection.x, mDirection.z);
					rotateY = std::fmodf(rotateY, 2.0f * PI);
					if (rotateY > PI) {
						rotateY -= 2.0f * PI;
					}
					if (rotateY < -PI) {
						rotateY += 2.0f * PI;
					}
					mObject->mWorldTransform->rotation.y = rotateY;

				}


			}

			// カメラの回転操作を停止
			MainCamera::GetInstance()->SetCameraRotateControll(false);

		}

		// 離すと攻撃する
		if (mChargeStatus.pushingFrame > 0.0f && mInput->GetReleased(Gamepad::Button::RIGHT_SHOULDER)) {

			// ちゃんと攻撃モーションが発生するようにする
			if (mObject->mSkinning->GetIsMotionbrending()) {
				mObject->mSkinning->EndMotionBrend();
			}

			// 溜めフラグの解除
			mBehavior = Behavior::kChargeAttack;
			// 攻撃フラグをtrueにする
			mChargeStatus.isCharge = true;

			// 始点の設定
			mChargeStatus.startPos = mObject->mWorldTransform->translation;
			// 初期回転量
			mChargeStatus.cameraStartRot = MainCamera::GetInstance()->GetRotate();
			mChargeStatus.cameraEndRot = { mChargeStatus.cameraStartRot.x,mDirection.y,mChargeStatus.cameraStartRot.z };
			// カメラの回転操作を停止
			MainCamera::GetInstance()->SetCameraRotateControll(false);

			// 正規化
			if (Length(mDirection) != 0.0f) {
				mDirection = Normalize(mDirection);
			}
			else {

				mDirection;
			}
			
			// カメラの回転量を反映
			//mDirection = TransformNomal(mDirection, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix());
			// y座標は移動しない
			mDirection.y = 0.0f;
			// 終点の設定
			mChargeStatus.attackRange = 14.0f;
			mChargeStatus.endPos = mObject->mWorldTransform->translation + Scalar(mChargeStatus.attackRange, mDirection);

			mChargeStatus.moveingTime = 0.0f;

			// モーションを切り替える(+モーションブレンド切替速度を早く)
			mObject->mSkinning->SetMotionBlendingInterval(10.0f);
			if  (mObject->mSkinning->GetNowSkinCluster()->name != "gatotu0" &&
				!mObject->mSkinning->SearchToWaitingSkinCluster("gatotu0")) {
				mObject->mSkinning->SetNextAnimation("gatotu0");
			}
		}


		// 何かしらのボタンでキャンセル

	}

	// 攻撃段階
	else if (mBehavior == Behavior::kChargeAttack) {

		// 線形補間を利用して座標を移動させる
		if (mChargeStatus.moveingTime < 1.0f) {

			// カメラ回転
			MainCamera::GetInstance()->SetRotate(ExponentialInterpolation(mChargeStatus.cameraStartRot, mChargeStatus.cameraEndRot, mChargeStatus.moveingTime, 1.0f));

			// 移動
			mChargeStatus.moveingTime += 1.0f / 30.0f;
			mObject->mWorldTransform->translation.x = (1.0f - mChargeStatus.moveingTime) * mChargeStatus.startPos.x + mChargeStatus.moveingTime * mChargeStatus.endPos.x;
			mObject->mWorldTransform->translation.z = (1.0f - mChargeStatus.moveingTime) * mChargeStatus.startPos.z + mChargeStatus.moveingTime * mChargeStatus.endPos.z;

		}
		else if (mChargeStatus.moveingTime >= 1.0f) {
			// タイムを補正
			mChargeStatus.moveingTime = 1.0f;

			// 攻撃フラグをfalseにする
			mChargeStatus.isCharge = false;

			// 移動終了
			mBehavior = Behavior::kRoot;
			mObject->mSkinning->SetMotionBlendingInterval(2.0f);
			
			// カメラの回転操作をON
			MainCamera::GetInstance()->SetCameraRotateControll(false);

		}
	}


}

void Player::SpecialAtkRT()
{

}

void Player::SpecialAtkLB()
{

}

void Player::SpecialAtkLT()
{

}

Arrow* Player::CreateArrow(Vector3 startPos, Vector3 endPos)
{
	// アローの進行方向を設定し、正規化しておく
	Vector3 directionForArrow = endPos - startPos;
	directionForArrow = Normalize(directionForArrow);
	Vector3 vel = Scalar(1.0f, directionForArrow);

	// 回転
	Vector3 rotate = { 0.0f,0.0f,0.0f };
	const float PI = 3.14f;
	// y軸回りの回転
	rotate.y = std::atan2f(directionForArrow.x, directionForArrow.z);

	// x軸回りの回転
	rotate.x = -std::atan2f(directionForArrow.y, directionForArrow.z);

	// 回転量が -PI から PI の範囲に収まるように補正
	rotate.y = std::fmod(rotate.y + PI, 2.0f * PI) - PI;
	rotate.x = std::fmod(rotate.x + PI, 2.0f * PI) - PI;

	// アロークラスの宣言と初期化を行う(上記で求めた移動方向と始点の座標を代入)
	Arrow* arrow = new Arrow();
	arrow->Init(this, startPos, vel);
	arrow->SetRotate(rotate);
	arrow->SetCubeMap(mObject->GetModel()->mTextureHandleCubeMap);

	return arrow;
}

void Player::ReciveDamageToBoss(float power)
{
	// ボスにダメージを与える
	StatusManager::GetInstance()->ReceiveDamage(mStatus, power, mBoss->GetStatus());
}
