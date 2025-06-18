#include "GageUI.h"
#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include "Externals/magic_enum/magic_enum.hpp"

void GageUI::Init(const std::string& name){

	// 名称の設定
	mName = name;

	// スプライトの初期化
	mCurrentGageSprite = std::make_unique<Sprite>();
	mCurrentGageSprite->Init();
	mMaxGageSprite = std::make_unique<Sprite>();
	mMaxGageSprite->Init();
	mDiffGageSprite = std::make_unique<Sprite>();
	mDiffGageSprite->Init();

	//UIを有効化する
	mIsActive = true;

}

void GageUI::Update(){

	// 現在値が減少していたら、差分値を更新する
	if (mCurrentGageSprite->GetScale().x < mDiffGageSprite->GetScale().x) {
		
		// ディレイをかける
		mGageDecreaseDelay -= BlackBoard::GetBattleFPS();

		// ディレイが終了したら、差分値のスケールを更新する
		if(mGageDecreaseDelay <= 0.0f) {
		
			// 差分値のスケールを更新
			Vector2 diffScale = mDiffGageSprite->GetScale();
			diffScale.x -= BlackBoard::CombertBattleFPS(mDiffGageDecreaseTime);
			if (diffScale.x < 0.0f) {
				diffScale.x = 0.0f;
			}
			mDiffGageSprite->SetScale(diffScale);
		}
	
	}


	// スプライトの更新処理
	mCurrentGageSprite->Update();
	mMaxGageSprite->Update();
	mDiffGageSprite->Update();

}

void GageUI::Draw(){

	// スプライトの描画処理 (描画順に注意!!!) 
	
	// 最大値
	mMaxGageSprite->Draw();
	// 差分値
	mDiffGageSprite->Draw();
	// 現在値
	mCurrentGageSprite->Draw();

}

void GageUI::DrawGui(){

#ifdef _DEBUG

	// ImGuiの描画処理

	// UIはまとめて描画・管理するため、Nodeを使用
	if (ImGui::TreeNode(mName.c_str())) {
		ImGui::Text("Scene Name: %s", mSceneName.c_str());

		// UIのデータを保存する
		if (ImGui::Button("Save")) {

			// 保存処理
			this->Save();
		}
		ImGui::Checkbox("Active", &mIsActive);
		mCurrentGageSprite->DrawGui("current");
		mMaxGageSprite->DrawGui("max");
		mDiffGageSprite->DrawGui("diff");
		ImGui::DragFloat("DiffGageDecreaseTime", &mDiffGageDecreaseTime, 0.1f, 0.0f, 10.0f, "%.1f sec");
		ImGui::Text("-----------------------------------------------------------");
		ImGui::TreePop();
	}

#endif // _DEBUG


}

void GageUI::Save(){

	// セーブする内容をグローバル変数管理クラスにわたす

	BlackBoard::GetGlobalVariables()->SetValue(mName, "Current_Pos", mCurrentGageSprite->GetPos());
	BlackBoard::GetGlobalVariables()->SetValue(mName, "Max_Pos", mMaxGageSprite->GetPos());
	BlackBoard::GetGlobalVariables()->SetValue(mName, "Diff_Pos", mDiffGageSprite->GetPos());

	BlackBoard::GetGlobalVariables()->SetValue(mName, "Anchor", mCurrentGageSprite->GetAnchorPoint());
	BlackBoard::GetGlobalVariables()->SetValue(mName, "Anchor", mMaxGageSprite->GetAnchorPoint());
	BlackBoard::GetGlobalVariables()->SetValue(mName, "Anchor", mDiffGageSprite->GetAnchorPoint());

	BlackBoard::GetGlobalVariables()->SetValue(mName, "Current_Scale", mCurrentGageSprite->GetScale());
	BlackBoard::GetGlobalVariables()->SetValue(mName, "Max_Scale", mMaxGageSprite->GetScale());
	BlackBoard::GetGlobalVariables()->SetValue(mName, "Diff_Scale", mDiffGageSprite->GetScale());

	// (ToDo)色・アルファ値の設定

	// (ToDo)現在値テキスト

	// (ToDo)最大値テキスト

	// 差分値減少時のアニメーション時間
	BlackBoard::GetGlobalVariables()->SetValue(mName, "DiffGageDecreaseTime", mDiffGageDecreaseTime);
	
	// UIのデータを保存する
	GlobalVariables::GetInstance()->SaveFile(mName);

	// 保存完了メッセージ
	std::string message = std::format("{}.json saved.", mName);
	MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);

}

