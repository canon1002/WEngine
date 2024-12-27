#pragma once
//#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include <stdint.h>
#include <memory>

/// <summary>
/// フレームレート管理クラス
/// </summary>
class Framerate
{
public: // -- 公開 メンバ関数 -- //

	// インスタンス取得
	static Framerate* GetInstance();

	void Init();
	void Update();
	void Draw() {};
	void DrawGui();

	// デルタタイム取得 (直前のフレームと今のフレーム間で経過した時間) ※未実装 
	//float GetDeltaTime();

	// コマ送り時に設定したフレーム数動いたのか取得
	bool IsActiveFrame();
	// コマ送り中であるか
	bool GetIsFrameAdvance() { return mIsFrameAdvance; }

	// コマ送りにする
	void SetFrameAdvance() { mIsFrameAdvance = true; }
	// コマ送り解除
	void SetFrameAdvanceCancellation() { mIsFrameAdvance = false; }

	// ゲーム全体の速度を取得
	inline float GetGameSpeed()const { return mGameSpeed; }
	// ゲーム全体の速度を設定( 0.5倍速 ~ 2.0倍速 を想定)
	inline void SetGameSpeed(float gameSpeed) { mGameSpeed = gameSpeed; }


	// バトルスピードを取得(ゲーム全体速度 * バトルスピードを返す)
	inline float GetBattleSpeed()const { return mBattleSpeed * mGameSpeed; }
	// バトルスピードを設定( 0.5倍速 ~ 2.0倍速 を想定)
	inline void SetBattleSpeed(float battleSpeed) { mBattleSpeed = battleSpeed; }

	// フレームレートの取得
	inline float GetFramerate()const { return mFramerate; }

	// フレームレート上限の指定(未実装)
	// SetFlamerateMax;


private: // -- 非公開 メンバ変数 -- // 

	// インスタンス
	static Framerate* instance;

	// フレームレート設定値
	float mFramerate;
	// ゲーム全体の実行速度
	float mGameSpeed;
	// バトルスピード
	float mBattleSpeed;

	// コマ送りするか 
	bool mIsFrameAdvance;
	// コマ送りフレーム数
	int32_t mStepFlameCount;
	// 現在の経過フレーム
	int32_t mStepFlame;
};

