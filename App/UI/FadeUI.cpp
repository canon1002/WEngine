#include "FadeUI.h"
#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"
#include "Externals/magic_enum/magic_enum.hpp"

void FadeUI::Init(const std::string& name){
	// 名称の設定
	mName = name;

	// スプライトの初期化
	mSprite = std::make_unique<Sprite>();
	mSprite->Init();

	//UIを有効化する
	mIsActive = true;

	// フェードの時間を1秒に設定
	mFadeDuration = 1.0f;
	// フェードの経過時間を0に
	mElapsedTime = 0.0f;
	// フェードの振る舞いは初期はFadeInにしておく
	mStyle = FadeIn;
	// フェードループ時の上昇フラグを解除
	mIsFadeIn = false;
	// フェードの有効フラグを解除
	mIsFadeActive = false;

}

void FadeUI::Update(){

	// フラグに応じ、フェード処理を行う
	if (mIsFadeActive) {

		// フェードの進行時間が設定時間より小さい場合、フェードを進行する
		if (mElapsedTime < mFadeDuration) {

			// デルタタイムの取得
			// なお、この場合はゲーム内の時間経過速度を考慮するため、
			// ゲーム速度・バトルスピードを乗算したもの使用する
			mElapsedTime += BlackBoard::GetBattleFPS();

			// フェードの進行度を計算
			float progress = mElapsedTime / mFadeDuration;

			// フェードの進行度に応じてアルファ値を設定
			switch (mStyle)
			{
			case FadeIn:

				// フェードインの場合、アルファ値を増加させる
				mSprite->SetAlpha(progress);

				break;
			case FadeOut:

				// フェードアウトの場合、アルファ値を減少させる
				mSprite->SetAlpha(1.0f - progress);

				break;
			case Loop:

				// ループフェードの場合、フラグに応じて増減する
				if (mIsFadeIn) {
					// フェードインの場合、アルファ値を増加させる
					mSprite->SetAlpha(progress);
				}
				else {
					// フェードアウトの場合、アルファ値を減少させる
					mSprite->SetAlpha(1.0f - progress);
				}

				break;
			default:
				break;
			}

		}

		// フェードが完了したら、フラグをオフにする
		if (mElapsedTime >= mFadeDuration) {

			switch (mStyle)
			{
			case FadeIn:

				// フェードインが完了した場合、アルファ値を1に設定
				mSprite->SetAlpha(1.0f);
				// フェードのフラグをオフにする
				mIsFadeActive = false;

				break;
			case FadeOut:

				// フェードアウトが完了した場合、アルファ値を0に設定
				mSprite->SetAlpha(0.0f);
				// フェードのフラグをオフにする
				mIsFadeActive = false;

				break;
			case Loop:

				// ループフェードは、フェードイン・アウトを繰り返すため、再度開始する
				// フェードイン・アウトを切り替える
				mIsFadeIn = !mIsFadeIn; 
				// 経過時間をリセット
				mElapsedTime = 0.0f; 

				break;
			default:
				break;
			}

		}

	}

	// スプライトの更新処理
	mSprite->Update();

}

void FadeUI::Draw(){

	// スプライトの描画処理
	mSprite->Draw();

}

void FadeUI::DrawGui(){


#ifdef _DEBUG

	// ImGuiの描画処理

	// UIはまとめて描画・管理するため、Nodeを使用
	if(ImGui::TreeNode(mName.c_str())) {
		ImGui::Text("Scene Name: %s", mSceneName.c_str());
		ImGui::Text("Fade Type: %s", magic_enum::enum_name(mStyle).data());

		// UIのデータを保存する
		if (ImGui::Button("Save")) {

			// 保存処理
			this->Save();
		}
		ImGui::Checkbox("Active", &mIsActive);
		mSprite->DrawGui();
		ImGui::Text("-----------------------------------------------------------");
		ImGui::TreePop();
	}

#endif // _DEBUG

}

void FadeUI::Save(){

	// セーブする内容をグローバル変数管理クラスにわたす

	// 座標
	GlobalVariables::GetInstance()->SetValue(mName, "Pos", mSprite->GetPos());

	// アンカーポイント
	GlobalVariables::GetInstance()->SetValue(mName, "Anchor", mSprite->GetAnchorPoint());

	// スケール
	GlobalVariables::GetInstance()->SetValue(mName, "Scale", mSprite->GetScale());

	// 色・アルファ値

	// 色を取得し、Vector3にまとめてからセットする
	Vector3 rgb = Vector3(mSprite->GetColor().r, mSprite->GetColor().g, mSprite->GetColor().b);
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

void FadeUI::Load(){

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

void FadeUI::StartFade(FadeStyle style,float duration){

	// フェードを開始するフラグをオンにする
	mIsFadeActive = true;
	// フェードの時間を設定
	mFadeDuration = duration;
	// 経過時間をリセット
	mElapsedTime = 0.0f; 
	// フェードの振る舞いを設定
	mStyle = style;
}
