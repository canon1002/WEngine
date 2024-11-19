#include "TrailEffect.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include  "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/GameMaster/Framerate.h"

void TrailEffect::Init() {

	mDxCommon = DirectXCommon::GetInstance();
	mCamera = MainCamera::GetInstance();
	mWorldTransform.scale = { 1.0f,1.0f,1.0f };
	mWorldTransform.rotation = { 0.0f,0.0f,0.0f };
	mWorldTransform.translation = { 0.0f,0.0f,0.0f };

	instanceCount_ = kNumMaxInstance;

	// エミッター初期設定
	mEmitter = {};
	mEmitter.worldTransform.Init();
	mEmitter.count = 1;
	mEmitter.frequency = 5.0f;// 0.1秒ごとに発生
	mEmitter.frequencyTime = 0.0f;// 発生頻度用の時刻 0で初期化

	// リソースを生成
	CreateTransformation();
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateInstancing();

	// テクスチャハンドル
	mTextureHandle = mDxCommon->srv_->LoadTexture("white2x2.png");
	mInstancingHandle = mDxCommon->srv_->SetStructuredBuffer(kNumMaxInstance, mInstancingResource);

}

void TrailEffect::Update() {

#ifdef _DEBUG
	ImGui::Begin("TrailPatricle");
	ImGui::DragScalar("Emitter Count", ImGuiDataType_U16, &mEmitter.count, 1, 0, nullptr, "%u");
	ImGui::DragFloat("Emitter Frequency", &mEmitter.frequency, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat("Emitter Scale", &mEmitter.worldTransform.scale.x, 0.1f, 0.01f, 10.0f);
	ImGui::DragFloat3("Emitter Rotation", &mEmitter.worldTransform.rotation.x, 0.01f, -100.0f, 100.0f);
	ImGui::DragFloat3("Emitter Translation", &mEmitter.worldTransform.translation.x, 0.01f, -100.0f, 100.0f);

	if (ImGui::BeginTable("Particles Table", 5, ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg))
	{
		// テーブルのヘッダーを設定
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Current Time");
		ImGui::TableSetupColumn("Life Time");
		ImGui::TableSetupColumn("Color");
		ImGui::TableSetupColumn("Alpha");
		ImGui::TableHeadersRow();

		// パーティクルごとの情報を表示
		int id = 0; // IDを管理するカウンタ

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
				++id;
				continue;
			}

			// パーティクルの移動を行う
			//(*it).worldTransform.translation += (*it).vel * kDeltaTime;

			// 経過時間の加算
			(*it).currentTime += kDeltaTime;

			if (instanceCount_ < kNumMaxInstance) {

				// 徐々に透明にする
				float alpha = 1.0f - ((*it).currentTime / (*it).lifeTime);

				mInstancingData[instanceCount_].WVP = Multiply((*it).worldTransform.GetWorldMatrix(), mCamera->GetViewProjectionMatrix());
				mInstancingData[instanceCount_].World = (*it).worldTransform.GetWorldMatrix();
				mInstancingData[instanceCount_].color = (*it).color;
				//mInstancingData[instanceCount_].color.a = std::min((*it).color.a, alpha);

				#ifdef _DEBUG

				ImGui::TableNextRow();

				// ID列
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%d", id);

				// Current Time列
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.2f", (*it).currentTime);

				// Life Time列
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%.2f", (*it).lifeTime);

				// Color列
				ImGui::TableSetColumnIndex(3);
				ImGui::Text("(%.2f, %.2f, %.2f, %.2f)",
					(*it).color.r,
					(*it).color.g,
					(*it).color.b,
					(*it).color.a);

				// Alpha列
				ImGui::TableSetColumnIndex(4);
				ImGui::Text("%.2f", std::clamp(alpha, 0.0f, 1.0f));
#endif // _DEBUG

				instanceCount_++;
			}


			// イテレーターを次に進める
			++it;
			++id;
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
		ImGui::EndTable();
	}
	ImGui::End();
#endif // _DEBUG

}

void TrailEffect::Draw() {
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
		mDxCommon->mCommandList->SetGraphicsRootDescriptorTable(2, mDxCommon->srv_->textureData_.at(mTextureHandle).textureSrvHandleGPU);
		// ドローコール
		mDxCommon->mCommandList->DrawInstanced(6, instanceCount_, 0, 0);
	}
}


Particle TrailEffect::Create(const Vector3& translate, std::mt19937& randomEngine) {

	// 返り値
	Particle particle{};
	// 乱数の最小・最大値
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	// SRT・移動量・色の設定
	particle.worldTransform.scale = { 1.0f,1.0f,1.0f };
	particle.worldTransform.rotation = { 0.0f,0.0f,0.0f };
	particle.worldTransform.translation = translate;
	particle.vel = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.color = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine),0.5f };

	// 乱数の最小・最大値
	std::uniform_real_distribution<float> distTime(-1.0f, 3.0f);
	// 生存時間の設定
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

Particle TrailEffect::Create(const WorldTransform& world, const uint32_t count)
{
	// 返り値
	Particle particle{};

	// SRT・移動量・色の設定
	//particle.worldTransform.scale = { 0.02f,0.02f,0.1f };
	particle.worldTransform.scale = { 1.0f,1.0f,1.0f };
	particle.worldTransform.rotation = world.GetWorldRotatiton();
	particle.worldTransform.translation = world.GetWorldPosition();
	particle.worldTransform.translation += { 0.0f,0.0f, 0.0f };
	particle.vel = { 0.0f,0.0f,0.0f };
	particle.color = { 0.0f,0.2f,0.8f,0.5f };

	// 生存時間の設定
	if (count % 5 == 0 || count % 5 == 4) {
		particle.lifeTime = kLifeTimeLow;
	}
	else if (count % 5 == 1 || count % 5 == 3) {
		particle.lifeTime = kLifeTimeMidiam;
	}
	else {
		particle.lifeTime = kLifeTimeHigh;
	}
	particle.currentTime = 0.0f;

	return particle;
}

std::list<Particle> TrailEffect::Emit(const Emitter& emtter, std::mt19937& randomEngine)
{
	// パーティクルのリストを生成
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emtter.count; count++) {
		// 新規パーティクルを生成
		randomEngine;
		//particles.push_back(Create(mEmitter.worldTransform.translation, randomEngine));
		particles.push_back(Create(mEmitter.worldTransform, count));
	}
	// 生成したリストを返す
	return particles;
}

