#include "BTNodeEditor.h"
#include "App/Actor/Actor.h"

BTNodeEditor::BTNodeEditor(Actor* actor) {
	// アクターのポインタを参照する
	mOwner = actor;
	// アクターの名称を取得
	mOwnerName = actor->GetName();
}

BTNodeEditor::~BTNodeEditor() {

}

void BTNodeEditor::Init() {

	// ノードをJsonファイルから読み込む
	//Load();

}

void BTNodeEditor::Update() {

	// ノード実行処理


	// 親ノードを実行し、子ノードを配列の先頭から順々に実行していく





#ifdef _DEBUG

	ImGui::Begin("BehaviorTree Node Editor");

	// ノード追加
	if (ImGui::Button("新規ノードを生成する")) {
		ImGui::OpenPopup("生成するノードの詳細");
	}

	if (ImGui::BeginPopup("生成するノードの詳細")) {

		// -- 以下 ノード詳細の選択処理 -- //

		// ノードの種類を設定
		static int nodeType = 0;
		const char* types[] = { "SELECT", "SEQUENCE", "ACTION", "DECRATOR","CONDITION" };
		ImGui::Combo("ノードの種類", &nodeType, types, IM_ARRAYSIZE(types));

		// ノード名を設定
		static std::string nodeName;
		static char nodeNameBuffer[128] = "";
		ImGui::InputText("ノード名", nodeNameBuffer, IM_ARRAYSIZE(nodeNameBuffer));
		nodeName = nodeNameBuffer;

		// タスクノード系統が指定されていたら行動内容の文字列を入力


		if (ImGui::MenuItem("ノードを生成する")) {
			// ノード生成処理
			Create((AINodeType)nodeType, nodeName);
			// ノードを生成が成功したら閉じる
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// セーブ機能
	if (ImGui::Button("SAVE")) {

		// 保存処理
		Save();

		// 保存後の通知

	}

	// ノードの情報を表示(デバッグのデバッグ用)
	if (ImGui::BeginTable("Node", 4)) {
		// ヘッダーの設定
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Tag");
		ImGui::TableSetupColumn("Child ID");
		ImGui::TableHeadersRow();

		// ノード一覧の表示
		for (const auto& node : mNodes) {

			if (!node) { continue; }

			ImGui::TableNextRow();

			// ID
			ImGui::TableNextColumn();
			ImGui::Text("%d", node->mIndex);

			// Name
			ImGui::TableNextColumn();
			ImGui::Text("%s", node->mName.c_str());

			// Tag
			ImGui::TableNextColumn();
			ImGui::Text("%s", node->mTag.c_str());

			// Child ID の取得
			ImGui::TableNextColumn();

			// 子ノード未所持
			if (node->mChildren.empty()) {
				ImGui::Text("None");
			}

			// 子ノード所持の場合
			else {

				std::string s = "";
				for (auto& child : node->mChildren) {
					s += std::to_string(child->mIndex);
					s += ",";
				}
				ImGui::Text("%s", s.c_str());
			}
			

		}

		ImGui::EndTable();
	}

	if (ImGui::CollapsingHeader("Links")) {
		for (auto& link : mLinks) {
			int linkID = std::get<0>(link);
			int parentID = std::get<1>(link);
			int childID = std::get<2>(link);

			ImGui::Text("ID:%d [In:%d] [Out:%d]", linkID, parentID, childID);
		}
	}

	// ノードエディタ
	ImNodes::BeginNodeEditor();

	// ノードと詳細情報の表示
	for (auto& node : mNodes) {
		// ノードの種類に応じて色を変更
		AINodeType nodeType = magic_enum::enum_cast<AINodeType>(node->mTag).value();
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, GetNodeColor(nodeType));
		ImNodes::BeginNode(node->mIndex);

		// ノード名
		ImNodes::BeginNodeTitleBar();
		ImGui::Text(node->mName.c_str());
		ImGui::Text("ID:%d", node->mIndex);
		ImNodes::EndNodeTitleBar();

		// ノードの種類

		// 入力（子ノードの接続先）
		ImNodes::BeginInputAttribute(-node->mIndex);
		ImGui::Text("入力");
		ImNodes::EndInputAttribute();

		// 親ノードのみ
		if (node->mTag == "SELECT" || node->mTag == "SEQUENCE") {

			// 出力（親ノードの接続先）
			if (std::dynamic_pointer_cast<AINode::Composite>(node)) {
				ImNodes::BeginOutputAttribute(node->mIndex);
				ImGui::Text("出力");
				ImNodes::EndOutputAttribute();
			}

		}


		// ノードの中身



		// ノードの座標を保持
		ImVec2 pos = ImNodes::GetNodeEditorSpacePos(node->mIndex);
		node->mGuiPos = Vector2(pos.x, pos.y);

		// ノード終了
		ImNodes::EndNode();
		// 色を戻す
		ImNodes::PopColorStyle();
	}

	// リンクの描画
	// 親ノードと子ノードをリンクする線を描画
	for (auto& link : mLinks) {
		int linkID = std::get<0>(link);
		int parentID = std::get<1>(link);
		int childID = std::get<2>(link);
		ImNodes::Link(linkID, parentID, childID);
	}

	// リンクの追加処理
	// 入力または出力からドラックしてリンクを作成する

	// ミニマップの表示
	ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomLeft);
	ImNodes::EndNodeEditor();

	// フレーム内でリンクが作成されたかどうかを確認
	HandleNodeLinking();
	// フレーム内でリンクが削除されたかどうかを確認
	LinkDeletion();

	ImGui::End();

#endif // _DEBUG
}

