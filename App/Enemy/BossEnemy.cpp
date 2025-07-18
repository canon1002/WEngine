#include "BossEnemy.h"
#include "GameEngine/Resource/Model/ModelManager.h"
#include "GameEngine/Object/ObjectAdministrator.h"
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "App/Player/Player.h"
#include "GameEngine/Component/Collider/AABBCollider.h"
#include "GameEngine/Component/Collider/SphereCollider.h"
#include "GameEngine/Object/Skybox/Skybox.h"

#include "GameEngine/Component/Collider/CollisionManager.h"
#include "GameEngine/Editor/Framerate.h"
#include "App/Enemy/Action/ActionList.h"

#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Resource/Material/MaterialManager.h"

BossEnemy::BossEnemy() {}

BossEnemy::~BossEnemy() {}

void BossEnemy::Init() {

	// クラス名を設定
	mName = "Boss";

	// モデルの読み込み
	ModelManager::GetInstance()->LoadModel("Sphere", "Sphere.gltf");

	// オブジェクト生成
	mObject = std::make_unique<Object3d>();
	mObject->Init("BossEnemyObj");
	mObject->mWorldTransform->scale = { 3.0f,3.0f,3.0f };
	mObject->mWorldTransform->translation = { 0.0f,0.0f,20.0f };

	// モデルを設定
	mObject->SetModelFullPath("Actor", "Actor.gltf");
	mObject->mMaterial->SetColorAll({ 1.0f,1.0f,1.0f,1.0f });
	// スキニングアニメーションの生成
	mObject->mSkinning = make_unique<Skinning>();
	mObject->mSkinning->Init("Actor", "Actor_Idle.gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->SetMotionBlendingInterval(2.0f);
	// 使用するアニメーションを登録しておく
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Idle", ".gltf", mObject->GetModel()->mModelData, true);
	
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Avoid", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_BackStep", ".gltf", mObject->GetModel()->mModelData);

	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Walk", ".gltf", mObject->GetModel()->mModelData, true);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Run", ".gltf", mObject->GetModel()->mModelData, true);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_BackRun", ".gltf", mObject->GetModel()->mModelData, true);
	
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_S0", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_S1", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_S2", ".gltf", mObject->GetModel()->mModelData);

	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Knockback", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_Death", ".gltf", mObject->GetModel()->mModelData);

	mObject->mSkinning->CreateSkinningData("Actor", "Actor_MagicCastLong", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Actor", "Actor_MagicCast", ".gltf", mObject->GetModel()->mModelData);

	/*mObject->mSkinning->CreateSkinningData("Boss", "Slash", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Boss", "Slash1", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Boss", "Slash2", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Boss", "SlashR", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Boss", "SlashDash", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Boss", "SlashJamp", ".gltf", mObject->GetModel()->mModelData);
	mObject->mSkinning->CreateSkinningData("Boss", "Thrust", ".gltf", mObject->GetModel()->mModelData);*/

	// アニメーションの再生速度を1.5倍速に変更
	mObject->mSkinning->SetAnimationPlaySpeed(1.5f);

	// 移動量を初期化
	mVelocity = { 0.0f,0.0f ,0.002f };
	// 重力の影響を受ける
	mIsGravity = true;
	// ジャンプ回数のセット
	mJumpCount = kJumpCountMax;

	// 各行動クラスの初期化
	this->InitActions();
	// ビヘイビアツリーの初期化
	this->InitBehavior();
	

	// ステータス取得
	mStatus = GameManager::GetInstance()->GetStatus("Boss");

	// コライダーの宣言
	mObject->mCollider = std::make_unique<SphereCollider>(mObject->mWorldTransform.get(), mObject->GetWorldTransform()->scale.x);
	mObject->mCollider->Init();
	mObject->mCollider->SetAddTranslation(Vector3(0.0f, mObject->GetWorldTransform()->scale.y, 0.0f));
	mObject->mCollider->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kEnemy));

	// 身体の部位に合わせたコライダーを生成
	CreateBodyPartCollider("Head", 0.15f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("Neck", 0.3f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("Spine", 1.25f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("Spine1", 0.4f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("Spine2", 0.4f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("LeftUpLeg", 0.15f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("LeftLeg", 0.15f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("LeftFoot", 0.15f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("RightUpLeg", 0.15f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("RightLeg", 0.15f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("RightFoot", 0.15f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("LeftForeArm", 0.15f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("LeftHand", 0.1f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("RightForeArm", 0.15f, static_cast<uint32_t>(CollisionTypeId::kEnemy));
	CreateBodyPartCollider("RightHand", 0.1f, static_cast<uint32_t>(CollisionTypeId::kEnemy));

	mRightHandWorldMat = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });

	mWeapon = std::make_unique<Object3d>();
	mWeapon->Init("Weapon");
	mWeapon->SetModelFullPath("Weapons","ASword.gltf");
	mWeapon->mSkinning = make_unique<Skinning>();
	mWeapon->GetModel()->SetCubeTexture(Skybox::GetInstance()->mTextureHandle);
	mWeapon->mSkinning->Init("Weapons", "ASword.gltf",
		mWeapon->GetModel()->mModelData);
	mWeapon->mSkinning->IsInactive();

	// 拡大率を変更
	mWeapon->mWorldTransform->scale = { 1.5f,1.5f,1.5f };
	// ペアレント
	mWeapon->mWorldTransform->SetParent(mRightHandWorldMat);

	// 武器にコライダーをセットする
	for (int32_t i = 0; i < 5; i++) {
		mWeaponWorldMat[i] = MakeAffineMatrix(Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f });
		// コライダー 宣言
		std::shared_ptr<GameCollider> newCollider = std::make_shared<GameCollider>();
		newCollider->collider = std::make_unique<SphereCollider>(new WorldTransform(), 0.4f);
		// 初期化
		newCollider->collider->Init();
		newCollider->collider->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kEnemyWeapon));
		// 武器に設定したボーンのワールド座標をセット
		newCollider->collider->GetWorld()->SetParent(mWeaponWorldMat[i]);
		// 配列にプッシュする
		mWeaponColliders.push_back(std::move(newCollider));
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

	// エディタ並びにノードの初期化
	mBTNodeEditor = std::make_unique<BTNodeEditor>(this);
	mBTNodeEditor->Init();

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
	
	// 複数地点攻撃
	mActions["EarthSword"] = make_shared<ACT::EarthSword>();
	mActions["EarthSword"]->Init(this);

	// 初期は行動しない
	mActiveAction.reset();

	// -- 以下 条件関数の初期化 -- //

	// 条件関数の設定
	mConditionFunctions["IsNearDistance"] =
		std::bind(&Actor::IsNearDistance, this, 10.0f); // 例として10.0fを渡す
	mConditionFunctions["IsFarDistance"] =
		std::bind(&Actor::IsFarDistance, this, 10.0f); // 例として10.0fを渡す
	mConditionFunctions["InvokeNearDistance"] =
		std::bind(&Actor::InvokeNearDistance, this);
	mConditionFunctions["InvokeFarDistance"] =
		std::bind(&Actor::InvokeFarDistance, this);
	mConditionFunctions["IsReceivedDamage"] =
		std::bind(&Actor::IsReceivedDamage, this);
	mConditionFunctions["IsHitAttackToTarget"] =
		std::bind(&Actor::IsHitAttackToTarget, this);
	mConditionFunctions["IsNearFieldEdge"] =
		std::bind(&Actor::IsNearFieldEdge, this);
	mConditionFunctions["IsFarFieldEdge"] =
		std::bind(&Actor::IsFarFieldEdge, this);


}

void BossEnemy::Update() {

	// テーブルから関数を呼び出す
	//(this->*CommandTable[0])();

	if (mStatus->HP > 0.0f) {


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

		// ステートの更新処理を行う
		this->UpdateState();

		// BehaviorTreeの更新処理を行う
		if (mIsUpdateBehavior) {
			UpdateBehaviorTree();
		}
	}


	// シェイクの更新
	ShakeUpdate();
	// 方向修正
	AdJustDirection();
	// オブジェクト更新
	//UpdateObject();
}

void BossEnemy::UpdateBehaviorTree() {

	// リロードカウントを減らす
	mReloadBTCount -= BlackBoard::CombertBattleFPS(6.0f);

	// リロードカウントが0以下になったら行動を実行
	if (mReloadBTCount <= 0.0f) {

		// リロードカウントをリセット
		mReloadBTCount = 1.0f;

		// ビヘイビアツリーの実行
		mBTStatus = mBTNodeEditor->Tick();

		// ビヘイビアツリーの実行結果が成功か失敗だったら(≒実行中ではない場合)
		if (mBTStatus != AINode::State::RUNNING) {

			// ビヘイビアツリーをリセット
			mBTNodeEditor->Reset();

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

	// 無敵時間時の処理
	InvincibleObjectUpdate();

	// アクションの中身が存在する場合はアクションクラスの更新を行う
	if (!mActiveAction.expired()) {
		mActiveAction.lock()->Update();
	}

	// ステージ限界値に合わせた座標の補正
	mObject->mWorldTransform->translation.x = std::clamp(mObject->mWorldTransform->translation.x,
		-BlackBoard::GetFieldRadius().x, BlackBoard::GetFieldRadius().x);
	mObject->mWorldTransform->translation.z = std::clamp(mObject->mWorldTransform->translation.z,
		-BlackBoard::GetFieldRadius().z, BlackBoard::GetFieldRadius().z);

	// オブジェクト 更新処理
	mObject->Update();
	mObject->mCollider->Update();

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
		GetObject3D()->mSkinning->GetSkeleton().joints[GetObject3D()->mSkinning->GetSkeleton().jointMap["mixamorig:Sword_joint"]
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
	
	// 身体の部位のワールド行列を更新
	UpdateBodyCollider();

	// 武器のコライダー 更新
	for (auto& collider : mWeaponColliders) {
		collider->collider->Update();
	}

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

	// ノードエディタの更新(デバッグ表示)
	mBTNodeEditor->Update();

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

void BossEnemy::SetColliderList() {

	// ボス敵本体の本体のコライダー
	for (auto& collider : mBodyPartColliders) {
		// ゲームマネージャのリストに追加
		GameManager::GetInstance()->SetCollider(collider.second);
	}

	// ボス敵の武器コライダー

	// 行動設定時
	if (!mActiveAction.expired()) {

		// 攻撃中 かつ 攻撃が命中していなければ
		if (mActiveAction.lock()->GetIsOperating() &&
			!mActiveAction.lock()->GetIsHit()) {

			// コライダーセット
			for (auto& collider : mWeaponColliders) {
				GameManager::GetInstance()->SetCollider(collider);
			}
		}

	}

	// 弾のコライダーを登録
	for (auto& bullet : mBullets) {
		GameManager::GetInstance()->SetCollider(bullet->GetCollider());
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


bool BossEnemy::GetIsOperating() const {

	// 実行中のアクションで攻撃中であるか取得して返す
	if (!mActiveAction.expired()) {
		return mActiveAction.lock()->GetIsOperating();
	}

	// 実行アクションのポインタがなければ
	return false;
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

void BossEnemy::KnockBackRequest() {

	// 現在の行動の体勢値が0でない場合はノックバックを行わない
	//if(mActiveAction.lock()->GetBodyState() != 0) {
	//	return;
	//}

	// 死亡時もノックバックを行わない
	if (mStatus->HP <= 0.0f) {
		return;
	}

	// 行動を終了
	mActiveAction.lock()->End();
	// 行動を設定(ノックバック)
	SetAction("knockBack");
}

Vector3 BossEnemy::GetBodyPos()
{
	// 体のワールド行列を更新
	Matrix4x4 bodyWorldMat = Multiply(
		GetObject3D()->mSkinning->GetSkeleton().joints[GetObject3D()->mSkinning->GetSkeleton().jointMap["mixamorig:Spine1"]
		].skeletonSpaceMatrix, GetObject3D()->GetWorldTransform()->GetWorldMatrix());

	return Vector3(bodyWorldMat.m[3][0],bodyWorldMat.m[3][1],bodyWorldMat.m[3][2]);
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
				collider->collider->Draw();
			}
		}
	}


	// 身体の部位に合わせたコライダーを描画
	for (auto& collider : mBodyPartColliders) {
		collider.second->collider->Draw();
	}

#endif // _DEBUG
}