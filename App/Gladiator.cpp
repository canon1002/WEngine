#include "Gladiator.h"
#include "App/Scene/SceneFactory.h"

void Gladiator::Init(){

	// 基底クラスの初期化処理
	Framework::Init();

	// シーン工場を作成し、マネージャにセット
	mSceneFactory = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(mSceneFactory.get());

	// 最初のシーンを生成
	SceneManager::GetInstance()->ChangeScene("Title");
	SceneManager::GetInstance()->Init();
}

void Gladiator::Update(){

	// 終了リクエストの確認
	if (SceneManager::GetInstance()->mEndRequest) {

		// true だったらゲームの終了リクエストを基底クラスに送る
		mEndRequest = true;
	}

	// 基底クラスの更新処理
	Framework::Update();
}

void Gladiator::Draw(){

	// 描画処理
	SceneManager::GetInstance()->Draw();

}

void Gladiator::Final(){

	// 基底クラスの終了処理
	Framework::Final();
}
