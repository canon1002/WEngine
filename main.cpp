#include "Draw.h"
#include "MatrixCamera.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// WindowsAPI
	WinAPI* win = WinAPI::GetInstance();
	// DirectX
	DirectX* dx = DirectX::GetInstance();
	// Draw
	Draw* draw = Draw::GetInstance();

	// カメラ
	MatrixCamera* mainCamera = new MatrixCamera({ (float)win->kClientWidth ,(float)win->kClientHeight });

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
	draw->Initialize(win, dx, mainCamera);

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


	// 開始前にTransform変数を作る
	Transform transform = { {1.0f,1.0f,1.0f}, { 0.0f,0.0f,0.0f }, {0.0f,0.0f,0.0f} };
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
			// ゲーム用の処理

			///
			/// 更新処理(推定)
			///

			// 三角形のY軸回転
			transform.rotate.y += 0.03f;
			mainCamera->SetWorldAffine(transform.scale, transform.rotate, transform.translate);
			mainCamera->Update();
			*(draw->wvpData) = mainCamera->GetWorldViewProjection();
			
			draw->vertexData[0] = mainCamera->GetNdcPos({ -0.5f,-0.5f,0.0f,1.0f });
			draw->vertexData[1] = mainCamera->GetNdcPos({ 0.0f,0.5f,0.0f,1.0f });
			draw->vertexData[2] = mainCamera->GetNdcPos({ 0.5f,-0.5f,0.0f,1.0f });

			///
			/// 描画処理(推定) 
			/// 

			// 描画前処理
			dx->DrawBegin();
			draw->DrawBegin();

			// 描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
			dx->commandList->DrawInstanced(3, 1, 0, 0);

			// 描画後処理
			dx->DrawEnd();
			
		}

	}

#pragma endregion

	// 解放処理

#pragma region リソースリークチェック 01_03 P.5~6

	// 開放処理 P.7
	// P.5のチェックよりも前に行う

	// 生成順と逆の順番で開放していく
	//CloseHandle(fenceEvent);
	//fence->Release();
	//// リソース
	//wvpResource->Release();
	//materialResource->Release();
	//vertexResource->Release();

	//graphicsPipelineState->Release();
	//signatureBlob->Release();
	//if (errorBlob) {
	//	errorBlob->Release();
	//}
	//rootSignature->Release();
	//pixelShaderBlob->Release();
	//vertexShaderBlob->Release();
	////
	//rtvDescriptorHeap->Release();
	//swapChainResources[0]->Release();
	//swapChainResources[1]->Release();
	//swapChain->Release();
	//commandList->Release();
	//commandAllocator->Release();
	//commandQueue->Release();
	//device->Release();
	//useAdapter->Release();
	//dxgiFactory->Release();

	// 解放処理
	delete mainCamera;
	draw->Delete();
	dx->Delete();

#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG

	win->Delete();
	
	/*CloseWindow(hwnd);*/

	// リソースリークチェック P.5
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

#pragma endregion


	return 0;
}
