#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/Camera/CameraCommon.h"
#include "GameEngine/Input/InputManager.h"

class MainCamera:
	public CameraCommon
{
public: // -- public メンバ関数 -- //

	MainCamera() = default;
	~MainCamera() = default;

	// インスタンス取得
	static MainCamera* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(WinAPI* winApp);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;


	// フォローカメラ機能 -- フォロー対象設定 -- 
	inline void SetTarget(const WorldTransform* target) { mTarget = target; }

	// フォローカメラ機能 -- 追加平行移動値の設定 -- 
	inline void SetAddTranslation(const Vector3 translation){
		mAddTranslation = translation;
	}

private: // -- private メンバ関数 -- //

	// コピーコンストラクタと演算子オーバーロードの禁止
	MainCamera(const MainCamera& obj) = delete;
	MainCamera& operator=(const MainCamera& obj) = delete;
	
private: // -- private メンバ変数 -- //

	// インスタンス
	static MainCamera* instance;
	
	// 入力マネージャ
	InputManager* mInput;

	// フォロー対象の座標
	const WorldTransform* mTarget;
	// 追加平行移動値
	Vector3 mAddTranslation;

};

