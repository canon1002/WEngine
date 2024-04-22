#pragma once
#include "../../math/Math.h"

class WorldTransform
{ 
public: // -- public メンバ関数 -- //

	WorldTransform();
	WorldTransform(Vec3 t);
	WorldTransform(Vec3 t, Vec3 r);
	WorldTransform(Vec3 t, Vec3 r, Vec3 s);
	~WorldTransform();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// ワールド行列を返す
	/// </summary>
	/// <returns></returns>
	Mat44 GetWorldMatrix() const;

	/// <summary>
	/// ワールド座標を返す
	/// </summary>
	/// <returns></returns>
	Vec3 GetWorldPosition();

	/// <summary>
	/// 親となるWorldTrandsfromを登録する
	/// </summary>
	void SetParent(WorldTransform* parent);

public: // -- public メンバ変数 -- //

	// 拡大
	Vec3 scale;
	// 縮小
	Vec3 rotation;
	// 平行移動
	Vec3 translation;

private: // -- private メンバ変数 -- //

	// 親となるワールド変換へのポインタ（読み取り専用）
	WorldTransform* parent_ = nullptr;

};

