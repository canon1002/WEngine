#include "Resource.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "GameEngine/Append/Animation/Skinning/Skinnig.h"

#include "GameEngine/Object/Model/Model.h"
#include "GameEngine/Object/Model/MultiModel.h"

namespace Resource
{
	// テクスチャ読み込み
	DirectX::ScratchImage LoadTextrue(const std::string& filePath) {
		// テクスチャファイルを読んでプログラムで使えるようにする
		DirectX::ScratchImage image{};
		const std::string& fullPath = "Resources/objs/" + filePath;
		std::wstring filePathW = WinAPI::ConvertString(fullPath);
		// .ddsで終わっていれば.ddsとみなす。別の方法も存在するらしい
		HRESULT hr;
		if (filePathW.ends_with(L".dds")) {
			hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		}
		else {
			hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		}
		assert(SUCCEEDED(hr));

		// ミニマップの作成
		DirectX::ScratchImage mipImages{};
		// 圧縮データかどうか確認する
		if (DirectX::IsCompressed(image.GetMetadata().format)) {
			// 圧縮データであればそのまま使うのでmoveする
			mipImages = std::move(image);
		}
		else {
			hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		}
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
		HRESULT hr;
		hr = device->CreateCommittedResource(
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
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		// 深度値のクリア設定
		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth = 1.0f;// 1.0f(最大値)でクリア
		depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる

		// リソースを生成する
		Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
		HRESULT hr;
		hr = device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));

