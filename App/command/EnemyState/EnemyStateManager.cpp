#include "EnemyStateManager.h"
#include "App/object/Enemy.h"

// コンストラクタ
EnemyStateManager::EnemyStateManager() {
	// 各状態の配列
	enemyStateArr_[Approatch] = std::make_unique<EnemyStateApproach>();
	enemyStateArr_[Leave] = std::make_unique<EnemyStateLeave>();
	currentStateNo_ = Approatch;
}

// デストラクタ
EnemyStateManager::~EnemyStateManager() {}

void EnemyStateManager::Init(Enemy* enemy){
	// あらかじめ初期化を行う
	// 敵のポインタを入れておく
	enemyStateArr_[currentStateNo_]->Init(enemy);
}

// 実行
void EnemyStateManager::StateExecution(Enemy* enemy){
	// 状態チェック
	prevStateNo_ = currentStateNo_;
	currentStateNo_ = enemyStateArr_[currentStateNo_]->GetStateNo();

	// 状態変更チェック
	if (prevStateNo_ != currentStateNo_) {
		// 初期化を行う
		enemyStateArr_[currentStateNo_]->Init(enemy);
	}

	// 更新処理
	enemyStateArr_[currentStateNo_]->Update();

}
