#include "TitleScene.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

void TitleScene::Finalize(){}

//　継承した関数
void TitleScene::Init() {
	// 入力を取得する
	mInput = InputManager::GetInstance();
	
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
	testObject_->GetModel()->skinning_->Init("human", "walk.gltf",
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

	// グリッド生成  // 左の引数はグリッドのセル数、右の引数はセルの大きさを入れる
	grid_ = std::make_unique<Grid3D>(5,1.0f);

	MainCamera::GetInstance()->SetTarget(AnimeObject_->GetWorldTransform());
}

void TitleScene::Update() {
#ifdef _DEBUG
	skybox_->DrawGUI("Skybox");
#endif // _DEBUG

	MainCamera::GetInstance()->Update();

	// RBボタンを押してたら歩く
	if (mInput->GetPused(Gamepad::Button::RIGHT_SHOULDER)) {
		AnimeObject_->GetModel()->skinning_->Init("human", "walk.gltf",
			AnimeObject_->GetModel()->modelData);
	}

	// RBを離したらスニーク
	if (mInput->GetReleased(Gamepad::Button::RIGHT_SHOULDER)) {
		AnimeObject_->GetModel()->skinning_->Init("human", "sneakWalk.gltf",
			AnimeObject_->GetModel()->modelData);
	}

	// スティック入力の量
	const static int stickValue = 6000;
	// いずれかの数値が、以上(以下)であれば移動処理を行う
	if (mInput->GetStick(Gamepad::Stick::LEFT_X) < -stickValue || // 左 
		mInput->GetStick(Gamepad::Stick::LEFT_X) > stickValue || // 右
		mInput->GetStick(Gamepad::Stick::LEFT_Y) < -stickValue || // 上
		mInput->GetStick(Gamepad::Stick::LEFT_Y) > stickValue	  // 下
		) {

		// Xの移動量とYの移動量を設定する
		Vector3 direction = { 
			(float)mInput->GetStick(Gamepad::Stick::LEFT_X) ,
			0.0f,
			(float)mInput->GetStick(Gamepad::Stick::LEFT_Y)
		};
		// 念のために正規化
		direction = Normalize(direction);
		
		// 移動速度を設定
		float moveSpeed = 0.05f;
		// RB入力時、移動速度を上げる
		if (mInput->GetLongPush(Gamepad::Button::RIGHT_SHOULDER)) {
			moveSpeed *= 2.0f;
		}

		// カメラの回転量を反映
		direction = TransformNomal(direction, MainCamera::GetInstance()->worldTransform_->GetWorldMatrix());
		// y座標は移動しない
		direction.y = 0.0f;

		// 平行移動を行う
		AnimeObject_->worldTransform_->translation += direction * moveSpeed;

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
	

	grid_->Update();

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

	//grid_->PreDraw();
	//grid_->Draw();

	// Object3D(3DModel)の描画前処理
	ModelManager::GetInstance()->PreDraw();

	testObject02_->Draw();

	// Object3D(Skinning)の描画前処理
	ModelManager::GetInstance()->PreDrawForSkinning();

	testObject_->Draw();
	AnimeObject_->Draw();


}
