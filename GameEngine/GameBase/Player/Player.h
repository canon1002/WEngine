#pragma once
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/Input/InputManager.h"

// 振る舞い
enum class Behavior {
	kRoot,   // 通常状態
	kAttack, // 攻撃中
	kJump,	 // ジャンプ中
};

class Player {

public: // -- 公開 メンバ関数 -- //

	Player() = default;
	~Player() = default;

	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw();
	// デバッグUI表示
	void DrawGUI();

	Model* GetModel() { return mObject->mModel; }

private: // -- 非公開 メンバ変数 -- //

	// 入力を取得
	InputManager* mInput;

	// オブジェクトクラス
	std::unique_ptr<Object3d> mObject;

	// 自機の振る舞い
	Behavior mBehavior;

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

};

