#include "Seartch.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Editor/BlackBoard.h"

void ACT::Seartch::Init(Actor* actor)
{
	// ボスのポインタを取得
	mActor = actor;

	// 移動の始点
	mStartPos = mActor->GetWorldPos();
	// 移動の終点
	mEndPos = Vector3(0, 0, 0);
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / Framerate::GetInstance()->GetFramerate()) * 4.0f * Framerate::GetInstance()->GetBattleSpeed();
	// 移動量の設定
	mVelocity = Scalar(mMoveSpeed, mDirection);
	// 追跡までの時間をリセット
	mSearchTime = 0.0f;

	// 初期化する
	mCondition = Condition::IDOL;

}

void ACT::Seartch::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {
		// 移動させる
		mActor->AddTransform(mVelocity);

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
		mActor->SetRotation(Vector3(0.0f, rotateY, 0.0f));

		// 一定時間ごとにプレイヤー座標を取得
		mSearchTime += BlackBoard::GetBattleFPS();
		if (mSearchTime >= kSearchCycle) {
			// 移動の終点
			mEndPos = mActor->GetWorldPosForTarget();
			mEndPos.y = 0.0f;
			// 方向の設定
			mDirection = Normalize(mEndPos - mActor->GetWorldPos());
			mDirection.y = 0.0f;
			// 移動速度の設定
			mMoveSpeed = (1.0f / Framerate::GetInstance()->GetFramerate()) * 8.0f;
			// 移動量の設定
			mVelocity = Scalar(mMoveSpeed, mDirection);

			// 追跡までの時間をリセット
			mSearchTime = 0.0f;
		}

		// 終了処理
		if (mActor->InvokeNearDistance()) {
			mCondition = Condition::FINISHED;
		}
	}
}

void ACT::Seartch::Draw()
{
}

void ACT::Seartch::Start()
{
	// パラメータの初期化

	// 移動の始点
	mStartPos = mActor->GetWorldPos();
	mStartPos.y = 0.0f;
	// 移動の終点
	mEndPos = mActor->GetWorldPosForTarget();
	mEndPos.y = 0.0f;
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / Framerate::GetInstance()->GetFramerate()) * 4.0f * Framerate::GetInstance()->GetBattleSpeed();
	// 移動量の設定
	mVelocity = Scalar(mMoveSpeed, mDirection);
	// 追跡までの時間をリセット
	mSearchTime = 0.0f;

	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Run");

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::Seartch::End()
{
	// アニメーションの変更
	//mActor->GetObject3D()->mSkinning->SetNextAnimation("Idle");


	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::Seartch::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;

	// パラメータの初期化

	// 移動の始点
	mStartPos = mActor->GetWorldPos();
	mStartPos.y = 0.0f;
	// 移動の終点
	mEndPos = mActor->GetWorldPosForTarget();
	mEndPos.y = 0.0f;
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / Framerate::GetInstance()->GetFramerate()) * 2.0f * Framerate::GetInstance()->GetBattleSpeed();
	// 移動量の設定
	mVelocity = Scalar(mMoveSpeed, mDirection);
}

void ACT::Seartch::Save()
{
}

void ACT::Seartch::Load()
{
}

void ACT::Seartch::DrawGui()
{
}
