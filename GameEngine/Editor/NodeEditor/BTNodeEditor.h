#pragma once
#include "GameEngine/Editor/ImGui/ImGuiManager.h"
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

	// BT実行処理
	AINode::State Tick();
	// BT再起動処理
	void Reset();

	// 新規ノード生成処理
	void Create(const AINodeType& type, const std::string& name);

	// ファクトリメソッド
	std::shared_ptr<AINode::INode> CreateNodeByType(const AINodeType& type,const std::string& Nodeame);

	// ノードの種類から色を取得
	inline ImU32 GetNodeColor(const AINodeType& type) {
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

	// 親ノードから子ノードを削除
	bool DetachChild(std::shared_ptr<AINode::Composite> parent, std::shared_ptr<AINode::INode> child);
	// 指定した子ノードを新しい親に接続
	bool AttachChild(std::shared_ptr<AINode::Composite> newParent, std::shared_ptr<AINode::INode> child);
	// ノードを削除し、親ノードからも切り離す
	bool RemoveNode(std::shared_ptr<AINode::INode> node);
	// ルートノードを変更
	void SetRootNode(std::shared_ptr<AINode::Composite> newRoot);
	// ノードの親を変更
	bool MoveNode(std::shared_ptr<AINode::INode> node, std::shared_ptr<AINode::Composite> newParent);

	// ノードの親子関係をドラッグ＆ドロップで変更
	void HandleNodeLinking();
	// リンクの削除
	void LinkDeletion();
	// IDからノードを検索
	std::shared_ptr<AINode::INode> FindNodeByID(int nodeID);


	// 保存処理 
	void Save();
	// 読み込み
	void Load();

	// ノード生成処理
	std::shared_ptr<AINode::INode> CreateForJson(json j);
	// リンク生成処理
	void CreateLink(std::shared_ptr<AINode::INode> node);


public: // -- 公開 メンバ変数 -- //

	// 持ち主(アクター)のポインタ
	Actor* mOwner;
	// 持ち主情報(文字列 ・ これがjsonファイル名になる)
	std::string mOwnerName;
	// ルートノード
	std::shared_ptr<AINode::Composite> mRootNode;
	// ノードリスト
	std::vector<std::shared_ptr<AINode::INode>> mNodes;
	// ノード番号
	int32_t mNodeIndex;

private: // -- 非公開 メンバ変数 -- //

	/// <summary>
	/// ImNodes上で使用するリンク
	/// </summary>
	/// <param name=" first : "> LinkID </param>
	/// <param name=" second : "> parentID </param>
	/// <param name=" third : "> childID </param>
	std::vector<std::tuple<int, int, int>> mLinks;
	int32_t mLinkIndex;
};