void BTNodeEditor::Create(const AINodeType& type, const std::string& name) {

	// 新規ノードを作成
	std::shared_ptr<AINode::INode> newNode;
	newNode = CreateNodeByType(type, name);
	newNode->mName = name;
	newNode->mIndex = mNodeIndex;
	newNode->mTag = magic_enum::enum_name(type);
	//newNode->children.clear();
	//newNode->parent = std::nullopt;
	mNodes.push_back(newNode);
	mNodeIndex++;

}

std::shared_ptr<AINode::INode> BTNodeEditor::CreateNodeByType(const AINodeType& type, const std::string& nodeName) {


	// 各ノードごとの生成処理を行う
	switch (type)
	{
		// セレクターノード
	case AINodeType::SELECT:
		return std::make_shared<AINode::Selector>(nodeName);
		break;

		// シークエンスノード
	case AINodeType::SEQUENCE:
		return std::make_shared<AINode::Sequence>(nodeName);
		break;

		// アクションノード
	case AINodeType::ACTION:
		return std::make_shared<AINode::Action>(mOwner, nodeName);
		break;

		// デコレータノード
	case AINodeType::DECRATOR:
		//return std::make_shared<AINode::Decrator>();
		return nullptr;
		break;

		// コンディションノード
	case AINodeType::CONDITION:
		return std::make_shared<AINode::Condition>(mOwner, nodeName);
		break;

		// 指定範囲外の場合はnullを返す
	default:
		return nullptr;
		break;
	}

}




bool BTNodeEditor::DetachChild(std::shared_ptr<AINode::Composite> parent, std::shared_ptr<AINode::INode> child) {

	// 引数が未入力ならfalseを返す
	if (!parent || !child) { return false; }

	// 子ノードを探す
	auto it = std::find(parent->mChildren.begin(), parent->mChildren.end(), child);

	// 子ノードがあったら削除する
	if (it != parent->mChildren.end()) {
		parent->mChildren.erase(it);
		return true;
	}
	return false;
}

bool BTNodeEditor::AttachChild(std::shared_ptr<AINode::Composite> newParent, std::shared_ptr<AINode::INode> child) {

	// 引数が未入力ならfalseを返す
	if (!newParent || !child) { return false; }

	// 指定した親ノードに子ノードをセット
	newParent->SetChild(child);
	return true;
}

