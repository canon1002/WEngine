#pragma once
#include <unordered_map>
#include <sstream>
#include "Model.h"

class ModelManager
{
private: // シングルトン関連処理

	static ModelManager* instance;

	ModelManager() = default;
	~ModelManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	ModelManager(const ModelManager& obj) = delete;
	ModelManager& operator=(const ModelManager& obj) = delete;

public:

	static ModelManager* GetInstance();
	void Finalize();
	void Initialize();
	void LoadModel(const std::string& filepath);
	Model* FindModel(const std::string filepath);

private:

	// モデルデータ
	std::unordered_map<std::string, std::unique_ptr<Model>> models;

};

