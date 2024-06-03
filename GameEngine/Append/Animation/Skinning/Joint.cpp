#include "Joint.h"


int32_t Joint::Create(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());// 現在登録されてるindexに
	joint.parent = parent;
	joints.push_back(joint);// skeleton	のJoint列に追加
	for (const Node& child : node.children) {
		// 子jointを作成し、そのIndexを登録
		int32_t childIndex = Joint::Create(child, joint.index, joints);
		joints[joint.index].childlen.push_back(childIndex);
	}
	// 自身のindexを返す
	return joint.index;
}
