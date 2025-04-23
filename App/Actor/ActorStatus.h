#pragma once

/// <summary>
/// アクター共通の攻撃・防御・回避などのパラメータ等を記述するヘッダー
/// </summary>


/// <summary>
/// 攻撃段階
/// </summary>
enum AttackPhase {
	Default,//非攻撃時
	OperatingExtra, // 攻撃前の予備動作
	Attack,// 攻撃中
	After,// 攻撃終了後
};

/// <summary>
/// 攻撃関連パラメータ
/// <para> 攻撃に関する変数をまとめる </para>
/// <para> また、キャラステータスの"STR"に依存する項目を設定する </para>
/// </summary>
struct AttackStatus {
	AttackPhase attackPhase;

};