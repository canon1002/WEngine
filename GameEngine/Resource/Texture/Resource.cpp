#include "Resource.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <algorithm>

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
		HRESULT hr = 
			device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			//D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
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

			for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones;++boneIndex) {
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
				Matrix4x4 bindPoseMatrix = MakeAffineMatrix({ scale.x,scale.y,scale.z },
					{	rotation.x,-rotation.y,rotation.z,rotation.w },{ -translation.x,translation.y,translation.z }
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

	Node ReadNode(aiNode* node) {
		Node result;
		aiVector3D scale, translate;
		aiQuaternion rotate;
		node->mTransformation.Decompose(scale, rotate, translate);// assimpの行列からSRTを抽出する関数を利用
		rotate.w = 1.0f; // w = 1.0f でないと行けなかったはずなので、念のために代入しておく
		result.transform.scale_ = { scale.x,scale.y ,scale.z };// スケールはそのまま
		result.transform.rotation_ = { rotate.x,-rotate.y ,-rotate.z,rotate.w };// x軸を反転、更に回転方向が逆なので軸を反転させる
		result.transform.translation_ = { -translate.x,translate.y ,translate.z };// x軸を回転
		// ↓ココがおかしい Matrix4x4のm[3][3] が 1.0fにならない
		result.localMatrix = MakeAffineMatrix(result.transform.scale_, result.transform.rotation_, result.transform.translation_);
		
		assert(result.localMatrix.m[3][3] == 1.0f);
		result.name = node->mName.C_Str();// Node名を格納
		
		result.children.resize(node->mNumChildren); // 子供の数だけ確保
		for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
			// 再帰的に読んで階層構造を作っていく
			result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
		}
		return result;
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

namespace Animations {

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {
		// キーがないものは返す値がわからないのでだめ
		assert(!keyframes.empty());
		// キーが1つか、時刻がキーフレーム前なら最初の値とする
		if (keyframes.size() == 1 || time <= keyframes[0].time) {
			return keyframes[0].value;
		}

		for (size_t index = 0; index < keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			// indexとnextIndexの2つの値のkeyframeを取得して範囲内に時刻があるかお判定
			if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
				// 範囲内を補間する
				float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
				return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
			}
		}
		// ここまできた場合は一番後ろの時刻よりも後ろなので最後の値を返す
		return (*keyframes.rbegin()).value;
	}

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
		// キーがないものは返す値がわからないのでだめ
		assert(!keyframes.empty());
		// キーが1つか、時刻がキーフレーム前なら最初の値とする
		if (keyframes.size() == 1 || time <= keyframes[0].time) {
			return keyframes[0].value;
		}

		for (size_t index = 0; index < keyframes.size() - 1; ++index) {
			size_t nextIndex = index + 1;
			// indexとnextIndexの2つの値のkeyframeを取得して範囲内に時刻があるかお判定
			if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
				// 範囲内を補間する
				float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
				return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
			}
		}
		// ここまできた場合は一番後ろの時刻よりも後ろなので最後の値を返す
		return (*keyframes.rbegin()).value;
	}

	Skeleton CreateSkeleton(const Node& rootNode) {
		Skeleton skeleton;
		skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

		// 名前とindexのマッピングを行いアクセスしやすくする
		for (const Joint& joint : skeleton.joints) {
			skeleton.jointMap.emplace(joint.name, joint.index);
		}
		// 作成したら一度更新しておく
		UpdateSkeleton(skeleton);

		return skeleton;
	}

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
		Joint joint;
		joint.name = node.name;
		joint.localMatrix = node.localMatrix;
		joint.skeletonSpaceMatrix = MakeIdentity();
		joint.transform = node.transform;
		joint.index = int32_t(joints.size());// 現在登録されてるindexに
		joint.parent = parent;
		joints.push_back(joint);// skeleton	のJoint列に追加
		for (const Node& child : node.children) {
			// 子jointを作成し、そのIndexを登録
			int32_t childIndex = CreateJoint(child, joint.index, joints);
			joints[joint.index].childlen.push_back(childIndex);
		}
		// 自身のindexを返す
		return joint.index;
	}

	void UpdateSkeleton(Skeleton& skeleton){
		// すべてのJointを更新。親が若いので通常ループ処理可能になっている
		for (Joint& joint : skeleton.joints) {
			joint.localMatrix = MakeAffineMatrix((Vector3)joint.transform.scale_, (Quaternion)joint.transform.rotation_, (Vector3)joint.transform.translation_);
			
			if (joint.parent) {// 親がいれば親の行列を掛ける
				joint.skeletonSpaceMatrix = Multiply(joint.localMatrix, skeleton.joints[*joint.parent].skeletonSpaceMatrix);
			}
			else { // 親がいないのでlocalMatrixとSkeletonSpaceMatrixは一致する
				joint.skeletonSpaceMatrix = joint.localMatrix;
			}
		}

	}

	void UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton){
		for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
			assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());

			skinCluster.mappedPallete[jointIndex].SkeletonSpaceMatrix =
				Multiply(skinCluster.inverseBindPoseMatrices[jointIndex],
					skeleton.joints[jointIndex].skeletonSpaceMatrix);
			
 			skinCluster.mappedPallete[jointIndex].SkeletonSpaceInverseTransposeMatrix =
				Transpose(Inverse(skinCluster.mappedPallete[jointIndex].SkeletonSpaceMatrix));
		}
	}

	void ApplyAniation(Skeleton& skeleton, const Animation& animation, float animationTime) {
		for (Joint& joint : skeleton.joints) {
			// 対象のJointのAnimationがあれば、値の適用を行う。
			// 下記のif文はC++17から可能になった初期化付きif文
			if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
				const NodeAnimation& rootAnimation = (*it).second;
				joint.transform.translation_ = CalculateValue(rootAnimation.translation, animationTime);
				joint.transform.rotation_ = CalculateValue(rootAnimation.rotation, animationTime);
				joint.transform.scale_ = CalculateValue(rootAnimation.scale, animationTime);
			}
		}
	}

}

