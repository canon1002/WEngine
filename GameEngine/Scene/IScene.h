#pragma once

enum SCENE { TITLE, STAGE,RESULT,OVER };

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

	// 仮想デストラクタがないと警告される
	virtual ~IScene();

	// シーン番号のゲッター
	int GetSceneNo();

};

