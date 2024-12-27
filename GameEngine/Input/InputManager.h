#pragma once
#include "Gamepad.h"
#include "Keyboard.h"
//#include "Mouse"

#include <memory>

using namespace std;

enum class InputType {
	KEYBOARD,
	GAMEPAD,
	MOUSE,
};

class InputManager
{
private: // -- 非公開 メンバ関数 -- //

	InputManager() = default;
	~InputManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	InputManager(const InputManager& obj) = delete;
	InputManager& operator=(const InputManager& obj) = delete;

public: // -- 公開 メンバ関数 -- //

	// インスタンスを取得
	static InputManager* GetInstance();
	// 終了処理
	void Final();

	// 初期化
	void Init();
	// 更新
	void Update();
	// デバッグUIの表示
	void DrawGui();

	// -- 共通入力関数 -- //


	bool GetPush(BYTE type);
	bool GetPush(const Gamepad::Button& type);

	bool GetPress(BYTE type);
	bool GetPress(const Gamepad::Button& type);
	
	bool GetTrigger(BYTE type);
	bool GetTrigger(const Gamepad::Button& type);
	
	bool GetRelese(BYTE type);
	bool GetRelese(const Gamepad::Button& type);

	// -- アクセッサ -- //

	// キーボード入力にアクセスする
	Keyboard* GetKey() { return keyboard; }
	// ゲームパッド入力にアクセスする
	Gamepad* GetGamepad() { return gamepad; }

	// -- キーボード関連 -- //

	// 押したら
	bool GetPushKey(BYTE keyNumber);
	// 押しっぱの場合
	bool GetPressKey(BYTE keyNumber);
	// 単押しの場合
	bool GetTriggerKey(BYTE keyNumber);
	// 離したとき
	bool GetReleseKey(BYTE keyNumber);

	// -- ゲームパッド関連 -- //

	// 押した瞬間
	bool GetPused(const Gamepad::Button& type);
	// 長押し
	bool GetLongPush(const Gamepad::Button& type);
	// 離した瞬間
	bool GetReleased(const Gamepad::Button& type);
	// トリガーのボタンの状態を取得
	unsigned char GetTriger(const Gamepad::Triger& type);
	// スティックの倒した量を取得
	short GetStick(const Gamepad::Stick& type);
	// スティックの入力量を引数(0.0f)~最大値(1.0f)の間で返す関数
	float GetStickRatio(const Gamepad::Stick& type, const short min);


private:

	// マウス入力クラス


	// キーボード入力クラス
	Keyboard* keyboard;

	// ゲームパッド入力クラス
	Gamepad* gamepad;

	// インスタンス
	static InputManager* instance;

};