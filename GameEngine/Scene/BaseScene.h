#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Object/Sprite/Sprite.h"
#include <memory>

class SceneManager;

class BaseScene{

public: // -- 公開メンバ 関数 -- //

	// 継承先で実装される関数
	// 抽象クラスなので純粋仮想関数にする

	// 初期化
	virtual void Init() = 0;
	// 更新
	virtual void Update() = 0;
	// 描画
	virtual void Draw() = 0;
	// UI描画
	virtual void DrawUI() = 0;
	// 終了処理
	virtual void Final() = 0;

	// 仮想デストラクタ(これがないと開放ミスが発生)
	virtual ~BaseScene() {};

	// シーンマネージャのポインタをセットする
	virtual void SetSceneManager(SceneManager* sceneManager) { mSceneManager = sceneManager; }

	// シーン名の取得
	virtual std::string GetSceneName() { return mSceneName; }

protected: // -- 限定公開 メンバ変数 -- //

	// シーンマネージャのポインタ
	SceneManager* mSceneManager;

	// シーン名
	std::string mSceneName;

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