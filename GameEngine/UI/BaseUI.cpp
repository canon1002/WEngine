#include "BaseUI.h"
#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"


void BaseUI::Final() {

	// スプライトの終了処理
	mSprite.reset();
}


void BaseUI::Init(const std::string& name){

	// 名称の設定
	mName = name;

	// スプライトの初期化
	mSprite = std::make_unique<Sprite>();
	mSprite->Init();

	//UIを有効化する
	mIsActive = true;
}

void BaseUI::Update(){

	// スプライトの更新処理
	mSprite->Update();

}

void BaseUI::Draw(){

	// スプライトの描画処理
	mSprite->Draw();

}
