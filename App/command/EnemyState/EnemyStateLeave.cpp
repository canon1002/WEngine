#include "EnemyStateLeave.h"
#include "App/object/Enemy.h"

void EnemyStateLeave::Init(Enemy* enemy){
	enemy_ = enemy;
}

void EnemyStateLeave::Update(){
	// Enemyポインタがnullなら早期リターンする
	if (enemy_ == nullptr) { return; }

	enemy_->worldTransform_.translate.z += 1.0f;
	if (enemy_->worldTransform_.translate.z >= 50.0f) {
		StateNo = Approatch;
	}
}
