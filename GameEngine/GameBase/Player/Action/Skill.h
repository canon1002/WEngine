#pragma once
#include "Object/3d/Object3d.h"

class Skill
{
public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	Skill() = default;
	// 仮想デストラクタ
	virtual ~Skill() = default;

	// 初期化
	virtual void Init() = 0;
	// 更新処理
	virtual void Update(Object3d* object) = 0;
	// 描画処理
	virtual void Draw() = 0;

	// 開始処理
	virtual void Start(Object3d* object) = 0;
	// 終了処理
	virtual void End(Object3d* object) = 0;
	// 再起動処理
	virtual void Reset() = 0;


protected: // -- 限定公開 メンバ変数 -- // 


};

