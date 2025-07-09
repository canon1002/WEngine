#include "Material.h"
#include "GameEngine/Resource/Texture/TextureManager.h"


MaterialExt::~MaterialExt() {
	mMaterialDatas.clear();
}

std::unique_ptr<MaterialExt> MaterialExt::CreateMaterialResource(const std::vector<std::string>& textureFilePaths) {

	// 返す用の変数を宣言
	std::unique_ptr<MaterialExt> result = std::make_unique<MaterialExt>();

	// メッシュごとにマテリアルリソースを生成
	for (const auto& textureFilePath : textureFilePaths) {

		// マテリアル用のResourceを作る
		auto resource = DirectXCommon::GetInstance()->
			CreateBufferResource(DirectXCommon::GetInstance()->
				mDevice.Get(), sizeof(Material));

		// マテリアルにデータを書き込む
		std::unique_ptr<Material> materialData = nullptr;
		// 書き込むためのアドレスを取得
		resource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));


		// テクスチャの情報を転送
		int32_t textureHandle = 0;
		if (textureFilePath.empty()) {
			textureHandle = TextureManager::GetInstance()->mDefaultTextureIndex;
		}
		else {
			textureHandle = TextureManager::GetInstance()->LoadTexture(textureFilePath);
		}


		// 色の書き込み・Lightingの無効化
		materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		materialData->enableLighting = true;
		materialData->shininess = 100.0f;
		materialData->uvTransform = MakeIdentity();
		materialData->environmentCoefficient = 0.0f;

		resource->Unmap(0, nullptr);

		// 配列に追加
		result->mMaterialResources.push_back(resource);
		result->mMaterialDatas.push_back(std::move(materialData)); // ※必要ならUnmap前にコピー
		result->mTextureHandles.push_back(textureHandle);
	}

	return result;

}
