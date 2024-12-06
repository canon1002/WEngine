#pragma once
#include "../../math/Math.h"

class WorldTransform
{ 
public: // -- public メンバ関数 -- //

	WorldTransform();
	WorldTransform(Vector3 t);
	WorldTransform(Vector3 t, Vector3 r);
	WorldTransform(Vector3 t, Vector3 r, Vector3 s);
	~WorldTransform();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// ワールド行列を返す
	/// </summary>
	/// <returns></returns>
	Matrix4x4 GetWorldMatrix() const;

	/// <summary>
	/// ワールド座標を返す
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()const;

	/// <summary>
	/// ワールド回転量の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldRotatiton()const;

	/// <summary>
	/// 親となるWorldTrandsfromを登録する
	/// </summary>
	void SetParent(const Matrix4x4& parent);

	const Matrix4x4* GetParent() { return parent_; }

public: // -- public メンバ変数 -- //

	// 拡大
	Vector3 scale;
	// 回転
	Vector3 rotation;
	// 平行移動
	Vector3 translation;

private: // -- private メンバ変数 -- //

	// 親となるワールド変換へのポインタ（読み取り専用）
	const Matrix4x4* parent_ = nullptr;

};

