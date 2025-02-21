#pragma once

// クラスの前方宣言
class StatusManager;
class Framerate;

/// <summary>
/// ブラックボード
/// <para> ゲーム上でAIなどが参照するデータにアクセスしやすくしたもの </para>
/// </summary>
namespace BlackBoard {

	// -- AI・ビヘイビア関係 -- //


	// -- ステータス関係 -- //
	static StatusManager* GetStatusManager();
	
	// -- フレームレート関係 -- //
	static Framerate* GetFramerate();
	static Framerate* GetGameSpeed();


};

