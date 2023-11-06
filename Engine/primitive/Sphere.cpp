#include "Sphere.h"
#include "../object/camera/MatrixCamera.h"

Sphere::Sphere() { this->Initialize(); }

Sphere::~Sphere()
{
	/*wvpResource->Release();
	materialResource->Release();
	vertexResource->Release();*/
}

void Sphere::Initialize() {

	dx_ = DirectXCommon::GetInstance();
	mainCamera_ = MatrixCamera::GetInstance();

	worldTransform_ = new WorldTransform;
	cameraWorldTransform_ = new WorldTransform;
	cameraWorldTransform_->translate.z = -5.0f;

	CreateVertexResource();
	CreateTransformationRsource();
	CreateBufferView();

}

void Sphere::Update() {


	ImGui::Begin("camera");
	ImGui::SliderFloat3("Scale", &cameraWorldTransform_->scale.x,0.0f,1.0f);
	ImGui::SliderFloat3("rotate", &cameraWorldTransform_->rotate.x, -2.0f, 2.0f);
	ImGui::SliderFloat3("translate", &cameraWorldTransform_->translate.x, -20.0f, 20.0f);
	ImGui::End();

	ImGui::Begin("Sphere");
	ImGui::DragFloat3("scale", &worldTransform_->scale.x, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat3("rotate", &worldTransform_->rotate.x, 0.01f, -2.0f, 2.0f);
	ImGui::DragFloat3("tranlate", &worldTransform_->translate.x, 0.01f, -2.0f, 2.0f);
	if (ImGui::TreeNode("tex")) {
		if (ImGui::Button("texcood.x +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.x += 0.01f;
					vertexData[start + 1].texcoord.x += 0.01f;
					vertexData[start + 2].texcoord.x += 0.01f;
					vertexData[start + 3].texcoord.x += 0.01f;
					vertexData[start + 4].texcoord.x += 0.01f;
					vertexData[start + 5].texcoord.x += 0.01f;
				}
			}
		}
		if (ImGui::Button("texcood.x -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.x -= 0.01f;
					vertexData[start + 1].texcoord.x -= 0.01f;
					vertexData[start + 2].texcoord.x -= 0.01f;
					vertexData[start + 3].texcoord.x -= 0.01f;
					vertexData[start + 4].texcoord.x -= 0.01f;
					vertexData[start + 5].texcoord.x -= 0.01f;
				}
			}
		}
		if (ImGui::Button("texcood.y +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.y += 0.1f;
					vertexData[start + 1].texcoord.y += 0.1f;
					vertexData[start + 2].texcoord.y += 0.1f;
					vertexData[start + 3].texcoord.y += 0.1f;
					vertexData[start + 4].texcoord.y += 0.1f;
					vertexData[start + 5].texcoord.y += 0.1f;
				}
			}
		}
		if (ImGui::Button("texcood.y -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.y -= 0.1f;
					vertexData[start + 1].texcoord.y -= 0.1f;
					vertexData[start + 2].texcoord.y -= 0.1f;
					vertexData[start + 3].texcoord.y -= 0.1f;
					vertexData[start + 4].texcoord.y -= 0.1f;
					vertexData[start + 5].texcoord.y -= 0.1f;
				}
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Left")) {
		if (ImGui::Button("Left.x +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.x += 0.01f;
					vertexData[start + 1].texcoord.x += 0.01f;
					vertexData[start + 3].texcoord.x += 0.01f;
				}
			}
		}
		if (ImGui::Button("Left.x -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.x -= 0.01f;
					vertexData[start + 1].texcoord.x -= 0.01f;
					vertexData[start + 3].texcoord.x -= 0.01f;
				}
			}
		}
		if (ImGui::Button("Left.y +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.y += 0.01f;
					vertexData[start + 1].texcoord.y += 0.01f;
					vertexData[start + 3].texcoord.y += 0.10f;
				}
			}
		}
		if (ImGui::Button("Left.y -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.y -= 0.01f;
					vertexData[start + 1].texcoord.y -= 0.01f;
					vertexData[start + 3].texcoord.y -= 0.01f;
				}
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Right")) {
		if (ImGui::Button("Right.x +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start + 2].texcoord.x += 0.01f;
					vertexData[start + 4].texcoord.x += 0.01f;
					vertexData[start + 5].texcoord.x += 0.01f;
				}
			}
		}
		if (ImGui::Button("Right.x -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start + 2].texcoord.x -= 0.01f;
					vertexData[start + 4].texcoord.x -= 0.01f;
					vertexData[start + 5].texcoord.x -= 0.01f;
				}
			}
		}
		if (ImGui::Button("Right.y +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start + 2].texcoord.y += 0.01f;
					vertexData[start + 4].texcoord.y += 0.01f;
					vertexData[start + 5].texcoord.y += 0.01f;
				}
			}
		}
		if (ImGui::Button("Right.y -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start + 2].texcoord.y -= 0.01f;
					vertexData[start + 4].texcoord.y -= 0.01f;
					vertexData[start + 5].texcoord.y -= 0.01f;
				}
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Top")) {
		if (ImGui::Button("Top.x +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start + 1].texcoord.x += 0.01f;
					vertexData[start + 3].texcoord.x += 0.01f;
					vertexData[start + 4].texcoord.x += 0.01f;
				}
			}
		}
		if (ImGui::Button("Top.x -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start + 1].texcoord.x -= 0.01f;
					vertexData[start + 3].texcoord.x -= 0.01f;
					vertexData[start + 4].texcoord.x -= 0.01f;
				}
			}
		}
		if (ImGui::Button("Top.y +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start + 1].texcoord.y += 0.01f;
					vertexData[start + 3].texcoord.y += 0.01f;
					vertexData[start + 4].texcoord.y += 0.01f;
				}
			}
		}
		if (ImGui::Button("Top.y -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start + 1].texcoord.y -= 0.01f;
					vertexData[start + 3].texcoord.y -= 0.01f;
					vertexData[start + 4].texcoord.y -= 0.01f;
				}
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Buttom")) {
		if (ImGui::Button("Buttom.x +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.x += 0.01f;
					vertexData[start + 2].texcoord.x += 0.01f;
					vertexData[start + 5].texcoord.x += 0.01f;
				}
			}
		}
		if (ImGui::Button("Buttom.x -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.x -= 0.01f;
					vertexData[start + 2].texcoord.x -= 0.01f;
					vertexData[start + 5].texcoord.x -= 0.01f;
				}
			}
		}
		if (ImGui::Button("Buttom.y +0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.y += 0.01f;
					vertexData[start + 2].texcoord.y += 0.01f;
					vertexData[start + 5].texcoord.y += 0.01f;
				}
			}
		}
		if (ImGui::Button("Buttom.y -0.1f")) {
			for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
				for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
					uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
					vertexData[start].texcoord.y -= 0.01f;
					vertexData[start + 2].texcoord.y -= 0.01f;
					vertexData[start + 5].texcoord.y -= 0.01f;
				}
			}
		}
		ImGui::TreePop();
	}


	ImGui::End();

	//　球体のワールド行列
	worldTransform_->worldM = W::Math::MakeAffineMatrix(
		worldTransform_->scale, worldTransform_->rotate, worldTransform_->translate);

	// カメラのワールド行列
	
	cameraM = W::Math::MakeAffineMatrix(
		cameraWorldTransform_->scale, cameraWorldTransform_->rotate, cameraWorldTransform_->translate);
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = W::Math::Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = W::Math::MakePerspectiveMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = W::Math::Multiply(viewM, pespectiveM);

}

