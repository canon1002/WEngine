#include "Action.h"
#include "App/Actor/Actor.h"
#include "GameEngine/GameMaster/Framerate.h"

void ACT::MoveToPlayer::Init(Actor* actor)
{
	// ボスのポインタを取得
	mActor = actor;
	
	// 移動の始点
	mStartPos = mActor->GetWorldPos();
	// 移動の終点
	mEndPos = Vector3(0,0,0);
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / Framerate::GetInstance()->GetFramerate()) * 4.0f * Framerate::GetInstance()->GetBattleSpeed();
	// 移動量の設定
	mVelocity = Scalar(mMoveSpeed, mDirection);
	// 追跡までの時間をリセット
	mSearchTime = 0.0f;
	// 追跡を行う時間
	mSearchCount = 0.0f;

	// 初期化する
	mCondition = Condition::IDOL;

}

void ACT::MoveToPlayer::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {
		
		// 移動させる
		mActor->AddTransform(mVelocity);

		// 追跡時間を加算する
		if (mSearchCount < kSearchCountMax) {
			mSearchCount += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();
		}

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
		mSearchTime += (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetBattleSpeed();
		if (mSearchTime>= kSearchCycle) {
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

		// 終了処理(攻撃範囲内に近づけた場合 または 一定時間が経過した場合)
		if (mActor->InvokeNearDistance() || mSearchCount > kSearchCountMax) {
			mCondition = Condition::FINISHED;
		}
	}
}

void ACT::MoveToPlayer::Draw()
{
}

void ACT::MoveToPlayer::Start()
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
	// 追跡を行う時間をリセット
	mSearchCount = 0.0f;

	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Walk");

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::MoveToPlayer::End()
{
	// アニメーションの変更
	//mActor->GetObject3D()->mSkinning->SetNextAnimation("Idle");
	

	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::MoveToPlayer::Reset()
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

void ACT::MoveToPlayer::Save()
{
}

void ACT::MoveToPlayer::Load()
{
}

void ACT::MoveToPlayer::DrawGui()
{
}

void ACT::BackStep::Init(Actor* actor)
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
	mMoveSpeed = 6.0f / (Framerate::GetInstance()->GetFramerate() * Framerate::GetInstance()->GetBattleSpeed());
	// 移動量の設定
	mVelocity = Scalar(-mMoveSpeed, mDirection);

	// 初期化する
	mCondition = Condition::IDOL;

}

void ACT::BackStep::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {
		// 移動させる // XZ座標のみ逆方向にする
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

		// 終了処理
		if (mActor->GetObject3D()->mSkinning->GetIsAnimationFinished("backStep")) {
			mCondition = Condition::FINISHED;
		}
	}
}

void ACT::BackStep::Draw()
{
}

void ACT::BackStep::Start()
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
	mMoveSpeed = 6.0f / (Framerate::GetInstance()->GetFramerate() * Framerate::GetInstance()->GetBattleSpeed());
	// 移動量の設定
	mVelocity = Scalar(-mMoveSpeed, mDirection);

	// 少しジャンプさせる
	//mActor->Jump(1.0f);

	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("backStep");
	//mActor->GetObject3D()->mSkinning->SetMotionBlendingInterval(30.0f);

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::BackStep::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;

	// アニメーション変更
	//mActor->GetObject3D()->mSkinning->SetNextAnimation("Idle");
	//mActor->GetObject3D()->mSkinning->SetMotionBlendingInterval(10.0f);

}

void ACT::BackStep::Reset()
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
	mVelocity = Scalar(-mMoveSpeed, mDirection);
}

void ACT::BackStep::Save()
{
}

void ACT::BackStep::Load()
{
}

void ACT::BackStep::DrawGui()
{
}
