#pragma once
#include <vector>
#include <iostream>

// 前方宣言
class BossCommandManager;
class BossEnemy;


/// <summary>
/// ビヘイビアツリー用のノードクラス
/// </summary>
class BehaviorTreeNode  {
public:
	virtual ~BehaviorTreeNode () {};

	virtual bool Run() = 0;// ノードを実行し、結果を返す純粋仮想関数
	virtual bool Run(BossEnemy* boss) = 0;// ノードを実行し、結果を返す純粋仮想関数 (デバッグ用)
};

/// <summary>
/// シーケンスノード
/// <para> 積まれたノードをすべて順番に処理するノード </para>
/// <para> 実行したノードが一つでもfalseを返した場合、 </para>
/// <para> falseを返し、処理を終了する </para>
/// <para> [ A -> B -> C -> D のように特定の順番で実行したい処理 ] を実装する用途で使用する </para>
/// </summary>
class SequenceNode :public BehaviorTreeNode  {
private:
	std::vector<BehaviorTreeNode *> children;

public:
	//入っているものを全部消す
	~SequenceNode();

	//追加
	void AddBehavior(BehaviorTreeNode * behavior);
	void AddBehavior(BehaviorTreeNode * behavior1, BehaviorTreeNode* behavior2);

	// 実行
	virtual bool Run() override;
	virtual bool Run(BossEnemy* boss) override;

};


/// <summary>
/// セレクターノード
/// <para> 積まれたノードを順番に処理するノード </para>
/// <para> 実行したノードが一つでもtrueを返した場合、 </para>
/// <para> trueを返し、処理を終了する </para>
/// <para> [ A or B or C のようにn個の中からいずれかを実行する処理 ] を実装する用途で使用する </para>
/// </summary>
class SelectorNode : public BehaviorTreeNode  {
private:
	std::vector<BehaviorTreeNode *> children;

public:
	~SelectorNode();

	void AddBehavior(BehaviorTreeNode * behavior);


	virtual bool Run() override;
	virtual bool Run(BossEnemy* boss) override;
};

// デコレーターノード
class DecoratorNode : public BehaviorTreeNode {
private:
	BehaviorTreeNode* child;
public:
	virtual bool Run() override {
		// 何らかの修飾処理を行う
		return child->Run();
	}
	
	virtual bool Run(BossEnemy* boss) override {
		// 何らかの修飾処理を行う
		return child->Run(boss);

	}
};

// アクションノード
class ActionNode : public BehaviorTreeNode {
public:
	virtual bool Run() override {
		// 実際のアクションを実行する
		return true; // あるいは成功か失敗の結果を返す
	}
	
	virtual bool Run(BossEnemy* boss) override {
		boss;
		// 実際のアクションを実行する
		return true; // あるいは成功か失敗の結果を返す
	}


};

// シンプルな動作の例

// プレイヤーの付近に移動する
class MoveToPlayer : public BehaviorTreeNode  
{
public:

	virtual bool Run() override;
	virtual bool Run(BossEnemy* boss) override;

};

class EscapeToPlayer : public BehaviorTreeNode
{
public:

	virtual bool Run() override;
	virtual bool Run(BossEnemy* boss) override;

};

class MoveToArea : public BehaviorTreeNode
{
public:

	virtual bool Run() override;
	virtual bool Run(BossEnemy* boss) override;

};

class Stop : public BehaviorTreeNode
{
public:

	virtual bool Run() override;
	virtual bool Run(BossEnemy* boss) override;

};

// 攻撃
class TestAttack: public BehaviorTreeNode
{
public:

	virtual bool Run() override;
	virtual bool Run(BossEnemy* boss) override;

};


// 殲滅 -- モブへの攻撃 --
class Extermination : public BehaviorTreeNode
{
public:

	virtual bool Run() override;
	virtual bool Run(BossEnemy* boss) override;

};