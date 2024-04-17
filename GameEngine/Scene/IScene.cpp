#include "IScene.h"

// 初期シーンを代入(タイトルで初期化)
int IScene::sceneNo = TITLE;

// 仮想デストラクタの定義
// 純粋仮想関数化していないので、ここで定義できる
IScene::~IScene(){}

// シーン番号のゲッター
int IScene::GetSceneNo() { return sceneNo; }