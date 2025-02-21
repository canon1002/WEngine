#pragma once
#include "GameEngine/Math/Math.h"
#include <fstream>
#include <iostream>
#include <variant>
#include <string>
#include <map>
#include <json.hpp>
#include <memory>

// 項目
struct Item {
	// 項目の値
	std::variant<int32_t, float, Vector2, Vector3, std::string> value;
};

// グループ
struct Group {
	std::map<std::string, Item> items;
};

// jsonと記載するだけでnlohmann::jsonを使用可能にしておく
using json = nlohmann::json;

/// <summary>
/// グローバル変数
/// </summary>
class GlobalVariables
{
public: // -- 公開 メンバ関数 -- //
	
	// インスタンス取得
	static GlobalVariables* GetInstance();
	// 更新処理
	void Update();

	// グループ生成
	void CreateGroup(const std::string& groupName);
	// 値のセット(int型)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	// 値のセット(float型)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	// 値のセット(Vector2型)
	void SetValue(const std::string& groupName, const std::string& key, const Vector2& value);
	// 値のセット(Vector3型)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);
	// 値のセット(文字列型)
	void SetValue(const std::string& groupName, const std::string& key, const std::string& value);

	// 項目の追加(int型)
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	// 項目の追加(float型)
	void AddItem(const std::string& groupName, const std::string& key, float value);
	// 値のセット(Vector2型)
	void AddItem(const std::string& groupName, const std::string& key, const Vector2& value);
	// 項目の追加(Vector3型)
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);
	// 項目の追加(文字列型)
	void AddItem(const std::string& groupName, const std::string& key, const std::string& value);

	// 値の取得
	int32_t GetIntValue(const std::string& groupName, const std::string& key);
	float GetFloatValue(const std::string& groupName, const std::string& key);
	Vector2 GetVector2Value(const std::string& groupName, const std::string& key);
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key);
	std::string GetStringValue(const std::string& groupName, const std::string& key);

	// ファイルへの書き出し
	void SaveFile(const std::string& groupName);
	// ディレクトリの全ファイル読み込み
	void LoadFiles();
	// ファイル読み込み
	void LoadFile(const std::string& groupName);

private: // -- 非公開 メンバ関数 -- //

	// 外部からの呼び出し禁止
	GlobalVariables() = default;
	~GlobalVariables() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	GlobalVariables(const GlobalVariables& obj) = delete;
	GlobalVariables& operator=(const GlobalVariables& obj) = delete;

public: // -- 公開メンバ 変数 -- //

	// インスタンス
	static GlobalVariables* instance;

	// 全データ
	std::map<std::string, Group> mDatas;

};

