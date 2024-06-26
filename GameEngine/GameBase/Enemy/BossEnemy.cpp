#include "BossEnemy.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"

void BossEnemy::Init() {
	mObject = std::make_unique<Object3d>();
	mObject->Init("BossEnemyObj");
	mObject->SetModel("sneakWalk.gltf");
	mObject->GetModel()->skinning_ = new Skinnig();
	mObject->GetModel()->skinning_->Init("human", "sneakWalk.gltf",
		mObject->GetModel()->modelData);
	mObject->SetTranslate({ 3.0f,1.0f,7.0f });
	mObject->GetModel()->materialData_->color = { 1.0f,0.7f,0.7f,1.0f };

	// 移動量を初期化
	mVelocity = { 0.0f,0.0f ,0.002f };
	// 重力の影響を受ける
	mIsGravity = true;
	// ジャンプ回数のセット
	mJumpCount = kJumpCountMax;

	// ビヘイビアツリーの初期化
	this->InitBehavior();

}

void BossEnemy::InitBehavior() {

	// ボスのState番号の配列(実行中に増減しないため、長さ固定の配列を宣言)
	mStateArr[VITALITY] = std::make_unique<VitalityBossState>();
	

	mRoot = std::make_unique<SelectorNode>();
	mRoot->AddBehavior(new ActionNode());
}

// 関数ポインタテーブルの実体
void (BossEnemy::* BossEnemy::CommandTable[])() = {
	&BossEnemy::MoveForward,
	&BossEnemy::BackStep,
};


void BossEnemy::Update() {

	// テーブルから関数を呼び出す
	(this->*CommandTable[0])();
	mCurrentSceneNo;
	mPrevSceneNo;

	// オブジェクト更新
	mObject->Update();
	mObject->mModel->skinning_->GetSkeleton().joints;
}

void BossEnemy::Draw() {
	mObject->Draw();
}

void BossEnemy::DrawGUI() {
	mObject->DrawGUI();
}