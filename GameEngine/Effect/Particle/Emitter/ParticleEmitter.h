#pragma once
#include "GameEngine/Append/Transform/WorldTransform.h"
#include <random>
#include <memory>

/// <summary>
/// パーティクル発生機クラス
/// </summary>
class ParticleEmitter{

public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	ParticleEmitter(const std::string& name);
	// デストラクタ
	~ParticleEmitter() = default;
	// 初期化
	void Init();
	// 更新
	void Update();
	// パーティクルの発生
	void Emit();

protected: // -- 限定公開 メンバ変数 -- //

	// 発生装置の名称
	std::string mName;

	// エミッターの座標
	std::unique_ptr<WorldTransform> mWorldTransform;
	// 発生数
	uint32_t mCount;
	// 発生頻度
	float mFrequency;
	// 頻度用時刻
	float mFrequencyTime;


};

