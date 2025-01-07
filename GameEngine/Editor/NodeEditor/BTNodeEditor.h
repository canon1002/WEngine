#pragma once
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/GlobalVariables.h"


// ノードの種類
enum class AINodeType {
	BASE,
	SELECT,
	SEQUENCE,
	ACTION,
	DECRATOR,
};

// ノードの基底部分
struct AINodeBase {
	std::string name;				// 名前
	int32_t index;					// 自身のIndex
	std::vector<int32_t> children;	// 子jointのIndexリスト。いなければ空
	std::optional<int32_t> parent;	// 親JointのIndex。いなければnull
	AINodeType type;				// ノードの動き方
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
	void CreateNode(AINodeType type, const std::string& name);

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

public: // -- 公開 メンバ変数 -- //

	// 持ち主情報(文字列 ・ これがjsonファイル名になる)
	std::string mOwnerName;
	// ノードリスト
	std::vector<AINodeBase> mNodes;
	// ノード番号
	int32_t mNodeIndex;

private: // -- 非公開 メンバ変数 -- //

};

