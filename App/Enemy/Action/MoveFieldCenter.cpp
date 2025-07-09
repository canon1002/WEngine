#include "MoveFieldCenter.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Editor/BlackBoard.h"

void ACT::MoveFieldCenter::Init(Actor* actor){
	// ボスのポインタを取得
	mActor = actor;
	// パラメータ 初期化
	Reset();
}

void ACT::MoveFieldCenter::Update(){

	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 移動させる
		mActor->AddTransform(mVelocity);

		// 終了処理
		// フィールド中心に移動が完了したら終わる
		if (mActor->GetWorldPos().x == 0.0f && mActor->GetWorldPos().z == 0.0f) {
			mCondition = Condition::FINISHED;
		}
	}
}

void ACT::MoveFieldCenter::Draw(){}

void ACT::MoveFieldCenter::Start(){
	// パラメータ 初期化
	Reset();
	// 向きの指定
	mActor->InputDirection(mDirection);
	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Actor_Walk");
	// 実行する
	mCondition = Condition::RUNNING;

}

void ACT::MoveFieldCenter::End(){
	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::MoveFieldCenter::Reset(){
	// jsonから数値を取得
	mMoveSpeed = BlackBoard::GetGlobalVariables()->GetFloatValue("MoveFieldCenter", "MoveSpeed");
	// 移動の始点
	mStartPos = mActor->GetWorldPos();
	// 移動の終点
	mEndPos = { 0.0f,0.0f,0.0f };
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;

	// 移動速度の設定
	mMoveSpeedInFrame = BlackBoard::CombertBattleFPS(mMoveSpeed);
	// 移動量の設定
	mVelocity = Scalar(mMoveSpeedInFrame, mDirection);
	// 初期化する
	mCondition = Condition::IDOL;
}

void ACT::MoveFieldCenter::Save()
{
}

void ACT::MoveFieldCenter::Load()
{
}

void ACT::MoveFieldCenter::DrawGui()
{
}
