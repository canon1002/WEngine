#include "EnemyStateApproach.h"
#include "App/object/Enemy.h"

void EnemyStateApproach::Init(Enemy* enemy){
	enemy_ = enemy;

	// 弾の射撃する間隔
	// 最初は長めに待つ
	shotCoolTIme_ = 180;
}

void EnemyStateApproach::Update(){

	// Enemyポインタがnullなら早期リターンする
	if (enemy_ == nullptr) { return; }

	// クールタイム中は攻撃しない
	if (shotCoolTIme_ > 0) { shotCoolTIme_--; }
	// クールタイムが終了したら攻撃する
	if (shotCoolTIme_ <= 0) {
		// 攻撃フラグをTrueにする
		enemy_->ShotBullet();
		// クールタイムを再設定
		// 二発目以降は短めにする
		shotCoolTIme_ = 60;
	}

	enemy_->worldTransform_.translate.z -= 0.05f;

	if (enemy_->GetWorldPos().z <= -20.0f) {
		StateNo = Leave;
	}

}
