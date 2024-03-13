#include "ModelManager.h"


ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ModelManager;
	}
	return instance;
}

void ModelManager::Finalize() {
	delete instance;
	instance = nullptr;
}


void ModelManager::Initialize(){



}

void ModelManager::LoadModel(const std::string& filepath)
{
	// 読み込み済みのモデルを検索
	if (models.contains(filepath)) {
		// 読み込み済みの場合は早期リターンする
		return;
	}

	// モデルの生成とファイル読み込み
	std::unique_ptr<Model> model = std::make_unique<Model>();
	//初期化
	model->Initialize("Resources/objs", filepath);

	// モデルをmapコンテナに格納
	models.insert(std::make_pair(filepath, std::move(model)));

}

Model* ModelManager::FindModel(const std::string filepath)
{
	// 読み込み済みモデルを検索
	if (models.contains(filepath)) {
		// 読み込み済みモデルを戻り値としてreturn
		return models.at(filepath).get();
	}
	// ファイル名不一致の場合はnullptrを返す
	return nullptr;
}


