#include "IScene.h"

// 初期シーン
int IScene::sceneNo = START;

// 仮想デストラクタの定義
// 純粋仮想関数化していないので、ここで定義できる
IScene::~IScene(){}

// シーン番号のゲッター
int IScene::GetSceneNo() { return sceneNo; }