#include "ParticleEmitter.h"
#include "GameEngine/GameMaster/Framerate.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"

ParticleEmitter::ParticleEmitter(const std::string& name){
	mName = name;
}

void ParticleEmitter::Init(){

	// (ToDo)jsonからパラメータを取得する

	// エミッター初期設定
	mWorldTransform = std::make_unique<WorldTransform>();
	mWorldTransform->Init();
	mCount = 8;
	mFrequency = 1.0f;// 1.0秒ごとに発生
	mFrequencyTime = 0.0f;// 発生頻度用の時刻 0で初期化

}

void ParticleEmitter::Update(){

	// Δtを定義 60fps固定してあるが、実時間を計測して可変fpsで動かせるようにしておきたい
	const float kDeltaTime = (1.0f / Framerate::GetInstance()->GetFramerate());

	// instancingCountが最大値を上回らないようにする
	//if (mInstanceCount > kNumMaxInstance) { mInstanceCount = kNumMaxInstance; }
	
	// 時刻を進める
	mFrequencyTime += kDeltaTime;

	// 発生頻度より数値が大きくなったら発生
	if (mFrequency <= mFrequencyTime) {
		// 発生処理
		Emit();

		// 余計に過ぎた時間も加味して頻度計算を行う
		mFrequencyTime -= mFrequency;
	}

}

void ParticleEmitter::Emit(){

	// パーティクル生成処理を呼び出す
	ParticleManager::GetInstance()->Emit(mName, mWorldTransform->translation, mCount);
}
