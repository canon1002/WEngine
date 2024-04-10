#pragma once

#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

// FPS固定
#pragma comment(lib,"winmm.lib")


// Comポインタ
#include <wrl.h>
#include <memory>

class WinAPI final
{
private: // ** メンバ関数 ** //

	WinAPI();
	~WinAPI();

public: // ** 静的メンバ関数 ** //

	// コピーコンストラクタと演算子オーバーロードの禁止
	WinAPI(const WinAPI& obj) = delete;
	WinAPI& operator=(const WinAPI& obj) = delete;

	// インスタンスを取得
	static WinAPI* GetInstance();

public: // ** メンバ関数 ** //

	// 初期化
	void Initialize();

	// 消去
	void Finalize();

public: // -- アクセッサ -- //

	HWND GetHWND() { return hwnd; }


	// システム 及び デバッグメッセージ

	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	// 出力ウィンドウに文字を出す
	static void Log(const std::string& messege);
	// シェーダーをコンパイルする
	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		// CompilerするShederファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したものを3つ
		Microsoft::WRL::ComPtr < IDxcUtils> dxcUtils,
		Microsoft::WRL::ComPtr < IDxcCompiler3> dxcCompiler,
		Microsoft::WRL::ComPtr < IDxcIncludeHandler> includeHandler);

	// メッセージ処理
	bool ProcessMessage();

	// ** 変換 ** //

	// string->wstring
	static std::wstring ConvertString(const std::string& str);
	// wstring->string
	static std::string ConvertString(const std::wstring& str);


public:

	// ウィンドウクラスの宣言
	WNDCLASS wc{};

	// クライアント領域
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	HWND hwnd;

	// メッセージ
	MSG msg{};

private:
	static WinAPI* instance;
};


