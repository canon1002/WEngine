#pragma once
#include "GameEngine/Resource/Material/Color.h"
#include "GameEngine/Utility/Math/Math.h"
#include <iostream>
#include <wrl.h>
#include <d3d12.h>

struct Material {
	Color color;
	Matrix4x4 uvTransform;
	int32_t enableLighting;
	float shininess;
	float environmentCoefficient;
	float paddding;
};

struct Material2D {
	Color color;
	Matrix4x4 uvTransform;
};

struct MaterialParticle {
	Material* materialData;
	std::string filePath;
	uint32_t srvIndex;
};


/// <summary>
///  マテリアルをモデルクラスから一旦外部に出すためのサンプルコード
/// </summary>
class MaterialExt {

public: // -- 公開 メンバ変数 -- //

	// コンストラクタ
	MaterialExt() = default;
	// デストラクタ
	~MaterialExt();

	// マテリアルリソース 生成
	static std::unique_ptr<MaterialExt> CreateMaterialResource(const std::vector<std::string>& textureFilePaths);

	// マテリアルデータ取得
	Material* GetMaterialData(const int32_t& index = 0) const {
		return mMaterialDatas[index].get();
	}

	// カラー一括設定
	void SetColorAll(const Color& color) {
		for (auto& material : mMaterialDatas) {
			material->color = color;
		}
	}

	// -- Material -- //

	// マテリアルリソース
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mMaterialResources;
	// マテリアルデータ
	std::vector<std::unique_ptr<Material>> mMaterialDatas;
	// テクスチャハンドル
	std::vector<int32_t> mTextureHandles;

};
