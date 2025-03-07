#include "SkinCluster.h"
#include "GameEngine/Object/Model/Model.h"

SkinCluster SkinCluster::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device,
	const Skeleton& skeleton, const ModelData& modelData) {
	SkinCluster skinCluster;

	// -- Palette用のResourceを確保 -- //

	skinCluster.paletteResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device.Get(), sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPallete_ = { mappedPalette,skeleton.joints.size() };// spanを使ってアクセスできるようにする
	int32_t indexNum = DirectXCommon::GetInstance()->mSrv->GetEmptyIndex();
	skinCluster.paletteSrvHandle_.first = DirectXCommon::GetInstance()->mSrv->mTextureData.at(indexNum).textureSrvHandleCPU;
	skinCluster.paletteSrvHandle_.second = DirectXCommon::GetInstance()->mSrv->mTextureData.at(indexNum).textureSrvHandleGPU;

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
	device->CreateShaderResourceView(skinCluster.paletteResource_.Get(),
		&palletteSrvDesc, skinCluster.paletteSrvHandle_.first);

	// -- influence用のResourceを確保 -- //

	// 頂点ごとにinfluence情報を追加できるようにする
	skinCluster.influenceResource_ = DirectXCommon::GetInstance()->CreateBufferResource(device.Get(),
		sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource_->Map(0, nullptr,
		reinterpret_cast<void**>(&mappedInfluence));
	// 0埋め。weightを0にしておく
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mappedInfluence_ = { mappedInfluence,modelData.vertices.size() };

	// -- influence用のVBV(頂点バッファビュー)を作成 -- //

	skinCluster.influenceBufferView_.BufferLocation = skinCluster.influenceResource_->GetGPUVirtualAddress();
	skinCluster.influenceBufferView_.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView_.StrideInBytes = sizeof(VertexInfluence);

	// -- InverseBindPoseMatricxの保存領域を作成 -- //

	// InverseBindPosMatrixを格納する場所を生成して、単位行列で埋める
	skinCluster.inverseBindPoseMatrices_.resize(skeleton.joints.size());
	// シーケンス初期化に便利なstd::generate を使用する。// 以下リファレンス
	// https://cpprefjp.github.io/reference/algorithm/generate.html
	// 関数に()をつけると戻り値を渡してしまうので()を外して関数として渡すこと
	std::generate(skinCluster.inverseBindPoseMatrices_.begin(),
		skinCluster.inverseBindPoseMatrices_.end(), MakeIdentity);

	// -- ModelDataのSkinCluster情報を解析してinfluenceの中身を埋める -- //

	// ModelのSkinClusterの情報を解析
	for (const auto& jointWeight : modelData.skinClusterData) {
		//	jointWeight.firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
		auto it = skeleton.jointMap.find(jointWeight.first);
		// そんな名前のjointは存在しないので次に回す
		if (it == skeleton.jointMap.end()) {
			continue;
		}
		// (*it).second にはjointのindexが入っているので、該当のinverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			// 該当のvertexIndexのinfluence情報を参照しておく
			auto& currentInfluence = skinCluster.mappedInfluence_[vertexWeight.vertexIndex];
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

	// データを返す
	return skinCluster;
}

void SkinCluster::Update(const Skeleton& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < inverseBindPoseMatrices_.size());

		mappedPallete_[jointIndex].SkeletonSpaceMatrix =
			Multiply(inverseBindPoseMatrices_[jointIndex], skeleton.joints[jointIndex].skeletonSpaceMatrix);

		mappedPallete_[jointIndex].SkeletonSpaceInverseTransposeMatrix =
			Transpose(Inverse(mappedPallete_[jointIndex].SkeletonSpaceMatrix));
	}
}