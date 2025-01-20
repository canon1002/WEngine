#pragma once
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "GameEngine/Editor/NodeEditor/Nodes/AINodeBase.h"


// ノードの種類
enum class AINodeType {
	BASE,
	SELECT,
	SEQUENCE,
	ACTION,
	DECRATOR,
	CONDITION,
};


/// <summary>
/// BehaviorTree Nodeエディタクラス
/// </summary>
class BTNodeEditor{

public:// -- 公開 メンバ関数 -- //

	// コンストラクタ
	BTNodeEditor();
	// デストラクタ
	~BTNodeEditor();
	// 初期化処理
	void Init();
	// 更新処理
	void Update();

	// 新規ノード生成処理
	void Create(const std::shared_ptr<AINode::INode>& node, const std::string& name);

	// ファクトリメソッド(文字列に応じてノードを作成する)
	std::shared_ptr<AINode::INode> CreateNodeByType(const std::string& type);

	// ノードの種類を文字列に変換
	inline std::string AINodeTypeToString(AINodeType type) {
		switch (type) {
		case AINodeType::BASE: return "BASE";
		case AINodeType::SELECT: return "SELECT";
		case AINodeType::SEQUENCE: return "SEQUENCE";
		case AINodeType::ACTION: return "ACTION";
		case AINodeType::DECRATOR: return "DECRATOR";
		default: return "UNKNOWN";
		}
	}

	// ノードの種類から色を取得
	inline ImU32 GetNodeColor(AINodeType type) {
		switch (type) {
		case AINodeType::BASE: return IM_COL32(200, 200, 200, 255); // 白
		case AINodeType::SELECT: return IM_COL32(0, 150, 200, 255); // 水色
		case AINodeType::SEQUENCE: return IM_COL32(0, 200, 0, 255); // 緑
		case AINodeType::ACTION: return IM_COL32(200, 0, 0, 255); // 赤
		case AINodeType::DECRATOR: return IM_COL32(200, 200, 0, 255); // 黄
		default: return IM_COL32(128, 128, 128, 255); // グレー
		}
	}


private: // -- 非公開 メンバ関数 -- //


	// ノードをJSONファイルへの書き込み用に変換する
	json ConvertNodeToJson(const std::shared_ptr<AINode::INode>& node);

	// ノード保存処理
	void SaveNode();


public: // -- 公開 メンバ変数 -- //

	// 持ち主情報(文字列 ・ これがjsonファイル名になる)
	std::string mOwnerName;
	// ノードリスト
	std::vector<std::shared_ptr<AINode::INode>> mNodes;
	// ノード番号
	int32_t mNodeIndex;

private: // -- 非公開 メンバ変数 -- //

};

