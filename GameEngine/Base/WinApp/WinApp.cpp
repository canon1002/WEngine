#include "WinApp.h"
#include "Editor/ImGui/ImGuiManager.h"

WinApp* WinApp::instance = nullptr;

WinApp* WinApp::GetInstance() {
	if (instance == nullptr) {
		instance = new WinApp();
	}
	return instance;
}

// 初期化
void WinApp::Init() {

	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// 初期化
	wc = {};
	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// ウィンドウクラスの登録
	RegisterClass(&wc);

	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	hwnd = CreateWindow(
		wc.lpszClassName,		// 利用するクラス名
		L"WEngine",				// タイトルバーの文字 (なんでも良い)
		WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
		CW_USEDEFAULT,			// 表示X座標 (Windowsに任せる)
		CW_USEDEFAULT,			// 表示Y座標 (WindowsOSに任せる)
		wrc.right - wrc.left,	// ウィンドウ横幅
		wrc.bottom - wrc.top,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		wc.hInstance,			// インスタンスハンドル
		nullptr);				// オプション


	// ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);

	// システムタイマーの分解能を上げる
	timeBeginPeriod(1);

}

// 消去
void WinApp::Finalize() {
	// ウィンドウを消去
	CloseWindow(hwnd);
	// インスタンス消去
	delete instance;
	instance = nullptr;
}


// システム 及び デバッグメッセージ

// ウィンドウプロシージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg,
	WPARAM wparam, LPARAM lparam) {

	// ImGuiにメッセージを渡し、ImGuiが処理していたら早期リターンを行う
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);

}

// 出力ウィンドウに文字を出す
void WinApp::Log(const std::string& messege) {
	OutputDebugStringA(messege.c_str());
}

// シェーダーをコンパイルする
Microsoft::WRL::ComPtr <IDxcBlob> WinApp::CompileShader(
	// CompilerするShederファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したものを3つ
	Microsoft::WRL::ComPtr < IDxcUtils> dxcUtils,
	Microsoft::WRL::ComPtr < IDxcCompiler3> dxcCompiler,
	Microsoft::WRL::ComPtr < IDxcIncludeHandler> includeHandler)
{
	/// 手順1.hlslファイルを読む


	// これからシェーダーをコンパイル旨をログに出す
	WinApp::Log(WinApp::ConvertString(std::format(L"Bigin CompileShader,path:{},profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //UTF-8の文字コードであることを通知

	/// 手順2.Compileする

	LPCWSTR arguments[] = {
		filePath.c_str(),		//コンパイル対象のhlslファイル名
		L"-E",L"main",			// エントリーポイントの指定。基本的にmain以外にはしない。
		L"-T",profile,			// ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",					// 最適化を外しておく
		L"-Zpr",				// コンパイル結果
	};

	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,				// 読み込んだファイル
		arguments,						// コンパイルオプション
		_countof(arguments),			// コンパイルオプション
		includeHandler.Get(),		// Includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	// コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	///	手順3.警告・エラーがでていないかの確認

	// 警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		WinApp::Log(shaderError->GetStringPointer());
		// 警告・エラーダメゼッタイ
		assert(false);
	}

	// Compile結果を受け取って返す

	// コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr < IDxcBlob> shaderBlob = nullptr;
	shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	WinApp::Log(WinApp::ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));

	// 実行用のバイナリを返却
	return shaderBlob;

}

bool WinApp::ProcessMessage()
{
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}

// ** 変換 ** //

// string->wstring
std::wstring WinApp::ConvertString(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0)
	{
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}
// wstring->string
std::string WinApp::ConvertString(const std::wstring& str)
{
	if (str.empty())
	{
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0)
	{
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}
