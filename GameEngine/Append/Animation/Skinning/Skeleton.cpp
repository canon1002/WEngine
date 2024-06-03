#include "Skeleton.h"

Skeleton Skeleton::Create(const Node& rootNode) {
	Skeleton skeleton;
	skeleton.root = Joint::Create(rootNode, {}, skeleton.joints);

	// 名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}
	// 作成したら一度更新しておく
	skeleton.Update();

	return skeleton;
}

void Skeleton::Update() {
	// すべてのJointを更新。親が若いので通常ループ処理可能になっている
	for (Joint& joint : joints) {
		joint.localMatrix = MakeAffineMatrix(joint.transform.scale_, joint.transform.rotation_, joint.transform.translation_);
		Matrix4x4 local = joint.localMatrix;
		if (joint.parent) {// 親がいれば親の行列を掛ける
			joint.skeletonSpaceMatrix = Multiply(local, joints[*joint.parent].skeletonSpaceMatrix);
		}
		else { // 親がいないのでlocalMatrixとSkeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = local;
		}
	}
}
