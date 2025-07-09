#include "MaterialManager.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/TextureManager.h"

std::unique_ptr<MaterialManager> MaterialManager::instance = nullptr;

MaterialManager* MaterialManager::GetInstance() {

	// インスタンスが生成されていない場合は、新しいインスタンスを作成
	if (!instance) {
		instance = std::make_unique<MaterialManager>();
	}
	return instance.get();
}

void MaterialManager::Final() {
	// マテリアルの解放
	mMaterials.clear();
	// インスタンスの解放
	instance.reset();
	instance = nullptr;
}


MaterialExt* MaterialManager::CreateMaterial(const std::string& name,
	const std::vector<std::string>& textureFilePaths){

	// 読み込み済みのマテリアルをチェック
	if (mMaterials.contains(name)) {
		// 読み込み済みモデルを戻り値としてreturn
		return mMaterials.at(name).get();
	}

	// 読み込み済みモデルがない場合は新規に生成する

	// モデルの生成とファイル読み込み
	std::unique_ptr<MaterialExt> material = MaterialExt::CreateMaterialResource(textureFilePaths);
	// 初期化
	//material->Init();

	// モデルをmapコンテナに格納
	mMaterials.insert(std::make_pair(name, std::move(material)));

	// 生成したモデルを戻り値として返す
	return mMaterials.at(name).get();
}

