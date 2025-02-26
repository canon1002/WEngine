#include "Action.h"
#include "App/Actor/Actor.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "App/BlackBoard.h"

void ACT::ChaseTarget::Init(Actor* actor)
{
	// ボスのポインタを取得
	mActor = actor;
	// パラメータ 初期化
	Reset();

}

void ACT::ChaseTarget::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {
		
		// 移動させる
		mActor->AddTransform(mVelocity);

		// 追跡時間を加算する
		if (mSearchCount < mSearchCountMax) {
			mSearchCount += BlackBoard::GetBattleFPS();
		}

		// 一定時間ごとにプレイヤー座標を取得
		mRotateTime += BlackBoard::GetBattleFPS();
		if (mRotateTime>= mRotateCycle) {
			// 移動の終点
			mEndPos = mActor->GetWorldPosForTarget();
			mEndPos.y = 0.0f;
			// 方向の設定
			mDirection = Normalize(mEndPos - mActor->GetWorldPos());
			mDirection.y = 0.0f;
			// 向きの指定
			mActor->InputDirection(mDirection);
			// 移動量の設定
			mVelocity = Scalar(mMoveSpeedInFrame, mDirection);

			// 追跡までの時間をリセット
			mRotateTime = 0.0f;
		}

		// 終了処理(攻撃範囲内に近づけた場合 または 一定時間が経過した場合)
		if (mActor->InvokeNearDistance() || mSearchCount > mSearchCountMax) {
			mCondition = Condition::FINISHED;
		}
	}
}

void ACT::ChaseTarget::Draw()
{
}

void ACT::ChaseTarget::Start()
{
	// パラメータ 初期化
	Reset();
	// 向きの指定
	mActor->InputDirection(mDirection);
	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Walk");
	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::ChaseTarget::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::ChaseTarget::Reset()
{
	// jsonから数値を取得
	mMoveSpeed = BlackBoard::GetGlobalVariables()->GetFloatValue("ChaseTarget", "MoveSpeed");
	mSearchCountMax = BlackBoard::GetGlobalVariables()->GetFloatValue("ChaseTarget", "SearchCount");
	mRotateCycle = BlackBoard::GetGlobalVariables()->GetFloatValue("ChaseTarget", "RotateCycle");

	// 移動の始点
	mStartPos = mActor->GetWorldPos();
	// 移動の終点
	mEndPos = mActor->GetWorldPosForTarget();
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;

	// 移動速度の設定
	mMoveSpeedInFrame = BlackBoard::CombertBattleFPS(mMoveSpeed);
	// 移動量の設定
	mVelocity = Scalar(mMoveSpeedInFrame, mDirection);
	// 追跡までの時間をリセット
	mRotateTime = 0.0f;
	// 追跡を行う時間
	mSearchCount = 0.0f;

	// 初期化する
	mCondition = Condition::IDOL;
}

void ACT::ChaseTarget::Save(){

}

void ACT::ChaseTarget::Load()
{
}

void ACT::ChaseTarget::DrawGui()
{
}

void ACT::BackStep::Init(Actor* actor)
{
	// ボスのポインタを取得
	mActor = actor;
	// 初期化
	this->Reset();

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
	// jsonから数値を取得
	mMoveSpeed = BlackBoard::GetGlobalVariables()->GetFloatValue("ChaseTarget", "MoveSpeed");

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
	mMoveSpeedInFrame = BlackBoard::CombertBattleFPS(mMoveSpeed);
	// 移動量の設定
	mVelocity = Scalar(-mMoveSpeedInFrame, mDirection);

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
	// jsonから数値を取得
	mMoveSpeed = BlackBoard::GetGlobalVariables()->GetFloatValue("ChaseTarget", "MoveSpeed");

	// 移動の始点
	mStartPos = mActor->GetWorldPos();
	// 移動の終点
	mEndPos = Vector3(0, 0, 0);
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeedInFrame = BlackBoard::CombertBattleFPS(mMoveSpeed);
	mVelocity = Scalar(-mMoveSpeedInFrame, mDirection);

	// 初期化する
	mCondition = Condition::IDOL;
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
