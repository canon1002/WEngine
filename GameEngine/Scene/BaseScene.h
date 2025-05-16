#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Object/Sprite/Sprite.h"
#include <memory>

class SceneManager;

class BaseScene{

public: // -- 公開メンバ 関数 -- //

	

	// 継承先で実装される関数
	// 抽象クラスなので純粋仮想関数にする
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawUI() = 0;
	virtual void Final() = 0;

	// 仮想デストラクタ(これがないと開放ミスが発生)
	virtual ~BaseScene() {};

	// シーンマネージャのポインタをセットする
	virtual void SetSceneManager(SceneManager* sceneManager) { mSceneManager = sceneManager; }

protected: // -- 限定公開 メンバ変数 -- //

	// シーンマネージャのポインタ
	SceneManager* mSceneManager;

};

// とりあえずここに仮で配置

// 3次元ベクトル補間
struct EasingVector3 {
	Vector3 s;	// 始点
	Vector3 e;	// 終点
	float t;	// 時間
};

// UI
struct UISet {
	std::unique_ptr<Sprite> sprite;
	float t;
	float displayCount;
	bool isActive;
};