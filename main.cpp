#include "./Engine/base/DirectXCommon.h"
#include "./Engine/object/camera/MatrixCamera.h"
#include "./Engine/primitive/Triangle.h"
#include "./Engine/primitive/Sprite.h"
#include "./Engine/resources/Section/Resource.h"
#include "./Engine/primitive/Sphere.h"
#include "./Engine/object/model/Model.h"
#include "./Audio.h"

#include <d3d12.h>

// リソースリークチェック
struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker()
	{
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			debug->Release();
		}
	}
};


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// リリースチェック
	D3DResourceLeakChecker leakCheck;
	// WindowsAPI
	WinAPI* win = WinAPI::GetInstance();
	// DirectX
	DirectXCommon* dx = DirectXCommon::GetInstance();
	// Audio
	Audio* audio = Audio::GetWaveInstance();

	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// デバッグレイヤーでエラーと警告を受け取る
#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG

	// 初期化
	win->Initialize();
	dx->Initialize(win);

	// 三角形
	Triangle* triangleA = new Triangle;
	Triangle* triangleB = new Triangle;
	triangleA->Initialize();
	triangleB->Initialize();
	triangleB->SetTransform({ 1.2f,0.0f,0.0f });
	triangleB->SetColor({ 0.7f,0.0f,0.0f,1.0f });

	//
	dx->TexLoadEnd();

	// 音声データの生成
	SoundData sound1 = audio->LoadWave("./Resources/sound/Alarm01.wav");

	// 警告やエラーが発生した際に停止させる
#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(dx->device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージを抑制する
		infoQueue->PushStorageFilter(&filter);
		// 解放
		infoQueue->Release();

	}
#endif // _DEBUG

	// ウィンドウの×ボタンが押されるまでループ
	while (win->msg.message != WM_QUIT) {
		// Windowsにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&win->msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&win->msg);
			DispatchMessage(&win->msg);
		}
		else {

			// フレームの先頭でImGuiに、ここからフレームが始まる旨を伝える
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// ゲーム用の処理

			///
			/// 更新処理(推定)
			///

			// 開発用UIの表示
			ImGui::ShowDemoWindow();

			triangleA->DisplayGUI("triangleA");
			triangleB->DisplayGUI("triangleB");
			triangleA->Update();
			triangleB->Update();


			// 描画処理に入る前に、ImGui内部のコマンドを生成する
			ImGui::Render();

			///
			/// 描画処理(推定) 
			/// 

			// 描画前処理
			dx->DrawBegin();
			
			triangleA->Draw();
			triangleB->Draw();


			// 描画後処理
			dx->DrawEnd();

		}

	}


	// Comの終了処理
	CoUninitialize();

	// 解放処理


	delete triangleA;
	delete triangleB;

	audio->Finalize();

	ImGui_ImplDX12_Shutdown();
	dx->Delete();

#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	
	win->Delete();
	/*CloseWindow(hwnd);*/

#pragma endregion

	return 0;
}
