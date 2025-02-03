#pragma once
#include "GameEngine/Base/Debug/ImGuiManager.h"
#include "GameEngine/Editor/GlobalVariables.h"
#include "Nodes/AINodeBase.h"
#include "Nodes/AINodeActions.h"
#include "Nodes/AINodeComposites.h"
#include "Externals/magic_enum/magic_enum.hpp"

// 前方宣言
class Actor;

// ノードの種類
enum class AINodeType {
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
	BTNodeEditor(Actor* actor);
	// デストラクタ
	~BTNodeEditor();
	// 初期化処理
	void Init();
	// 更新処理
	void Update();

	// 新規ノード生成処理
	void Create(const AINodeType& type, const std::string& name);

	// ファクトリメソッド
	std::shared_ptr<AINode::INode> CreateNodeByType(const AINodeType& type,const std::string& Nodeame);

	// ノードの種類から色を取得
	inline ImU32 GetNodeColor(AINodeType type) {
		switch (type) {
		//case AINodeType::BASE: return IM_COL32(200, 200, 200, 255); // 白
		case AINodeType::SELECT: return IM_COL32(0, 150, 200, 255); // 水色
		case AINodeType::SEQUENCE: return IM_COL32(0, 200, 0, 255); // 緑
		case AINodeType::ACTION: return IM_COL32(200, 0, 0, 255); // 赤
		case AINodeType::DECRATOR: return IM_COL32(200, 200, 0, 255); // 黄
		default: return IM_COL32(128, 128, 128, 255); // グレー
		}
	}


private: // -- 非公開 メンバ関数 -- //


	// 保存処理 
	void Save();
	// 読み込み
	void Load();


public: // -- 公開 メンバ変数 -- //

	// 持ち主(アクター)のポインタ
	Actor* mOwner;
	// 持ち主情報(文字列 ・ これがjsonファイル名になる)
	std::string mOwnerName;
	// ルートノード
	std::shared_ptr<AINode::INode> mRootNode;
	// ノードリスト
	std::vector<std::shared_ptr<AINode::INode>> mNodes;
	// ノード番号
	int32_t mNodeIndex;

private: // -- 非公開 メンバ変数 -- //

};

