#include "BTNodeEditor.h"

BTNodeEditor::BTNodeEditor(){

}

BTNodeEditor::~BTNodeEditor(){

    // 各ノードの内容を保存


	// 保存した内容をjsonに書き出し


}

void BTNodeEditor::Init(){
	// ノード番号を0で初期化
	mNodeIndex = 0;


}

void BTNodeEditor::Update(){

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
            CreateNode(static_cast<AINodeType>(nodeType), nodeName);
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
        ImNodes::PushColorStyle(ImNodesCol_TitleBar, GetNodeColor(node.type));
        ImNodes::BeginNode(node.index);

        // ノード名
        ImNodes::BeginNodeTitleBar();
		ImGui::Text(node.name.c_str());
        ImNodes::EndNodeTitleBar();

		// ノードの種類
        ImGui::Text("種類 : %s", AINodeTypeToString(node.type).c_str());

		// 入力(親ノードとのリンク)


		// 出力(子ノードとのリンク)


        // ノードの中身


        // ノード終了
        ImNodes::EndNode();
        // 色を戻す
		ImNodes::PopColorStyle();
    }

    // リンクの描画
	// 親ノードと子ノードをリンクする線を描画
    

    // リンクの追加処理
	// 入力または出力からドラックしてリンクを作成する
  

    ImNodes::EndNodeEditor();

    ImGui::End();

#endif // _DEBUG
}

void BTNodeEditor::CreateNode(AINodeType type, const std::string& name){
	AINodeBase node;
	node.name = name;
	node.index = mNodeIndex;
	node.children.clear();
	node.parent = std::nullopt;
	node.type = type;
	mNodes.push_back(node);
	mNodeIndex++;
}
