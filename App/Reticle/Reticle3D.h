#pragma once
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Math/Math.h"
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Object/Sprite/Sprite.h"
#include <numbers>

// レティクルクラス(ロックオン機能も有する)
class Reticle3D {
public: // -- 公開 メンバ関数 -- //
	
	// コンストラクタ
	Reticle3D();
	// デストラクタ
	~Reticle3D();
	// 初期化
	void Init();
	// 更新
	void Update();
	// 3Dレティクル 描画
	void Draw3DReticle();
	// 2Dレティクル 描画
	void Draw2DReticle();

	// CubeMapのテクスチャをセットする
	void SetCubeMap(const int32_t& textureHandle) { mObject->GetModel()->SetCubeTexture(textureHandle); }
	// レティクルの距離を設定
	void SetReticleDistance(float distance) { mReticleDistance = distance; }
	// レティクルの座標を設定
	void SetWorldPos(const Vector3& pos) { mObject->mWorldTransform->translation = pos; }

	// ワールド座標を取得する
	Vector3 GetWorld3D() { return mObject->GetWorldTransform()->translation; }
	// レティクルの色を変更する
	void SetReticleColor(const Color& color) { mSprite->SetColor(color); }

	bool IsLockOn(const Vector3& target);

private: // -- 非公開 メンバ変数 -- //

	// 3Dレティクルオブジェクト
	std::unique_ptr<Object3d> mObject;

	// 2Dレティクルの座標
	Vector2 mPostionReticle2D;
	// 2Dレティクルスプライト
	std::unique_ptr<Sprite> mSprite;

	// プレイヤーから3Dレティクルへの距離
	float mReticleDistance;

	Vector3 mPosNear;
	Vector3 mPosFar;

	// -- ロックオン関係 -- //

	// ロックオン対象範囲(Z座標)
	Vector2 mLockOnDistance = { 0.01f,100.0f };
	// ロックオン対象範囲角度(左は度数、右の値でradianに変換する)
	float mAngleRange = 15.0f * ((float)std::numbers::pi / 180.0f);

};

