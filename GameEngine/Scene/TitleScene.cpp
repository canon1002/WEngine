#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {
	// 入力を取得する
	input_ = InputManager::GetInstance();
	
	//testObject_ = ObjectAdministrator::GetInstance()->CreateObject("Resources/objs", "emptyAxis.obj");
	
	// モデル 読み込み
	ModelManager::GetInstance()->LoadModel("plane", "plane.gltf");
	ModelManager::GetInstance()->LoadModel("box", "box.gltf");
	ModelManager::GetInstance()->LoadModel("ball", "ball.obj");
	ModelManager::GetInstance()->LoadModel("AnimatedCube", "AnimatedCube.gltf");
	ModelManager::GetInstance()->LoadModel("simpleSkin", "simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("human", "sneakWalk.gltf");
	ModelManager::GetInstance()->LoadModel("human", "walk.gltf");

	// SkyBox 読み込み
	DirectXCommon::GetInstance()->srv_->LoadTexture("skybox/rostock_laage_airport_4k.dds");

	// 3dオブジェクトの宣言
	testObject_ = std::make_unique<Object3d>();
	testObject_->Init("TestObj");
	testObject_->SetModel("sneakWalk.gltf");
	testObject_->GetModel()->skinning_ = new Skinnig();
	testObject_->GetModel()->skinning_->Init("human", "sneakWalk.gltf",
		testObject_->GetModel()->modelData);
	testObject_->SetTranslate({ 0.0f,0.0f,5.0f });
	
	AnimeObject_ = std::make_unique<Object3d>();
	AnimeObject_->Init("AnimeObj");
	AnimeObject_->SetModel("walk.gltf");
	AnimeObject_->GetModel()->skinning_ = new Skinnig();
	AnimeObject_->GetModel()->skinning_->Init("human", "walk.gltf",
		AnimeObject_->GetModel()->modelData);
	AnimeObject_->SetTranslate({ 1.0f,1.0f,7.0f });
	
	testObject02_ = std::make_unique<Object3d>("Test Plane");
	testObject02_->Init("Test Plane");
	testObject02_->SetModel("plane.gltf");
	testObject02_->SetRotate({ 0.15f,0.4f,0.0f });
	testObject02_->SetTranslate({ 2.0f,0.0f,7.5f });

	// skyboxの宣言
	skybox_ = std::make_unique<Skybox>();
	skybox_->Init("skybox","rostock_laage_airport_4k.dds");

}

void TitleScene::Update() {
#ifdef _DEBUG
	skybox_->DrawGUI("Skybox");
#endif // _DEBUG

	MainCamera::GetInstance()->Update();

	// スティック入力の
	const static int stickValue = 2000;
	// いずれかの数値が、以上(以下)であれば移動処理を行う
	if (input_->GetStick(Gamepad::Stick::LEFT_X) < -stickValue || // 左 
		input_->GetStick(Gamepad::Stick::LEFT_X) > stickValue || // 右
		input_->GetStick(Gamepad::Stick::LEFT_Y) < -stickValue || // 上
		input_->GetStick(Gamepad::Stick::LEFT_Y) > stickValue	  // 下
		) {

		// Xの移動量とYの移動量を設定する
		Vector3 direction = { 
			(float)input_->GetStick(Gamepad::Stick::LEFT_X) ,
			0.0f,
			(float)input_->GetStick(Gamepad::Stick::LEFT_Y)
		};
		// 念のために正規化
		direction = Normalize(direction);
		
		// 平行移動を行う
		AnimeObject_->worldTransform_->translation += direction * 0.05f;

		// ここから回転処理
		const float PI = 3.14f;
		float rotateY = std::atan2f(direction.x, direction.z);
		rotateY = std::fmodf(rotateY, 2.0f * PI);
		if (rotateY > PI) {
			rotateY -= 2.0f * PI;
		}
		if (rotateY < -PI) {
			rotateY += 2.0f * PI;
		}
		AnimeObject_->worldTransform_->rotation.y = rotateY;
	}
	

	// SkinningModel 忍び歩き
	testObject_->Update();
	testObject_->DrawGUI();

	// SkinningModel 歩き
	AnimeObject_->Update();
	AnimeObject_->DrawGUI();
	
	// 平面
	testObject02_->Update();
	testObject02_->DrawGUI();

	// スカイボックス
	skybox_->Update();

}

void TitleScene::Draw(){


	// Skyboxの描画前処理
	skybox_->PreDraw();
	skybox_->Draw();

	// Object3D(3DModel)の描画前処理
	ModelManager::GetInstance()->PreDraw();

	testObject02_->Draw();

	// Object3D(Skinning)の描画前処理
	ModelManager::GetInstance()->PreDrawForSkinning();

	//testObject_->Draw();
	AnimeObject_->Draw();
}