bool BTNodeEditor::RemoveNode(std::shared_ptr<AINode::INode> node) {

	// 引数が未入力ならfalseを返す 
	if (!node) { return false; }

	// ルートノードなら削除不可
	if (node == mRootNode) { return false; }

	// すべての親ノードを走査し、削除対象ノードを親の子リストから削除
	for (auto& n : mNodes) {
		auto compositeNode = std::dynamic_pointer_cast<AINode::Composite>(n);
		if (compositeNode) {
			DetachChild(compositeNode, node);
		}
	}

	// `mNodes` から削除
	auto it = std::remove(mNodes.begin(), mNodes.end(), node);
	if (it != mNodes.end()) {
		mNodes.erase(it);
		return true;
	}

	return false;
}

void BTNodeEditor::SetRootNode(std::shared_ptr<AINode::Composite> newRoot) {

	// 引数が未入力ならfalseを返す 
	if (!newRoot) { return; }

	// 以前のルートノードを `mNodes` から削除
	auto it = std::find(mNodes.begin(), mNodes.end(), mRootNode);
	if (it != mNodes.end()) {
		mNodes.erase(it);
	}

	// 新しいルートノードを設定
	mRootNode = newRoot;

	// `mNodes` に追加（新しいノードであれば）
	if (std::find(mNodes.begin(), mNodes.end(), newRoot) == mNodes.end()) {
		mNodes.push_back(newRoot);
	}
}

bool BTNodeEditor::MoveNode(std::shared_ptr<AINode::INode> node, std::shared_ptr<AINode::Composite> newParent) {

	// 引数が未入力ならfalseを返す 
	if (!node || !newParent) { return false; }

	// 既存の親ノードを見つけ、切り離す
	for (auto& n : mNodes) {
		auto compositeNode = std::dynamic_pointer_cast<AINode::Composite>(n);
		if (compositeNode && DetachChild(compositeNode, node)) {
			break;  // 1つの親からのみ切り離す
		}
	}

	// 新しい親ノードに追加
	return AttachChild(newParent, node);
}

void BTNodeEditor::HandleNodeLinking() {

	// フレーム内でリンクが作成されたかどうかを確認
	int pPin = 0;
	int cPin = 0;
	if (ImNodes::IsLinkCreated(&pPin, &cPin) == true) {

		// 新規リンクの作成
		++mLinkIndex;
		mLinks.emplace_back(mLinkIndex, pPin, cPin);

		
		// 親ノードと子ノードを検索
		auto parentNode = FindNodeByID(pPin);
		// エディタ用に負の数値を設定しているため、絶対値に戻して検索する
		auto childNode = FindNodeByID(std::abs(cPin));

		// 親子関係を結ぶ
		parentNode->SetChild(childNode);
	}
}

void BTNodeEditor::LinkDeletion() {
	for (auto it = mLinks.begin(); it != mLinks.end(); ++it) {
		int linkID, parentID, childID;
		std::tie(linkID, parentID, childID) = *it;

		// リンクを指定し、右クリックでリンク削除
		if (ImNodes::IsLinkHovered(&linkID) && ImGui::IsMouseClicked(1)) {
			std::cout << "Deleted link ID: " << linkID << std::endl;
			mLinks.erase(it);
			break; // イテレータが無効になるため、ループを抜ける
		}
	}

}


std::shared_ptr<AINode::INode> BTNodeEditor::FindNodeByID(int nodeID) {
	for (auto& node : mNodes) {
		if (node->mIndex == nodeID) {
			return node;
		}
	}
	return nullptr;
}


void BTNodeEditor::Save() {

	// ルートノードから走査しながらJsonに変換
	json j = mRootNode->ConvertToJson();

	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/AINodeEditor/";
	// ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	// 書き込むJSONファイルのフルパスを合成する
	std::string filepath = kDirectoryPath + mOwner->GetName() + ".json";
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
	ofs << std::setw(4) << j << std::endl;
	// ファイルを閉じる
	ofs.close();

}

