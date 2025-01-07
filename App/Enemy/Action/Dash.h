#pragma once
#include "Action.h"

namespace ACT {

	// ダッシュ
	class Dash :
		public IAction
	{
	public: // -- 公開 メンバ関数 -- //

		Dash() = default;
		~Dash() = default;

		// 初期化
		virtual void Init(BossEnemy* boss) override;
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

		// コライダーをリストに追加する
		virtual void SetCollider(CollisionManager* cManager)override;

	private:

		// 移動の始点
		Vector3 mStartPos;
		// 移動の終点
		Vector3 mEndPos;
		// 始点から終点への移動方向
		Vector3 mDirection;
		// それぞれの軸の移動量
		Vector3 mVelocity;
		// 移動速度
		float mMoveSpeed;
		// 追跡周期
		float mSearchTime;
		const float kSearchCycle = 0.5f;
	};

}

