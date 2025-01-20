#include "BTNodeEditor.h"

BTNodeEditor::BTNodeEditor(){

}

BTNodeEditor::~BTNodeEditor(){

  
}

void BTNodeEditor::Init(){

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

	// ノード番号を0で初期化
	mNodeIndex = 0;

    // ノード生成
   

}

void BTNodeEditor::Update(){
    
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
		const char* types[] = { "BASE","SELECT", "SEQUENCE", "ACTION", "DECRATOR"};
        ImGui::Combo("ノードの種類", &nodeType, types, IM_ARRAYSIZE(types));

		// ノード名を設定
        static std::string nodeName;
        static char nodeNameBuffer[128] = "";
        ImGui::InputText("ノード名", nodeNameBuffer, IM_ARRAYSIZE(nodeNameBuffer));
        nodeName = nodeNameBuffer;

        if (ImGui::MenuItem("ノードを生成する")) {
            // ノード生成処理
            Create(CreateNodeByType(types[nodeType]), nodeName);
            // ノードを生成が成功したら閉じる
			ImGui::CloseCurrentPopup();
        }
		ImGui::EndPopup();
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
		//ImGui::Text(node.name.c_str());
        ImNodes::EndNodeTitleBar();

		// ノードの種類
        //ImGui::Text("種類 : %s", AINodeTypeToString(node.type).c_str());

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

void BTNodeEditor::Create(const std::shared_ptr<AINode::INode>& node, const std::string& name){

	// 新規ノードを作成
	std::shared_ptr<AINode::INode> newNode;
	newNode = node;
	newNode->mName = name;
	newNode->mIndex = mNodeIndex;
	//newNode->children.clear();
	//newNode->parent = std::nullopt;
	mNodes.push_back(newNode);
	mNodeIndex++;

}

// ファクトリメソッドの実装
std::shared_ptr<AINode::INode> BTNodeEditor::CreateNodeByType(const std::string& type) {
    
    type;

    //if (type == "SELECT") {.
    //    return std::make_shared<AINode::Selecter>();
    //}
    //else if (type == "SEQUENCE") {
    //    return std::make_shared<AINode::Sequence>();
    //}
    //else if (type == "ACTION") {
    //    return std::make_shared<AINode::Action>();
    //}
    //else if (type == "DECRATOR") {
    //    return std::make_shared<AINode::Decrator>();
    //}
	
    return nullptr;
}

nlohmann::json BTNodeEditor::ConvertNodeToJson(const std::shared_ptr<AINode::INode>& node){

    nlohmann::json j;
    j["name"] = node->mName;
    j["index"] = node->mIndex;
    //j["type"] = AINodeTypeToString(node->type); // ノードの種類を文字列に変換する関数が必要です

    //// 子ノードを再帰的にJSONに変換
    //for (const auto& child : node->children) {
    //    j["children"].push_back(NodeToJson(child));
    //}

    return j;
}

void BTNodeEditor::SaveNode(){

    // 各ノードの内容を保存
    nlohmann::json root;
    //for (const auto& node : mNodes) {
    //    if (!node->parent.has_value()) {
    //        root["nodes"].push_back();
    //    }
    //}

    // 保存した内容をjsonに書き出し

    // 書き込むJSONファイルのフルパスを合成する
    std::string filepath = "Resources/AINodeEditor" + mOwnerName + ".json";
    // 書き込み用ファイルストリーム
    std::ofstream ofs;
    // ファイルの書き込み用に開く
    ofs.open(filepath);

    // エラーハンドリング
    if (ofs.fail()) {
        std::string message = "Filed open data file for write.";
        MessageBoxA(nullptr, message.c_str(), "AINodeEditor", 0);
        assert(0);
    }

    // ファイルにjson文字列を書き込む
    ofs << std::setw(4) << root << std::endl;
    // ファイルを閉じる
    ofs.close();


}
