#include "ObjectAdministrator.h"
#include "GameEngine/Object/Camera/MainCamera.h"

ObjectAdministrator* ObjectAdministrator::instance = nullptr;

std::shared_ptr<Object3d> ObjectAdministrator::CreateObject(const std::string& directrypath, const std::string& filename){
	// オブジェクトをSheard_ptrで生成する
	std::shared_ptr<Object3d> newObject = std::make_shared<Object3d>();
	newObject->Init("Object3D");

	// モデル登録チェック
	newObject->SetModel(filename);
	newObject->mModel->Initialize(directrypath,filename);

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
	mDxCommon = dxCommon;

	// モデル管理者クラス
	modelAdmin_ = ModelManager::GetInstance();
	modelAdmin_->Initialize(mDxCommon, MainCamera::GetInstance());

	// スプライト管理者クラス
	spriteAdmin_ = SpriteAdministrator::GetInstance();
	spriteAdmin_->Init(mDxCommon);


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
		object->mWvpData->WVP = Multiply(object->mWorldTransform->GetWorldMatrix(), object->wvpM);
		object->mWvpData->World = object->mWorldTransform->GetWorldMatrix();
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
		mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(1, object->mWvpResource->GetGPUVirtualAddress());
		// VBV
		mDxCommon->mCommandList->IASetVertexBuffers(0, 1, &object->GetModel()->mVertexBufferView);
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
		mDxCommon->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// マテリアルのCBufferの場所を指定
		mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(0, object->GetModel()->materialResource->GetGPUVirtualAddress());
		mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(3, object->GetModel()->directionalLightResource->GetGPUVirtualAddress());
		mDxCommon->mCommandList->SetGraphicsRootConstantBufferView(4, object->GetModel()->CameraResource->GetGPUVirtualAddress());
		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		mDxCommon->mCommandList->SetGraphicsRootDescriptorTable(2, mDxCommon->srv_->textureData_.at(object->GetModel()->mTextureHandle).textureSrvHandleGPU);
		// ドローコール
		mDxCommon->mCommandList->DrawInstanced(UINT(object->GetModel()->modelData.vertices.size()), 1, 0, 0);

	}

}

void ObjectAdministrator::Finalize(){
	modelAdmin_->Finalize();
	delete instance;
	instance = nullptr;
}
