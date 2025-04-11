#pragma once
#include "Bullet.h"

/// <summary>
/// プレイヤー用の弾クラス
/// </summary>
class PlayerBullet :
	public Bullet
{
public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="bulletStatus">弾の初期パラメータ</param>
	PlayerBullet(const BulletStatus& bulletStatus);
	// デストラクタ
	~PlayerBullet() = default;
	// 初期化
	virtual void Init()override;
	// 更新処理
	virtual void Update()override;

protected: // -- 限定公開 メンバ変数 -- //

	// ワールド行列
	std::array<Matrix4x4, 6> mBoneWorldMat;
	// 部位コライダー
	std::array<std::shared_ptr<GameCollider>, 6> mColliders;


};