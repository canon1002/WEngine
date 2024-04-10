#include "BehaviourTree.h"
#include "BossCommandManager.h"
#include "../BossEnemy.h"
#include "../../Player/Player.h"
#include <random>

#pragma region SequenceNode

SequenceNode::~SequenceNode(){
	for (auto behavior : children) {
		delete behavior;
	}
}

void SequenceNode::AddBehavior(BehaviorTreeNode * behavior){
	children.push_back(behavior);
}

void SequenceNode::AddBehavior(BehaviorTreeNode* behavior1, BehaviorTreeNode* behavior2){
	children.push_back(behavior1);
	children.push_back(behavior2);
}

bool SequenceNode::Run(){
	for (auto child : children) {
		if (!child->Run()) {
			return false; // 一つでも失敗したら失敗
		}
	}
	return true; // 全ての子ノードが成功した場合は成功
}


bool SequenceNode::Run(BossEnemy* boss) {

	for (auto child : children) {
		if (!child->Run(boss)) {
			boss->GetCommand()->AddLogMessage("Failure to SequenceNode");
			return false; // 一つでも失敗したら失敗
		}
	}
	boss->GetCommand()->AddLogMessage("Success to SequenceNode");
	return true; // 全ての子ノードが成功した場合は成功
}

#pragma endregion

#pragma region Selector

SelectorNode::~SelectorNode() {
	for (auto behavior : children) {
		delete behavior;
	}
}

void SelectorNode::AddBehavior(BehaviorTreeNode * behavior) {
	children.push_back(behavior);
}

bool SelectorNode::Run() {

	for (auto child : children) {
		if (child->Run()) {
			return true; // 一つでも成功したら成功
		}
	}
	return false; // 全ての子ノードが失敗した場合は失敗
}

bool SelectorNode::Run(BossEnemy* boss) {
	for (auto child : children) {
		if (child->Run(boss)) {
			boss->GetCommand()->AddLogMessage("Success to SelectorNode");
			return true; // 一つでも成功したら成功
		}
	}
	boss->GetCommand()->AddLogMessage("Failure to SelectorNode");
	return false; // 全ての子ノードが失敗した場合は失敗
}

#pragma endregion


bool MoveToPlayer::Run(){
	return true;
}
bool MoveToPlayer::Run(BossEnemy* boss){
	

	// コマンドの種類は移動、対象はプレイヤー
	// 取得したポインタから成否を取得し、成功していればコマンドを積む
	if (boss->IsPossible(BossCommand::MoveToPlayer)) {

		// 成功したらtrueを返す
		boss->GetCommand()->AddLogMessage("Moving forward");
		boss->GetCommand()->AddCommand(BossCommand::MoveToPlayer);
		return true;
	}
	// 失敗したらfalseを返す
	boss->GetCommand()->AddLogMessage("Failure Moving forward");
	return false;
}

bool EscapeToPlayer::Run(){
	return true;
}
bool EscapeToPlayer::Run(BossEnemy* boss){

	// コマンドの種類は移動、対象はプレイヤー
	// 取得したポインタから成否を取得し、成功していればコマンドを積む
	if (boss->IsPossible(BossCommand::Escape)) {

		// 成功したらtrueを返す
		boss->GetCommand()->AddLogMessage("Moving Escape");
		boss->GetCommand()->AddCommand(BossCommand::Escape);
		return true;
	}
	// 失敗したらfalseを返す
	boss->GetCommand()->AddLogMessage("Failure Moving Escape");
	return false;
}

bool MoveToArea::Run(){
	return true;
}
bool MoveToArea::Run(BossEnemy* boss){
	
	// コマンドの種類は移動、対象はプレイヤー
	// 取得したポインタから成否を取得し、成功していればコマンドを積む
	if (boss->IsPossible(BossCommand::MoveToArea)) {

		// 成功したらtrueを返す
		boss->GetCommand()->AddLogMessage("Moving MoveToArea");
		boss->GetCommand()->AddCommand(BossCommand::MoveToArea);
		return true;
	}
	// 失敗したらfalseを返す
	boss->GetCommand()->AddLogMessage("Failure MoveToArea");
	return false;
}

bool Stop::Run(){
	return true;
}
bool Stop::Run(BossEnemy* boss){

	// コマンドの種類は移動、対象はプレイヤー
	// 取得したポインタから成否を取得し、成功していればコマンドを積む
	if (boss->IsPossible(BossCommand::Stop)) {

		// 成功したらtrueを返す
		boss->GetCommand()->AddLogMessage("Stop");
		boss->GetCommand()->AddCommand(BossCommand::Stop);
		return true;
	}
	// 失敗したらfalseを返す
	boss->GetCommand()->AddLogMessage("Failure Stop");
	return false;
}

bool TestAttack::Run(){
	return true;
}
bool TestAttack::Run(BossEnemy* boss){
	
	// コマンドの種類は移動、対象はプレイヤー
	// 取得したポインタから成否を取得し、成功していればコマンドを積む
	if (boss->IsPossible(BossCommand::Attack)) {

		// 成功したらtrueを返す
		boss->GetCommand()->AddLogMessage("Attack");
		boss->GetCommand()->AddCommand(BossCommand::Attack);
		return true;
	}
	// 失敗したらfalseを返す
	boss->GetCommand()->AddLogMessage("Failure Attack");
	return false;
}

bool Extermination::Run(){
	return true;
}
bool Extermination::Run(BossEnemy* boss){
	// 行動を実行
	
	// コマンドの種類は移動、対象はプレイヤー
	// 取得したポインタから成否を取得し、成功していればコマンドを積む
	if (boss->IsPossible(BossCommand::Extermination)) {

		// 成功したらtrueを返す
		boss->GetCommand()->AddLogMessage("Extermination");
		boss->GetCommand()->AddCommand(BossCommand::Extermination);
		return true;
	}
	// 失敗したらfalseを返す
	boss->GetCommand()->AddLogMessage("Failure Extermination");
	return false;
}
