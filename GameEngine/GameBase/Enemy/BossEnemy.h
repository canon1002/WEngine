#pragma once
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/GameBase/AI/BehaviorTree/BehaviorTree.h"
#include "GameEngine/GameBase/AI/State/IBossState.h"
#include "GameEngine/GameBase/AI/State/VitalityBossState.h"

class BossEnemy
{
public: // -- 公開 メンバ関数 -- //

	BossEnemy() = default;
	~BossEnemy() = default;

	// 初期化
	void Init();
	// 初期化 -- ビヘイビアツリー --
	void InitBehavior();
	// 更新
	void Update();
	// 描画
	void Draw();
	// デバッグUI表示
	void DrawGUI();

	Model* GetModel() { return mObject->mModel; }

#pragma region コマンド・ステート関連

	inline void MoveForward() { mObject->mWorldTransform->translation.z += mVelocity.z; }
	inline void BackStep(){ mObject->mWorldTransform->translation.z -= mVelocity.z; }

#pragma endregion

private: // -- 非公開 メンバ変数 -- //

	// オブジェクトクラス
	std::unique_ptr<Object3d> mObject;

	// 移動量
	Vector3 mVelocity;

	// 重力の影響を受けるか
	bool mIsGravity;
	// 残りジャンプ回数
	int32_t mJumpCount;
	// 最大ジャンプ回数
	const int32_t kJumpCountMax = 2;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.4f;

	// 最大コンボ回数
	const int32_t kComboCountMax = 3;

	// ビヘイビアツリー
	std::unique_ptr<SelectorNode> mRoot;

	// メンバ関数ポインタ
	static void(BossEnemy::*CommandTable[])();

	// ボスのState番号の配列(実行中に増減しないため、長さ固定の配列を宣言)
	std::array<std::unique_ptr<IBossState>, 1> mStateArr;
	int mCurrentSceneNo;
	int mPrevSceneNo;
};