		return resource;

	}

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device,
		int32_t width, int32_t height, DXGI_FORMAT format, const Vector4& clearColor) {

		//Resourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = width;
		resourceDesc.Height = height;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = format;
		resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		// 利用するヒープの設定
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に生成

		// 色の設定
		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = format;
		clearValue.Color[0] = clearColor.x;
		clearValue.Color[1] = clearColor.y;
		clearValue.Color[2] = clearColor.z;
		clearValue.Color[3] = clearColor.w;

		// Resourceの生成
		Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
		HRESULT hr;
		hr =
			device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				//D3D12_RESOURCE_STATE_RENDER_TARGET,
				&clearValue,
				IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));

		// 作成したら値を返す
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
			HRESULT hr;
			hr = texture->WriteToSubresource(
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
		DirectXCommon* mDxCommon = dxCommon;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		DirectX::PrepareUpload(mDxCommon->device_.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
		uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
		Microsoft::WRL::ComPtr <ID3D12Resource> intermediateResource = mDxCommon->CreateBufferResource(mDxCommon->device_.Get(), intermediateSize);
		UpdateSubresources(mDxCommon->commandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
		// Textureの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPYからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;// Noneにしておく
		barrier.Transition.pResource = texture.Get();// バリアを張る対象の
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;		// 遷移前(現在)のResourceState
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;	// 遷移後のResourceState
		mDxCommon->commandList->ResourceBarrier(1, &barrier);		// TransitionBarrierを張る
		return intermediateResource;
	}



	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename) {
		/// 変数の宣言
		ModelData modelData;// 構築するモデルのデータ
		std::vector<Vector4> positions;//位置
		std::vector<Vector3> normals;//法線
		std::vector<Vec2> texcoords;//テクスチャ座標
		std::string line; // ファイルから読んだ一行を格納する

		// -- ファイルを開く -- //
		const std::string& forwardPath = "Resources/objs/";
		const std::string& fullPath = forwardPath + directoryPath + "/" + filename;
		std::ifstream file(fullPath); // ファイルを開く
		assert(file.is_open());

		// assimpを利用する
		Assimp::Importer importer;

		// オプションを指定
		// aiProcess_FlipWindingOrder -- 三角形の並び順を逆にする --
		// aiProcess_FlipUVs -- UVをフリップする(texcoord.y=1.0f-texcoord.y の処理) --
		const aiScene* scene = importer.ReadFile(fullPath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
		assert(scene->HasMeshes()); // Mesh無しは対応しない

		// SceneのRootNodeを読んでシーン全体の階層構造を作り上げる
		modelData.rootNode = ReadNode(scene->mRootNode);

		// -- Meshの解析 --// 

		for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
			aiMesh* mesh = scene->mMeshes[meshIndex];
			assert(mesh->HasNormals()); // 法線がないMeshは今回非対応
			assert(mesh->HasTextureCoords(0)); // TexcoordがないMeshは今回非対応
			modelData.vertices.resize(mesh->mNumVertices);// 最初に頂点数分のメモリを確保しておく
			// Meshの中身の解析
			for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				// 右手系から左手系に変換を行う
				modelData.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
				modelData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
				modelData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
			}

			// Indexの解析
			for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
				aiFace& face = mesh->mFaces[faceIndex];
				assert(face.mNumIndices == 3);

				// Faseの中身を解析
				for (uint32_t element = 0; element < face.mNumIndices; ++element) {
					uint32_t vertexIndex = face.mIndices[element];
					modelData.indices.push_back(vertexIndex);
				}

			}

			// -- SkinClusterを構築するデータを取得 -- //

			for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
				// MeshからBoneを取得
				aiBone* bone = mesh->mBones[boneIndex];
				// BoneからJointの名前を取得
				std::string jointName = bone->mName.C_Str();
				// モデルのSkinClusterDataからJointWeightDataを取得
				JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

				// BindPose(バインドポーズ)行列を取得する
				aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
				aiVector3D scale, translation;
				aiQuaternion rotation;
				bindPoseMatrixAssimp.Decompose(scale, rotation, translation);
				// asiimpの行列を基本の float4x4行列 に変換する
				// この際座標系を修正しておく
				Matrix4x4 bindPoseMatrix = MakeAffineMatrix(
					{ scale.x,scale.y,scale.z },
					{ rotation.x,-rotation.y,-rotation.z,rotation.w },
					{ -translation.x,translation.y,translation.z }
				);
				// 逆行列を格納
				jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

				// Weight情報を取り出す
				for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
					jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
				}

			}
		}

		// -- Materialの解析 -- //


		for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
			aiMaterial* material = scene->mMaterials[materialIndex];
			if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				aiString textureFilePath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
				modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
			}
		}

		// モデルデータを返す
		return modelData;
	}

	MultiModelData LoadMultiModelFile(const std::string& directoryPath, const std::string& filename)
	{
		/// 変数の宣言
		MultiModelData modelData;// 構築するモデルのデータ
		std::vector<Vector4> positions;//位置
		std::vector<Vector3> normals;//法線
		std::vector<Vec2> texcoords;//テクスチャ座標
		std::string line; // ファイルから読んだ一行を格納する

		// -- ファイルを開く -- //
		const std::string& forwardPath = "Resources/objs/";
		const std::string& fullPath = forwardPath + directoryPath + "/" + filename;
		std::ifstream file(fullPath); // ファイルを開く
		assert(file.is_open());

		// assimpを利用する
		Assimp::Importer importer;

		// オプションを指定
		// aiProcess_FlipWindingOrder -- 三角形の並び順を逆にする --
		// aiProcess_FlipUVs -- UVをフリップする(texcoord.y=1.0f-texcoord.y の処理) --
		const aiScene* scene = importer.ReadFile(fullPath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
		assert(scene->HasMeshes()); // Mesh無しは対応しない

		// SceneのRootNodeを読んでシーン全体の階層構造を作り上げる
		modelData.rootNode = ReadNode(scene->mRootNode);

		// -- Meshの解析 --// 

		for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {

			Mesh mesh;

			aiMesh* aiMesh = scene->mMeshes[meshIndex];
			assert(aiMesh->HasNormals()); // 法線がないMeshは今回非対応
			assert(aiMesh->HasTextureCoords(0)); // TexcoordがないMeshは今回非対応
			mesh.vertices.resize(aiMesh->mNumVertices);// 最初に頂点数分のメモリを確保しておく
			// Meshの中身の解析
			for (uint32_t vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; ++vertexIndex) {
				aiVector3D& position = aiMesh->mVertices[vertexIndex];
				aiVector3D& normal = aiMesh->mNormals[vertexIndex];
				aiVector3D& texcoord = aiMesh->mTextureCoords[0][vertexIndex];
				// 右手系から左手系に変換を行う
				mesh.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
				mesh.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
				mesh.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
			}

			// Indexの解析
			for (uint32_t faceIndex = 0; faceIndex < aiMesh->mNumFaces; ++faceIndex) {
				aiFace& face = aiMesh->mFaces[faceIndex];
				assert(face.mNumIndices == 3);

				// Faseの中身を解析
				for (uint32_t element = 0; element < face.mNumIndices; ++element) {
					uint32_t vertexIndex = face.mIndices[element];
					mesh.indices.push_back(vertexIndex);
				}

			}

			// -- SkinClusterを構築するデータを取得 -- //

			for (uint32_t boneIndex = 0; boneIndex < aiMesh->mNumBones; ++boneIndex) {
				// MeshからBoneを取得
				aiBone* bone = aiMesh->mBones[boneIndex];
				// BoneからJointの名前を取得
				std::string jointName = bone->mName.C_Str();
				// モデルのSkinClusterDataからJointWeightDataを取得
				JointWeightData& jointWeightData = mesh.skinClusterData[jointName];

				// BindPose(バインドポーズ)行列を取得する
				aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
				aiVector3D scale, translation;
				aiQuaternion rotation;
				bindPoseMatrixAssimp.Decompose(scale, rotation, translation);
				// asiimpの行列を基本の float4x4行列 に変換する
				// この際座標系を修正しておく
				Matrix4x4 bindPoseMatrix = MakeAffineMatrix(
					{ scale.x,scale.y,scale.z },
					{ rotation.x,-rotation.y,-rotation.z,rotation.w },
					{ -translation.x,translation.y,translation.z }
				);
				// 逆行列を格納
				jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

				// Weight情報を取り出す
				for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
					jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
				}

			}

			mesh.materialIndex = aiMesh->mMaterialIndex;
			modelData.meshes.push_back(mesh);
		}

		// -- Materialの解析 -- //


		for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
			aiMaterial* aiMaterial = scene->mMaterials[materialIndex];
			if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				MultiMaterial material;
				aiString textureFilePath;

				// テクスチャのファイルパスを取得
				aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
				material.filepath= directoryPath + "/" + textureFilePath.C_Str();
				
				// マテリアルの配列に追加する
				modelData.materials.push_back(material);
			}
		}

		// モデルデータを返す
		return modelData;
	}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	/// 変数の宣言
	ModelData modelData;// 構築するモデルのデータ
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vec2> texcoords;//テクスチャ座標
	std::string line; // ファイルから読んだ一行を格納する

	/// ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open());


	/// モデルデータの構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 先頭の識別子を読む

		/// 先頭の識別子ごとに応じた処理を行う

		// 位置
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		}
		// テクスチャ座標
		else if (identifier == "vt") {
			Vec2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		// 法線
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (identifier == "f")
		{
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');// /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのインデックスから、実際の要素の値を取得しt、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vec2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position,texcoord,normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}
			// 頂点を逆順にすることで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}

	}
	return modelData;
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

Animation LoadAnmation(const std::string& directoryPath, const std::string& filePath) {
	Animation animation;
	Assimp::Importer impoter;
	std::string fullPath = "Resources/objs/" + directoryPath + "/" + filePath;
	const aiScene* scene = impoter.ReadFile(fullPath.c_str(), 0);
	assert(scene->mNumAnimations != 0);// アニメーションがなければ停止

	// 最初のアニメーションだけ採用 そのうち複数対応する
	aiAnimation* animationAssimp = scene->mAnimations[0];
	// 時間の単位を秒に変換
	// mTicksPerSecond -- 周波数 -- 
	// mDuration -- mTicksPerSecondで指定された周波数における長さ -- 
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	// -- ここから "NodeAnimation" を解析する -- //

	// assimpでは個々のNodeのAnimationをchannelとよんでいるのでchannelを回してNodeAnimationの情報を取ってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		// AniamtionのNameを取得する
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

		// -- キーフレームごとの情報を取得する -- //

		// translation
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe{};
			// 時間の単位を秒に変換する
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			// translationを取得する
			// 左手座標系 -> 右手座標系 (xを反転させる)
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.translation.push_back(keyframe);
		}

		// rotation
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe{};
			// 時間の単位を秒に変換する
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			// rotationを取得する
			// 左手座標系 -> 右手座標系 (yとzを反転させる)
			keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z ,keyAssimp.mValue.w };
			nodeAnimation.rotation.push_back(keyframe);
		}

		// scale
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe{};
			// 時間の単位を秒に変換する
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			// scaleを取得する
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.scale.push_back(keyframe);
		}
	}

	// 解析が完了したら結果を返す
	return animation;
}
}