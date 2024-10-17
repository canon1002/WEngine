#include "LevelEditor.h"
#include "Editor/ImGui/ImGuiManager.h"
#include "Resource/Model/ModelManager.h"

LevelEditor* LevelEditor::instance = nullptr;

LevelEditor* LevelEditor::GetInstance()
{
	if (instance == nullptr) {
		instance = new LevelEditor();
	}
	return instance;
}

void LevelEditor::CheckLevelEditorFile(){

	mObjects.clear();

	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/LevelEditor/";

	// 読み込むJSONファイルのフルパスを合成する
	std::string filepath = kDirectoryPath + "LevelData" + ".json";
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込み用に開く
	ifs.open(filepath);
	// エラーハンドリング
	if (ifs.fail()) {
		std::string message = "Filed open LevelEditor file for read.";
		MessageBoxA(nullptr, message.c_str(), "LevelEditor", 0);
		assert(0);
	}

	json root;
	// json文字列からjsonのデータ構造を展開
	ifs >> root;

	// ファイルを閉じる
	ifs.close();

	// 正しいレベルデータファイルかチェック
	assert(root.is_object());
	assert(root.contains("name"));
	assert(root["name"].is_string());

	// "name"を文字列として取得
	std::string name = root["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	// レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();

	// "objects"の全オブジェクトを走査
	for (json& object : root["objects"]) {
		assert(object.contains("type"));
		// 種別を取得
		std::string type = object["type"].get<std::string>();

		// MESH
		if (type.compare("MESH") == 0) {
			// 要素追加
			levelData->objects.emplace_back(LevelData::ObjectData{});
			// 追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData->objects.back();

			if (object.contains("file_name")) {
				// ファイル名
				objectData.fileName = object["file_name"];
			}

			if (object.contains("name")) {
				// ファイル名
				objectData.objectName = object["name"];
			}

			// トランスフォーム読み込み
			json& transform = object["transform"];
			// 平行移動
			objectData.worldTransform.translation.x = (float)transform["translation"][0];
			objectData.worldTransform.translation.y = (float)transform["translation"][2];
			objectData.worldTransform.translation.z = (float)transform["translation"][1];
			// 回転
			objectData.worldTransform.rotation.x = -GetRadian((float)transform["rotate"][0]);
			objectData.worldTransform.rotation.y = -GetRadian((float)transform["rotate"][2]);
			objectData.worldTransform.rotation.z = -GetRadian((float)transform["rotate"][1]);
			// スケーリング
			objectData.worldTransform.scale.x = (float)transform["scaling"][0];
			objectData.worldTransform.scale.y = (float)transform["scaling"][2];
			objectData.worldTransform.scale.z = (float)transform["scaling"][1];

		}

	}

	// レベルデータからオブジェクトを生成&配置
	for (auto& objectData : levelData->objects) {
		// モデルを指定して3Dオブジェクトを生成
		Object3d* newObject = new Object3d();
		newObject->Init(objectData.objectName);
		// ファイル拡張子を指定 とりあえずgltfのみ読み込む
		std::string extension = ".gltf";
		const std::string& modelFilepath = objectData.fileName + extension;
		ModelManager::GetInstance()->LoadModel("MapObjects", modelFilepath);
		newObject->SetModel(modelFilepath);
		// ワールド座標を設定
		newObject->mWorldTransform.translation = objectData.worldTransform.translation;
		newObject->mWorldTransform.rotation = objectData.worldTransform.rotation;
		newObject->mWorldTransform.scale = objectData.worldTransform.scale;
		// 配列に登録
		mObjects.push_back(newObject);
	}

}


void LevelEditor::Update(){

#ifdef _DEBUG
	// メニューバーを表示する
	if (!ImGui::Begin("LevelEditor", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) return;

#endif // _DEBUG

	for (auto& object : mObjects) 
	{
		object->Update();
	}

#ifdef _DEBUG
	ImGui::EndMenuBar();
	ImGui::End();
#endif // _DEBUG

}

void LevelEditor::Draw(Camera camera){

	// スキニングなしの3Dオブジェクトを表示
	ModelManager::GetInstance()->PreDraw();
	for (auto& object : mObjects) {
		object->Draw(camera);
	}

}


void LevelEditor::SetTextureCubeMap(const int32_t& trextureHandle)
{
	// テクスチャマップのハンドルをセット
	for (auto& object : mObjects) {
		object->GetModel()->SetCubeTexture(trextureHandle);
	}
}

float LevelEditor::GetRadian(float degree)
{
	float radian = degree * (3.14f / 180.0f);

	return radian;
}
