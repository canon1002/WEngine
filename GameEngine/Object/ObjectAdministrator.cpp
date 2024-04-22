#include "ObjectAdministrator.h"
#include "GameEngine/Object/Camera/MainCamera.h"

ObjectAdministrator* ObjectAdministrator::instance = nullptr;

std::shared_ptr<Object3d> ObjectAdministrator::CreateObject(const std::string& directrypath, const std::string& filename){
	// オブジェクトをSheard_ptrで生成する
	std::shared_ptr<Object3d> newObject = std::make_shared<Object3d>();
	newObject->Init();

	// モデル登録チェック
	newObject->SetModel(filename);
	newObject->model_->Initialize(directrypath,filename);

	return newObject;
}

ObjectAdministrator* ObjectAdministrator::GetInstance(){
	// nullptrであれば新規作成する
	if (instance == nullptr) {
		instance = new ObjectAdministrator();
	}
	return instance;
}

void ObjectAdministrator::Init(DirectXCommon* dxCommon){
	dxCommon_ = dxCommon;

	// モデル管理者クラス
	modelAdmin_ = ModelManager::GetInstance();
	modelAdmin_->Initialize(dxCommon_, MainCamera::GetInstance());
	// スプライト管理者クラス
	spriteAdmin_ = std::make_unique<SpriteAdministrator>();
	spriteAdmin_->Initialize(dxCommon_);


}

void ObjectAdministrator::Update(){

	// メインカメラ仮取得
	CameraCommon camera = *MainCamera::GetInstance();

	// 登録したオブジェクトをfor文で一個ずつ処理する
	for (auto& object : objectList_) {

		// カメラ行列のビュー行列(カメラのワールド行列の逆行列)
		object->viewM = camera.GetViewMatrix();
		// 正規化デバイス座標系(NDC)に変換(正射影行列をかける)
		object->pespectiveM = camera.GetProjectionMatrix();
		// WVPにまとめる
		object->wvpM = camera.GetViewProjectionMatrix();
		// 矩形のワールド行列とWVP行列を掛け合わした行列を代入
		object->wvpData->WVP = Multiply(object->worldTransform_.GetWorldMatrix(), object->wvpM);
		object->wvpData->World = object->worldTransform_.GetWorldMatrix();
	}

}

void ObjectAdministrator::Draw(){

	// 登録したオブジェクトをfor文で一個ずつ処理する
	for (auto& object : objectList_) {

		// モデル未登録のオブジェクト や 非表示に設定しているオブジェクトは早期リターンする(forなのでcontinue)
		if (object->GetModel() == nullptr) {
			continue;
		}

		//wvp用のCBufferの場所を指定
		dxCommon_->commandList->SetGraphicsRootConstantBufferView(1, object->wvpResource->GetGPUVirtualAddress());
		// VBV
		dxCommon_->commandList->IASetVertexBuffers(0, 1, &object->GetModel()->vertexBufferView);
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
		dxCommon_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// マテリアルのCBufferの場所を指定
		dxCommon_->commandList->SetGraphicsRootConstantBufferView(0, object->GetModel()->materialResource->GetGPUVirtualAddress());
		dxCommon_->commandList->SetGraphicsRootConstantBufferView(3, object->GetModel()->directionalLightResource->GetGPUVirtualAddress());
		dxCommon_->commandList->SetGraphicsRootConstantBufferView(4, object->GetModel()->CameraResource->GetGPUVirtualAddress());
		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		dxCommon_->commandList->SetGraphicsRootDescriptorTable(2, dxCommon_->srv_->textureData_.at(object->GetModel()->textureHandle_).textureSrvHandleGPU);
		// ドローコール
		dxCommon_->commandList->DrawInstanced(UINT(object->GetModel()->modelData.vertices.size()), 1, 0, 0);

	}

}

void ObjectAdministrator::Finalize(){
	modelAdmin_->Finalize();
	delete instance;
	instance = nullptr;
}
