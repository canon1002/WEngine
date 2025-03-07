#include "HitEffect.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include  "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "GameEngine/Resource/Texture/TextureManager.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"

void HitEffect::Init() {

	mWorldTransform.scale = { 1.0f,1.0f,1.0f };
	mWorldTransform.rotation = { 0.0f,0.0f,0.0f };
	mWorldTransform.translation = { 0.0f,0.0f,0.0f };

	mInstanceCount = kNumMaxInstance;

	// エミッター初期設定
	mEmitter = {};
	mEmitter.worldtransform = std::make_unique<WorldTransform>();
	mEmitter.worldtransform->Init();
	mEmitter.count = 8;
	mEmitter.frequency = 0.05f;// 0.05秒ごとに発生
	mEmitter.frequencyTime = 0.0f;// 発生頻度用の時刻 0で初期化

	// テクスチャの設定
	//mTextureHandle = DirectXCommon::GetInstance()->srv_->LoadTexture("uvChecker.png");
	//mTextureHandle = DirectXCommon::GetInstance()->srv_->LoadTexture("circleWhite.png");
	mTextureHandle = TextureManager::GetInstance()->LoadTexture("circle.png");


	// リソースを生成
	CreateTransformation();
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateInstancing();

	mInstancingHandle = ParticleManager::GetInstance()->SetInstancingBuffer(kNumMaxInstance, mInstancingResource);

	// 最初は表示しない
	mIsActive = false;

}

