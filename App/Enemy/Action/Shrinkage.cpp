#include "Shrinkage.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Editor/BlackBoard.h"

void ACT::Shrinkage::Init(Actor* actor){
	// ボスのポインタを取得
	mActor = actor;
	// パラメータ 初期化
	Reset();
}

void ACT::Shrinkage::Update(){

	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {
		// 移動させる
		mActor->AddTransform(mVelocity);

		// 一定時間ごとにプレイヤー座標を取得
		mRotateTime += BlackBoard::GetBattleFPS();
		if (mRotateTime >= mRotateCycle) {
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

		// 終了処理
		if (mActor->InvokeNearDistance()) {
			mCondition = Condition::FINISHED;
		}
	}
}

void ACT::Shrinkage::Draw(){}

void ACT::Shrinkage::Start(){
	// パラメータ 初期化
	Reset();
	// 向きの指定
	mActor->InputDirection(mDirection);
	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Actor_Walk");
	// 実行する
	mCondition = Condition::RUNNING;

}

void ACT::Shrinkage::End(){
	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::Shrinkage::Reset(){
	// jsonから数値を取得
	mMoveSpeed = BlackBoard::GetGlobalVariables()->GetFloatValue("Shrinkage", "MoveSpeed");
	mRotateCycle = BlackBoard::GetGlobalVariables()->GetFloatValue("Shrinkage", "RotateCycle");

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

	// 初期化する
	mCondition = Condition::IDOL;
}

void ACT::Shrinkage::Save()
{
}

void ACT::Shrinkage::Load()
{
}

void ACT::Shrinkage::DrawGui()
{
}
