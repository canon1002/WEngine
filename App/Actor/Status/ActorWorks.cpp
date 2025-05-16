#include "ActorWorks.h"
#include "GameEngine/Editor/BlackBoard.h"
#include "GameEngine/Editor/GlobalVariables.h"

ActorWorks::ActorWorks(){}

ActorWorks::~ActorWorks(){}

void ActorWorks::Init(const std::string& name){
	
	// ファイル名
	std::string filepath = name + "Works";

	// ブラックボードからグローバル変数クラスを取得
	GlobalVariables* global = BlackBoard::GetGlobalVariables();

	// アクターの状態(初期は通常状態)
	mBehavior = Behavior::kRoot;


	// 攻撃パラメータ
	mWorkAttack.attackPhase = AttackPhase::Default; // 非攻撃時
	mWorkAttack.parryReceptionTime = global->GetVector2Value(filepath, "ParryTime");
	mWorkAttack.inputWaitingTime = 0.0f;
	mWorkAttack.comboCount = 0;
	mWorkAttack.isComboRequest = false;
	mWorkAttack.isOperating = false;
	mWorkAttack.isHit = false;

	// 回避パラメータ
	mWorkAvoid.isAvoidRequest = false;
	mWorkAvoid.invinsivleCount = global->GetVector2Value(filepath, "InvinsivleCount");// 無敵時間発生期間
	mWorkAvoid.avoidSpeed = global->GetFloatValue(filepath, "AvoidSpeed");// 回避速度
	mWorkAvoid.avoidRange = global->GetFloatValue(filepath, "AvoidRange");// 回避距離
	mWorkAvoid.elapsedTime = 0.0f;// 回避時間


	// 防御パラメータ
	mWorkGuard.isGuardRequest = false;
	mWorkGuard.startGuardCount = global->GetFloatValue(filepath, "StartGuardCount");// ガード状態の開始カウント
	mWorkGuard.justGuardCount = global->GetVector2Value(filepath, "JustGuardCount");// ジャストガード発生期間
	mWorkGuard.guardTime = 0.0f;// ガード時間
	mWorkGuard.guardSpeed = global->GetFloatValue(filepath, "GuardSpeed");// 1フレームあたりのガードカウント加算量
	mWorkGuard.damageProtection = global->GetFloatValue(filepath, "Protection");// ガード時のダメージ軽減率


}
