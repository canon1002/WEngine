#include "Model.h"
#include <fstream>
#include <sstream>
#include "../camera/MatrixCamera.h"

Model::Model() {}

Model::~Model() {}

ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	/// 変数の宣言
	ModelData modelData;// 構築するモデルのデータ
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line; // ファイルから読んだ一行を格納する

	/// ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open());

	/// モデルデータの構築
	while (std::getline(file,line)){
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
			Vector2 texcoord;
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
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
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

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	// 中で必要となる変数の宣言
	MaterialData materialData;
	std::string line;// ファイルから読んだ一行を格納する
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open());// 開けない場合はとりあえず止める

	// 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file,line))
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

void Model::Initialize() {

	dx_ = DirectXCommon::GetInstance();
	mainCamera_ = MatrixCamera::GetInstance();
	worldTransform_ = new WorldTransform;

	CreateVertexResource();
	CreateIndexResource();
	CreateTransformationRsource();
	CreateBufferView();
	
}

void Model::Update() {

	ImGui::Begin("Model");
	float treeScale = worldTransform_->scale.x;
	ImGui::DragFloat("ObjScale", &treeScale,0.05f);
	worldTransform_->scale = { treeScale ,treeScale ,treeScale };
	ImGui::SliderAngle("ObjRotateX", &worldTransform_->rotate.x);
	ImGui::SliderAngle("ObjRotateY", &worldTransform_->rotate.y);
	ImGui::SliderAngle("ObjRotateZ", &worldTransform_->rotate.z);
	ImGui::DragFloat3("ObjTranlate", &worldTransform_->translate.x);
	ImGui::Spacing();
	ImGui::DragFloat2("UVScale", &uvTransform_.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVTranlate", &uvTransform_.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransform_.rotate.z);
	ImGui::ColorEdit4("Color", &materialData_->color.x);
	ImGui::End();

	//　矩形のワールド行列
	worldTransform_->worldM = W::Math::MakeAffineMatrix(
		worldTransform_->scale, worldTransform_->rotate, worldTransform_->translate);

	// カメラのワールド行列
	cameraM = W::Math::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-5.0f });
	// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
	viewM = W::Math::Inverse(cameraM);
	// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
	pespectiveM = W::Math::MakePerspectiveMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
	// WVPにまとめる
	wvpM = W::Math::Multiply(viewM, pespectiveM);
	// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
	wvpData->WVP = W::Math::Multiply(worldTransform_->worldM, wvpM);
	wvpData->World = worldTransform_->worldM;



	/// マテリアル・UVTransform
	Matrix4x4 uvTransformMatrix = W::Math::MakeAffineMatrix(
		uvTransform_.scale,
		{ 0.0f,0.0f,uvTransform_.rotate.z },
		uvTransform_.translate
	);
	// 変換したデータを代入する
	materialData_->uvTransform = uvTransformMatrix;

}

void Model::Draw() {

	dx_->commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	dx_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// CBV設定

	// マテリアルのCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を指定
	dx_->commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	dx_->commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	dx_->commandList->SetGraphicsRootDescriptorTable(2, dx_->srv_->textureData_.at(textureHandle_).textureSrvHandleGPU);

	// インデックスを使用してドローコール
	dx_->commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

}

// インデックスデータの生成
void Model::CreateIndexResource() {

}

//
void Model::CreateVertexResource() {

	// モデル読み込み
	modelData = LoadObjFile("resources/obj", "axis.obj");
	// 実際に頂点リソースを作る
	vertexResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(VertexData) * modelData.vertices.size());
	// マテリアル用のResourceを作る
	materialResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(Material));
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// テクスチャの情報を転送
	textureHandle_ = dx_->srv_->LoadTexture(modelData.material.textureFilePath);
	// 色の書き込み・Lightingの無効化
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	// UVTransformを設定
	materialData_->uvTransform = W::Math::MakeIdentity();
	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	// Light
	directionalLightResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(DirectionalLight));
	// データを書き込む
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };
	directionalLightDate->intensity = 1.0f;

}

//
void Model::CreateTransformationRsource() {

	// Transformation用のResourceを作る
	wvpResource = dx_->CreateBufferResource(dx_->device.Get(), sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	// 単位行列を書き込む
	wvpData->WVP = mainCamera_->GetWorldViewProjection();
	wvpData->World = W::Math::MakeIdentity();
}

//
void Model::CreateBufferView() {

	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースサイズは頂点分のサイズ
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む

	// 頂点リソースにデータを書き込む
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));// 書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

}
