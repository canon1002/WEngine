#pragma once
#include "Action.h"
#include "App/Bullet/EarthSwordBullet.h"
#include "GameEngine/Effect/Particle/Emitter/ParticleEmitter.h"
#include <random>

// ----------------------------------------------------
// 大地の剣(複数地点遠距離攻撃)
// ----------------------------------------------------
namespace ACT {

	class EarthSword :
		public Action
	{
	public:
		EarthSword() = default;
		~EarthSword() = default;

		// 初期化
		virtual void Init(Actor* actor) override;
		// 更新処理
		virtual void Update() override;
		// 描画処理
		virtual void Draw() override;
		// 開始処理
		virtual void Start() override;
		// 終了処理
		virtual void End() override;
		// 再起動処理
		virtual void Reset() override;

		// 駆動状態取得関数
		virtual ACT::Condition GetCondition()const override { return mCondition; }

		// 保存処理
		virtual void Save() override;
		// 読み込み処理
		virtual void Load() override;
		// ImGuiの表示
		virtual void DrawGui() override;

	private: // -- 非公開 メンバ変数 -- // 

		// 弾のパラメータ
		std::vector<BulletStatus> mBulletsStatus;
		// 射撃を行ったか
		bool mIsShot;

		// パーティクル用エミッター
		std::unique_ptr<ParticleEmitter> mEmitter;

		// 攻撃発生数
		int32_t mAttackCount;
		// 攻撃範囲の中心座標
		std::vector<Vector3> mAttackAreaPos;
		// 乱数生成機
		std::random_device mSeedGenerator;
		std::mt19937 mRandomEngine;

	};
}