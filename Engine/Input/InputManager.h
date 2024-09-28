#pragma once
#include "Gamepad.h"
#include "Keyboard.h"
//#include "Mouse"

class InputManager
{
private:

	InputManager() = default;
	~InputManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	InputManager(const InputManager& obj) = delete;
	InputManager& operator=(const InputManager& obj) = delete;

public:

	// インスタンスを取得
	static InputManager* GetInstance();
	// 終了処理
	void Finalize();

	// 初期化
	void Initialize(WinApp* winApp);
	// 更新
	void Update();
	// デバッグUIの表示
	void DrawGUI();

	// キーボード入力にアクセスする
	Keyboard* GetKey() {
		return Keyboard::GetInstance();
	}
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

private:

	// 外部
	WinApp* winApp_ = nullptr;
	Keyboard* keyboard = nullptr;
	Gamepad* gamepad = nullptr;

	// インスタンス
	static InputManager* instance;

};