#pragma once
#include "IScene.h"
#include "../../App/object/Player.h"
#include "../../App/object/Enemy.h"
#include "../../App/object/PlayerBullet.h"
#include <memory>

class GameMainScene:
	public IScene
{
public:

	//　継承した関数
	void Init() override;
	void Update() override;
	void Draw() override;

public:

	// プレイヤー
	std::unique_ptr<Player> player_;


};

