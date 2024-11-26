#pragma once
#include "Action.h"

// ----------------------------------------------------
// ダッシュ攻撃
// ----------------------------------------------------
namespace ACT {
	class AttackDash :
		public IAction
	{
	public:
		AttackDash() = default;
		~AttackDash() = default;

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

		std::vector<Collider*> mColliders;
		float mActiveColliderCount;
		const Vector2 kActiveColliderCount = { 0.8f,1.4f };


	};
}