#include "UIManager.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Editor/GlobalVariables.h"

std::unique_ptr<UIManager> UIManager::instance = nullptr;

UIManager* UIManager::GetInstance() {

	// インスタンスがnullptrであれば生成する
	if (instance == nullptr) {
		instance.reset(new UIManager());
	}

	// インスタンスを返す
	return instance.get();
}

void UIManager::Final() {

	// インスタンスのポインタを破棄する
	instance.reset();

}

void UIManager::Init() {


}

void UIManager::Update() {

	// UIの更新処理
	for (auto& ui : mUIList) {
		ui->Update();
	}

}

void UIManager::Draw() {

	// 2DSprite(画像)の描画前処理
	SpriteAdministrator::GetInstance()->PreDraw();

	// UIの描画処理
	for (auto& ui : mUIList) {
		ui->Draw();
	}

}

void UIManager::CreateUI(const std::string& name){

	// JSON情報に基づいてUIを生成する
	
	// UIのタイプを取得
	auto newUI = mUIFactory->CreateUI(BlackBoard::GetGlobalVariables()->GetStringValue(name, "type"));
	
	// UIを初期化
	newUI->Init(name);
	// スプライトのポインタを取得
	Sprite* newUISprite = newUI->GetSprite();

	newUISprite->SetPos(BlackBoard::GetGlobalVariables()->GetVector2Value(name, "pos"));

	
}
