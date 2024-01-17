#include "./Engine/base/DirectXCommon.h"
#include "./Engine/object/camera/MatrixCamera.h"
#include "./Engine/primitive/Triangle.h"
#include "./Engine/primitive/Sprite.h"
#include "./Engine/resources/Section/Resource.h"
#include "./Engine/primitive/Sphere.h"
#include "./Engine/object/model/Model.h"
#include "./Engine/primitive/VoxelParticle.h"
#include "./Input.h"
#include "./Engine/resources/Section/Audio.h"

#include <d3d12.h>


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// リリースチェック
	D3DResourceLeakChecker leakCheck;
	// WindowsAPI
	WinAPI* win = WinAPI::GetInstance();
	// DirectX
	DirectXCommon* dx = DirectXCommon::GetInstance();
	// Input
	Input* input = Input::GetInstance();
	// Audio
	Audio* audio = Audio::GetInstance();

	// 初期化
	win->Initialize();
	dx->Initialize(win);
	input->Init();
	audio->Init();

	// 球
	Sphere* sphere = new Sphere;
	sphere->Initialize();

	// 平面(sprite)
	Sprite* sprite = new Sprite;
	sprite->Initialize();

	// モデル
	Model* model = new Model;
	model->Initialize();

	// パーティクル
	VoxelParticle* voxels = new VoxelParticle;
	voxels->Initialize();

	// 音声データの生成
	SoundData sound1 = audio->LoadWave("Resources/sound/Alarm01.wav");
	audio->PlayWave(sound1);

	// ウィンドウの×ボタンが押されるまでループ
	while (true) {
		// Windowsのメッセージ処理
		if (win->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}

		// フレームの先頭でImGuiに、ここからフレームが始まる旨を伝える
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// フレームの先頭で入力処理の更新を行う
		input->Update();

		// ゲーム用の処理

		if (input->GetTriggerKey(DIK_2)) {
			audio->StopWave(sound1);
		}
		if (input->GetTriggerKey(DIK_SPACE)) {
			audio->PlayWave(sound1);
		}

		///
		/// 更新処理(推定)
		///

		// 開発用UIの表示
		ImGui::ShowDemoWindow();

		ImGui::Text("FPS : %.2f", ImGui::GetIO().Framerate);

		//sphere->Update();
		//sprite->Update();
		//model->Update();
		//voxels->Update();

		// 描画処理に入る前に、ImGui内部のコマンドを生成する
		ImGui::Render();

		///
		/// 描画処理(推定) 
		/// 

		// 描画前処理
		dx->DrawBegin();

		//sphere->Draw();
		//sprite->Draw();
		//model->Draw();

		// パーティクル
		dx->DrawPariticleBegin();
		//voxels->Draw();

		// 描画後処理
		dx->DrawEnd();

	}


	// Comの終了処理
	CoUninitialize();

	// 解放処理
	delete voxels;
	delete model;
	delete sprite;
	delete sphere;
	ImGui_ImplDX12_Shutdown();
	dx->Delete();



	win->Delete();
	/*CloseWindow(hwnd);*/

	return 0;
}