void HitEffect::Update() {

#ifdef _DEBUG
	ImGui::Begin("DTCPatricle");
	ImGui::DragScalar("Emitter Count", ImGuiDataType_U16, &mEmitter.count, 1, 0, nullptr, "%u");
	ImGui::DragFloat("Emitter Frequency", &mEmitter.frequency, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("Emitter Scale", &mEmitter.worldtransform->scale.x, 0.1f, 0.01f, 10.0f);
	ImGui::DragFloat3("Emitter Rotation", &mEmitter.worldtransform->rotation.x, 0.01f, -100.0f, 100.0f);
	ImGui::DragFloat3("Emitter Translation", &mEmitter.worldtransform->translation.x, 0.01f, -100.0f, 100.0f);

#endif // _DEBUG


	// Δtを定義 60fps固定してあるが、実時間を計測して可変fpsで動かせるようにしておきたい
	const float kDeltaTime = (1.0f / Framerate::GetInstance()->GetFramerate()) * Framerate::GetInstance()->GetGameSpeed();

	// instancingCountが最大値を上回らないようにする
	if (mInstanceCount > kNumMaxInstance) { mInstanceCount = kNumMaxInstance; }

	// エミッター更新処理
	if (mIsActive) {
		mEmitter.frequencyTime += kDeltaTime;// 時刻を進める
	}

	if (mEmitter.frequency <= mEmitter.frequencyTime) {// 発生頻度より数値が大きくなったら発生
		mParticles.splice(mParticles.end(), Emit(mEmitter, mRandomEngine));// 発生処理
		mEmitter.frequencyTime -= mEmitter.frequency;// 余計に過ぎた時間も加味して頻度計算を行う
	}

	// ワールド行列とWVP行列を掛け合わした行列を代入
	mWVPData->WVP = Multiply(mWorldTransform.GetWorldMatrix(), MainCamera::GetInstance()->GetViewProjectionMatrix());
	mWVPData->World = mWorldTransform.GetWorldMatrix();

	// イテレーターの位置を保持する数値
	mInstanceCount = 0;

	// イテレーターを使用してfor文を回す
	for (std::list<Particle>::iterator it = mParticles.begin(); it != mParticles.end();) {
		if ((*it).lifeTime <= (*it).currentTime) {
			// 生存期間の過ぎたパーティクルをリストから削除する
			it = mParticles.erase(it);
			continue;
		}

		// パーティクルの縮小を行う
		(*it).worldTransform.scale -= ((*it).vel * kDeltaTime);
		// 経過時間の加算
		(*it).currentTime += kDeltaTime;

		if (mInstanceCount < kNumMaxInstance) {

			// 徐々に透明にする
			float alpha = 1.0f - ((*it).currentTime / (*it).lifeTime);

			mInstancingData[mInstanceCount].WVP = Multiply((*it).worldTransform.GetWorldMatrix(), MainCamera::GetInstance()->GetViewProjectionMatrix());
			mInstancingData[mInstanceCount].World = (*it).worldTransform.GetWorldMatrix();
			mInstancingData[mInstanceCount].color = (*it).color;
			mInstancingData[mInstanceCount].color.a = alpha;

#ifdef _DEBUG

			if (ImGui::TreeNode(std::to_string(mInstanceCount).c_str())) {
				float treeScale = mWorldTransform.scale.x;
				ImGui::DragFloat("Scale", &treeScale, 0.05f);
				mWorldTransform.scale = { treeScale ,treeScale ,treeScale };
				ImGui::SliderAngle("RotateX", &(*it).worldTransform.rotation.x);
				ImGui::SliderAngle("RotateY", &(*it).worldTransform.rotation.y);
				ImGui::SliderAngle("RotateZ", &(*it).worldTransform.rotation.z);
				ImGui::DragFloat3("Tranlate", &(*it).worldTransform.translation.x);
				ImGui::DragFloat("lifeTime", &(*it).lifeTime);
				ImGui::Spacing();
				ImGui::DragFloat2("UVScale", &mUVTransform.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("UVTranlate", &mUVTransform.translation.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("UVRotate", &mUVTransform.rotation.z);
				ImGui::ColorEdit4("Color", &(*it).color.r);
				ImGui::TreePop();
			}

#endif // _DEBUG

			mInstanceCount++;
		}


		// イテレーターを次に進める
		++it;
	}



	/// マテリアル・UVTransform
	Matrix4x4 uvTransformMatrix = MakeAffineMatrix(
		mUVTransform.scale,
		Vector3{ 0.0f,0.0f,mUVTransform.rotation.z },
		mUVTransform.translation
	);
	// 変換したデータを代入する
	mMaterialData->uvTransform = uvTransformMatrix;

#ifdef _DEBUG
	ImGui::End();
#endif // _DEBUG

}

void HitEffect::Draw() {
	// isntancceCountttが1以上のときに描画処理を行う
	if (mInstanceCount > 0) {
		// 頂点バッファをセット
		DirectXCommon::GetInstance()->mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
		// 形状を三角形に設定
		DirectXCommon::GetInstance()->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// マテリアルのCBufferの場所を指定
		DirectXCommon::GetInstance()->mCommandList->SetGraphicsRootConstantBufferView(0, mMaterialResource->GetGPUVirtualAddress());

		DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(1, mInstancingHandle);
		// テクスチャをセット
		DirectXCommon::GetInstance()->mSrv->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->mDefaultTextureIndex);
		// ドローコール
		DirectXCommon::GetInstance()->mCommandList->DrawInstanced(6, mInstanceCount, 0, 0);
	}
}


Particle HitEffect::Create(const Vector3& translate, std::mt19937& randomEngine) {

	// 返り値
	Particle particle{};
	// 乱数の最小・最大値
	std::uniform_real_distribution<float> distribution(-0.1f, 0.1f);

	// SRT・移動量・色の設定
	particle.worldTransform.scale = { 0.1f,0.1f,0.1f };
	particle.worldTransform.rotation = { 0.0f,0.0f,0.0f };
	Vector3 randomTranslate = { distribution(randomEngine),0.0f,distribution(randomEngine) };
	particle.worldTransform.translation = translate + randomTranslate;
	particle.vel = { 0.1f,0.1f,0.1f };
	particle.color = { 1.0f,1.0f,1.0f,0.5f };

	// 乱数の最小・最大値
	std::uniform_real_distribution<float> distTime(0.25f, 0.5f);
	// 生存時間の設定
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

std::list<Particle> HitEffect::Emit(const Emitter& emtter, std::mt19937& randomEngine)
{
	// パーティクルのリストを生成
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emtter.count; count++) {
		// 新規パーティクルを生成
		particles.push_back(Create(mEmitter.worldtransform->translation, randomEngine));
	}
	// 生成したリストを返す
	return particles;
}

