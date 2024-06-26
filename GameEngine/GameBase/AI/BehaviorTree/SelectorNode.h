#include "IBehavior.h"
#include <vector>

/// <summary>
/// セレクターノード
/// <para> 積まれたノードを順番に処理するノード </para>
/// <para> 実行したノードが一つでもtrueを返した場合、 </para>
/// <para> trueを返し、処理を終了する </para>
/// <para> [ A or B or C のようにn個の中からいずれかを実行する処理 ] を実装する用途で使用する </para>
/// </summary>
class SelectorNode : public IBehavior {
private:
	// 子ノードを格納する配列
	std::vector<IBehavior*> children;

public:

	// 子ノードの解放処理
	inline ~SelectorNode(){
		
	}

	// 子ノードを追加する
	inline void AddBehavior(IBehavior* behavior) { children.push_back(behavior); }

	// 実行
	inline virtual bool Run() override {
		for (auto child : children) {
			if (child->Run()) {
				return true; // 一つでも成功したら成功
			}
		}
		return false; // 全ての子ノードが失敗した場合は失敗
	}
};

