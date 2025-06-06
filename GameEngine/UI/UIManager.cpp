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
	for (auto& ui : mUIList) {
		
		// UIが現在のシーンに属しているか確認する
		if (magic_enum::enum_cast<SceneName>(ui->GetSceneName()) !=
			SceneManager::GetInstance()->GetCurrentSceneName()) {

			// 現在のシーン以外のものはスキップ
			continue;
		}
		
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
	for (auto& ui : mUIList) {

		// UIが現在のシーンに属しているか確認する
		if (magic_enum::enum_cast<SceneName>(ui->GetSceneName()) !=
			SceneManager::GetInstance()->GetCurrentSceneName()) {
			
			// 現在のシーン以外のものはスキップ
			continue;
		}

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

	// 更新・描画処理のときに実行シーンのもののみを表示するために
	// magic_enumで文字列に変換したうえで所属シーン名を登録する
	newUI->SetSceneName(magic_enum::enum_name<SceneName>(sceneName).data());
	
	// Jsonから取得した情報をUIに設定する
	newUI->Load();

	// 新規UIを配列に追加
	mUIList.push_back(std::move(newUI));
	
}

void UIManager::DeleteUI(const std::string& name){
	name;
}

void UIManager::DeleteSceneUI(SceneName sceneName){
	sceneName;
	// UIのポインタを取得する
	for (auto& ui : mUIList) {

		// UIが指定したシーンに属しているか確認する
		if (magic_enum::enum_cast<SceneName>(ui->GetSceneName()) != sceneName) {

			// 指定したシーン以外のものはスキップ
			continue;
		}
		
		// UIのポインタを削除する
		//ui.reset();

	}

}

std::shared_ptr<BaseUI> UIManager::GetUIPtr(const std::string name){

	// UIのポインタを取得する
	for (auto& ui : mUIList) {
		// 名前が一致するUIを探す
		if (ui->GetName() == name) {

			// 見つかった場合はポインタを返す
			return ui;
		}
	}

	// 見つからなかった場合はエラー
	assert(0 && "指定したUIのポインタを取得できませんでした ");
	return std::shared_ptr<BaseUI>();
}
