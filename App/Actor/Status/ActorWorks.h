#pragma once

#include <string>
#include <variant>
#include <magic_enum/magic_enum.hpp>

#include "WorkAttack.h"
#include "WorkAvoid.h"
#include "WorkGuard.h"

/// <summary>
/// アクター共通の攻撃・防御・回避などのパラメータを保持するクラス
/// </summary>


/// <summary>
/// 振る舞い
/// <para> 各行動中の状態を示す </para>
/// </summary>
enum class Behavior {
	kRoot,  // 通常状態
	kMove,  // 移動状態
	kDash,	// ダッシュ状態
	kAttack,// 攻撃中
	kJump,	// ジャンプ中
	kCharge,// 溜め動作中
	kAvoid, // 回避行動
	kGuard, // 防御行動
	kParry,	// パリィ中
	kBreak, // ブレイク中
};


/// <summary>
/// アクターの各ワーククラス
/// </summary>
class ActorWorks {

public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ActorWorks();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ActorWorks();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(const std::string& name);

public: // -- 公開 メンバ変数 -- //

	// アクターの状態
	Behavior mBehavior;

	// 攻撃パラメータ
	WorkAttack mWorkAttack;
	// 回避パラメータ
	WorkAvoid mWorkAvoid;
	// 防御パラメータ
	WorkGuard mWorkGuard;


};