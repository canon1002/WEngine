#include "BTNodeEditor.h"
#include "App/Actor/Actor.h"

BTNodeEditor::BTNodeEditor(Actor* actor) {
	// アクターのポインタを弱参照する
	mOwner = actor;
	// アクターの名称を取得
	mOwnerName = actor->GetName();
}

BTNodeEditor::~BTNodeEditor() {


}

void BTNodeEditor::Init() {



	//// グローバル変数の保存先ファイルパス
	//const std::string kDirectoryPath = "Resources/AINodeEditor/";

	//// 読み込むJSONファイルのフルパスを合成する
	//std::string filepath = kDirectoryPath + "NodeTest" + ".json";
	//// 読み込み用ファイルストリーム
	//std::ifstream ifs;
	//// ファイルを読み込み用に開く
	//ifs.open(filepath);
	//// エラーハンドリング
	//if (ifs.fail()) {
	//	std::string message = "Filed open Node file for read.";
	//	MessageBoxA(nullptr, message.c_str(), "Node", 0);
	//	assert(0);
	//}
	//
	//json root;
	//// json文字列からjsonのデータ構造を展開
	//ifs >> root;

	//// ファイルを閉じる
	//ifs.close();

	//// ノード番号を0で初期化
	//mNodeIndex = 0;

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


	// ノードエディタ
	ImNodes::BeginNodeEditor();

	// ノードと詳細情報の表示
	for (auto& node : mNodes) {
		// ノードの種類に応じて色を変更
		//ImNodes::PushColorStyle(ImNodesCol_TitleBar, GetNodeColor(node.type));
		ImNodes::BeginNode(node->mIndex);

		// ノード名
		ImNodes::BeginNodeTitleBar();
		ImGui::Text(node->mName.c_str());
		ImNodes::EndNodeTitleBar();

		// ノードの種類
		

		// 入力(親ノードとのリンク)


		// 出力(子ノードとのリンク)


		// ノードの中身


		// ノード終了
		ImNodes::EndNode();
		// 色を戻す
		//ImNodes::PopColorStyle();
	}

	// リンクの描画
	// 親ノードと子ノードをリンクする線を描画


	// リンクの追加処理
	// 入力または出力からドラックしてリンクを作成する


	ImNodes::EndNodeEditor();

	ImGui::End();

#endif // _DEBUG
}

void BTNodeEditor::Create(const AINodeType& type, const std::string& name) {

	// 新規ノードを作成
	std::shared_ptr<AINode::INode> newNode;
	newNode = CreateNodeByType(type,name);
	newNode->mName = name;
	newNode->mIndex = mNodeIndex;
	//newNode->children.clear();
	//newNode->parent = std::nullopt;
	mNodes.push_back(newNode);
	mNodeIndex++;

}

std::shared_ptr<AINode::INode> BTNodeEditor::CreateNodeByType(const AINodeType& type, const std::string& nodeName) {

	type;
	nodeName;
	return nullptr;

	//// 各ノードごとの生成処理を行う
	//switch (type)
	//{
	//	// セレクターノード
	//case AINodeType::SELECT:
	//	return std::make_shared<AINode::Selector>(nodeName);
	//	break;

	//	// シークエンスノード
	//case AINodeType::SEQUENCE:
	//	return std::make_shared<AINode::Sequence>(nodeName);
	//	break;

	//	// アクションノード
	//case AINodeType::ACTION:
	//	return std::make_shared<AINode::Action>(mOwner,nodeName);
	//	break;

	//	// デコレータノード
	//case AINodeType::DECRATOR:
	//	//return std::make_shared<AINode::Decrator>();
	//	return nullptr;
	//	break;

	//	// コンディションノード
	//case AINodeType::CONDITION:
	//	return std::make_shared<AINode::Condition>(mOwner,nodeName);
	//	break;

	//	// 指定範囲外の場合はnullを返す
	//default:
	//	return nullptr;
	//	break;
	//}

}




void BTNodeEditor::Save() {

	// ルートノードから走査しながらJsonに変換
	json j = mRootNode->ConvertToJson();



	// グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";
	// ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	// 書き込むJSONファイルのフルパスを合成する
	std::string filepath = kDirectoryPath + ".json";
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

}

