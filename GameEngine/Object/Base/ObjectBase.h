#pragma once
#include <string>
#include "Component/Transform/WorldTransform.h"

// 前方宣言
class Camera;

/// <summary>
/// オブジェクト基底クラス
/// <para> 3D空間に存在するオブジェクトの基底クラス </para>
/// </summary>
class ObjectBase
{
public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ObjectBase() = default;
	
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ObjectBase() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="objectName">オブジェクト名</param>
	virtual void Init(std::string objectName);

	/// <summary>
	///  更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	///  描画処理
	/// </summary>
	/// <param name="camera">カメラ</param>
	virtual void Draw(Camera camera);

	/// <summary>
	/// デバッグ描画
	/// </summary>
	virtual void Debug() = 0;

	/// <summary>
	/// ワールド座標取得関数
	/// </summary>
	virtual WorldTransform GetWorld() { return mWorldTransform; }

	/// <summary>
	/// スケール変更
	/// </summary>
	virtual void SetScale(Vector3 scale) { mWorldTransform.scale = scale; }

	/// <summary>
	/// 回転変更
	/// </summary>
	virtual void SetRotation(Vector3 rotation) { mWorldTransform.rotation = rotation; }

	/// <summary>
	/// 平行移動変更
	/// </summary>
	virtual void SetTranslation(Vector3 translation) { mWorldTransform.translation = translation; }

	/// <summary>
	/// 平行移動量加算
	/// </summary>
	virtual void AddTranslate(Vector3 translation) { mWorldTransform.translation += translation; }


public: // -- 公開 メンバ変数 -- //

	// オブジェクト名
	std::string mObjectName;

	// ワールド座標
	WorldTransform mWorldTransform;

};

