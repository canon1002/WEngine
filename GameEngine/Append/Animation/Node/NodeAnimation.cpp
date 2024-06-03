#include "NodeAnimation.h"


Node ReadNode(aiNode* node) {
	Node result;
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);// assimpの行列からSRTを抽出する関数を利用
	//rotate.w = 1.0f; // w = 1.0f でないと行けなかったはずなので、念のために代入しておく
	result.transform.scale_ = { scale.x,scale.y ,scale.z };// スケールはそのまま
	result.transform.rotation_ = { rotate.x,-rotate.y ,-rotate.z,rotate.w };// x軸を反転、更に回転方向が逆なので軸を反転させる
	result.transform.translation_ = { -translate.x,translate.y ,translate.z };// x軸を回転
	result.localMatrix = MakeAffineMatrix(result.transform.scale_, result.transform.rotation_, result.transform.translation_);

	assert(result.localMatrix.m[3][3] == 1.0f);
	result.name = node->mName.C_Str();// Node名を格納

	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}


Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {
	// キーがないものは返す値がわからないのでだめ
	assert(!keyframes.empty());
	// キーが1つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つの値のkeyframeを取得して範囲内に時刻があるかお判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまできた場合は一番後ろの時刻よりも後ろなので最後の値を返す
	return (*keyframes.rbegin()).value;
}

Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
	// キーがないものは返す値がわからないのでだめ
	assert(!keyframes.empty());
	// キーが1つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの2つの値のkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまできた場合は一番後ろの時刻よりも後ろなので最後の値を返す
	return (*keyframes.rbegin()).value;
}
