#include "BossEnemy.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "App/Player/Player.h"
#include "GameEngine/Append/Collider/AABBCollider.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Object/Model/Skybox/Skybox.h"

#include "GameEngine/Append/Collider/CollisionManager.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "App/Enemy/Action/ActionList.h"

#include "App/BlackBoard.h"

BossEnemy::BossEnemy(){}

BossEnemy::~BossEnemy(){}

void BossEnemy::Init() {

	// モデルの読み込み
	ModelManager::GetInstance()->LoadModel("twoHanded", "twoHanded.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "Idle.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "Run.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "backStep.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "death.gltf");

	ModelManager::GetInstance()->LoadModel("Boss", "Slash.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "SlashR.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "SlashJamp.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "SlashDash.gltf");

	ModelManager::GetInstance()->LoadModel("Boss", "ShotBullet.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "Thrust.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "magic.gltf");
	ModelManager::GetInstance()->LoadModel("Boss", "kick.gltf");

	ModelManager::GetInstance()->LoadModel("Sphere", "Sphere.gltf");

	// オブジェクト生成
	mObject = std::make_unique<Object3d>();
	mObject->Init("BossEnemyObj");
	mObject->mWorldTransform->scale = { 1.5f,1.5f,1.5f };
	mObject->mWorldTransform->translation = { 0.0f,0.0f,20.0f };

	// モデルを設定
	mObject->SetModel("boss.gltf");
	mObject->GetModel()->mMaterialData->color = { 1.0f,0.7f,0.7f,1.0f };
	// スキニングアニメーションの生成
	mObject->mSkinning = make_unique<Skinning>();
	mObject->mSkinning->Init("Boss", "Idle.gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->SetMotionBlendingInterval(2.0f);
	// 使用するアニメーションを登録しておく
	mObject->mSkinning->CreateSkinningData("Boss", "Idle", ".gltf", mObject->GetModel()->modelData, true);
	mObject->mSkinning->CreateSkinningData("Boss", "Walk", ".gltf", mObject->GetModel()->modelData, true);
	mObject->mSkinning->CreateSkinningData("Boss", "Dash", ".gltf", mObject->GetModel()->modelData, true);
	mObject->mSkinning->CreateSkinningData("Boss", "backStep", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "Knockback", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "death", ".gltf", mObject->GetModel()->modelData);

	mObject->mSkinning->CreateSkinningData("Boss", "Slash", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "Slash1", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "Slash2", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "SlashR", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "SlashDash", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "SlashJamp", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "Thrust", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "ShotBullet", ".gltf", mObject->GetModel()->modelData);

	mObject->mSkinning->CreateSkinningData("Boss", "magic", ".gltf", mObject->GetModel()->modelData);
	mObject->mSkinning->CreateSkinningData("Boss", "kick", ".gltf", mObject->GetModel()->modelData);

	// アニメーションの再生速度を1.5倍速に変更
	mObject->mSkinning->SetAnimationPlaySpeed(1.5f);

	// 移動量を初期化
	mVelocity = { 0.0f,0.0f ,0.002f };
	// 重力の影響を受ける
	mIsGravity = true;
	// ジャンプ回数のセット
	mJumpCount = kJumpCountMax;

	// ビヘイビアツリーの初期化
	this->InitBehavior();
	// 各行動クラスの初期化
	this->InitActions();

	// ステータス取得
	mStatus = StatusManager::GetInstance()->GetBossStatus();

	// コライダーの宣言
	mObject->mCollider = std::make_unique<SphereCollider>(mObject->mWorldTransform.get(), mObject->GetWorldTransform()->scale.x);
	mObject->mCollider->Init();
	mObject->mCollider->SetAddTranslation(Vector3(0.0f, mObject->GetWorldTransform()->scale.y, 0.0f));
	mObject->mCollider->SetCollisionAttribute(kCollisionAttributeEnemy);
	mObject->mCollider->SetCollisionMask(kCollisionAttributePlayerBullet);

	// 身体の部位に合わせたコライダーを生成
	CreateBodyPartCollider("Head", 0.15f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("Neck", 0.3f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("Spine", 0.4f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("Spine1", 0.4f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("Spine2", 0.4f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("LeftUpLeg", 0.15f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("LeftLeg", 0.15f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("LeftFoot", 0.15f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("RightUpLeg", 0.15f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("RightLeg", 0.15f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("RightFoot", 0.15f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("LeftForeArm", 0.15f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("LeftHand", 0.1f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("RightForeArm", 0.15f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);
	CreateBodyPartCollider("RightHand", 0.1f, kCollisionAttributeEnemy, kCollisionAttributePlayerBullet);

	mRightHandWorldMat = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });

	mWeapon = std::make_unique<Object3d>();
	mWeapon->Init("Weapon");
	mWeapon->SetModel("twoHanded.gltf");
	mWeapon->mSkinning = make_unique<Skinning>();
	//mWeapon->GetModel()->SetCubeTexture(Skybox::GetInstance()->mTextureHandle);
	mWeapon->mSkinning->Init("twoHanded", "twoHanded.gltf",
		mWeapon->GetModel()->modelData);
	mWeapon->mSkinning->IsInactive();

	// 拡大率を変更
	mWeapon->mWorldTransform->scale = { 4.0f,4.0f,4.0f };
	// 回転量を変更
	mWeapon->mWorldTransform->rotation = { 1.065f,0.0f,0.0f };
	// 平行移動を行う
	mWeapon->mWorldTransform->translation = { 4.6f,-5.9f,5.8f };

	// ペアレント
	mWeapon->mWorldTransform->SetParent(mRightHandWorldMat);

	// 武器にコライダーをセットする
	for (int32_t i = 0; i < 8; i++) {
		mWeaponWorldMat[i] = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
		// コライダー 宣言
		SphereCollider* newCollider = new SphereCollider(new WorldTransform, 0.4f);
		// 初期化
		newCollider->Init();
		newCollider->SetCollisionAttribute(kCollisionAttributeEnemyBullet);
		newCollider->SetCollisionMask(kCollisionAttributePlayer);
		// 武器に設定したボーンのワールド座標をセット
		newCollider->GetWorld()->SetParent(mWeaponWorldMat[i]);
		//// 配列にプッシュする
		mWeaponColliders.push_back(newCollider);
	}

	mReloadBTCount = 10.0f;

	// -- エフェクト関係 -- //

	// 剣先と根本のワールド座標
	for (size_t i = 0; i < mWorldTransformSword.size(); i++) {
		mWorldTransformSword[i] = make_unique<WorldTransform>();
		mWorldTransformSword[i]->Init();
	}
	// ペアレントを設定(後にワールド座標を取得する)
	mWorldTransformSword[0]->SetParent(mWeaponWorldMat[0]);
	mWorldTransformSword[1]->SetParent(mWeaponWorldMat[4]);

	// 仮ノックバック処理
	mKnockBackCount = 0;

	// 方向指定(とりあえず手前を向く)
	mDirection = { 0.0f,0.0f,-1.0f };
	mDirectionForInput = { 0.0f,0.0f,0.0f };

}

void BossEnemy::InitBehavior() {

	// ボスのState番号の配列(実行中に増減しないため、長さ固定の配列を宣言)
	mStateArr[VITALITY] = std::make_unique<VitalityBossState>();
	// 初期シーンの初期化
	mStateArr[VITALITY]->Init(this);

	// Behavior Treeを構築する 
	mRoot = std::make_unique<BT::Sequence>();

	// 接近 -> 近接攻撃
	BT::Sequence* newSequence = new BT::Sequence();
	BT::Selector* ReafOneSelector = new BT::Selector();

	// プレイヤーが遠い場合
	newSequence->SetChild(new BT::Condition(std::bind(&BossEnemy::InvokeFarDistance, this)));
	newSequence->SetChild(new BT::Action(this, "ShotBullet"));// 射撃
	newSequence->SetChild(new BT::Action(this, "MoveToPlayer"));// 接近

	// 接近行動の終了時、敵の飛距離に応じた行動を取る
	// 至近距離 : ジャンプ攻撃
	// 中遠距離 : ダッシュ → ダッシュ攻撃
	BT::Selector* atkSelector = new BT::Selector();

	BT::Sequence* farAtkActions = new BT::Sequence();
	farAtkActions->SetChild(new BT::Condition(std::bind(&BossEnemy::InvokeFarDistance, this)));
	farAtkActions->SetChild(new BT::Action(this, "Shrinkage"));
	farAtkActions->SetChild(new BT::Action(this, "AttackClose"));
	farAtkActions->SetChild(new BT::Action(this, "Shrinkage"));
	farAtkActions->SetChild(new BT::Action(this, "AttackDash"));

	atkSelector->SetChild(farAtkActions); // 中遠距離
	atkSelector->SetChild(new BT::Action(this, "AttackJump"));
	newSequence->SetChild(atkSelector);

	// ルートノードに追加
	ReafOneSelector->SetChild(newSequence);


	// 接近状態だったら
	BT::Sequence* startNear = new BT::Sequence();
	startNear->SetChild(new BT::Action(this, "AttackThrust"));	// 刺突
	startNear->SetChild(new BT::Condition(std::bind(&BossEnemy::InvokeNearDistance, this))); // 至近距離時
	startNear->SetChild(new BT::Action(this, "BackStep")); // 後退する
	ReafOneSelector->SetChild(startNear);


	mRoot->SetChild(ReafOneSelector);

}

void BossEnemy::InitActions()
{
	// 各行動をmap配列に追加していく

	// 接近
	mActions["MoveToPlayer"] = make_shared<ACT::ChaseTarget>();
	mActions["MoveToPlayer"]->Init(this);

	// ダッシュ
	mActions["Dash"] = make_shared<ACT::Dash>();
	mActions["Dash"]->Init(this);

	// 縮地
	mActions["Shrinkage"] = make_shared<ACT::Shrinkage>();
	mActions["Shrinkage"]->Init(this);

	// バックステップ
	mActions["BackStep"] = make_shared<ACT::BackStep>();
	mActions["BackStep"]->Init(this);

	// 近接攻撃
	mActions["AttackClose"] = make_shared<ACT::AttackClose>();
	mActions["AttackClose"]->Init(this);

	// 刺突攻撃
	mActions["AttackThrust"] = make_shared<ACT::AttackThrust>();
	mActions["AttackThrust"]->Init(this);

	// ダッシュ攻撃
	mActions["AttackDash"] = make_shared<ACT::AttackDash>();
	mActions["AttackDash"]->Init(this);

	// ジャンプ攻撃
	mActions["AttackJump"] = make_shared<ACT::AttackJump>();
	mActions["AttackJump"]->Init(this);

	// 射撃
	mActions["ShotBullet"] = make_shared<ACT::ShotBullet>();
	mActions["ShotBullet"]->Init(this);

	// のけぞり
	mActions["knockBack"] = make_shared<ACT::KnockBack>();
	mActions["knockBack"]->Init(this);

	// 初期は行動しない
	mActiveAction.reset();

}

void BossEnemy::Update() {

	// テーブルから関数を呼び出す
	//(this->*CommandTable[0])();

	// 一旦ここに落下処理をつくる
	if (mObject->mWorldTransform->translation.y > 0.0f) {
		// 移動量を加算
		mObject->mWorldTransform->translation.y += mVelocity.y;
		mVelocity.y -= 9.8f * (1.0f / 720.0f);
	}
	else if (mObject->mWorldTransform->translation.y < 0.0f) {
		mObject->mWorldTransform->translation.y = 0.0f;
		// 移動量修正
		mVelocity.y = 0.0f;
	}

	if (mStatus->HP > 0.0f) {

		// ステートの更新処理を行う
		this->UpdateState();

		// ノックバックカウントが最大の場合はBehaviorTreeの更新を行わない
		//if (mKnockBackCount>=kKnockBackCountMax) {
		//
		//	// ノックバックが終了したら
		//	if (mActiveAction.lock()->GetCondition() == ACT::Condition::FINISHED) {
		//		// カウント0に戻す
		//		mKnockBackCount = 0;
		//		
		//		// ビヘイビアツリーをリセット
		//		mRoot->Reset();

		//		// 各アクションの初期化もしておく
		//		for (auto& action : mActions) {
		//			action.second->End();
		//			action.second->Reset();
		//		}

		//		// 行動クラスのポインタをnullptrにする
		//		mActiveAction.reset();
		//	}
		//}
		//else {
		//	
		//}

		// BehaviorTreeの更新処理を行う
		if (mIsUpdateBehavior) {
			this->UpdateBehaviorTree();
		}
	}


	// シェイクの更新
	ShakeUpdate();

	// 方向修正
	AdJustDirection();

	// オブジェクト更新
	UpdateObject();

	// 衝突処理
	if (!mActiveAction.expired()) {

		if (mActiveAction.lock()->GetIsHit() == false &&
			mActiveAction.lock()->GetIsOperating() == true){

			for (Collider* collider : mWeaponColliders)
			{
				if (collider->GetOnCollisionFlag() == false)
				{
					continue;
				}

				// 次のフレームで消す
				mActiveAction.lock()->Hit();

				// 敵にダメージを与える
				ReciveDamageTolayer(1.8f);

			}
		}
	}



}

void BossEnemy::UpdateBehaviorTree() {

	// リロードカウントを減らす
	mReloadBTCount -= BlackBoard::CombertBattleFPS(6.0f);

	// リロードカウントが0以下になったら行動を実行
	if (mReloadBTCount <= 0.0f) {

		// リロードカウントをリセット
		mReloadBTCount = 1.0f;

		// ビヘイビアツリーの実行
		mBTStatus = mRoot->Tick();

		// ビヘイビアツリーの実行結果が成功か失敗だったら
		if (mBTStatus == BT::NodeStatus::SUCCESS || mBTStatus == BT::NodeStatus::FAILURE) {

			// ビヘイビアツリーをリセット
			mRoot->Reset();

			// 各アクションの初期化もしておく
			for (auto& action : mActions) {
				action.second->End();
				action.second->Reset();
			}

			// 行動クラスのポインタをnullptrにする
			mActiveAction.reset();
		}
	}
}

void BossEnemy::UpdateObject() {

	// アクションの中身が存在する場合はアクションクラスの更新を行う
	if (!mActiveAction.expired()) {
		mActiveAction.lock()->Update();
	}


	// ステージ限界値に合わせた座標の補正
	mObject->mWorldTransform->translation.x = std::clamp(mObject->mWorldTransform->translation.x, -20.0f, 20.0f);
	mObject->mWorldTransform->translation.z = std::clamp(mObject->mWorldTransform->translation.z, -20.0f, 20.0f);

	// 弾の更新処理
	for (auto& bullet : mBullets) {
		bullet->Update();
	}

	// 弾の消滅処理
	mBullets.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			return true;
		}
		return false;
		});

	// 右手のワールド行列を更新
	mRightHandWorldMat = Multiply(
		GetObject3D()->mSkinning->GetSkeleton().joints[GetObject3D()->mSkinning->GetSkeleton().jointMap["mixamorig:RightHandThumb1"]
		].skeletonSpaceMatrix, GetObject3D()->GetWorldTransform()->GetWorldMatrix());

	// 武器の更新処理
	mWeapon->Update();


	// ワールド座標更新
	mWeaponWorldMat[0] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade0"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[1] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade1"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[2] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade2"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[3] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade3"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[4] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade4"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[5] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade5"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[6] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade6"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());
	mWeaponWorldMat[7] = Multiply(
		mWeapon->mSkinning->GetSkeleton().joints[mWeapon->mSkinning->GetSkeleton().jointMap["Blade7"]
		].skeletonSpaceMatrix, mWeapon->GetWorldTransform()->GetWorldMatrix());

	// 身体の部位のワールド行列を更新
	UpdateBodyMatrix();

	// 身体の部位に合わせたコライダーを更新
	for (auto& collider : mBodyPartColliders) {
		collider.second->Update();
	}

	// 武器のコライダー 更新
	for (Collider* collider : mWeaponColliders) {
		collider->Update();
	}

	// オブジェクト更新
	mObject->Update();
	mObject->mCollider->Update();

	// UI更新
	mStatus->Update();

}

void BossEnemy::Draw() {
	mObject->Draw();
}

void BossEnemy::DrawGUI() {
#ifdef _DEBUG
	const char* behaviorState[] = {
		"SUCCESS", // 成功
		"FAILURE", // 失敗
		"RUNNING", // 実行中 
	};
	int32_t currentItem = (int32_t)mBTStatus;

	// 武器のワールド行列を取得
	Matrix4x4 matrix = mWeapon->GetWorldTransform()->GetWorldMatrix();

	// スケールを抽出
	Vector3 col0(matrix.m[0][0], matrix.m[1][0], matrix.m[2][0]);
	Vector3 col1(matrix.m[0][1], matrix.m[1][1], matrix.m[2][1]);
	Vector3 col2(matrix.m[0][2], matrix.m[1][2], matrix.m[2][2]);

	Vector3 scale = { 1.0f,1.0f,1.0f };
	scale.x = Length(col0);
	scale.y = Length(col1);
	scale.z = Length(col2);

	// スケール成分を取り除いて回転行列を正規化
	col0 = Normalize(col0);
	col1 = Normalize(col1);
	col2 = Normalize(col2);

	// 回転行列からEuler角を計算
	Vector3 rotation = { 0.0f,0.0f,0.0f };
	rotation.y = std::asin(-col0.z);
	if (std::cos(rotation.y) != 0) {
		rotation.x = std::atan2(col1.z, col2.z);
		rotation.z = std::atan2(col0.y, col0.x);
	}
	else {
		rotation.x = std::atan2(-col2.x, col1.y);
		rotation.z = 0;
	}

	ImGui::Begin("BossEnemy");
	ImGui::Checkbox("UpdateBehaviorTree", &mIsUpdateBehavior);
	if (ImGui::CollapsingHeader("Animation")) {

		std::string strAnimeT = "AnimationTime : " + std::to_string(mObject->mSkinning->GetNowSkinCluster()->animationTime / mObject->mSkinning->GetDurationTime());
		ImGui::ProgressBar(mObject->mSkinning->GetNowSkinCluster()->animationTime / mObject->mSkinning->GetDurationTime(), ImVec2(-1.0f, 0.0f), strAnimeT.c_str());

		std::string strNormalT = "MotionBlendingTime : " + std::to_string(mObject->mSkinning->GetMotionBlendingTime());
		ImGui::ProgressBar(mObject->mSkinning->GetMotionBlendingTime(), ImVec2(-1.0f, 0.0f), strNormalT.c_str());

		// Skinning
		mObject->mSkinning->DrawGui();
	}
	ImGui::ListBox("State", &currentItem, behaviorState, IM_ARRAYSIZE(behaviorState), 3);
	ImGui::DragInt("HP", &mStatus->HP, 1.0f, 0, 100);
	ImGui::DragInt("STR", &mStatus->STR, 1.0f, 0, 100);
	ImGui::DragInt("VIT", &mStatus->VIT, 1.0f, 0, 100);
	ImGui::DragInt("AGI", &mStatus->AGI, 1.0f, 0, 100);
	mObject->DrawGuiTree();
	mWeapon->DrawGuiTree();
	ImGui::DragFloat3("Scale", &scale.x);
	ImGui::DragFloat3("Rotation", &rotation.x);
	for (int i = 0; i < 4; ++i) {
		// Floatの4x4行列内の数値を表示
		ImGui::DragFloat4(("Row " + std::to_string(i)).c_str(), mWeapon->GetWorldTransform()->GetWorldMatrix().m[i]);
	}


	ImGui::End();
#endif // _DEBUG
}

void BossEnemy::SetAttackCollider(CollisionManager* cManager) {

	// 行動設定時
	if (!mActiveAction.expired()) {

		// 攻撃中 かつ 攻撃が命中していなければ
		if (mActiveAction.lock()->GetIsOperating() &&
			!mActiveAction.lock()->GetIsHit()) {

			// コライダーセット
			for (Collider* collider : mWeaponColliders) {
				cManager->SetCollider(collider);
			}

		}

	}

	// 弾のコライダーを登録
	for (auto& bullet : mBullets) {
		cManager->SetCollider(bullet->GetObject3d()->mCollider.get());
	}


}

void BossEnemy::UpdateState() {

	// ステートのチェック
	mPrevStateNo = mCurrentStateNo;
	mCurrentStateNo = mStateArr[mCurrentStateNo]->GetStateNo();

	// ステートが変更されているかの確認
	if (mPrevStateNo != mCurrentStateNo) {

		// ステートが変更されたら初期化を行う
		mStateArr[mCurrentStateNo]->Init(this);
	}

	// ステート 更新処理
	mStateArr[mCurrentStateNo]->Update();

}

void BossEnemy::ReciveDamageTolayer(float power)
{
	// プレイヤーにダメージを与える
	StatusManager::GetInstance()->ReceiveDamage(mStatus, power, mTarget->GetStatus());
}





void BossEnemy::SetShake(float duration, float power) {
	mShakeDuration = duration;
	mShakePower = power;
	mShakeTime = 0.0f;
}

void BossEnemy::ShakeUpdate() {

	// シェイク処理
	if (mShakeTime < mShakeDuration) {

		// シェイク時間を加算
		mShakeTime += BlackBoard::GetBattleFPS();

		// シェイクの振幅をX方向とZ方向をそれぞれ計算
		float shakeOffsetX = (rand() % 100 / 100.0f - 0.5f) * mShakePower;
		float shakeOffsetZ = (rand() % 100 / 100.0f - 0.5f) * mShakePower;
		mObject->mWorldTransform->translation.x += shakeOffsetX;
		mObject->mWorldTransform->translation.z += shakeOffsetZ;
	}
}

void BossEnemy::SetKnockBackCount(int32_t count) {
	count;
	if (mKnockBackCount < kNumMaxInfluence) {
		//mKnockBackCount += count;
	}
	else {
		// 行動を終了
		//mActiveAction.lock()->End();
		// 行動を設定(ノックバック)
		SetAction("knockBack");
	}
}

void BossEnemy::Save()
{
}

void BossEnemy::Load()
{
}

void BossEnemy::ColliderDraw() {
	
	// 武器の描画
	mWeapon->Draw();

	// 弾の描画
	for (auto& bullet : mBullets) {
		bullet->Draw();
	}


#ifdef _DEBUG

	if (!mActiveAction.expired()) {
		if (mActiveAction.lock()->GetIsOperating() &&
			!mActiveAction.lock()->GetIsHit()) {
			for (auto& collider : mWeaponColliders) {
				collider->Draw();
			}
		}
	}


	// 身体の部位に合わせたコライダーを描画
	for (auto& collider : mBodyPartColliders) {
		collider.second->Draw();
	}

#endif // _DEBUG
}