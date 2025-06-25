#include "Actor.h"
#include "GameEngine/Component/Collider/SphereCollider.h"
#include "GameEngine/Component/Collider/CollisionManager.h"
#include "GameEngine/Editor/BlackBoard.h"

Actor::Actor(){
	
}

Actor::Actor(const std::string& name){
	
	// アクター名を設定
	mName = name;

}

void Actor::SetCollider(CollisionManager* cManager)
{
	// コライダーをマネージャーに登録
	for (auto& collider : mBodyPartColliders) {
		cManager->SetCollider(collider.second->collider.get());
	}
}

void Actor::SetAction(const std::string& key) {
	// 現行アクションを設定
	mActiveAction = mActions[key];
	mActiveAction.lock()->Start();
}

ACT::Condition Actor::GetActionCondition(const std::string& key) {
	// 引数で指定した行動クラスの状態を取得する
	return mActions[key]->GetCondition();
}

ACT::Action* Actor::GetActionClass(const std::string& key) {
	// 引数で指定した行動クラスの状態を取得する
	return mActions[key].get();
}


void Actor::InputDirection(const Vector3& dir){
	mDirectionForInput = dir;
	// 正規化
	if (Length(mDirectionForInput) != 0.0f) {
		mDirectionForInput = Normalize(mDirectionForInput);
	}
	// y座標は移動しない
	mDirectionForInput.y = 0.0f;
	mDirection = mDirectionForInput;
}

void Actor::AdJustDirection() {

	// 方向指定をしていない場合は回転しない
	if (Length(mDirection) == 0.0f) {
		return;
	}

	// オブジェクトを回転
	mObject->mWorldTransform->rotation.y = LerpShortAngle(
		mObject->mWorldTransform->rotation.y,
		std::atan2f(mDirection.x, mDirection.z),
		0.2f
	);

}

void Actor::InvincibleObjectUpdate(){

	// 無敵時間中でなければ早期リターン
	if (mInvincibleTime <= 0.0f) {
		return;
	}

	// 時間を減産
	mInvincibleCurrent += BlackBoard::GetBattleFPS();

	// 無敵時間前半は赤くする
	if (mInvincibleCurrent < (mInvincibleTime * 0.5f)) {
		mObject->mModel->GetMaterialData()->color = { 1.0f,0.0f,0.0f,1.0f };
	}
	// 無敵時間後半に色を戻す
	else{
		mObject->mModel->GetMaterialData()->color = {1.0f,1.0f,1.0f,1.0f};
	}

	// 無敵時間が終了したら
	if (mInvincibleCurrent >= mInvincibleTime) {
		// 無敵時間を0秒に補正する
		mInvincibleCurrent = 0.0f;
		mInvincibleTime = 0.0f;
	}

}

void Actor::ReceiveDamage()
{
}

void Actor::CreateBodyPartCollider(std::string name, float radius, uint32_t colliderTypeID) {

	// 名称が重複している場合は処理を行わない
	if (mBodyPartColliders.find(name) != mBodyPartColliders.end()) {
		return;
	}

	//
	// -- ワールド行列の設定 -- //
	//

	// 4x4行列の宣言
	std::unique_ptr<Matrix4x4> newWorldMatrix = std::make_unique<Matrix4x4>();

	// 生成した行列のポインタをリストに追加
	mBodyPartWorldMatrix[name] = std::move(newWorldMatrix);


	//
	// -- コライダー生成 -- //
	//

	// 新規コライダーの宣言
	std::shared_ptr<GameCollider> newCollider = std::make_shared<GameCollider>();
	newCollider->collider = std::make_unique<SphereCollider>(new WorldTransform(), radius);
	// 初期化
	newCollider->collider->Init();
	// コライダーの属性IDを設定
	newCollider->collider->SetTypeID(colliderTypeID);
	// コライダーに親行列を設定
	newCollider->collider->GetWorld()->SetParent(*mBodyPartWorldMatrix[name].get());

	// コライダーの名称を設定し、リストに追加
	mBodyPartColliders[name] = std::move(newCollider);
}

