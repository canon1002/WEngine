#include "Actor.h"
#include "GameEngine/Append/Collider/SphereCollider.h"
#include "GameEngine/Append/Collider/CollisionManager.h"


void Actor::SetCollider(CollisionManager* cManager)
{
	// コライダーをマネージャーに登録
	for (auto& collider : mBodyPartColliders) {
		cManager->SetCollider(collider.second.get());
	}
}

void Actor::CreateBodyPartCollider(std::string name, float radius,uint32_t atribute, uint32_t mask) {

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
	std::shared_ptr<Collider> newCollider = std::make_unique<SphereCollider>(std::make_shared<WorldTransform>(), radius);
	// 初期化
	newCollider->Init();
	// コライダーの衝突属性を設定
	newCollider->SetCollisionAttribute(atribute);
	// コライダーの衝突対象を設定
	newCollider->SetCollisionMask(mask);
	// コライダーに親行列を設定
	newCollider->GetWorld()->SetParent(*mBodyPartWorldMatrix[name].get());

	// コライダーの名称を設定し、リストに追加
	mBodyPartColliders[name] = std::move(newCollider);
}

//void Actor::UpdateObject()
//{
//}

void Actor::UpdateBodyMatrix()
{

	// for文を利用して各部位の行列を更新
	for (auto& collider : mBodyPartColliders) {

		// ワールド行列の更新
		*mBodyPartWorldMatrix[collider.first] = Multiply(
			GetObject3D()->mSkinning->GetBoneMatrix(GetObject3D()->mSkinning->ConvertMixamoName(collider.first)),
			GetObject3D()->GetWorldTransform()->GetWorldMatrix());

	}

}