void BTNodeEditor::Load() {

	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/AINodeEditor/";

	// 読み込むJSONファイルのフルパスを合成する
	std::string filepath = kDirectoryPath + mOwner->GetName() + ".json";
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込み用に開く
	ifs.open(filepath);
	// エラーハンドリング
	if (ifs.fail()) {
		std::string message = "Filed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "AINodeEditor", 0);
		assert(0);
	}

	json root;
	// json文字列からjsonのデータ構造を展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// ファイルチェック
	assert(root.is_object());// オブジェクト型であるか
	// ルートノードであるか検索
	json& rootObj = root["root"];

	// "tag"を文字列として取得
	std::string tag = rootObj["tag"].get<std::string>();

	// RootNodeを作成(tagからセレクターかシークエンスか判別)
	std::shared_ptr<AINode::Composite> rootNode = nullptr;
	if (magic_enum::enum_cast<AINodeType>(tag) == AINodeType::SELECT) {
		rootNode = std::make_shared<AINode::Selector>("root");
		rootNode->mTag = "SELECT";
	}
	else if (magic_enum::enum_cast<AINodeType>(tag) == AINodeType::SEQUENCE) {
		rootNode = std::make_shared<AINode::Sequence>("root");
		rootNode->mTag = "SEQUENCE";
	}
	// 共通部分はここで命名する
	rootNode->mState = AINode::State::READY;
	rootNode->mIndex = rootObj["index"].get<int32_t>();
	if (rootObj.contains("Pos")) {
		rootNode->mGuiPos = { rootObj["Pos"].at(0),rootObj["Pos"].at(1) };
	}
	// ノードを保存
	mRootNode = rootNode;

	// デバッグ用に追加
	ImVec2 guiPos = ImVec2(mRootNode->mGuiPos.x, mRootNode->mGuiPos.y);
	ImNodes::SetNodeEditorSpacePos(mRootNode->mIndex, guiPos);
	// ノードを参照できるようにする
	mNodes.push_back(rootNode);

	// 以下 ノード保存処理
	if (rootObj.contains("child")) {

		// 子ノードを走査して登録する
		for (json& object : rootObj["child"]) {

			// 保存+子ノードセット処理
			mRootNode->SetChild(CreateForJson(object));
		}
	}

}

std::shared_ptr<AINode::INode> BTNodeEditor::CreateForJson(json j) {

	assert(j.is_object());// オブジェクト型であるか
	std::string name = j.begin().key();

	// 戻り値用の変数を宣言
	std::shared_ptr<AINode::INode> node;



	// ノードの種類を判別して生成
	std::string tag = j[name]["tag"];
	//magic_enumを利用し文字列から列挙体に変換し、引数に入力
	node = CreateNodeByType(magic_enum::enum_cast<AINodeType>(tag).value(), name);


	// 共通部分の設定
	node->mTag = tag;
	node->mIndex = j[name]["index"];
	node->mState = AINode::State::READY;

	// ImNodes上の座標を代入
	if (j[name].contains("Pos")) {
		node->mGuiPos = { j[name]["Pos"].at(0),j[name]["Pos"].at(1) };
	}

	// ノード番号の最高値の更新判定を取る
	if (mNodeIndex <= node->mIndex) {
		mNodeIndex = node->mIndex + 1;
	}

	// もし子ノードが存在するなら再帰処理を利用して子ノードとして登録する
	if (j[name].contains("child")) {
		for (json& object : j[name]["child"]) {
			node->SetChild(CreateForJson(object));
		}
	}

	// デバッグ用に参照
	ImVec2 guiPos = ImVec2(node->mGuiPos.x, node->mGuiPos.y);
	ImNodes::SetNodeEditorSpacePos(node->mIndex, guiPos);
	mNodes.push_back(node);

	return node;

}
