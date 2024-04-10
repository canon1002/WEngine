#include "ImGuiManager.h"

#ifdef _DEBUG

ImGuiManager* ImGuiManager::GetInstance(){
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(){
	WinAPI* winApi = WinAPI::GetInstance();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	DXGI_SWAP_CHAIN_DESC1 swapChain;
	dxCommon->swapChain->GetDesc1(&swapChain);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// フォントを変更
	ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontFromFileTTF("Resources/Font/M_PLUS_1_Code/MPLUS1Code-VariableFont_wght.ttf", 12.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	io.Fonts->AddFontFromFileTTF("Resources/Font/makinas4/Makinas-4-Flat.otf", 12.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApi->GetHWND());
	ImGui_ImplDX12_Init(dxCommon->device_.Get(),
		swapChain.BufferCount,
		dxCommon->rtvDesc.Format,
		dxCommon->srv_->srvDescriptorHeap.Get(),
		dxCommon->srv_->srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		dxCommon->srv_->srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

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
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInstance()->commandList.Get());
}

void ImGuiManager::ReleseProcess(){
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
#endif // _DEBUG