void GageUI::Load(){


	// グローバル変数管理クラスからデータを読み込む

	// 座標の設定
	mCurrentGageSprite->SetPos(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Current_Pos"));
	mMaxGageSprite->SetPos(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Max_Pos"));
	mDiffGageSprite->SetPos(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Diff_Pos"));

	// アンカーポイントの設定
	mCurrentGageSprite->SetAnchorPoint(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Anchor"));
	mMaxGageSprite->SetAnchorPoint(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Anchor"));
	mDiffGageSprite->SetAnchorPoint(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Anchor"));

	// スケールの設定
	mCurrentGageSprite->SetScale(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Current_Scale"));
	mMaxGageSprite->SetScale(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Max_Scale"));
	mDiffGageSprite->SetScale(BlackBoard::GetGlobalVariables()->GetVector2Value(mName, "Diff_Scale"));

	// テクスチャの設定(オブジェクト用リソースのUI用テクスチャフォルダ内を探す)
	std::string filePath = BlackBoard::GetGlobalVariables()->GetStringValue(mName, "Texture");
	mCurrentGageSprite->SetTexture("UI/" + filePath + ".png");
	mMaxGageSprite->SetTexture("UI/" + filePath + ".png");
	mDiffGageSprite->SetTexture("UI/" + filePath + ".png");

	// 色・アルファ値の設定
	Vector3 cColor = BlackBoard::GetGlobalVariables()->GetVector3Value(mName, "Current_Color");
	float cAlpha = BlackBoard::GetGlobalVariables()->GetFloatValue(mName, "Alpha");
	mCurrentGageSprite->SetColor(Color(cColor.x, cColor.y, cColor.z, cAlpha));

	Vector3 maxColor = BlackBoard::GetGlobalVariables()->GetVector3Value(mName, "Max_Color");
	float maxAlpha = BlackBoard::GetGlobalVariables()->GetFloatValue(mName, "Alpha");
	mMaxGageSprite->SetColor(Color(maxColor.x, maxColor.y, maxColor.z, maxAlpha));

	Vector3 diffColor = BlackBoard::GetGlobalVariables()->GetVector3Value(mName, "Diff_Color");
	float diffAlpha = BlackBoard::GetGlobalVariables()->GetFloatValue(mName, "Alpha");
	mDiffGageSprite->SetColor(Color(diffColor.x, diffColor.y, diffColor.z, diffAlpha));

	// (ToDo)現在値テキスト

	// (ToDo)最大値テキスト

	// 差分値減少時のアニメーション時間
	mDiffGageDecreaseTime = BlackBoard::GetGlobalVariables()->GetFloatValue(mName, "DiffGageDecreaseTime");
	// ディレイカウント
	mGageDecreaseDelayValue = BlackBoard::GetGlobalVariables()->GetFloatValue(mName, "GageDecreaseDelay");
	mGageDecreaseDelay = 0.0f;
}

void GageUI::SetGageValue(const float& value){

	// 現在値のスケールを更新
	Vector2 currentScale = mCurrentGageSprite->GetScale();
	currentScale.x = mMaxGageSprite->GetScale().x * value;
	mCurrentGageSprite->SetScale(currentScale);

	// ディレイカウントのリセット
	mGageDecreaseDelay = mGageDecreaseDelayValue;

}