void Sphere::Draw() const {

	// 三角形のワールド行列とWVP行列を掛け合わした行列を代入
	*wvpData = W::Math::Multiply(worldTransform_->worldM, wvpM);

	dx_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dx_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	dx_->commandList->SetGraphicsRootDescriptorTable(2, dx_->textureSrvHandleGPU_);

	// インスタンス生成
	dx_->commandList->DrawInstanced(((kSubdivision) * (kSubdivision) * 6), 1, 0, 0);

}

//
void Sphere::CreateVertexResource() {

	// VertexResourceを生成する(P.42)
	// 実際に頂点リソースを作る
	vertexResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(VertexData) * ((kSubdivision) * (kSubdivision) * 6));

	// マテリアル用のResourceを作る
	materialResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(VertexData));
	// マテリアルにデータを書き込む
	materialDate = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	// 色を書き込む
	*materialDate = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

}

//
void Sphere::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(Matrix4x4));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	*wvpData = mainCamera_->GetWorldViewProjection();

}

//
void Sphere::CreateBufferView() {

	// VertexBufferViewを作成する(P.43)
	// 頂点バッファビューを作成する
	vertexBufferView = {};
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * ((kSubdivision) * (kSubdivision) * 6);
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む

	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	// 緯度の方向に分割　-π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex; //	現在の緯度を求める

		// 経度の方向に分割　0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery; //	現在の経度を求める

			Vector3 a = {
				cos(lat) * cos(lon) * rad,
				sin(lat) * rad,
				cos(lat) * sin(lon) * rad };
			Vector3 b = {
				cos(lat + (pi / kSubdivision)) * cos(lon) * rad,
				sin(lat + (pi / kSubdivision)) * rad,
				cos(lat + (pi / kSubdivision)) * sin(lon) * rad };
			Vector3 c = {
				cos(lat) * cos(lon + ((pi * 2) / kSubdivision)) * rad,
				sin(lat) * rad,
				cos(lat) * sin(lon + ((pi * 2) / kSubdivision)) * rad };
			Vector3 d = {
				cos(lat + (pi / kSubdivision)) * cos(lon + ((pi * 2) / kSubdivision)) * rad,
				sin(lat + (pi / kSubdivision)) * rad,
				cos(lat + (pi / kSubdivision)) * sin(lon + ((pi * 2) / kSubdivision)) * rad };

			// 頂点インデックスにデータを入力する

			// 左下
			vertexData[start].position = { a.x,a.y,a.z,1.0f };
			vertexData[start].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start].texcoord.y = 1.0f - float(latIndex-1) / float(kSubdivision) ;
			//　左上
			vertexData[start + 1].position = { b.x,b.y,b.z,1.0f };
			vertexData[start + 1].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start + 1].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			// 右下
			vertexData[start + 2].position = { c.x,c.y,c.z,1.0f };
			vertexData[start + 2].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 2].texcoord.y = 1.0f - float(latIndex-1) / float(kSubdivision);
			/// 2枚目
			// 左上
			vertexData[start + 3].position = { b.x,b.y,b.z,1.0f };
			vertexData[start + 3].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start + 3].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			//　右上
			vertexData[start + 4].position = { d.x,d.y,d.z,1.0f };
			vertexData[start + 4].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 4].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			// 右下
			vertexData[start + 5].position = { c.x,c.y,c.z,1.0f };
			vertexData[start + 5].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 5].texcoord.y = 1.0f-float(latIndex-1) / float(kSubdivision);

		}
	}

}

//
void Sphere::DrawBeginResource() {


}