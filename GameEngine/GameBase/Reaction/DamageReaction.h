#pragma once
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Math/Vec2.h"

class MainCamera;

class DamageReaction{
public: // -- 公開 メンバ関数 -- //

	// インスタンス取得
	static DamageReaction* GetInstance();

	// 
	void Reaction(const Vector3 pos, const MainCamera* camera);

	// ワールド座標をスクリーン座標に変換する
	static Vec2 GetScreenPos(const Vector3 pos,const MainCamera* camera);

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static DamageReaction* instance;

};

