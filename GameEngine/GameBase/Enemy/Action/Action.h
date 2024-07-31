#pragma once
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Math/Math.h"
#include "GameEngine/Object/3d/Object3d.h"

// 前方宣言
class BossEnemy;
class CollisionManager;

// 行動
namespace ACT {
	
	// 駆動状態
	enum class Condition {
		IDOL,	 // 待機中
		RUNNING, // 実行中
		FINISHED,//終了
	};

	// ----------------------------------------------------
	// 基底クラス 
	// ----------------------------------------------------
	class IAction
	{
	public: // -- 公開 メンバ関数 -- //

		// コンストラクタ
		IAction() = default;
		// 仮想デストラクタ
		virtual ~IAction() = default;

		// 初期化
		virtual void Init(BossEnemy* boss) = 0;
		// 更新処理
		virtual void Update() = 0;
		// 描画処理
		virtual void Draw() =0;

		// 開始処理
		virtual void Start() = 0;
		// 終了処理
		virtual void End() = 0;
		// 再起動処理
		virtual void Reset() = 0;

		// 駆動状態取得関数
		virtual ACT::Condition GetCondition()const { return mCondition; }

		// コライダーをリストに追加する
		virtual void SetCollider(CollisionManager* cManager) = 0;

	protected: // -- 限定公開 メンバ変数 -- // 

		// BossEnemyクラスのポインタ
		BossEnemy* mBoss;

		// 駆動状態
		Condition mCondition;
	};


	// ----------------------------------------------------
	// 接近
	// ----------------------------------------------------
	class MoveToPlayer :
		public IAction
	{
	public:
		MoveToPlayer() = default;
		~MoveToPlayer() = default;

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
		const float kSearchCycle = 3.0f;
	};

	// ----------------------------------------------------
	// 近接攻撃
	// ----------------------------------------------------
	class AttackClose :
		public IAction
	{
	public:
		AttackClose() = default;
		~AttackClose() = default;

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
		std::unique_ptr<Object3d> mWeapon;
		std::vector<Collider*> mColliders;
		float mActiveColliderCount;
		const Vector2 kActiveColliderCount = { 0.8f,1.4f };
	};

}