#pragma once
#include "Gamepad.h"
#include "Keyboard.h"

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
	void Initialize();
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


private:

	// 外部
	Keyboard* keyboard = nullptr;
	Gamepad* gamepad = nullptr;

	// インスタンス
	static InputManager* instance;

};