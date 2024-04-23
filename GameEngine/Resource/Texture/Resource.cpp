#include "Resource.h"
#include <assert.h>
#include <fstream>
#include <sstream>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Resource
{
	// テクスチャ読み込み
	DirectX::ScratchImage LoadTextrue(const std::string& filePath) {
		// テクスチャファイルを読んでプログラムで使えるようにする
		DirectX::ScratchImage image{};
		std::wstring filePathW = WinAPI::ConvertString(filePath);
		HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));

		// ミニマップの作成
		DirectX::ScratchImage mipImages{};
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));

		// ミニマップ付きのデータを渡す
		return mipImages;
	}

	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metdate) {
		//metadateを基にResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = UINT(metdate.width);
		resourceDesc.Height = UINT(metdate.height);
		resourceDesc.MipLevels = UINT16(metdate.mipLevels);
		resourceDesc.DepthOrArraySize = UINT16(metdate.arraySize);
		resourceDesc.Format = metdate.format;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metdate.dimension);

		// 利用するヒープの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;// 細かい設定を行う
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

		// リソースを生成する
		Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
		HRESULT hr = device->CreateCommittedResource(
			&heapProperties,// Heapの設定
			D3D12_HEAP_FLAG_NONE,// Heapの特殊な設定(特になし)
			&resourceDesc, // Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST,// データ転送される設定
			nullptr,// Crear最適値。使わないのでnullptr
			IID_PPV_ARGS(&resource));// 作成するresourceポインタへのポインタ
		assert(SUCCEEDED(hr));

		return resource;

	}

	//ディープステンシルテクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDeapStencilTextureResource(
		Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height
	) {
		//metadateを基にResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = width;
		resourceDesc.Height = height;
		resourceDesc.MipLevels = 1;
		resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
		resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		// 利用するヒープの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上につくる

		// 深度値のクリア設定
		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth = 1.0f;// 1.0f(最大値)でクリア
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる

		// リソースを生成する
		Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
		HRESULT hr = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));

		return resource;

	}

	// データを転送する
	void UpdateTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
		// Mate情報を取得
		const DirectX::TexMetadata& metadate = mipImages.GetMetadata();
		// 全MipMapについて 
		for (size_t mipLevel = 0; mipLevel < metadate.mipLevels; ++mipLevel) {
			// MipMapを指定して各Imageを取得
			const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
			// Textureに転送
			HRESULT hr = texture->WriteToSubresource(
				UINT(mipLevel),
				nullptr,
				img->pixels,
				UINT(img->rowPitch),
				UINT(img->slicePitch)
			);
			assert(SUCCEEDED(hr));
		}
	}

	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource>UpdateTextureData(
		DirectXCommon* dxCommon, Microsoft::WRL::ComPtr <ID3D12Resource> texture, const DirectX::ScratchImage& mipImages)
	{
		DirectXCommon* dxCommon_ = dxCommon;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		DirectX::PrepareUpload(dxCommon_->device_.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
		uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
		Microsoft::WRL::ComPtr <ID3D12Resource> intermediateResource = dxCommon_->CreateBufferResource(dxCommon_->device_.Get(), intermediateSize);
		UpdateSubresources(dxCommon_->commandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
		// Textureの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPYからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;// Noneにしておく
		barrier.Transition.pResource = texture.Get();// バリアを張る対象の
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;		// 遷移前(現在)のResourceState
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;	// 遷移後のResourceState
		dxCommon_->commandList->ResourceBarrier(1, &barrier);		// TransitionBarrierを張る
		return intermediateResource;
	}



	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
		/// 変数の宣言
		ModelData modelData;// 構築するモデルのデータ
		std::vector<Vec4> positions;//位置
		std::vector<Vec3> normals;//法線
		std::vector<Vec2> texcoords;//テクスチャ座標
		std::string line; // ファイルから読んだ一行を格納する

		/// ファイルを開く
		std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
		assert(file.is_open());

		// assimpを利用する
		Assimp::Importer importer;
		std::string filepath = directoryPath + "/" + filename;
		// オプションを指定
		// aiProcess_FlipWindingOrder -- 三角形の並び順を逆にする --
		// aiProcess_FlipUVs -- UVをフリップする(texcoord.y=1.0f-texcoord.y の処理) --
		const aiScene* scene = importer.ReadFile(filepath.c_str(),
			aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
		assert(scene->HasMeshes()); // Mesh無しは対応しない

		for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
			aiMesh* mesh = scene->mMeshes[meshIndex];
			assert(mesh->HasNormals()); // 法線がないMeshは今回非対応
			assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回非対応

			// Meshの中身の解析
			for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
				aiFace& face = mesh->mFaces[faceIndex];
				assert(face.mNumIndices == 3);//三角形のみサポート

				// Faseの中身を解析
				for (uint32_t element = 0; element < face.mNumIndices; ++element) {
					uint32_t vertexIndex = face.mIndices[element];
					aiVector3D& position = mesh->mVertices[vertexIndex];
					aiVector3D& normal = mesh->mNormals[vertexIndex];
					aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
					VertexData vertex;
					vertex.position = { position.x,position.y,position.z,1.0f };
					vertex.normal = { normal.x,normal.y,normal.z };
					vertex.texcoord = { texcoord.x,texcoord.y };
					// aiProcess_MakeLeftHandedはz*=-1で、右手->左手の変換するので手動で対応
					vertex.position.x *= -1.0f;
					vertex.normal.x *= -1.0f;
					modelData.vertices.push_back(vertex);
				}

			}
		}

		// Materialの解析
		for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
			aiMaterial* material = scene->mMaterials[materialIndex];
			if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				aiString textureFilePath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
				modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
			}
		}

		// モデルデータを返す(assimpのテストのためこの位置に配置)
		return modelData;

		///// モデルデータの構築
		//while (std::getline(file, line)) {
		//	std::string identifier;
		//	std::istringstream s(line);
		//	s >> identifier;// 先頭の識別子を読む

		//	/// 先頭の識別子ごとに応じた処理を行う

		//	// 位置
		//	if (identifier == "v") {
		//		Vec4 position;
		//		s >> position.x >> position.y >> position.z;
		//		position.w = 1.0f;
		//		positions.push_back(position);
		//	}
		//	// テクスチャ座標
		//	else if (identifier == "vt") {
		//		Vec2 texcoord;
		//		s >> texcoord.x >> texcoord.y;
		//		texcoord.y = 1.0f - texcoord.y;
		//		texcoords.push_back(texcoord);
		//	}
		//	// 法線
		//	else if (identifier == "vn") {
		//		Vec3 normal;
		//		s >> normal.x >> normal.y >> normal.z;
		//		normals.push_back(normal);
		//	}
		//	else if (identifier == "f")
		//	{
		//		VertexData triangle[3];
		//		// 面は三角形限定。その他は未対応
		//		for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
		//			std::string vertexDefinition;
		//			s >> vertexDefinition;
		//			// 頂点の要素へのIndexは「位置/UV法線」で格納されているので、分解してIndexを取得する
		//			std::istringstream v(vertexDefinition);
		//			uint32_t elementIndices[3];
		//			for (int32_t element = 0; element < 3; ++element) {
		//				std::string index;
		//				std::getline(v, index, '/');// /区切りでインデックスを読んでいく
		//				elementIndices[element] = std::stoi(index);
		//			}
		//			// 要素へのインデックスから、実際の要素の値を取得しt、頂点を構築する
		//			Vec4 position = positions[elementIndices[0] - 1];
		//			Vec2 texcoord = texcoords[elementIndices[1] - 1];
		//			Vec3 normal = normals[elementIndices[2] - 1];
		//			//VertexData vertex = { position,texcoord,normal };
		//			//modelData.vertices.push_back(vertex);
		//			triangle[faceVertex] = { position,texcoord,normal };
		//		}
		//		// 頂点を逆順にすることで、周り順を逆にする
		//		modelData.vertices.push_back(triangle[2]);
		//		modelData.vertices.push_back(triangle[1]);
		//		modelData.vertices.push_back(triangle[0]);
		//	}
		//	else if (identifier == "mtllib") {
		//		// materialTemplateLibraryファイルの名前を取得する
		//		std::string materialFilename;
		//		s >> materialFilename;
		//		// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
		//		modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		//	}

		//}
		//return modelData;
	}
	
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
		// 中で必要となる変数の宣言
		MaterialData materialData;
		std::string line;// ファイルから読んだ一行を格納する
		std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
		assert(file.is_open());// 開けない場合はとりあえず止める

		// 実際にファイルを読み、MaterialDataを構築していく
		while (std::getline(file, line))
		{
			std::string identifier;
			std::istringstream s(line);
			s >> identifier;

			// identifierに応じた処理
			if (identifier == "map_Kd") {
				std::string textureFilename;
				s >> textureFilename;
				// 連結してファイルパスにする
				materialData.textureFilePath = directoryPath + "/" + textureFilename;

			}


		}

		// MaterialDataを返す
		return materialData;
	}


}