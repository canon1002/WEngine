#pragma once
#include "ParticleCommon.h"

class HitEffect : 
	public ParticleCommon
{
public: // -- 公開 メンバ変数 -- //

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual Particle Create(const Vector3& translate, std::mt19937& randomEngine)override;
	virtual std::list<Particle> Emit(const Emitter& emtter, std::mt19937& randomEngine) override;
};

