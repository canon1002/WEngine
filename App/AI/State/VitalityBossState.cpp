#include "VitalityBossState.h"
#include "App/Enemy/BossEnemy.h"

void VitalityBossState::Init(BossEnemy* boss) {

	// ポインタを渡す
	mBoss = boss;

	// Behavior Treeを構築する 
	// 子ノード生成時、ボスのポインタと条件用の関数ポインタを渡すこと
	

	// Behavior Treeの実行速度を設定
	mBehaviorRunCount = kBehaviorRunCountMax;

}

void VitalityBossState::Update() {

	//// ビヘイビアツリーの実行
	//mBehaviorRunCount--;
	//// カウントが0になったら実行する
	//if (mBehaviorRunCount <= 0) {
	//	// 行動選択を実行
	//	mBehaviorUpdate = mRoot->Execute();
	//	// カウントを最大に戻す
	//	mBehaviorRunCount = kBehaviorRunCountMax;
	//}
	//
	//// 取得したコマンドを実行
	//if (mBehaviorUpdate != nullptr) {
	//	mBehaviorUpdate();
	//}

};
