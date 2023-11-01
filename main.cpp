#include "./Engine/base/DirectXCommon.h"
#include "./Engine/object/camera/MatrixCamera.h"
#include "./Engine/primitive/Triangle.h"
#include "./Engine/resources/Section/Resource.h"

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
	// カメラ
	MatrixCamera* mainCamera = MatrixCamera::GetInstance();

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
	dx->Initialize(win, mainCamera);
	mainCamera->Initialize();

	// 三角形
	Triangle* triangle = new Triangle;
	triangle->Initialize();

	Triangle* triangle2 = new Triangle;
	triangle2->Initialize();
	triangle2->SetPos({ 0.3f,0.0f,0.0f });

	// 警告やエラーが発生した際に停止させる
#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(dx->device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);

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

	// 開始前にTransform変数を作る
	Math::Transform transform = { {1.0f,1.0f,1.0f}, { 0.0f,0.0f,0.0f }, {0.0f,0.0f,0.0f} };
	// ワールド行列の生成
	Matrix4x4 worldMatrix = W::Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	mainCamera->SetWorldAffine(transform.scale, transform.rotate, transform.translate);
	mainCamera->SetCameraAffine(transform.scale, transform.rotate, { 0.0f,0.0f,-5.0f });

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

			mainCamera->Update();
			triangle->Update();
			triangle2->Update();

			// 描画処理に入る前に、ImGui内部のコマンドを生成する
			ImGui::Render();

			///
			/// 描画処理(推定) 
			/// 

			// 描画前処理
			dx->DrawBegin();

			triangle->Draw();
			triangle2->Draw();
		

			// 描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
			//dx->commandList->DrawInstanced(3, 1, 0, 0);

			// 描画後処理
			dx->DrawEnd();

		}

	}


	// Comの終了処理
	CoUninitialize();

	// 解放処理
	delete triangle2;
	delete triangle;
	mainCamera->Delete();
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
