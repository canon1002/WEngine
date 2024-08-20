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
	mObject->mCollider->SetAddtranslation(Vector3(0.0f, 0.55f, 0.0f));
	mObject->mCollider->SetCollisionAttribute(kCollisionAttributePlayer);
	mObject->mCollider->SetCollisionMask(kCollisionAttributeEnemyBullet);

	// レティクルの初期化
	mReticle = std::make_unique<Reticle3D>();
	mReticle->Init();

	// 構え
	mIsAimMode = false;

	// 回避行動リクエスト
	mIsAvoidRequest = false;
	// 回避中か
	mIsAvoid = false;
	// 回避時の無敵状態であるか
	mIsAvoidInvincible = false;
	// 回避距離
	mAvoidRange = 10.0f;
	// 回避速度
	mAvoidSpeed = 0.0f;
	// 回避経過時間( 0.0f ~ 1.0f )
	mAvoidTime = 0.0f;
	// 回避開始地点
	mAvoidMoveStartPos = { 0.0f,0.0f,0.0f };
	// 回避終了地点
	mAvoidMoveEndPos = { 0.0f,0.0f,0.0f };

	// ステータス取得
	mStatus = StatusManager::GetInstance()->GetPlayerStatus();
}

void Player::Update() {

	if (mStatus->HP > 0.0f) {

		// 回避行動
		Avoid();

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

			// ジャンプ回数のリセット
			mJumpCount = kJumpCountMax;
		}


		if (mBehavior != Behavior::kAvoid) {

			// ジャンプ
			if (mJumpCount > 0) {
				if (mInput->GetPused(Gamepad::Button::Y)) {
					// 残りジャンプ回数を減らす
					--mJumpCount;
					// 初速度を加算
					mVelocity.y = kJumpFirstSpeed;
					// 状態をジャンプ中に変更
					mBehavior = Behavior::kJump;
				}
			}

			// ジャンプ中
			if (mBehavior == Behavior::kJump) {

				// 移動量を加算
				mObject->mWorldTransform->translation.y += mVelocity.y;

				// 地面を y==0 として地面にめり込んだら地面の上まで戻す
				if (mObject->mWorldTransform->translation.y < 0.0f) {
					mObject->mWorldTransform->translation.y = 0.0f;
					// 移動量も修正しておく 
					mVelocity.y = 0.0f;
					// ジャンプ回数のリセット
					mJumpCount = kJumpCountMax;
					// 通常状態に変更
					mBehavior = Behavior::kRoot;
				}
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
				Vector3 direction = {
					(float)mInput->GetStick(Gamepad::Stick::LEFT_X) ,
					0.0f,
					(float)mInput->GetStick(Gamepad::Stick::LEFT_Y)
				};
				// 正規化
				direction = Normalize(direction);

				// 移動速度を設定
				float moveSpeed = 0.15f;

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

		// レティクル 更新
		mReticle->Update();

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
#ifdef _DEBUG

		ImGui::Begin("AddCamera");
		ImGui::DragFloat3("Start", &s.x);
		ImGui::DragFloat3("End", &e.x);
		ImGui::DragFloat("t", &t);
		ImGui::DragFloat("k", &k);
		ImGui::DragFloat3("cVel", &cVel.x);
		ImGui::End();

#endif // _DEBUG

		// メインカメラに追加の平行移動値を与える
		MainCamera::GetInstance()->SetAddTranslation(TransformNomal(cVel, MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix()));

		// RBボタン長押しでため動作を行う
		if (mInput->GetLongPush(Gamepad::Button::RIGHT_SHOULDER)) {
			Charge();
		}
		// ボタンを離したら攻撃する
		if (mInput->GetReleased(Gamepad::Button::RIGHT_SHOULDER)) {
			ChengeChageToAttack();
		}
		if (mBehavior == Behavior::kAttack) {
			Attack();
			mBehavior = Behavior::kRoot;
		}
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
}

void Player::Draw() {
	mObject->Draw();

	for (const auto& arrow : mArrows) {
		arrow->Draw();
	}

}

void Player::DrawGUI() {
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::DragFloat("HP", &mStatus->HP, 1.0f, 0.0, 100.0f);
	ImGui::DragFloat("STR", &mStatus->STR, 1.0f, 0.0, 100.0f);
	ImGui::DragFloat("VIT", &mStatus->VIT, 1.0f, 0.0, 100.0f);
	ImGui::DragFloat("AGI", &mStatus->AGI, 1.0f, 0.0, 100.0f);
	ImGui::DragFloat("AvoidRange", &mAvoidRange);
	ImGui::DragFloat("AvoidSpeed", &mAvoidSpeed);
	ImGui::DragFloat("AvoidTime", &mAvoidTime);
	ImGui::DragFloat3("Avoid Start", &mAvoidMoveStartPos.x);
	ImGui::DragFloat3("Avoid End", &mAvoidMoveEndPos.x);
	mObject->DrawGuiTree();
	ImGui::End();

#endif // _DEBUG


}

void Player::ColliderDraw() {
#ifdef _DEBUG
	mObject->mCollider->Draw();
#endif // _DEBUG
	mReticle->Draw3DReticle();

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

void Player::Avoid()
{
	//  非回避状態で Aボタンで回避
	if (mBehavior != Behavior::kAvoid &&
		mInput->GetLongPush(Gamepad::Button::A)) {

		// パラメータの補正

		// 回避距離
		mAvoidRange = 10.0f;
		// 回避速度
		mAvoidSpeed = 2.0f;
		// 回避経過時間( 0.0f ~ 1.0f )
		mAvoidTime = 0.0f;
		// 回避開始地点に現在の座標を代入
		mAvoidMoveStartPos = mObject->GetWorldTransform()->translation;


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

			mAvoidMoveEndPos = mObject->mWorldTransform->translation + Scalar(mAvoidRange, direction);

			// 回避状態に変更
			mBehavior = Behavior::kAvoid;

		}
		// スティック入力がなければバックステップを行う
		else {

			Vector3 direction = TransformNomal(Vector3(0.0f, 0.0f, -1.0f), MainCamera::GetInstance()->mWorldTransform->GetWorldMatrix());
			// y座標は移動しない
			direction.y = 0.0f;
			mAvoidMoveEndPos = mObject->mWorldTransform->translation + Scalar(mAvoidRange, direction);
			// 回避状態に変更
			mBehavior = Behavior::kAvoid;

		}

	}

	// 回避中の処理
	if (mBehavior == Behavior::kAvoid) {

		// 線形補間を利用して座標を移動させる
		if (mAvoidTime < 1.0f) {
			// 回避時の移動速度/フレームレートで加算する
			mAvoidTime += mAvoidSpeed / 60.0f;
		}
		else if (mAvoidTime >= 1.0f) {
			// タイムを補正
			mAvoidTime = 1.0f;

			// 移動終了
			mBehavior = Behavior::kRoot;
		}

		// 移動
		mObject->mWorldTransform->translation.x = (1.0f - mAvoidTime) * mAvoidMoveStartPos.x + mAvoidTime * mAvoidMoveEndPos.x;
		mObject->mWorldTransform->translation.z = (1.0f - mAvoidTime) * mAvoidMoveStartPos.z + mAvoidTime * mAvoidMoveEndPos.z;


	}


}

void Player::Attack()
{


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
