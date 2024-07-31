#pragma once

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
	void Draw();

	// デルタタイム (直前のフレームと今のフレーム間で経過した時間) 取得
	float GetDeltaTime();

	// コマ送り時にフレームを動かしているか取得する
	bool IsActiveFrame();

	// コマ送りにする
	void SetFrameAdvance() { mIsFrameAdvance = true; }
	// コマ送り解除
	void SetFrameAdvanceCancellation() { mIsFrameAdvance = false; }

private: // -- 非公開 メンバ変数 -- // 

	// インスタンス
	static Framerate* instance;

	// コマ送りするか 
	bool mIsFrameAdvance;
};

