#pragma once
#include "IScene.h"
#include "Engine/Object/Sprite/Sprite.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Object/Camera/MainCamera.h"

#include "App/object/Player.h"
#include "App/object/Enemy.h"
#include "App/object/PlayerBullet.h"
#include "App/object/EnemyBullet.h"
#include "App/object/Skydome.h"
#include "App/command/InputHandle.h"
#include "App/camera/RailCamera.h"
#include "Engine/Collision/CollisionManager.h"

class GameMainScene :
	public IScene
{
public:

	~GameMainScene() {}
	//　継承した関数
	void Finalize()override;
	void Init() override;
	void Update() override;
	void Draw() override;

	void LoadEnemyPopDate();
	void UpdateEnemyPopCommands();

	// プレイヤー
	std::unique_ptr<Player> player_;
	// プレイヤーの弾
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
	// エネミー
	std::list< std::unique_ptr<Enemy>> enemies_;
	// エネミーの弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	// 天球
	std::unique_ptr<Skydome>skydome_;
	// レールカメラ
	std::unique_ptr<RailCamera> railCamera_;
	bool isActiveRailCamera_ = false;


private:

	InputManager* input_;
	MainCamera* camera_;

	std::unique_ptr <CollisionManager> cManager = nullptr;
	std::unique_ptr <InputHandle> inputHandler_ = nullptr;
	ICommand* command_ = nullptr;
	// エネミーの生存人数
	int aliveEnemyCount_;

	// 敵発生コマンド
	std::stringstream enemyPopCommands;
	// 待機中フラグ
	bool m_isWait;
	//  待機中タイマー
	int32_t m_waitTime;

};

