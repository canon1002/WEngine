#pragma once
#include "GameEngine/Utility/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Utility/Transform/WorldTransform.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Effect/EffectPixels/EffectPixels.h"

struct TwoPointVertex {
	Vector3 top;
	Vector3 bottom;
};

// 軌道エフェクト
class TrailEffect
{
public: // -- 公開 メンバ変数 -- //

	void Init();
	void Update();
	void Draw();
	void DrawGui();

	void Create(const WorldTransform& top,const WorldTransform& bottom);

	bool GetPositionFlag() { return mIsPositionSet; }

	// 色を設定
	void SetColor(Color color) { mEffectColor = color; }

	//void SetIsActive(bool flag) { mIsActive = flag; }
	//bool GetIsActive() { return mIsActive; }

private: // -- 非公開 メンバ関数 -- //

private: // -- 非公開 メンバ変数 -- //

	bool mIsActive;

	// ワールド座標
	WorldTransform* mWorldTransformTop;
	WorldTransform* mWorldTransformButtom;

	// 頂点セット
	std::vector<TwoPointVertex*> mVertices;
	// オブジェクト部分
	std::list<EffectPixels*> mEffectModels;

	// カウント
	float mPositionSetCount;
	float mPositionSetCountMax;

	// 座標設定の許可フラグ
	bool mIsPositionSet;

	// エフェクトの色
	Color mEffectColor;

};

