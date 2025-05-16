#pragma once
#include "GameEngine/Utility/Math/Vector3.h"

// クラスの前方宣言
class StatusManager;
class Framerate;
class GlobalVariables;

// フィールドの限界地点(半径)
const Vector3 kFieldRadius = { 20.0f,40.0f,20.0f };

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

