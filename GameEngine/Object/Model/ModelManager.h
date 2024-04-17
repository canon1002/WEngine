#pragma once
#include <unordered_map>
#include <sstream>
#include "Model.h"

class ModelManager{

	// コピーコンストラクタと演算子オーバーロードの禁止
	ModelManager(const ModelManager& obj) = delete;
	ModelManager& operator=(const ModelManager& obj) = delete;

public:

	ModelManager() = default;
	~ModelManager() = default;

	void Finalize();
	void Initialize(DirectXCommon* dxCommon, CameraCommon* camera);
	void LoadModel(const std::string& filepath);
	Model* FindModel(const std::string filepath);

	void PostDraw();

private:

	DirectXCommon* dxCommon_ = nullptr;
	CameraCommon* camera_ = nullptr;

	// モデルデータ
	std::unordered_map<std::string, std::unique_ptr<Model>> models;

};

