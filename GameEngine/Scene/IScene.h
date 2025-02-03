#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Object/Sprite/Sprite.h"
#include <memory>

enum SCENE { 
	START,
	TITLE, 
	STAGE,
	RESULT,
	OVER ,
	MENU,
};

class IScene
{
protected:
	// シーンを管理する番号
	static int sceneNo;

public:

	virtual void Finalize() = 0;

	// 継承先で実装される関数
	// 抽象クラスなので純粋仮想関数にする
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawUI() = 0;

	// 仮想デストラクタ(これがないと開放ミス)
	virtual ~IScene();

	// シーン番号のゲッター
	int GetSceneNo();

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