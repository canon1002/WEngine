#pragma once
#include "GameEngine/Editor/GlobalVariables.h"

struct Status {
	int32_t HP; // 体力
	int32_t STR;// 筋力
	int32_t VIT;// 生命力
	int32_t AGI;// 素早さ

};


class StatusManager {

	static StatusManager* instance;
	StatusManager() = default;
	~StatusManager() = default;
	// コピーコンストラクタと演算子オーバーロードの禁止
	StatusManager(const StatusManager& obj) = delete;
	StatusManager& operator=(const StatusManager& obj) = delete;

public:
	 
	static StatusManager* GetInstance();
	void Init();
	void Update();

	
	void ReceiveDamage();

private:
	// ポインタ
	GlobalVariables* globalVariables_ = nullptr;


};

