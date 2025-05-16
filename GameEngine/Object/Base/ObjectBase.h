#pragma once
#include <string>
#include <memory>
#include "Utility/Transform/WorldTransform.h"


// 描画フラグ
struct DrawFlags
{
	bool isActiveModel;	// モデル描画
	bool isActiveSkinning;	// スキニング描画
	bool isActiveSprite;	// 画像描画
};


/// <summary>
/// 3Dオブジェクト基底クラス
/// </summary>
class ObjectBase
{
public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ObjectBase() = default;

	/// <summary>
	/// コンストラクタ  
	/// </summary>
	/// <param name="objname">オブジェクト名</param>
	ObjectBase(const std::string& objname);
	

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ObjectBase();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init();
	
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画処理(3Dモデル)
	/// </summary>
	virtual void Draw();
	
	/// <summary>
	/// 描画処理(スキニング)
	/// </summary>
	virtual void DrawSkin();
	
	/// <summary>
	/// 描画処理(Sprite)
	/// </summary>
	virtual void DrawUI();
	
	/// <summary>
	/// 描画処理(ImGui)
	/// </summary>
	virtual void DrawGUI();

	// オブジェクト名取得
	std::string GetObjectname()const { return mObjname; }

public: // -- 公開 メンバ変数 -- //

	// オブジェクト名
	std::string mObjname;
	// ワールド座標
	std::unique_ptr<WorldTransform> mWorldTransform;
	// 描画フラグ
	DrawFlags mDrawFlags;

};
