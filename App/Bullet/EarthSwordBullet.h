#pragma once
#include "App/Bullet/Bullet.h"
#include "GameEngine/Object/3d/Object3d.h"
#include "App/Actor/Collider/GameCollider.h"
#include <memory>

/// <summary>
/// 大地の剣の弾オブジェクト
/// </summary>
class EarthSwordBullet :
	public Bullet
{
public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="bulletStatus">弾の初期パラメータ</param>
	EarthSwordBullet(const BulletStatus& bulletStatus);
	// デストラクタ
	~EarthSwordBullet() = default;
	// 初期化
	virtual void Init()override;
	// 更新処理
	virtual void Update()override;

protected: // -- 限定公開 メンバ変数 -- //

};

