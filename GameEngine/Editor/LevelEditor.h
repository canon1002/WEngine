#pragma once
#include <fstream>
#include <iostream>
#include <variant>
#include <string>
#include <map>
#include <json.hpp>
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/3d/Object3d.h"

/// <summary>
/// レベルデータを格納する構造体
/// </summary>
struct LevelData{

	struct ObjectData {
		// ワールド座標
		WorldTransform worldTransform;
		// ファイル名
		std::string fileName;
		// オブジェクト名
		std::string objectName;
	};

	std::vector<ObjectData> objects;
	//static ObjectData
};



/// <summary>
/// レベルエディタ
/// </summary>
class LevelEditor
{
private: // -- 非公開 メンバ関数 -- //


	// コンストラクタ
	LevelEditor() = default;
	// デストラクタ
	~LevelEditor() = default;

	//コピーコンストラクタと演算子オーバーロードの禁止
	LevelEditor(const LevelEditor& obj) = delete;
	LevelEditor& operator=(const LevelEditor& obj) = delete;


public: // -- 公開 メンバ関数 -- //

	// インスタンス取得
	static LevelEditor* GetInstance();

	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	// レベルエディタファイルのチェック
	void CheckLevelEditorFile();
	// キューブマップのテクスチャをセット
	void SetTextureCubeMap(const int32_t& trextureHandle);
	
	// Degree->Radian変換関数
	float GetRadian(float degree);

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static LevelEditor* instance;
	
	// 3Dオブジェクトの配列
	std::vector<std::unique_ptr<Object3d>> mObjects;


};

