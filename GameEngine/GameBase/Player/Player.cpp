#include "Player.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"

#include "GameEngine/Append/Collider/AABBCollider.h"

Player::~Player(){

	for (auto& arrow : mArrows) {
		delete arrow;
	}
}

void Player::Init(){
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
	mObject->mCollider = new AABBCollider(mObject->mWorldTransform, Vector3(0.5f, 0.5f, 0.5f));
	mObject->mCollider->Init();

	// レティクルの初期化
	mReticle = std::make_unique<Reticle3D>();
	mReticle->Init();

}

void Player::Update(){

	// RBボタンを押してたら歩く
	//if (mInput->GetPused(Gamepad::Button::RIGHT_SHOULDER)) {
	//	mObject->mSkinning->Init("human", "walk.gltf",
	//		mObject->GetModel()->modelData);
	//}

	//// RBを離したらスニーク
	//if (mInput->GetReleased(Gamepad::Button::RIGHT_SHOULDER)) {
	//	mObject->mSkinning->Init("human", "sneakWalk.gltf",
	//		mObject->GetModel()->modelData);
	//}

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

		mVelocity.y -= 9.8f * (1.0f / 360.0f);
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
		// 念のために正規化
		direction = Normalize(direction);

		// 移動速度を設定
		float moveSpeed = 0.05f;
		// RB入力時、移動速度を上げる
		if (mInput->GetLongPush(Gamepad::Button::RIGHT_SHOULDER)) {
			moveSpeed *= 2.0f;
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

	mReticle->Update();

	// プレイヤー攻撃処理
	if (mInput->GetKey()->GetTriggerKey(DIK_RETURN) || mInput->GetPused(Gamepad::Button::X)) {
		Vector3 startPos = this->GetWorldPos();// 移動の始点
		Vector3 endPos = mReticle->GetWorld3D();// 移動の終点

		// アローの進行方向を設定し、正規化しておく
		Vector3 directionForArrow = endPos - startPos;
		directionForArrow = Normalize(directionForArrow);
		Vector3 vel = Scalar(1.0f, directionForArrow);

		//回転
		Vector3 rotate = { 0.0f,0.0f,0.0f };
		const float PI = 3.14f;
		rotate.y = std::atan2f(directionForArrow.x, directionForArrow.z);
		rotate.y = std::fmodf(rotate.y, 2.0f * PI);
		// 回転量が超過していたり、下限を下回っていたら補正する
		if (rotate.y > PI) { rotate.y -= 2.0f * PI; }
		if (rotate.y < -PI) { rotate.y += 2.0f * PI; }
		rotate.x = -std::atan2f(directionForArrow.y, directionForArrow.z);
		rotate.x = std::fmodf(rotate.x, 2.0f * PI);
		// 回転量が超過していたり、下限を下回っていたら補正する
		if (rotate.x > PI) { rotate.y -= 2.0f * PI; }
		if (rotate.x < -PI) { rotate.y += 2.0f * PI; }
	
		// アロークラスの宣言と初期化を行う(上記で求めた移動方向と始点の座標を代入)
		Arrow* arrow = new Arrow();
		arrow->Init(startPos, vel);
		arrow->SetRotate(rotate);
		arrow->SetCubeMap(mObject->GetModel()->mTextureHandleCubeMap);
		// アロークラスの配列に要素を追加する
		mArrows.push_back(arrow);

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

void Player::Draw(){
	mObject->Draw();

	for (const auto& arrow : mArrows) {
		arrow->Draw();
	}

}

void Player::DrawGUI(){
	mObject->DrawGUI();
}

void Player::ColliderDraw(){
	mObject->mCollider->Draw();
	mReticle->Draw3DReticle();
}
