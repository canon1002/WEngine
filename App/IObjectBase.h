#pragma once

/// <summary>
/// ゲーム用オブジェクト基底クラス
/// </summary>
class IObjectBase{
public:

	// コンストラクタ
	IObjectBase(){};
	// デストラクタ
	~IObjectBase(){};
	// 初期化
	void Init();
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

protected:



};

