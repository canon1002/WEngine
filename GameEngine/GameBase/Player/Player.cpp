#include "Player.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Append/Collider/AABBCollider.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"
#include "GameEngine/GameBase/Reaction/DamageReaction.h"


Player::~Player() {

	for (auto& arrow : mArrows) {
		delete arrow;
	}
}

void Player::Init() {
	mObject = std::make_unique<Object3d>();
	mObject->Init("PlayerObj");
	mObject->SetModel("walk.gltf");
	mObject->mSkinning = new Skinnig();
	mObject->mSkinning->Init("human", "walk.gltf",
		mObject->GetModel()->modelData);
	mObject->SetTranslate({ 1.0f,1.0f,7.0f });

	// メインカメラをフォローカメラ仕様にする
	MainCamera::GetInstance()->SetTarget(mObject->GetWorldTransform());

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
	mAttackStatus.sword->mWorldTransform->scale = { 0.1f,0.1f,0.1f };
	mAttackStatus.sword->mWorldTransform->rotation = { 1.7f,-0.3f,0.0f };
	mAttackStatus.sword->mWorldTransform->translation = mObject->GetWorldTransform()->translation;
	mAttackStatus.pos = { 1000.0f,900.0f,0.0f };// 初期位置
	mAttackStatus.normalRot = { 1.7f,-0.3f,-0.2f };// 初期回転量
	mAttackStatus.endRot = { 3.8f,-0.3f,1.0f };// 最終回転量

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
		Avoid();

		// 防御関連処理
		Guard();

		// 攻撃関連処理
		Attack();

		// デバッグ操作
		DebagCtr();

		// レティクル 更新
		mReticle->Update();

	}
	// プレイヤーの弾 -- 更新 --
	auto arrowIt = mArrows.begin();  // イテレータを初期化
	while (arrowIt != mArrows.end()) {
		const auto& arrow = *arrowIt;
		// Bulletが消失条件を満たす場合
		if (!arrow->GetIsActive()) {
			arrowIt = mArrows.erase(arrowIt);  // erase()は削除された要素の次の有効なイテレータを返す
		}
		else {
			arrow->Update();
			arrow->DebugDraw();
			++arrowIt;  // イテレータを次に進める
		}
	}


	// オブジェクト更新
	mObject->Update();
	mObject->mSkinning->GetSkeleton().joints;
	mObject->mCollider->Update();
	// 剣
	mAttackStatus.sword->Update();
	// シールド
	mGuardStatus.shield->Update();

	// UI更新
	mStatus->Update();

}

void Player::Draw() {
	//mObject->Draw();

	for (const auto& arrow : mArrows) {
		arrow->Draw();
	}

}

