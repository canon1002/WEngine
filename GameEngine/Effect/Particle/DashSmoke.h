#pragma once
#include "ParticleCommon.h"

class DashSmoke :
    public ParticleCommon
{
public: // -- 公開 メンバ関数 -- //

	DashSmoke() = default;
	~DashSmoke() = default;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual Particle Create(const Vector3& translate,std::mt19937& randomEngine)override;
	virtual std::list<Particle> Emit(const Emitter& emtter, std::mt19937& randomEngine) override;

private: // -- 非公開 メンバ関数 -- //

	/*virtual void CreateTransformation() override;
	virtual void CreateVertex() override;
	virtual void CreateIndex() override;
	virtual void CreateMaterial() override;
	virtual void CreateInstancing() override;*/

private: // -- 非公開 メンバ変数 -- //

};

