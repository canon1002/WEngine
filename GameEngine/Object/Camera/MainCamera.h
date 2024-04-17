#pragma once
#include "GameEngine/Math/Math.h"
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Resource/Texture/Resource.h"
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Object/Camera/CameraCommon.h"

class MainCamera:
	public CameraCommon
{
public: // -- public メンバ関数 -- //

	MainCamera() = default;
	~MainCamera() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(WinAPI* winApp);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;


private: // -- private メンバ関数 -- //

	// コピーコンストラクタと演算子オーバーロードの禁止
	MainCamera(const MainCamera& obj) = delete;
	MainCamera& operator=(const MainCamera& obj) = delete;
	
};

