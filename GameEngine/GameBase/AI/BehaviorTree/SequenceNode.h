#include "IBehavior.h"
#include <vector>

/// <summary>
/// シーケンスノード
/// <para> 積まれたノードをすべて順番に処理するノード </para>
/// <para> 実行したノードが一つでもfalseを返した場合、 </para>
/// <para> falseを返し、処理を終了する </para>
/// <para> [ A -> B -> C -> D のように特定の順番で実行したい処理 ] を実装する用途で使用する </para>
/// </summary>
class SequenceNode :public IBehavior {
private:
	// 子ノードを格納する配列
	std::vector<IBehavior*> children;

public:

	// 子ノードの解放処理
	inline ~SequenceNode() {}

	// 子ノードを追加する
	inline void AddBehavior(IBehavior* behavior) { children.push_back(behavior); }

	// 実行
	inline bool Run() override {
		for (auto child : children) {
			if (!child->Run()) {
				return false; // 一つでも失敗したら失敗
			}
		}
		return true; // 全ての子ノードが成功した場合は成功
	}
	
	// 実行
	virtual std::function<void()> Execute()override {
		return nullptr; // 全ての子ノードが失敗した場合は失敗
	}

	// メンバ関数ポインタ
	//static void(BossEnemy::*condition_[])();

};