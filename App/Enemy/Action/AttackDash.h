#pragma once
#include "Action.h"

// ----------------------------------------------------
// ダッシュ攻撃
// ----------------------------------------------------
namespace ACT {
	class AttackDash :
		public Action
	{
	public:
		AttackDash() = default;
		~AttackDash() = default;

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

		float mColliderCount;
		Vector2 mActiveColliderCount;

	};
}