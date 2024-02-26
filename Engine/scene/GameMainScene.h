#pragma once
#include "IScene.h"
#include "../../App/object/Player.h"
#include "../../App/object/Enemy.h"
#include "../../App/object/PlayerBullet.h"
//#include "../../App/object/EnemyBullet.h"
#include "../../App/object/Skydome.h"
#include "../../App/command/InputHandle.h"
#include "../../App/camera/RailCamera.h"
#include "../collision/CollisionManager.h"

#include <memory>

class GameMainScene :
	public IScene
{
public:

	//　継承した関数
	void Finalize()override;
	void Init() override;
	void Update() override;
	void Draw() override;

public:

	~GameMainScene() {}

	// プレイヤー
	std::unique_ptr<Player> player_;
	// プレイヤーの弾
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	// エネミー
	std::unique_ptr<Enemy> enemy_;
	std::unique_ptr<Enemy> enemy1_;
	std::unique_ptr<Enemy> enemy2_;


	// 天球
	std::unique_ptr<Skydome>skydome_;
	// レールカメラ
	std::unique_ptr<RailCamera> railCamera_;
	bool isActiveRailCamera_ = false;

private:
	std::unique_ptr <CollisionManager> cManager = nullptr;
	std::unique_ptr <InputHandle> inputHandler_ = nullptr;
	ICommand* command_ = nullptr;
	// エネミーの生存人数
	int aliveEnemyCount_;
	// 弾の上限
	int32_t bulletSaveMax = 3;
	int32_t bulletSaveCount = 3;

	std::unique_ptr<Sprite> sprite_ = nullptr;
	float alpth_ = 0.0f;
	bool isSceneChange = false;
	int32_t fadeTimer_ = 60;

};

