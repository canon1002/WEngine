#pragma once
#include "Skill.h"

/// <summary>
/// 斬撃
/// </summary>
class Slash :
    public Skill
{

public: // -- 公開 メンバ関数 -- //

	// コンストラクタ
	Slash() = default;
	// 仮想デストラクタ
	virtual ~Slash() = default;

	// 初期化
	virtual void Init() override;
	// 更新処理
	virtual void Update(Object3d* object) override;
	// 描画処理
	virtual void Draw() override;

	// 開始処理
	virtual void Start(Object3d* object) override;
	// 終了処理
	virtual void End(Object3d* object) override;
	// 再起動処理
	virtual void Reset() override;


protected: // -- 限定公開 メンバ変数 -- // 

};

