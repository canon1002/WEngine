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
};

