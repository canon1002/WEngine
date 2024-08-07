#include "Framerate.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

Framerate* Framerate::instance = nullptr;

Framerate* Framerate::GetInstance()
{
	if (instance == nullptr) {
		instance = new Framerate();
	}
	return instance;
}

void Framerate::Init()
{

	//コマ送り設定
	mIsFrameAdvance = false;
	// コマ送りフレーム数
	mStepFlameCount = 1;
	// 現在の経過フレーム
	mStepFlame = 0;

}

void Framerate::Update()
{
	// 開始時にコマ送り上限まで達していたら初期化する
	if (mStepFlame >= mStepFlameCount) {
		mStepFlame = 0;
	}

}

void Framerate::DrawGui(){
#ifdef _DEBUG

	ImGui::Begin("Flamerate");
	ImGui::Checkbox("コマ送り", &mIsFrameAdvance);
	if (ImGui::Button(" フレーム数加算")) { mStepFlame++; }
	ImGui::DragInt("経過フレーム数", &mStepFlame);
	ImGui::DragInt("コマ送りフレーム数", &mStepFlameCount);
	ImGui::End();

#endif // _DEBUG
}

bool Framerate::IsActiveFrame()
{
	if (mStepFlame == mStepFlameCount) {
		return true;
	}
	return false;
}
