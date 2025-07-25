#include "ShotBullet.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Component/Collider/SphereCollider.h"
#include "GameEngine/Object/Skybox/Skybox.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Component/Collider/CollisionManager.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Editor/BlackBoard.h"

void ACT::ShotBullet::Init(Actor* boss)
{
	// ボスのポインタを取得
	mActor = boss;
	// 初期化する
	mCondition = Condition::IDOL;
	// パラメータ初期化
	Reset();

}

void ACT::ShotBullet::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 攻撃処理
		if (!mIsShot && mActor->GetObject3D()->mSkinning->GetAnimationTimeRate("Actor_MagicCast") >= 0.5f) {
			
			// 攻撃済みに設定
			mIsShot = true;

			// 弾のパラメータのうち、座標関連のみを補正する

			// アクターの左手の座標を取得
			Matrix4x4 leftHandMat = Multiply(mActor->GetObject3D()->mSkinning->GetSkeleton().joints[
				mActor->GetObject3D()->mSkinning->GetSkeleton().jointMap["mixamorig:RightHandThumb1"]].skeletonSpaceMatrix,
				mActor->GetObject3D()->GetWorldTransform()->GetWorldMatrix());
			
			 // パラメータに代入
			mBulletStatus.pos = Vector3(leftHandMat.m[3][0], leftHandMat.m[3][1], leftHandMat.m[3][2]);

			// 方向を指定
			if (Length(mActor->GetWorldPosForTarget() - mBulletStatus.pos) != 0.0f) {

				// ターゲットの座標に向かって放つ
				mBulletStatus.direction = Normalize(mActor->GetWorldPosForTarget() - mBulletStatus.pos);

				// ターゲット方向を向く
				mActor->InputDirection(mActor->GetWorldPosForTarget() - mBulletStatus.pos);

			}
			else {
				// 重なっていたら真下に発射する
				mBulletStatus.direction = { 0.0f,-1.0f,0.0f };
			}

			// 弾を生成(事前に設定したパラメータを使用)
			std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>(mBulletStatus);
			// アクターに弾の所有権を渡す
			mActor->SetNewBullet(std::move(newBullet));

		}

		// 終了処理
		if (mActor->GetObject3D()->mSkinning->GetIsAnimationFinished("Actor_MagicCast")) {
			mCondition = Condition::FINISHED;
		}

	}
}

void ACT::ShotBullet::Draw() {

}

void ACT::ShotBullet::Start()
{
	// パラメータ初期化
	Reset();
	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("Actor_MagicCast");

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::ShotBullet::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::ShotBullet::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;

	// 以下の2つの変数は衝突判定関係で不具合が発生するため使用しない
	// (例) 弾の射撃中に武器の衝突判定が発生し「理不尽な」ダメージが発生する
	mIsHit = false;
	mIsOperating = false;

	// 射撃行動フラグをfalseに
	mIsShot = false;

	// 弾のパラメータを設定する
	mBulletStatus.pos = mActor->GetWorldPos();
	mBulletStatus.radius = BlackBoard::GetGlobalVariables()->GetFloatValue("ShotBullet", "Radius");
	mBulletStatus.moveSpeed = BlackBoard::GetGlobalVariables()->GetFloatValue("ShotBullet", "MoveSpeed");

	// 方向
	if (Length(mActor->GetWorldPosForTarget() - mBulletStatus.pos) != 0.0f) {

		// ターゲットの座標に向かって放つ
		mBulletStatus.direction = Normalize(mActor->GetWorldPosForTarget() - mBulletStatus.pos);
	}
	else {
		// 重なっていたら真下に発射する
		mBulletStatus.direction = { 0.0f,-1.0f,0.0f };
	}
	// 生存時間
	mBulletStatus.lifeTime = BlackBoard::GetGlobalVariables()->GetFloatValue("ShotBullet", "LifeTime");
	// 威力
	mBulletStatus.power = BlackBoard::GetGlobalVariables()->GetFloatValue("ShotBullet", "Power");
	// ノックバックは1を指定されていた場合のみ発生する
	int32_t enableKnockback = BlackBoard::GetGlobalVariables()->GetIntValue("ShotBullet", "KnockBack");
	if (enableKnockback == 1) {
		mBulletStatus.isHitKnockBack = true;
	}
	// 衝突属性ID
	mBulletStatus.collisionAttribute = static_cast<uint32_t>(CollisionTypeId::kEnemyBullet);

}

void ACT::ShotBullet::Save()
{
}

void ACT::ShotBullet::Load()
{
}

void ACT::ShotBullet::DrawGui()
{
}
