#pragma once
#include "ParticleCommon.h"

// 軌道エフェクト
class TrailEffect:
	public ParticleCommon
{
public: // -- 公開 メンバ変数 -- //

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual Particle Create(const Vector3& translate, std::mt19937& randomEngine)override;
	Particle Create(const WorldTransform& world,const uint32_t count);
	virtual std::list<Particle> Emit(const Emitter& emtter, std::mt19937& randomEngine) override;

	inline void SetParentToEmiter(const Matrix4x4& parent) { mEmitter.worldTransform.SetParent(parent); }

private: // -- 非公開 メンバ関数 -- //

	// パーティクル表示時間
	const float kLifeTimeMidiam = 3.0f;	// 中間
	const float kLifeTimeHigh = kLifeTimeMidiam * 1.5f;	// 長い
	const float kLifeTimeLow = kLifeTimeMidiam * 0.5f;	// 短い

};

