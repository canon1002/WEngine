#pragma once
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

private: // -- 非公開 メンバ変数 -- // 

	// インスタンス
	static Framerate* instance;

};

