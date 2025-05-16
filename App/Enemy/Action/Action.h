#pragma once
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Object/3d/Object3d.h"

// 前方宣言
class Actor;
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
	class Action
	{
	public: // -- 公開 メンバ関数 -- //

		// コンストラクタ
		Action() = default;
		// 仮想デストラクタ
		virtual ~Action() = default;

		// 初期化
		virtual void Init(Actor* actor) = 0;
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

		// 保存処理
		virtual void Save() = 0;
		// 読み込み処理
		virtual void Load() = 0;
		// ImGuiの表示
		virtual void DrawGui() = 0;

		// 攻撃中であるか取得する
		inline bool GetIsOperating()const  { return mIsOperating; }
		// 攻撃が命中したか取得する
		inline bool GetIsHit()const  { return mIsHit; }
		// 衝突フラグをtrueに切り替える
		inline void Hit() { mIsHit = true; }

	protected: // -- 限定公開 メンバ変数 -- // 

		// アクターのポインタ
		Actor* mActor;

		// 駆動状態
		Condition mCondition;

		// アニメーション再生時間
		float mAnimationTime;
		// アニメーション終了時間
		float mAnimationEndTime;

		// 動作中であるか(攻撃判定フラグとしても扱う)
		bool mIsOperating = false;
		// 攻撃が命中したか
		bool mIsHit = false;	
	};



	// ----------------------------------------------------
	// 接近
	// ----------------------------------------------------
	class ChaseTarget :
		public Action
	{
	public:
		ChaseTarget() = default;
		~ChaseTarget() = default;

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

		// 追跡を行う時間
		float mSearchCount;
		float mSearchCountMax;

	};
	
	// ----------------------------------------------------
	// 後退
	// ----------------------------------------------------
	class BackStep :
		public Action
	{
	public:
		BackStep() = default;
		~BackStep() = default;

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

	};

	// ----------------------------------------------------
	// 近接攻撃
	// ----------------------------------------------------
	class AttackClose :
		public Action
	{
	public:
		AttackClose() = default;
		~AttackClose() = default;

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
	
		std::vector<Collider*> mColliders;
		float mColliderCount;
		Vector2 mActiveColliderCount;
	};

	// ----------------------------------------------------
	// 刺突攻撃
	// ----------------------------------------------------
	class AttackThrust :
		public Action
	{
	public:
		AttackThrust() = default;
		~AttackThrust() = default;

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

		std::vector<Collider*> mColliders;
		float mColliderCount;
		Vector2 mActiveColliderCount = { 0.4f,2.4f };
	};

}