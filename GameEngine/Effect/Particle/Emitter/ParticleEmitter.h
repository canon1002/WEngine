#pragma once
#include "GameEngine/Effect/Particle/ParticleCommon.h"
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
	
	// エミッターの発生座標を指定する
	inline void SetEmitterPos(const Vector3& pos) {
		mWorldTransform->translation = pos;
		mEmitter.pos = pos;
	}

public: // -- 公開 メンバ変数 -- //

	// 発生装置の名称
	std::string mName;

protected: // -- 限定公開 メンバ変数 -- //

	// エミッターの情報
	Emitter mEmitter;
	// エミッターの座標
	std::unique_ptr<WorldTransform> mWorldTransform;
	// 発生頻度
	float mFrequency;
	// 頻度用時刻
	float mFrequencyTime;


};

