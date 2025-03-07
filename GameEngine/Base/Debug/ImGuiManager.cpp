#include "ImGuiManager.h"

#ifdef _DEBUG

// インスタンスの初期化
ImGuiManager* ImGuiManager::mInstance = nullptr;

// インスタンスの取得
ImGuiManager* ImGuiManager::GetInstance(){

	// インスタンスがnullptrであれば生成する
	if (mInstance == nullptr) {
		mInstance = new ImGuiManager();
	}
	return mInstance;
}

void ImGuiManager::Init(){
	
	DXGI_SWAP_CHAIN_DESC1 swapChain;
	DirectXCommon::GetInstance()->mSwapChain->GetDesc1(&swapChain);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// フォントを変更
	ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontFromFileTTF("Resources/Font/M_PLUS_1_Code/MPLUS1Code-VariableFont_wght.ttf", 12.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	io.Fonts->AddFontFromFileTTF("Resources/Font/makinas4/Makinas-4-Flat.otf", 12.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//io.Fonts->AddFontFromFileTTF("Resources/Font/makinas4/Makinas-4-Square.otf", 12.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	
	// ImGuiの描画カラーを設定
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHWND());
	uint32_t srvIndex = DirectXCommon::GetInstance()->mSrv->Allocate();
	ImGui_ImplDX12_Init(DirectXCommon::GetInstance()->mDevice.Get(),
		swapChain.BufferCount,
		DirectXCommon::GetInstance()->mRtv->rtvDesc.Format,
		DirectXCommon::GetInstance()->mSrv->mDescriptorHeap.Get(),
		DirectXCommon::GetInstance()->mSrv->GetCPUDescriptorHandle(srvIndex),
		DirectXCommon::GetInstance()->mSrv->GetGPUDescriptorHandle(srvIndex));

	// imnodesを初期化
	ImNodes::CreateContext();

}

void ImGuiManager::Begin(){
	// フレームの先頭でImGuiに、ここからフレームが始まる旨を伝える
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End(){
	// 描画処理に入る前に、ImGui内部のコマンドを生成する
	ImGui::Render();
}

void ImGuiManager::Draw(){
	// ImGuiの描画
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInstance()->mCommandList.Get());
}

void ImGuiManager::Final(){
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ImNodes::DestroyContext();

	// インスタンスの破棄
	delete mInstance;
	mInstance = nullptr;
}
#endif // _DEBUG