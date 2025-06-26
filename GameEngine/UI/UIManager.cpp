#include "UIManager.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Scene/SceneManager.h"
#include "Externals/magic_enum/magic_enum.hpp"

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

	// ImGuiの更新処理
#ifdef _DEBUG
	ImGui::Begin("UIManager");
#endif // _DEBUG

	// UIの更新処理
	// 現在のシーンに応じてUIを更新する
	for (auto& ui : mUIMap[SceneManager::GetInstance()->GetCurrentSceneName()]) {
		
		// スプライトの更新処理
		ui->Update();
		// ImGuiの描画処理
		ui->DrawGui();

	}

#ifdef _DEBUG
	ImGui::End();
#endif // _DEBUG

}

void UIManager::Draw() {

	// 2DSprite(画像)の描画前処理
	SpriteAdministrator::GetInstance()->PreDraw();

	// UIの描画処理
	// 現在のシーンに応じてUIを描画する
	for (auto& ui : mUIMap[SceneManager::GetInstance()->GetCurrentSceneName()]) {

		// UIが有効化されていない場合は描画しない
		if(!ui->GetActive()) {
			continue;
		}

		// スプライトの描画処理
		ui->Draw();
	}

}

void UIManager::CreateUI(const std::string& name,SceneName sceneName){

	// JSON情報に基づいてUIを生成する
	
	// UIのタイプを取得
	auto newUI = mUIFactory->CreateUI(
		BlackBoard::GetGlobalVariables()->GetStringValue(name, "Type"));
	
	// UIを初期化
	newUI->Init(name);
	
	// Jsonから取得した情報をUIに設定する
	newUI->Load();

	// 新規UIを配列に追加
	mUIMap[sceneName].push_back(std::move(newUI));
	
}

void UIManager::DeleteUI(const std::string& name){
	name;
}

void UIManager::DeleteSceneUI(SceneName sceneName){
	
	// 指定されたシーンのUIを全て削除する
	// 該当シーンごと削除
	mUIMap.erase(sceneName);

}

BaseUI* UIManager::GetUIPtr(const std::string name){

	// UIのポインタを取得する
	// シーンは関係なく、全てのUIを検索する
	for (auto& scene : mUIMap) {
		for (auto& ui : scene.second) {

			// UIの名前が一致するか確認する
			if (ui->GetName() == name) {
				return ui.get();
			}

		}
	}

	// 見つからなかった場合はエラー
	assert(0 && "指定したUIのポインタを取得できませんでした ");
	return new BaseUI();
}
