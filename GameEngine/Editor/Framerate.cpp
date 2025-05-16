#include "Framerate.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"

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

	// フレームレート設定値
	mFramerate = 60.0f;
	// ゲーム実行速度
	mGameSpeed = 1.0f;
	// バトルスピード
	mBattleSpeed = 1.0f;

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

	ImGui::DragFloat("フレームレート", &mFramerate, 0.0f, 15.0f, 240.0f);
	ImGui::DragFloat("ゲーム全体速度", &mGameSpeed, 0.5f, 0.5f, 3.0f);
	ImGui::DragFloat("バトルスピード", &mBattleSpeed, 0.5f, 0.5f, 2.0f);

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
