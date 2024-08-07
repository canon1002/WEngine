#pragma once
//#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include <stdint.h>

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

	// デルタタイム (直前のフレームと今のフレーム間で経過した時間) 取得
	float GetDeltaTime();

	// コマ送り時に設定したフレーム数動いたのか取得
	bool IsActiveFrame();
	// コマ送り中であるか
	bool GetIsFrameAdvance() { return mIsFrameAdvance; }

	// コマ送りにする
	void SetFrameAdvance() { mIsFrameAdvance = true; }
	// コマ送り解除
	void SetFrameAdvanceCancellation() { mIsFrameAdvance = false; }

private: // -- 非公開 メンバ変数 -- // 

	// インスタンス
	static Framerate* instance;

	// コマ送りするか 
	bool mIsFrameAdvance;
	// コマ送りフレーム数
	int32_t mStepFlameCount;
	// 現在の経過フレーム
	int32_t mStepFlame;
};