SkinCluster CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
	const Skeleton& skeleton,const ModelData& modelData){
	SkinCluster skinCluster;
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();

	// -- Palette用のResourceを確保 -- //

	skinCluster.paletteResource = dxCommon_->CreateBufferResource(device.Get(), sizeof(WellForGPU)*skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPallete = { mappedPalette,skeleton.joints.size() };// spanを使ってアクセスできるようにする
	int32_t indexNum = dxCommon_->srv_->GetEmptyIndex();
	skinCluster.paletteSrvHandle.first = dxCommon_->srv_->textureData_.at(indexNum).textureSrvHandleCPU;
	skinCluster.paletteSrvHandle.second = dxCommon_->srv_->textureData_.at(indexNum).textureSrvHandleGPU;

	// -- Palette用のsrvを作成 -- //
	
	// StructredBufferでアクセスできるようにする
	D3D12_SHADER_RESOURCE_VIEW_DESC palletteSrvDesc{};
	palletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	palletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	palletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	palletteSrvDesc.Buffer.FirstElement = 0;
	palletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	palletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	palletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	device->CreateShaderResourceView(skinCluster.paletteResource.Get(),
		&palletteSrvDesc, skinCluster.paletteSrvHandle.first);
	
	// -- influence用のResourceを確保 -- //
	
	// 頂点ごとにinfluence情報を追加できるようにする
	skinCluster.influenceResource = dxCommon_->CreateBufferResource(device.Get(),
		sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr,
		reinterpret_cast<void**>(&mappedInfluence));
	// 0埋め。weightを0にしておく
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mappedInfluence = {mappedInfluence,modelData.vertices.size() };

	// -- influence用のVBV(頂点バッファビュー)を作成 -- //

	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	// -- InverseBindPoseMatricxの保存領域を作成 -- //
	
	// InverseBindPosMatrixを格納する場所を生成して、単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	// シーケンス初期化に便利なstd::generate を使用する。// 以下リファレンス
	// https://cpprefjp.github.io/reference/algorithm/generate.html
	// 関数に()をつけると戻り値を渡してしまうので()を外して関数として渡すこと
	std::generate(skinCluster.inverseBindPoseMatrices.begin(),
		skinCluster.inverseBindPoseMatrices.end(),MakeIdentity);

	// -- ModelDataのSkinCluster情報を解析してinfluenceの中身を埋める -- //
	
	// ModelのSkinClusterの情報を解析
	for (const auto& jointWeight : modelData.skinClusterData) {
		//	jointWeight.firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
		auto it = skeleton.jointMap.find(jointWeight.first);
		// そんな名前のjointは存在しないので次に回す
		if (it == skeleton.jointMap.end()) {
			continue;
		}
		// (*it).second にはjointのindexが入っているので、該当のinverseBindPosMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			// 該当のvertexIndexのinfluence情報を参照しておく
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			// 空いているところにいれる
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
				// weight==0 が空いている状態なので、その場所にweightとjointのindexを代入
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndex[index] = (*it).second;
					break;
				}
			}
		}
	}

	skinCluster.mappedInfluence;

	// データを返す
	return skinCluster;
}
