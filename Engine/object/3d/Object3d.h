#pragma once
#include "Math.h"
#include "../worldTransform/WorldTransform.h"
#include "../../resources/Section/Resource.h"
#include "../../object/light/DirectionalLight.h"

class Object3dCommon;
class Model;

class Object3d
{

public: // メンバ関数

	Object3d();
	~Object3d();

	///	初期化
	void Init();
	/// 更新
	void Update();
	/// 描画
	void Draw();

	void CreateTransformationRsource();
	void SetModel(const std::string& filepath);

private: // メンバ変数

	// 外部ポインタ
	MatrixCamera* mainCamera_ = nullptr;
	DirectXCommon* dx_ = nullptr;
	Object3dCommon* object3dCommon_ = nullptr;

	Model* model_ = nullptr;

	Mat44 cameraM, viewM, projectM, pespectiveM, wvpM;
	WorldTransform* worldTransform_ = nullptr;

	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;


};

