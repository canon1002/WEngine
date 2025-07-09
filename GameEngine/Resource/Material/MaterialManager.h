#pragma once
#include "Material.h"
#include <unordered_map>
#include <memory>

/// <summary>
/// マテリアル管理クラス
/// </summary>
class MaterialManager{

public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	MaterialManager() = default;
	// デストラクタ
	~MaterialManager() = default;

	// インスタンス取得関数
	static MaterialManager* GetInstance();
	// 終了処理
	void Final();

	
	// -- 生成処理 -- //

	// モデルの生成 & ポインタ取得 (※重複時 取得処理のみ実行)
	MaterialExt* CreateMaterial(const std::string& name,const std::vector<std::string>& textureFilePaths);


	// -- 検索・取得処理 -- //

	// モデルの取得
	MaterialExt* GetMaterial(const std::string name);


private: // -- 非公開 メンバ関数 -- //

	// コピーコンストラクタと演算子オーバーロードの禁止
	MaterialManager(const MaterialManager& obj) = delete;
	MaterialManager& operator=(const MaterialManager& obj) = delete;

private: // -- 非公開 メンバ変数 -- //

	// マテリアルデータマップ
	std::unordered_map<std::string, std::unique_ptr<MaterialExt>> mMaterials;

	// インスタンス
	static std::unique_ptr<MaterialManager> instance;

};

