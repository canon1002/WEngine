#include "Action.h"
#include "App/Enemy/BossEnemy.h"

void ACT::MoveToPlayer::Init(BossEnemy* boss)
{
	// ボスのポインタを取得
	mBoss = boss;
	
	// 移動の始点
	mStartPos = mBoss->GetWorldPos();
	// 移動の終点
	mEndPos = Vector3(0,0,0);
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / 60.0f) * 4.0f;
	// 移動量の設定
	mVelocity = Scalar(mMoveSpeed, mDirection);
	// 追跡までの時間をリセット
	mSearchTime = 0.0f;

	// 初期化する
	mCondition = Condition::IDOL;

}

void ACT::MoveToPlayer::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {
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

		// 一定時間ごとにプレイヤー座標を取得
		mSearchTime += 1.0f / 60.0f;
		if (mSearchTime>= kSearchCycle) {
			// 移動の終点
			mEndPos = mBoss->GetWorldPosForTarget();
			mEndPos.y = 0.0f;
			// 方向の設定
			mDirection = Normalize(mEndPos - mBoss->GetWorldPos());
			mDirection.y = 0.0f;
			// 移動速度の設定
			mMoveSpeed = (1.0f / 60.0f) * 8.0f;
			// 移動量の設定
			mVelocity = Scalar(mMoveSpeed, mDirection);

			// 追跡までの時間をリセット
			mSearchTime = 0.0f;
		}

		// 終了処理
		if (mBoss->InvokeNearDistance()) {
			mCondition = Condition::FINISHED;
		}
	}
}

void ACT::MoveToPlayer::Draw(Camera camera)
{
	camera;
}

void ACT::MoveToPlayer::Start()
{
	// パラメータの初期化

	// 移動の始点
	mStartPos = mBoss->GetWorldPos();
	mStartPos.y = 0.0f;
	// 移動の終点
	mEndPos = mBoss->GetWorldPosForTarget();
	mEndPos.y = 0.0f;
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / 60.0f) * 4.0f;
	// 移動量の設定
	mVelocity = Scalar(mMoveSpeed, mDirection);
	// 追跡までの時間をリセット
	mSearchTime = 0.0f;

	// アニメーションの変更
	mBoss->GetObject3D()->mSkinning->Init("boss", "Run.gltf",
		mBoss->GetObject3D()->GetModel()->modelData);

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::MoveToPlayer::End()
{

	// アニメーションの変更
	mBoss->GetObject3D()->mSkinning->Init("boss", "Idle.gltf",
		mBoss->GetObject3D()->GetModel()->modelData);

	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::MoveToPlayer::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;

	// パラメータの初期化

	// 移動の始点
	mStartPos = mBoss->GetWorldPos();
	mStartPos.y = 0.0f;
	// 移動の終点
	mEndPos = mBoss->GetWorldPosForTarget();
	mEndPos.y = 0.0f;
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / 60.0f) * 2.0f;
	// 移動量の設定
	mVelocity = Scalar(mMoveSpeed, mDirection);
}

void ACT::MoveToPlayer::SetCollider(CollisionManager* cManager)
{
	cManager;
}


void ACT::BackStep::Init(BossEnemy* boss)
{
	// ボスのポインタを取得
	mBoss = boss;

	// 移動の始点
	mStartPos = mBoss->GetWorldPos();
	// 移動の終点
	mEndPos = Vector3(0, 0, 0);
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / 60.0f) * 4.0f;
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

		// 終了処理
		if (mBoss->InvokeFarDistance()) {
			mCondition = Condition::FINISHED;
		}
	}
}

void ACT::BackStep::Draw(Camera camera)
{
	camera;
}

void ACT::BackStep::Start()
{
	// パラメータの初期化

	// 移動の始点
	mStartPos = mBoss->GetWorldPos();
	mStartPos.y = 0.0f;
	// 移動の終点
	mEndPos = mBoss->GetWorldPosForTarget();
	mEndPos.y = 4.0f;
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / 60.0f) * 40.0f;
	// 移動量の設定
	mVelocity = Scalar(-mMoveSpeed, mDirection);

	// 少しジャンプさせる
	mBoss->Jump(1.0f);

	// アニメーションの変更
	mBoss->GetObject3D()->mSkinning->Init("boss", "Idle.gltf",
		mBoss->GetObject3D()->GetModel()->modelData);

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::BackStep::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::BackStep::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;

	// パラメータの初期化

	// 移動の始点
	mStartPos = mBoss->GetWorldPos();
	mStartPos.y = 0.0f;
	// 移動の終点
	mEndPos = mBoss->GetWorldPosForTarget();
	mEndPos.y = 0.0f;
	// 方向の設定
	mDirection = Normalize(mEndPos - mStartPos);
	mDirection.y = 0.0f;
	// 移動速度の設定
	mMoveSpeed = (1.0f / 60.0f) * 2.0f;
	// 移動量の設定
	mVelocity = Scalar(-mMoveSpeed, mDirection);
}

void ACT::BackStep::SetCollider(CollisionManager* cManager)
{
	cManager;
}