//void Actor::UpdateObject()
//{
//}

void Actor::UpdateBodyCollider(){

	// for文を利用して各部位の行列を更新
	for (auto& collider : mBodyPartColliders) {

		// ワールド行列の更新
		*mBodyPartWorldMatrix[collider.first] = Multiply(
			GetObject3D()->mSkinning->GetBoneMatrix(GetObject3D()->mSkinning->ConvertMixamoName(collider.first)),
			GetObject3D()->GetWorldTransform()->GetWorldMatrix());

		// コライダーを更新
		collider.second->collider->Update();
	}

}


Vector3 Actor::GetWorldPosForTarget() {
	if (mTarget) {
		return mTarget->GetWorldPos();
	}
	return Vector3(0.0f, 0.0f, 0.0f);
}


std::function<bool()> Actor::GetConditionFunction(const std::string& key){
	
	// 引数で指定した条件関数があるか確認する
	if (mConditionFunctions.find(key) != mConditionFunctions.end()) {
		return mConditionFunctions[key];
	}
	
	// 条件関数が存在しない場合はnullptrを返す
	return nullptr;
}

bool Actor::InvokeNearDistance() {
	//	距離が近い場合のみ実行
	if (Length(Vector3(
		GetWorldPos().x - GetWorldPosForTarget().x,
		GetWorldPos().y - GetWorldPosForTarget().y,
		GetWorldPos().z - GetWorldPosForTarget().z))
		<= Length(Vector3(mObject->mWorldTransform->scale))) {
		return true;
	}
	return false;
}

bool Actor::IsNearDistance(float range) {
	//	距離が近い場合のみ実行
	if (Length(Vector3(
		GetWorldPos().x - GetWorldPosForTarget().x,
		GetWorldPos().y - GetWorldPosForTarget().y,
		GetWorldPos().z - GetWorldPosForTarget().z))
		<= range) {
		return true;
	}
	return false;
}

bool Actor::InvokeFarDistance() {
	//	距離が遠い場合のみ実行
	if (Length(Vector3(
		GetWorldPos().x - GetWorldPosForTarget().x,
		GetWorldPos().y - GetWorldPosForTarget().y,
		GetWorldPos().z - GetWorldPosForTarget().z))
		> Length(Vector3(mObject->mWorldTransform->scale))) {
		return true;
	}
	return false;
}

bool Actor::IsFarDistance(float range) {
	//	距離が遠い場合のみ実行
	if (Length(Vector3(
		GetWorldPos().x - GetWorldPosForTarget().x,
		GetWorldPos().y - GetWorldPosForTarget().y,
		GetWorldPos().z - GetWorldPosForTarget().z))
		> range) {
		return true;
	}
	return false;
}

bool Actor::IsNearFieldEdge(){

	// フィールド限界地点の取得
	Vector3 fieldRad = BlackBoard::GetFieldRadius();
	// 距離感の設定( フィールド半径/4 程度にする)
	float distance = fieldRad.x / 4.0f;

	//	距離が近い場合はtrueを返す
	if (-fieldRad.x > (GetWorldPos().x - distance) || // 左側
		fieldRad.x < (GetWorldPos().x + distance) || // 右側
		fieldRad.z < (GetWorldPos().x + distance) || // 奥行き
		-fieldRad.x >(GetWorldPos().x - distance) // 手前側
		) {
		return true;
	}
	return false;

}

bool Actor::IsFarFieldEdge(){

	// フィールド限界地点の取得
	Vector3 fieldRad = BlackBoard::GetFieldRadius();
	// 距離感の設定( フィールド半径/10 程度にする)
	float distance = fieldRad.x / 10.0f;

	//	距離が近い場合はtrueを返す
	if (-fieldRad.x > (GetWorldPos().x - distance) || // 左側
		fieldRad.x < (GetWorldPos().x + distance) || // 右側
		fieldRad.z < (GetWorldPos().x + distance) || // 奥行き
		-fieldRad.x >(GetWorldPos().x - distance) // 手前側
		) {
		return false;
	}
	return true;
}
