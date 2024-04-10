#include "IEnemyState.h"

// 初期状態をここで設定する
int IEnemyState::StateNo = Approatch;

// 仮想デストラクタ
IEnemyState::~IEnemyState(){}

// シーン番号のゲッター
int IEnemyState::GetStateNo() { return StateNo; }