void Player::DrawGUI() {
#ifdef _DEBUG

	// メニューバーを表示する
	ImGui::Begin("Player",nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {

		//mObject->DrawGuiTree();
		// プレイヤー ステータス
		if (ImGui::BeginMenu("Status")) {
			ImGui::DragInt("HP", &mStatus->HP, 1.0f, 0, 100);
			ImGui::DragInt("STR", &mStatus->STR, 1.0f, 0, 100);
			ImGui::DragInt("VIT", &mStatus->VIT, 1.0f, 0, 100);
			ImGui::DragInt("AGI", &mStatus->AGI, 1.0f, 0, 100);
			ImGui::EndMenu();
		}

		// 回避パラメータ
		if (ImGui::BeginMenu("Avoid")) {
			ImGui::DragFloat("AvoidRange", &mAvoidStatus.mAvoidRange);
			ImGui::DragFloat("AvoidSpeed", &mAvoidStatus.mAvoidSpeed);
			ImGui::DragFloat("AvoidTime", &mAvoidStatus.mAvoidTime);
			ImGui::DragFloat3("Avoid Start", &mAvoidStatus.mAvoidMoveStartPos.x);
			ImGui::DragFloat3("Avoid End", &mAvoidStatus.mAvoidMoveEndPos.x);
			ImGui::EndMenu();
		}

		// 防御パラメータ
		if (ImGui::BeginMenu("Guard")) {
			ImGui::DragFloat3("NormalPos", &mGuardStatus.normalPos.x, 1.0f, -1000.0f, 1000.0f);
			ImGui::DragFloat3("GuradPos", &mGuardStatus.guardPos.x, 1.0f, -1000.0f, 1000.0f);
			ImGui::DragFloat3("Pos", &mGuardStatus.pos.x, 0.05f, -10.0f, 10.0f);
			mGuardStatus.shield->DrawGuiTree();
			ImGui::EndMenu();
		}

		// 攻撃パラメータ
		if (ImGui::BeginMenu("Attack")) {
			ImGui::DragFloat("t", &mAttackStatus.t, 1.0f, -1000.0f, 1000.0f);
			ImGui::DragFloat3("Pos", &mAttackStatus.pos.x, 1.0f, -1000.0f, 1000.0f);
			ImGui::DragFloat3("normalRot", &mAttackStatus.normalRot.x, 0.01f, -6.28f, 6.28f);
			ImGui::DragFloat3("EndRot", &mAttackStatus.endRot.x, 0.01f, -6.28f, 6.28f);
			mAttackStatus.sword->DrawGuiTree();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
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
	// シールド
	mGuardStatus.shield->Draw();

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

		cManager->SetCollider(mReticle->GetReticleCollider());
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
	// 動作中でない場合のみキー入力を行う
	if (mAttackStatus.isUp == false &&
		mAttackStatus.isDown == false &&
		mAttackStatus.isOperating == false) {
	
		// Bボタン Triggerで攻撃
		if (mInput->GetPused(Gamepad::Button::B)) {
			mAttackStatus.isUp = true;
		}

	}

	// 数値上昇中の場合
	if (mAttackStatus.isUp) {
		// tを増加させる
		if (mAttackStatus.t < 1.0f) {
			mAttackStatus.t += 5.0f / 60.0f;

			// 攻撃が命中していない かつ tが0.1f以上であれば攻撃フラグをtrueにする
			if (mAttackStatus.t >= 0.1f && !mAttackStatus.isHit && !mAttackStatus.isOperating) {
				mAttackStatus.isOperating = true;
			}
			// tが最大値の1.0fを超過したら今度は減少させる
			if (mAttackStatus.t >= 1.0f) {
				mAttackStatus.isUp = false;
				mAttackStatus.isDown = true;
			}
		}
	}

	// 数値減少中の場合
	if (mAttackStatus.isDown) {
		// tを減少させる
		if (mAttackStatus.t > 0.0f) {
			mAttackStatus.t -= 4.0f / 60.0f;

			// 攻撃が命中していない かつ tが0.7f未満であれば攻撃フラグをfalseにする
			if (mAttackStatus.t <= 0.7f && mAttackStatus.isOperating) {
				mAttackStatus.isOperating = false;
			}
			// tが最小値の0.0f以下になったらフラグを解除
			if (mAttackStatus.t <= 0.0f) {
				mAttackStatus.isUp = false;
				mAttackStatus.isDown = false;
				mAttackStatus.isOperating = false;
				mAttackStatus.isHit = false;
			}
		}
	}

	// 剣の位置を更新(2D配置→ワールド座標に変換)
	mAttackStatus.sword->mWorldTransform->translation = DamageReaction::GetWorldPosForScreen(Vector2(mAttackStatus.pos.x, mAttackStatus.pos.y), 1.0f, MainCamera::GetInstance());

	// 回転量をtの数値に応じて設定する
	mAttackStatus.sword->mWorldTransform->rotation = ExponentialInterpolation(mAttackStatus.normalRot, mAttackStatus.endRot, mAttackStatus.t, 1.0f);
	mAttackStatus.sword->mWorldTransform->rotation.y += MainCamera::GetInstance()->GetRotate().y;

	// レティクルの位置(コライダー)をtの値に応じて移動させる
	mReticle->SetReticleDistance(ExponentialInterpolation(0.5f,10.0f, mAttackStatus.t, 1.0f));

	// 衝突時の処理
	if (mReticle->GetReticleCollider()->GetOnCollisionFlag()) {
		if (mAttackStatus.isOperating == true && mAttackStatus.isHit == false) {
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

void Player::Move()
{
	// 通常/防御時に有効
	if (mBehavior == Behavior::kRoot || mBehavior == Behavior::kGuard) {

		// スティック入力の量
		const static int stickValue = 6000;
		// いずれかの数値が、以上(以下)であれば移動処理を行う
		if (mInput->GetStick(Gamepad::Stick::LEFT_X) < -stickValue || // 左 
			mInput->GetStick(Gamepad::Stick::LEFT_X) > stickValue || // 右
			mInput->GetStick(Gamepad::Stick::LEFT_Y) < -stickValue || // 上
			mInput->GetStick(Gamepad::Stick::LEFT_Y) > stickValue	  // 下
			) {

			// Xの移動量とYの移動量を設定する
			Vector3 direction = {
				(float)mInput->GetStick(Gamepad::Stick::LEFT_X) ,
				0.0f,
				(float)mInput->GetStick(Gamepad::Stick::LEFT_Y)
			};
			// 正規化
			direction = Normalize(direction);

			// 移動速度を設定
			float moveSpeed = 0.10f;
			// ガード中は移動速度を減少
			if (mBehavior == Behavior::kGuard) {
				moveSpeed = 0.03f;
			}

			// カメラの回転量を反映
			direction = TransformNomal(direction, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix());
			// y座標は移動しない
			direction.y = 0.0f;

			// 平行移動を行う
			mObject->mWorldTransform->translation += direction * moveSpeed;

			// ここから回転処理
			const float PI = 3.14f;
			float rotateY = std::atan2f(direction.x, direction.z);
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
	static Vector3 s = { 0.0f,0.0f,0.0f };
	static Vector3 e = { 0.0f,0.5f,-8.0f };

	// 狙えるようにカメラの移動 
	if (mInput->GetLongPush(Gamepad::Button::LEFT_SHOULDER)) {
		mIsAimMode = true;
	}
	else {
		mIsAimMode = false;
	}
	if (mIsAimMode) {
		if (t < 1.0f) {
			t += 3.0f / 60.0f;
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
	// 補間後の数値を計算
	Vector3 cVel = ExponentialInterpolation(s, e, t, k);

	// メインカメラに追加の平行移動値を与える
	MainCamera::GetInstance()->SetAddTranslation(TransformNomal(cVel, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix()));

}

void Player::Charge()
{
	// [ため動作中]に状態を変更
	mBehavior = Behavior::kCharge;

	// 設定したボタンが長押しされている間、ため動作を行う
	mChargeCount++;

}

void Player::ChengeChageToAttack()
{
	// [攻撃動作中]に状態を変更
	mBehavior = Behavior::kAttack;
	// ため動作の継続時間リセット
	mChargeCount = 0;

	// アローの生成
	Vector3 startPos = this->GetWorldPos();// 移動の始点
	Vector3 endPos = mReticle->GetWorld3D();// 移動の終点

	// 初期位置をずらし、アローの着弾位置をずらす
	for (int32_t i = 0; i < 3; i++) {
		Vector3 randomStartPos = startPos;
		randomStartPos.x += (rand() % (i * 2 + 2)) * 0.25f;
		randomStartPos.y += (rand() % (i + 2)) * 0.25f;
		// 複数のアローを生成し、アロークラスの配列に要素を追加する
		mArrows.push_back(CreateArrow(randomStartPos, endPos));
	}

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
