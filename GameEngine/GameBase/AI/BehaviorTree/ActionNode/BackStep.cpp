#include "BackStep.h"
#include "GameEngine/GameBase/Enemy/BossEnemy.h"

BackStep::BackStep(BossEnemy* boss, std::function<bool()> func) {

	// ボスのポインタを取得
	mBoss = boss;
	// 条件用の関数ポインタをセット
	this->func = func;
	// 移動の始点
	mStartPos = Vector3(0.0f, 0.0f, 0.0f);
	// 移動の終点
	mEndPos = Vector3(0.0f, 0.0f, 0.0f);
	// 方向の設定
	mDirection = Vector3(0.0f, 0.0f, 0.0f);
	// 移動速度の設定
	mMoveSpeed = 0.0f;
	// 移動量の設定
	mVelocity = Vector3(0.0f, 0.0f, 0.0f);
}

void BackStep::Init() {
	// 移動の始点
	mStartPos = mBoss->GetWorldPos();
	// 移動の終点
	mEndPos = mBoss->GetWorldPosForTarget();
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	// 移動速度の設定
	mMoveSpeed = (1.0f / 60.0f) * 2.0f;
	// 移動量の設定
	mVelocity = Scalar(-mMoveSpeed, mDirection);
}

void BackStep::Update() {

	// 移動させる
	mBoss->AddTransform(mVelocity);

	// 移動方向への回転を行う
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

	// 計算結果をBossクラスに渡す
	mBoss->SetRotation(Vector3(0.0f, rotateY, 0.0f));

	if (Length(mEndPos - mStartPos) >= 10.0f) {
		mVelocity = { 0.0f,0.0f,0.0f };
	}

}