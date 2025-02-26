#include "GlobalVariables.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

// staticメンバ変数で宣言したインスタンスを初期化
GlobalVariables* GlobalVariables::instance = nullptr;

// インスタンスを取得
GlobalVariables* GlobalVariables::GetInstance() {
	if (instance == nullptr) {
		instance = new GlobalVariables();
	}
	return instance;
}

void GlobalVariables::Update() {

#ifdef _DEBUG

	// メニューバーを表示する
	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) return;

	// 各グループについて
	for (std::map<std::string, Group>::iterator itGroup = mDatas.begin();
		itGroup != mDatas.end(); itGroup++) {
		// グループ名の取得 // ->firstはキーを指す
		const std::string& groupName = itGroup->first;
		// グループの参照を取得 // ->secondは値を指す
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		// 各項目について
		for (std::map<std::string, Item>::iterator itItem = group.items.begin();
			itItem != group.items.end(); itItem++) {

			// 項目名の取得
			const std::string& itemName = itItem->first;
			// 項目の参照を取得
			Item& item = itItem->second;

			// 以下項目の型による分岐

			// int32_t型の場合
			if (std::holds_alternative<int32_t>(item.value)) {
				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::DragInt(itemName.c_str(), ptr, 1.0f, 0, 100);
			}

			// float型の場合
			else if (std::holds_alternative<float>(item.value)) {
				float* ptr = std::get_if<float>(&item.value);
				ImGui::DragFloat(itemName.c_str(), ptr, 0.1f, 0, 100);
			}

			// Vector2型の場合
			else if (std::holds_alternative<Vector2>(item.value)) {
				Vector2* ptr = std::get_if<Vector2>(&item.value);
				// float型の配列としてImGuiに渡す
				ImGui::DragFloat2(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.1f, 0, 100);
			}

			// Vector3型の場合
			else if (std::holds_alternative<Vector3>(item.value)) {
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				// float型の配列としてImGuiに渡す
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.1f, 0, 100);
			}

			// std::string型の場合
			else if (std::holds_alternative<std::string>(item.value)) {
				std::string* ptr = std::get_if<std::string>(&item.value);
				// string型の配列としてImGuiに渡す
				ImGui::Text("%s",ptr->c_str());
			}

		}

		// 改行
		ImGui::Text("\n");
		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();

#endif // _DEBUG
}

void GlobalVariables::CreateGroup(const std::string& groupName)
{
	// 指定名のオブジェクトがなければ追加する
	mDatas[groupName];
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, int32_t value) {
	// グループの参照を取得
	Group& group = mDatas[groupName];
	// 新しい項目のデータを取得
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd::mapに追加
	group.items[key] = newItem;

}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	// グループの参照を取得
	Group& group = mDatas[groupName];
	// 新しい項目のデータを取得
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector2& value)
{
	// グループの参照を取得
	Group& group = mDatas[groupName];
	// 新しい項目のデータを取得
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector3& value) {
	// グループの参照を取得
	Group& group = mDatas[groupName];
	// 新しい項目のデータを取得
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const std::string& value)
{
	// グループの参照を取得
	Group& group = mDatas[groupName];
	// 新しい項目のデータを取得
	Item newItem{};
	newItem.value = value;
	// 設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, int32_t value)
{
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	//未登録チェック
	assert(itGroup != mDatas.end());
	//グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::iterator itKey = group.items.find(key);
	if (itKey == group.items.end()) {
		SetValue(groupName, key, value);
	}

}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, float value)
{
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	//未登録チェック
	assert(itGroup != mDatas.end());
	//グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::iterator itKey = group.items.find(key);
	if (itKey == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector2& value)
{
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	//未登録チェック
	assert(itGroup != mDatas.end());
	//グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::iterator itKey = group.items.find(key);
	if (itKey == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector3& value)
{
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	//未登録チェック
	assert(itGroup != mDatas.end());
	//グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::iterator itKey = group.items.find(key);	
	if (itKey == group.items.end()) {
		SetValue(groupName, key, value);
	}

}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const std::string& value)
{
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	//未登録チェック
	assert(itGroup != mDatas.end());
	//グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::iterator itKey = group.items.find(key);
	if (itKey == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key){
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	// 未登録チェック
	assert(itGroup != mDatas.end());
	// グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::const_iterator itKey = group.items.find(key);
	// 未登録チェック
	assert(itKey != group.items.end());
	// int32_t型の値を返す
	return std::get<int32_t>(itKey->second.value);
}

float GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key)
{
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	// 未登録チェック
	assert(itGroup != mDatas.end());
	// グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::iterator itKey = group.items.find(key);
	// 未登録チェック
	assert(itKey != group.items.end());
	// 項目の参照を取得
	Item& item = itKey->second;

	// float型の値を返す
	return std::get<float>(item.value);
}

Vector2 GlobalVariables::GetVector2Value(const std::string& groupName, const std::string& key)
{
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	// 未登録チェック
	assert(itGroup != mDatas.end());
	// グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::iterator itKey = group.items.find(key);
	// 未登録チェック
	assert(itKey != group.items.end());
	// 項目の参照を取得
	Item& item = itKey->second;

	// Vector2型の値を返す
	return std::get<Vector2>(item.value);
}

Vector3 GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key)
{
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	// 未登録チェック
	assert(itGroup != mDatas.end());
	// グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::iterator itKey = group.items.find(key);
	// 未登録チェック
	assert(itKey != group.items.end());
	// 項目の参照を取得
	Item& item = itKey->second;

	// Vector3型の値を返す
	return std::get<Vector3>(item.value);
}

