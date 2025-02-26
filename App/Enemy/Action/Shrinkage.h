#pragma once
#include "Action.h"

namespace ACT {

	// ダッシュ
	class Shrinkage :
		public Action
	{
	public: // -- 公開 メンバ関数 -- //

		Shrinkage() = default;
		~Shrinkage() = default;

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
		float mMoveSpeedInFrame;
		// 追跡周期
		float mRotateTime;
		float mRotateCycle;
	};

}

