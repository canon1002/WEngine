#include "DiffusionToCircleParticle.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include  "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/GameMaster/Framerate.h"

void DiffusionToCircleParticle::Init() {

	mDxCommon = DirectXCommon::GetInstance();
	mCamera = MainCamera::GetInstance();
	mWorldTransform.scale = { 1.0f,1.0f,1.0f };
	mWorldTransform.rotation = { 0.0f,0.0f,0.0f };
	mWorldTransform.translation = { 0.0f,0.0f,0.0f };

	instanceCount_ = kNumMaxInstance;

	// エミッター初期設定
	mEmitter = {};
	mEmitter.worldTransform.Init();
	mEmitter.count = 3;
	mEmitter.frequency = 1.5f;// 1.5秒ごとに発生
	mEmitter.frequencyTime = 0.0f;// 発生頻度用の時刻 0で初期化

	// リソースを生成
	CreateTransformation();
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateInstancing();

	mDxCommon->srv_->SetStructuredBuffer(kNumMaxInstance, mInstancingResource);

}

void DiffusionToCircleParticle::Update() {

#ifdef _DEBUG
	ImGui::Begin("DTCPatricle");
	ImGui::DragScalar("Emitter Count", ImGuiDataType_U16, &mEmitter.count, 1, 0, nullptr, "%u");
	ImGui::DragFloat("Emitter Frequency", &mEmitter.frequency, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("Emitter Scale", &mEmitter.worldTransform.scale.x, 0.1f, 0.01f, 10.0f);
	ImGui::DragFloat3("Emitter Rotation", &mEmitter.worldTransform.rotation.x, 0.01f, -100.0f, 100.0f);
	ImGui::DragFloat3("Emitter Translation", &mEmitter.worldTransform.translation.x, 0.01f, -100.0f, 100.0f);

#endif // _DEBUG


	// Δtを定義 60fps固定してあるが、実時間を計測して可変fpsで動かせるようにしておきたい
	const float kDeltaTime = (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

	// instancingCountが最大値を上回らないようにする
	if (instanceCount_ > kNumMaxInstance) { instanceCount_ = kNumMaxInstance; }
	// エミッター更新処理
	mEmitter.frequencyTime += kDeltaTime;// 時刻を進める
	if (mEmitter.frequency <= mEmitter.frequencyTime) {// 発生頻度より数値が大きくなったら発生
		mParticles.splice(mParticles.end(), Emit(mEmitter, randomEngine_));// 発生処理
		mEmitter.frequencyTime -= mEmitter.frequency;// 余計に過ぎた時間も加味して頻度計算を行う
	}

	// ワールド行列とWVP行列を掛け合わした行列を代入
	mWVPData->WVP = Multiply(mWorldTransform.GetWorldMatrix(), mCamera->GetViewProjectionMatrix());
	mWVPData->World = mWorldTransform.GetWorldMatrix();

	// イテレーターの位置を保持する数値
	instanceCount_ = 0;

	// イテレーターを使用してfor文を回す
	for (std::list<Particle>::iterator it = mParticles.begin(); it != mParticles.end();) {
		if ((*it).lifeTime <= (*it).currentTime) {
			// 生存期間の過ぎたパーティクルをリストから削除する
			it = mParticles.erase(it);
			continue;
		}

		// パーティクルの移動を行う
		(*it).worldTransform.translation += (*it).vel * kDeltaTime;
		// 経過時間の加算
		(*it).currentTime += kDeltaTime;
		
		if (instanceCount_ < kNumMaxInstance) {

			// 徐々に透明にする
			float alpha = 1.0f - ((*it).currentTime / (*it).lifeTime);

			mInstancingData[instanceCount_].WVP = Multiply((*it).worldTransform.GetWorldMatrix(), mCamera->GetViewProjectionMatrix());
			mInstancingData[instanceCount_].World = (*it).worldTransform.GetWorldMatrix();
			mInstancingData[instanceCount_].color = (*it).color;
			mInstancingData[instanceCount_].color.a = alpha;

#ifdef _DEBUG

			if (ImGui::TreeNode(std::to_string(instanceCount_).c_str())) {
				float treeScale = mWorldTransform.scale.x;
				ImGui::DragFloat("Scale", &treeScale, 0.05f);
				mWorldTransform.scale = { treeScale ,treeScale ,treeScale };
				ImGui::SliderAngle("RotateX", &(*it).worldTransform.rotation.x);
				ImGui::SliderAngle("RotateY", &(*it).worldTransform.rotation.y);
				ImGui::SliderAngle("RotateZ", &(*it).worldTransform.rotation.z);
				ImGui::DragFloat3("Tranlate", &(*it).worldTransform.translation.x);
				ImGui::DragFloat("lifeTime", &(*it).lifeTime);
				ImGui::Spacing();
				ImGui::DragFloat2("UVScale", &uvTransform_.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("UVTranlate", &uvTransform_.translation.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("UVRotate", &uvTransform_.rotation.z);
				ImGui::ColorEdit4("Color", &(*it).color.r);
				ImGui::TreePop();
			}

#endif // _DEBUG

			instanceCount_++;
		}


		// イテレーターを次に進める
		++it;
	}



	/// マテリアル・UVTransform
	Matrix4x4 uvTransformMatrix = MakeAffineMatrix(
		uvTransform_.scale,
		Vector3{ 0.0f,0.0f,uvTransform_.rotation.z },
		uvTransform_.translation
	);
	// 変換したデータを代入する
	mMaterialData->uvTransform = uvTransformMatrix;

#ifdef _DEBUG
	ImGui::End();
#endif // _DEBUG

}

void DiffusionToCircleParticle::Draw() {
	// isntancceCountttが1以上のときに描画処理を行う
	if (instanceCount_ > 0) {
		// 頂点バッファをセット
		mDxCommon->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
		// 形状を三角形に設定
		mDxCommon->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// マテリアルのCBufferの場所を指定
		mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(0, mMaterialResource->GetGPUVirtualAddress());

		mDxCommon->mCommandList->SetGraphicsRootDescriptorTable(1, mDxCommon->srv_->instancingSrvHandleGPU);
		// テクスチャをセット
		mDxCommon->mCommandList->SetGraphicsRootDescriptorTable(2, mDxCommon->srv_->textureData_.at(mDxCommon->srv_->defaultTexId_).textureSrvHandleGPU);
		// ドローコール
		mDxCommon->mCommandList->DrawInstanced(6, instanceCount_, 0, 0);
	}
}


Particle DiffusionToCircleParticle::Create(const Vector3& translate, std::mt19937& randomEngine){

	// 返り値
	Particle particle{};
	// 乱数の最小・最大値
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	// SRT・移動量・色の設定
	particle.worldTransform.scale = { 1.0f,1.0f,1.0f };
	particle.worldTransform.rotation = { 0.0f,0.0f,0.0f };
	Vector3 randomTranslate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.worldTransform.translation = translate + randomTranslate;
	particle.vel = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.color = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine),0.5f };

	// 乱数の最小・最大値
	std::uniform_real_distribution<float> distTime(-1.0f, 3.0f);
	// 生存時間の設定
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

std::list<Particle> DiffusionToCircleParticle::Emit(const Emitter& emtter, std::mt19937& randomEngine)
{
	// パーティクルのリストを生成
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emtter.count; count++) {
		// 新規パーティクルを生成
		particles.push_back(Create(mEmitter.worldTransform.translation,randomEngine));
	}
	// 生成したリストを返す
	return particles;
}

