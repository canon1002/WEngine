#include "BaseUI.h"
#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"

BaseUI::~BaseUI(){

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

void BaseUI::DrawGui(){

#ifdef _DEBUG

	// ImGuiの描画処理

	// UIはまとめて描画・管理するため、Childウィンドウを使用
	ImGui::BeginChild(mName.c_str());
	ImGui::Text("Scene Name: %s", mSceneName.c_str());
	
	// UIのデータを保存する
	if (ImGui::Button("Save")) {

		// 保存処理
		this->Save();
	}
	ImGui::Checkbox("Active", &mIsActive);
	mSprite->DrawGui();
	ImGui::Text("-----------------------------------------------------------");
	ImGui::EndChild();


#endif // _DEBUG

}

void BaseUI::Save(){

	// セーブする内容をグローバル変数管理クラスにわたす

	// 座標
	GlobalVariables::GetInstance()->SetValue(mName, "Pos", mSprite->GetPos());
	
	// アンカーポイント
	GlobalVariables::GetInstance()->SetValue(mName, "Anchor", mSprite->GetAnchorPoint());
	
	// スケール
	GlobalVariables::GetInstance()->SetValue(mName, "Scale", mSprite->GetScale());
	
	// 色・アルファ値
	
	// 色を取得し、Vector3にまとめてからセットする
	Vector3 rgb = Vector3(mSprite->GetColor().r,mSprite->GetColor().g,mSprite->GetColor().b);
	GlobalVariables::GetInstance()->SetValue(mName, "Color", rgb);
	
	// アルファ値を取得し、float型でセットする
	float alpha = mSprite->GetColor().a;
	GlobalVariables::GetInstance()->SetValue(mName, "Alpha", alpha);

	// UIのデータを保存する
	GlobalVariables::GetInstance()->SaveFile(mName);

	// 保存完了メッセージ
	std::string message = std::format("{}.json saved.", mName);
	MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);

}

void BaseUI::Load(){

	// 座標設定
	mSprite->SetPos(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Pos"));

	// アンカーポイントの設定
	mSprite->SetAnchorPoint(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Anchor"));

	// スケールの設定
	mSprite->SetScale(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Scale"));

	// テクスチャの設定(オブジェクト用リソースのUI用テクスチャフォルダ内を探す)
	std::string filePath = BlackBoard::GetGlobalVariables()->GetStringValue(mName, "Texture");
	mSprite->SetTexture("UI/" + filePath + ".png");

	// 色・アルファ値の設定(色・透明度を取得し、まとめてからセットする)
	Vector3 color = BlackBoard::GetGlobalVariables()->GetVector3Value(mName, "Color");
	float alpha = BlackBoard::GetGlobalVariables()->GetFloatValue(mName, "Alpha");
	mSprite->SetColor(Color(color.x, color.y, color.z, alpha));

}
