#include "KnockBack.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Editor/Framerate.h"

void ACT::KnockBack::Init(Actor* actor){
	// ボスのポインタを取得
	mActor = actor;
	// 初期化する
	mCondition = Condition::IDOL;
	
	// 移動の始点
	mStartPos = { 0.0f,0.0f,0.0f };
	// 移動の終点
	mEndPos = { 0.0f,0.0f,0.0f };
	// 始点から終点への移動方向
	mDirection = { 0.0f,0.0f,1.0f };
	// 移動量
	mVelocity = { 0.0f,0.0f,1.0f };
	// 移動速度
	mMoveSpeed = (2.0f/mActor->GetObject3D()->mSkinning->GetSkinCluster("Actor_Knockback")->animation.duration);
	// 攻撃対象の座標
	mTargetPos = { 0.0f,0.0f,0.0f };

}

void ACT::KnockBack::Update(){
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 移動させる
		mActor->AddTransform(mVelocity);

		// 終了処理 // アニメーションが終了したら終了
		if (mActor->GetObject3D()->mSkinning->GetIsAnimationFinished("Actor_Knockback")) {
			mCondition = Condition::FINISHED;
		}
	}

}

void ACT::KnockBack::Draw(){}

void ACT::KnockBack::Start(){

	// 移動の始点
	mStartPos = mActor->GetWorldPos();
	// 攻撃対象の座標
	mTargetPos = mActor->GetWorldPosForTarget();
	// 始点から終点への移動方向の設定
	mDirection = Normalize(mTargetPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動量
	mVelocity = Scalar(-mMoveSpeed,mDirection);
	// 移動の終点
	mEndPos = mStartPos + mDirection;

	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Actor_Knockback");

}

void ACT::KnockBack::End(){
	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::KnockBack::Reset(){
	// 初期化する
	mCondition = Condition::IDOL;
}

void ACT::KnockBack::Save()
{
}

void ACT::KnockBack::Load()
{
}

void ACT::KnockBack::DrawGui()
{
}
