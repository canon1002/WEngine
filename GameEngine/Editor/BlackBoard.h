#pragma once
#include "GameEngine/Utility/Math/Vector3.h"

// クラスの前方宣言
class StatusManager;
class Framerate;
class GlobalVariables;

// フィールドの限界地点(半径)
const Vector3 kFieldRadius = { 600.0f,40.0f,600.0f };

/// <summary>
/// ブラックボード
/// <para> ゲーム上でAIなどが参照するデータにアクセスしやすくしたもの </para>
/// </summary>
class BlackBoard {

public: // -- 公開 メンバ関数 -- //

	// -- AI・ビヘイビア関係 -- //


	// -- ステータス関係 -- //
	static StatusManager* GetStatusManager();
	
	// -- フレームレート関係 -- //
	static Framerate* GetFramerate();

	/// <summary>
	/// 入力した数値をゲーム全体の速度を考慮した1フレームあたりの速度に変換
	/// </summary>
	/// <param name="speed">変換するゲームの速度値。</param>
	/// <returns>指定された速度値に対応するFPS値（float型）。</returns>
	static float CombertGameFPS(float speed);

	/// <summary>
	/// ゲーム全体の速度を考慮した1フレームあたりの速度を取得
	/// </summary>
	/// <returns></returns>
	static float GetGameFPS();

	/// <summary>
	/// 入力した数値をバトルスピードを考慮した1フレームあたりの速度に変換
	/// </summary>
	/// <param name="speed"></param>
	/// <returns></returns>
	static float CombertBattleFPS(float speed);

	/// <summary>
	/// バトルスピードを考慮した1フレームあたりの速度を算出
	/// </summary>
	/// <returns></returns>
	static float GetBattleFPS();

	// -- グローバル変数関係 -- //
	static GlobalVariables* GetGlobalVariables();

	// フィールド限界地点を取得
	static Vector3 GetFieldRadius() { return kFieldRadius; }

};

