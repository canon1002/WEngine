#include "TrailEffect.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include  "GameEngine/Base/Debug/ImGuiManager.h"
#include "App/BlackBoard.h"


void TrailEffect::Init()
{
	// カウント
	mPositionSetCountMax = 0.001f;
	mPositionSetCount = 0.0f;

	// 色の設定
	mEffectColor = { 1.0f, 0.1f, 0.1f, 0.8f };
}

void TrailEffect::Update() {

	if (mPositionSetCount > 0.0f) {
		mPositionSetCount -= BlackBoard::GetBattleFPS();
	}
	else if(mPositionSetCount <= 0.0f){
		mPositionSetCount = mPositionSetCountMax;
		mIsPositionSet = true;
	}

	// オブジェクトの更新
	for(std::list<EffectPixels*>::iterator it = mEffectModels.begin(); it != mEffectModels.end();) {
		
		if (!(*it)->GetIsActive()) {
			// オブジェクトをリストから削除する
			it = mEffectModels.erase(it);
			continue;
		}

		// 更新処理
		(*it)->Update();

		// イテレーターを次に進める
		++it;
	}

}

void TrailEffect::Draw() {
	
	// オブジェクトの表示
	for (auto& objects : mEffectModels) {
		objects->Draw();
	}

}

void TrailEffect::DrawGui()
{
#ifdef _DEBUG
	ImGui::Begin("TrailEffect");
	if (ImGui::BeginTable("Particles Table", 8, ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg))
	{
		// テーブルのヘッダーを設定
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Current Time");
		ImGui::TableSetupColumn("Life Time");
		ImGui::TableSetupColumn("Vertex UpLeft");
		ImGui::TableSetupColumn("Vertex UpRight");
		ImGui::TableSetupColumn("Vertex BottomLeft");
		ImGui::TableSetupColumn("Vertex BottomRight");
		ImGui::TableSetupColumn("Alpha");
		ImGui::TableHeadersRow();

		// パーティクルごとの情報を表示
		int id = 0; // IDを管理するカウンタ
		for (std::list<EffectPixels*>::iterator it = mEffectModels.begin(); it != mEffectModels.end(); ++it, ++id)
		{
			ImGui::TableNextRow();

			// ID列
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%d", id);

			// Current Time列
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.2f", (*it)->mActiveCount);

			// Life Time列
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%.2f", (*it)->mActiveCountMax);

			// Vertex 左上列
			ImGui::TableSetColumnIndex(3);
			ImGui::Text("(%.2f, %.2f, %.2f)",(*it)->GetVertices(0).x,(*it)->GetVertices(0).y,(*it)->GetVertices(0).z);
			// Vertex 右上列
			ImGui::TableSetColumnIndex(4);
			ImGui::Text("(%.2f, %.2f, %.2f)", (*it)->GetVertices(1).x, (*it)->GetVertices(1).y, (*it)->GetVertices(1).z);
			// Vertex 左下列
			ImGui::TableSetColumnIndex(5);
			ImGui::Text("(%.2f, %.2f, %.2f)", (*it)->GetVertices(2).x, (*it)->GetVertices(2).y, (*it)->GetVertices(2).z);
			// Vertex 右下列
			ImGui::TableSetColumnIndex(6);
			ImGui::Text("(%.2f, %.2f, %.2f)", (*it)->GetVertices(3).x, (*it)->GetVertices(3).y, (*it)->GetVertices(3).z);

			// Alpha列
			ImGui::TableSetColumnIndex(7);
			ImGui::Text("%.2f", (*it)->GetColor().a);
		}

		ImGui::EndTable();
	}
	
	
	ImGui::End();
#endif // _DEBUG
}

void TrailEffect::Create(const WorldTransform& top, const WorldTransform& bottom)
{

	// 剣先及び根本の座標を登録する
	TwoPointVertex* newPoints = new TwoPointVertex();
	newPoints->top = top.GetWorldPosition();
	newPoints->bottom = bottom.GetWorldPosition();

	// 座標を配列に追加
	mVertices.push_back(newPoints);

	// 追加したらフラグを解除
	mIsPositionSet = false;

	// 登録された座標の数が2つ以上であれば生成処理。2つ未満なら早期リターンする
	if (mVertices.size() < 2) { return; }

	// 上下各2点の座標が登録されていたらオブジェクトを生成
	EffectPixels* newEffectModel = new EffectPixels();
	newEffectModel->Init();
	newEffectModel->SetVertices(mVertices[0]->top, mVertices[1]->top, mVertices[0]->bottom, mVertices[1]->bottom);
	newEffectModel->SetColor(mEffectColor);

	// 前に登録した頂点を配列から削除
	mVertices.erase(mVertices.begin());

	// オブジェクトを配列に追加
	mEffectModels.push_back(newEffectModel);
}

