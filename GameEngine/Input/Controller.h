#pragma once

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")

class Controller
{
	// 押し込みのボタン
	enum class Button {
		UP = 0, // 十字キー上
		DOWN, // 十字キー下
		LEFT, // 十字キー左
		RIGHT, // 十字キー右
		START, // 漢字の三みたいなやつ
		BACK, // 四角が重なったようなやつ
		LEFT_THUMB, // 左スティック押し込み
		RIGHT_THUMB, // 右スティック押し込み 
		LEFT_SHOULDER, // LB
		RIGHT_SHOULDER, // RB
		A = 12, // A
		B, // B
		X, // X
		Y, // Y
	};

	// RT,LT
	enum class Triger {
		LEFT,
		RIGHT,
	};

	// スティック
	enum class Stick {
		LEFT_X,
		LEFT_Y,
		RIGHT_X,
		RIGHT_Y
	};

private: // -- 非公開 メンバ関数 -- //

	inline Controller() = default;
	inline ~Controller() = default;

public: // -- 公開 メンバ関数 -- //

	// 入力関数
	static void Input();

	// 押し込みのボタンの状態を取得
	static bool GetButton(const Button& type);

	// 押し込みのボタンの前フレームの状態を取得
	static bool GetPreButton(const Button& type);

	// 押し込みのボタンを押した瞬間
	static bool Pushed(const Button& type);

	// 押し込みのボタンを長押し
	static bool LongPush(const Button& type);

	// 押し込みのボタンを離した瞬間
	static bool Released(const Button& type);

	// トリガーのボタンの状態を取得
	static unsigned char GetTriger(const Triger& type);

	// スティックがどのくらい倒れたか
	// 左右のスティックのX,Yをそれぞれ取れる
	// 引数でどれをとるか指定
	// 戻り値の範囲は-32768～32767
	// 
	// ex: 左スティックのxをどのくらい倒したかをとる場合
	// getStick(Gamepad::Stick::LEFT_X);
	// 
	static short GetStick(const Stick& type);

	// バイブレーション
	static void IsVibration(const int& flag);

	// デバッグ用
	static void DrawGUI();

private:
	static unsigned short mPreButton;
	static XINPUT_STATE mState;
	static XINPUT_VIBRATION mVibration;
};

