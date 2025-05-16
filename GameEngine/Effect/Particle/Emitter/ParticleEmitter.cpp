#include "ParticleEmitter.h"
#include "GameEngine/Editor/Framerate.h"
#include "GameEngine/Effect/Particle/ParticleManager.h"
#include "GameEngine/Editor/GlobalVariables.h"

ParticleEmitter::ParticleEmitter(const std::string& name){
	mName = name;
}

void ParticleEmitter::Init(){

	// エミッターの位置を初期化
	mWorldTransform = std::make_unique<WorldTransform>();
	mWorldTransform->Init();
	mEmitter.pos = mWorldTransform->GetWorldPosition();

	// グローバル変数クラスのインスタンスを取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();


	// -- jsonからパラメータを取得する -- //


	//	初期座標の分布を取得
	mEmitter.posXDistr = std::uniform_real_distribution<float>(
		globalVariables->GetVector2Value(mName, "PosXDister").x,
		globalVariables->GetVector2Value(mName, "PosXDister").y
	);
	mEmitter.posYDistr = std::uniform_real_distribution<float>(
		globalVariables->GetVector2Value(mName, "PosYDister").x,
		globalVariables->GetVector2Value(mName, "PosYDister").y
	);
	mEmitter.posZDistr = std::uniform_real_distribution<float>(
		globalVariables->GetVector2Value(mName, "PosZDister").x,
		globalVariables->GetVector2Value(mName, "PosZDister").y
	);

	// サイズの分布を取得
	mEmitter.scaleDistr = std::uniform_real_distribution<float>(
		globalVariables->GetVector2Value(mName, "ScaleDister").x,
		globalVariables->GetVector2Value(mName, "ScaleDister").y
	);

	// 移動量の分布を取得
	mEmitter.velXDistr = std::uniform_real_distribution<float>(
		globalVariables->GetVector2Value(mName, "VelXDister").x,
		globalVariables->GetVector2Value(mName, "VelXDister").y
	);
	mEmitter.velYDistr = std::uniform_real_distribution<float>(
		globalVariables->GetVector2Value(mName, "VelYDister").x,
		globalVariables->GetVector2Value(mName, "VelYDister").y
	);
	mEmitter.velZDistr = std::uniform_real_distribution<float>(
		globalVariables->GetVector2Value(mName, "VelZDister").x,
		globalVariables->GetVector2Value(mName, "VelZDister").y
	);

	// 色の分布を取得
	mEmitter.colorDistr = std::uniform_real_distribution<float>(
		globalVariables->GetVector2Value(mName, "ColorDister").x,
		globalVariables->GetVector2Value(mName, "ColorDister").y
	);

	// 生存時間の分布を取得
	mEmitter.timeDistr = std::uniform_real_distribution<float>(
		globalVariables->GetVector2Value(mName, "TimeDister").x,
		globalVariables->GetVector2Value(mName, "TimeDister").y
	);

	// 一度に発生させるパーティクルの数を取得
	mEmitter.count = globalVariables->GetIntValue(mName, "Count");
	// 発生頻度の取得
	mFrequency = globalVariables->GetFloatValue(mName, "Frequency");
	// 発生頻度用の時刻 0で初期化
	mFrequencyTime = 0.0f;

}

void ParticleEmitter::Update(){

	// Δtを定義 60fps固定してあるが、実時間を計測して可変fpsで動かせるようにしておきたい
	const float kDeltaTime = (1.0f / Framerate::GetInstance()->GetFramerate());
	
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
	ParticleManager::GetInstance()->Emit(mName, mEmitter);
}
