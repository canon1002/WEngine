#include "EarthSword.h"
#include "App/Actor/Actor.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "App/BlackBoard.h"


void ACT::EarthSword::Init(Actor* boss)
{
	// ボスのポインタを取得
	mActor = boss;
	// 初期化する
	mCondition = Condition::IDOL;
	// パラメータ初期化
	Reset();
	// ランダムエンジンの初期化
	mRandomEngine = std::mt19937(mSeedGenerator());

	// エミッター 初期化
	mEmitter = std::make_unique<ParticleEmitter>("EarthSword");
	mEmitter->Init();
	ParticleManager::GetInstance()->CreateParticleGroupe(mEmitter->mName, "EarthEmit.png");
}

void ACT::EarthSword::Update()
{
	// 実行時のみ処理を行う
	if (mCondition == Condition::RUNNING) {

		// 攻撃範囲の表示
		if (!mIsShot) {
			for (size_t i = 0; i < mAttackCount; i++) {
				// 発生座標の更新
				mEmitter->SetEmitterPos(Vector3(mAttackAreaPos[i].x, 0.0f, mAttackAreaPos[i].z));
				// パーティクルを発生させる
				mEmitter->Update();
			}
		}

		// 攻撃処理
		if (!mIsShot && mActor->GetObject3D()->mSkinning->GetAnimationTimeRate("magic") >= 0.6f) {
			
			// 攻撃済みに設定
			mIsShot = true;

			for (size_t i = 0; i < mAttackCount; i++) {
				// 弾を生成(事前に設定したパラメータを使用)
				std::unique_ptr<EarthSwordBullet> newBullet = std::make_unique<EarthSwordBullet>(mBulletsStatus[i]);
				// アクターに弾の所有権を渡す
				mActor->SetNewBullet(std::move(newBullet));
			}

		}

		// 終了処理
		if (mActor->GetObject3D()->mSkinning->GetIsAnimationFinished("magic")) {
			mCondition = Condition::FINISHED;
			// 再生速度をもとに戻す
			mActor->GetObject3D()->mSkinning->SetAnimationPlaySpeed(1.5f);

		}

	}
}

void ACT::EarthSword::Draw() {

}

void ACT::EarthSword::Start()
{
	// パラメータ初期化
	Reset();

	// 生成数に応じた数の生成・攻撃範囲を設定
	for (size_t i = 0; i < mAttackCount; i++) {
		
		// ランダムな座標に攻撃範囲を設定
		std::uniform_real_distribution<float> randomPos = uniform_real_distribution(-BlackBoard::GetFieldRadius().x, BlackBoard::GetFieldRadius().x);
		mAttackAreaPos[i] = Vector3(randomPos(mRandomEngine), -mBulletsStatus[i].radius, randomPos(mRandomEngine));

		// (TODO) 完全ランダムではなく、プレイヤーに近かったり、集中箇所、過疎場所 を 意図的に作り出したい

		// 弾の初期座標を決定
		mBulletsStatus[i].pos = mAttackAreaPos[i];
	}
	// アニメーションの変更
	mActor->GetObject3D()->mSkinning->SetNextAnimation("magic");
	// 再生速度を遅くする
	mActor->GetObject3D()->mSkinning->SetAnimationPlaySpeed(0.75f);

	// 実行する
	mCondition = Condition::RUNNING;
}

void ACT::EarthSword::End()
{
	// 行動を終了させる
	mCondition = Condition::FINISHED;
}

void ACT::EarthSword::Reset()
{
	// 初期化する
	mCondition = Condition::IDOL;

	// 以下の2つの変数は衝突判定関係で不具合が発生するため使用しない
	// (例) 弾の射撃中に武器の衝突判定が発生し「理不尽な」ダメージが発生する
	mIsHit = false;
	mIsOperating = false;

	// 射撃行動フラグをfalseに
	mIsShot = false;

	// 生成個数の取得
	mAttackCount = BlackBoard::GetGlobalVariables()->GetIntValue("EarthSword", "AttackCount");
	mAttackAreaPos.resize(mAttackCount);

	// 弾の個数分生成する
	for (int32_t i = 0; i < mAttackCount; i++) {
		BulletStatus mBulletStatus;

		// 弾のパラメータを設定する
		mBulletStatus.pos = mActor->GetWorldPos();
		mBulletStatus.radius = BlackBoard::GetGlobalVariables()->GetFloatValue("EarthSword", "Radius");
		mBulletStatus.moveSpeed = BlackBoard::GetGlobalVariables()->GetFloatValue("EarthSword", "MoveSpeed");

		// 方向(上方向に打つ)
		mBulletStatus.direction = { 0.0f,1.0f,0.0f };

		// 生存時間
		mBulletStatus.lifeTime = BlackBoard::GetGlobalVariables()->GetFloatValue("EarthSword", "LifeTime");
		// 威力
		mBulletStatus.power = BlackBoard::GetGlobalVariables()->GetFloatValue("EarthSword", "Power");
		// ノックバックは1を指定されていた場合のみ発生する
		int32_t enableKnockback = BlackBoard::GetGlobalVariables()->GetIntValue("EarthSword", "KnockBack");
		if (enableKnockback == 1) {
			mBulletStatus.isHitKnockBack = true;
		}
		// 衝突属性ID
		mBulletStatus.collisionAttribute = static_cast<uint32_t>(CollisionTypeId::kEnemyBullet);

		// 配列に追加
		mBulletsStatus.push_back(mBulletStatus);
	}
}

void ACT::EarthSword::Save()
{
}

void ACT::EarthSword::Load()
{
}

void ACT::EarthSword::DrawGui()
{
}
