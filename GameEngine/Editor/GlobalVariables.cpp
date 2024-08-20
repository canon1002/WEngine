#include "GlobalVariables.h"
#include "GameEngine/Base/Debug/ImGuiManager.h"

// staticメンバ変数で宣言したインスタンスを初期化
GlobalVariables* GlobalVariables::instance = nullptr;

// インスタンスを取得
GlobalVariables* GlobalVariables::GetInstance() {
	// 関数内staticは初めて通ったときのみ実行される
	//static GlobalVariables* instance = nullptr;
	if (instance == nullptr) {
		instance = new GlobalVariables;
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

			// Vector3型の場合
			else if (std::holds_alternative<Vector3>(item.value)) {
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				// float型の配列としてImGuiに渡す
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.1f, 0, 100);
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

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector3& value) {
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

int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key){
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
	// int32_t型の値を返す
	return (int32_t)std::get<int32_t>(item.value);
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

		// Vector3型の場合
		else if (std::holds_alternative<Vector3>(item.value)) {
			// float型のjson配列登録
			Vector3 value = std::get<Vector3>(item.value);
			root[groupName][itemName] = json::array({value.x,value.y,value.z});
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
		if (itItem->is_number_float()) {
			// int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}

		// float型の場合
		else if (itItem->is_number_integer()) {
			// double型として値を取得する
			double value = itItem->get<double>();
			// float型に変換した値を登録
			SetValue(groupName, itemName, static_cast<float>(value));
		}

		// Vector3型の場合
		else if (itItem->is_array() && itItem->size() == 3) {
			// float型のjson配列取得
			Vector3 value = { itItem->at(0),itItem->at(1),itItem->at(2) };
			// float型に変換した値を登録
			SetValue(groupName, itemName, value);
		}

	}

}

