#pragma once
#include "Resource/Base/Resource.h"

// templateを用いて<Vector3型>と<Quaternion型>のキーフレーム構造体を作成
template <typename tValue>
struct Keyframe {
	float time;		// 値キーフレームの時刻(単位は秒)
	tValue value;	// キーフレームの値
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

struct NodeAnimation {
	std::vector<KeyframeVector3> translation;
	std::vector<KeyframeQuaternion> rotation;
	std::vector<KeyframeVector3> scale;
};

// アニメーション構造体
struct Animation {
	float duration; // アニメーション全体の尺 (単位は秒)
	// NodeAnimationの集合 Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};

/// <summary>
/// 親子関係を持つNodeクラス
/// </summary>
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};


/// <summary>
/// AssimpのNode(aiNode)を自作のNode構造体に変換する関数
/// </summary>
/// <param name="node">AssimpのNode(aiNode)</param>
/// <returns>Node</returns>
Node ReadNode(aiNode* node);

// 任意の時刻の値を取得する関数(Vector3型)
Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
// 任意の時刻の値を取得する関数(Quaternion型)
Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);