#pragma once
#include "GameEngine/Object/3d/Object3d.h"
#include <memory>

// 外部から取得するパラメータ
struct BulletStatus {

	// 初期座標
	Vector3 pos;
	// 半径
	float radius = 0.5f;
	// 移動速度
	float moveSpeed;
	// 移動方向
	Vector3 direction;
	// 生成後からの時間
	float currentTime = 0.0f;
	// 生存時間
	float lifeTime = 1.0f;
	
	// 威力(攻撃力 * 威力 で計算する)
	float power = 1.0f;
	// ノックバックの有無
	bool isHitKnockBack = false;

	// 衝突属性(自分)
	uint32_t collisionAttribute = 0xffffffff;
	// 衝突可能な相手の属性
	uint32_t collisionMask = 0xffffffff;

};


/// <summary>
/// アクターで呼び出す弾オブジェクト
/// </summary>
class Bullet
{
public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="bulletStatus">弾の初期パラメータ</param>
	Bullet(const BulletStatus& bulletStatus);
	// デストラクタ
	~Bullet() = default;
	// 初期化
	void Init();
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	// 消滅条件を満たしたか
	bool IsDead() const;

	// オブジェクトの取得
	Object3d* GetObject3d() { return mObject.get(); }

private: // -- 非公開 メンバ変数 -- //

	// 外部から取得するパラメータ郡
	BulletStatus mBulletStatus;
	// デスフラグ
	bool mIsDead;

	// オブジェクト
	std::unique_ptr<Object3d> mObject;


};