std::string GlobalVariables::GetStringValue(const std::string& groupName, const std::string& key)
{
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	// 未登録チェック
	assert(itGroup != mDatas.end());
	// グループの参照を取得
	Group& group = itGroup->second;

	//キーを検索
	std::map<std::string, Item>::iterator itKey = group.items.find(key);
	// 未登録チェック
	assert(itKey != group.items.end());
	// 項目の参照を取得
	Item& item = itKey->second;

	// std::string型の値を返す
	return std::get<std::string>(item.value);
}

Group GlobalVariables::GetGroupValue(const std::string& groupName)
{
	// グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	// 未登録チェック
	assert(itGroup != mDatas.end());
	// グループの参照を取得
	Group& group = itGroup->second;
	return group;
}

void GlobalVariables::SaveFile(const std::string& groupName) {

	// グループを検索
	std::map<std::string, Group>::iterator itGroup = mDatas.find(groupName);
	// 未登録チェック
	assert(itGroup != mDatas.end());
	// jsonにオブジェクト登録する
	json root;
	root = json::object();
	// jsonオブジェクト登録
	root[groupName] = json::object();

	// グループの参照を取得
	Group& group = itGroup->second;

	// 各項目について
	for (std::map<std::string, Item>::iterator itItem = group.items.begin();
		itItem != group.items.end(); itItem++) {

		// 項目名の取得
		const std::string& itemName = itItem->first;
		// 項目の参照を取得
		Item& item = itItem->second;

		// 以下項目の型による分岐

		// int32_t型の場合
		if (std::holds_alternative<int32_t>(item.value)) {
			// int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item.value);
		}

		// float型の場合
		else if (std::holds_alternative<float>(item.value)) {
			// float型の値を登録
			root[groupName][itemName] = std::get<float>(item.value);
		}

		// Vector2の場合
		else if (std::holds_alternative<Vector2>(item.value)) {
			// float型のjson配列登録
			Vector2 value = std::get<Vector2>(item.value);
			root[groupName][itemName] = json::array({ value.x,value.y });
		}

		// Vector3型の場合
		else if (std::holds_alternative<Vector3>(item.value)) {
			// float型のjson配列登録
			Vector3 value = std::get<Vector3>(item.value);
			root[groupName][itemName] = json::array({value.x,value.y,value.z});
		}

		// 文字列型の場合
		else if (std::holds_alternative<std::string>(item.value)) {
			// float型のjson配列登録
			std::string value = std::get<std::string>(item.value);
			root[groupName][itemName].push_back(value);
		}

		// グローバル変数の保存先ファイルパス
		const std::string kDirectoryPath = "Resources/GlobalVariables/";
		// ディレクトリがなければ作成する
		std::filesystem::path dir(kDirectoryPath);
		if (!std::filesystem::exists(kDirectoryPath)) {
			std::filesystem::create_directories(kDirectoryPath);
		}

		// 書き込むJSONファイルのフルパスを合成する
		std::string filepath = kDirectoryPath + groupName + ".json";
		// 書き込み用ファイルストリーム
		std::ofstream ofs;
		// ファイルの書き込み用に開く
		ofs.open(filepath);

		// エラーハンドリング
		if (ofs.fail()) {
			std::string message = "Filed open data file for write.";
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
			assert(0);
		}

		// ファイルにjson文字列を書き込む
		ofs << std::setw(4) << root << std::endl;
		// ファイルを閉じる
		ofs.close();

	}



}

void GlobalVariables::LoadFiles()
{
	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
	// ディレクトリがなければスキップする
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}

	// 各ファイルの処理を行う
	std::filesystem::directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it) {

		// ファイルパスを取得
		const std::filesystem::path& filepath = entry.path();

		// ファイル拡張子を取得
		std::string extension = filepath.extension().string();
		// .json以外はスキップする
		if (extension.compare(".json") != 0) {
			continue;
		}

		// ファイル読み込み
		LoadFile(filepath.stem().string());

	}

}

void GlobalVariables::LoadFile(const std::string& groupName){

	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";

	// 読み込むJSONファイルのフルパスを合成する
	std::string filepath = kDirectoryPath + groupName + ".json";
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込み用に開く
	ifs.open(filepath);
	// エラーハンドリング
	if (ifs.fail()) {
		std::string message = "Filed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
	}

	json root;
	// json文字列からjsonのデータ構造を展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// グループを検索
	json::iterator itGroup = root.find(groupName);
	// 未登録チェック
	assert(itGroup != root.end());

	// 各アイテムの処理
	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); itItem++) {
		// アイテム名を取得
		const std::string& itemName = itItem.key();
		
		// int32_t型の場合
		if (itItem->is_number_integer()) {
			// int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}

		// float型の場合
		else if (itItem->is_number_float()) {
			// double型として値を取得する
			double value = itItem->get<double>();
			// float型に変換した値を登録
			SetValue(groupName, itemName, static_cast<float>(value));
		}

		// Vector2型の場合
		else if (itItem->is_array() && itItem->size() == 2) {
			// float型のjson配列取得
			Vector2 value = { itItem->at(0),itItem->at(1) };
			// Vector2型に変換した値を登録
			SetValue(groupName, itemName, value);
		}

		// Vector3型の場合
		else if (itItem->is_array() && itItem->size() == 3) {
			// float型のjson配列取得
			Vector3 value = { itItem->at(0),itItem->at(1),itItem->at(2) };
			// Vector3型に変換した値を登録
			SetValue(groupName, itemName, value);
		}

		// 文字列型の場合
		else if (itItem->is_string()) {
			// std::string型の値を登録
			SetValue(groupName, itemName, itItem->get<std::string>());
		}

	}

}

