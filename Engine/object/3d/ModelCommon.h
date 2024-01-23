#pragma once
#include "Math.h"
#include "../worldTransform/WorldTransform.h"
#include "../../resources/Section/Resource.h"
#include "../../object/light/DirectionalLight.h"
#include "../base/DirectXCommon.h"

class ModelCommon
{
private:

	ModelCommon();
	~ModelCommon();

public: // メンバ関数

	// コピーコンストラクタと演算子オーバーロードの禁止
	ModelCommon(const ModelCommon& obj) = delete;
	ModelCommon& operator=(const ModelCommon& obj) = delete;
	
	// インスタンスの取得
	static ModelCommon* GetInstance();

	///	初期化
	void Init();
	/// 更新
	void Update();
	/// 描画
	void Draw();

public:

private: // メンバ変数

	DirectXCommon* dx_;


	static ModelCommon* instance;

};

