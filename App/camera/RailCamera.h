#pragma once
#include <memory>
#include "../camera/CameraCommon.h"
#include "../worldTransform/WorldTransform.h"
#include "../math/Math.h"
#include "../color/Color.h"
#include "../3d/Object3d.h"

/// <summary>
/// レールカメラ
/// </summary>
class RailCamera:
	public CameraCommon
{
public:
	RailCamera() = default;
	~RailCamera() = default;

	void Initialize();
	void Initialize(Vec3 t, Vec3 r);
	void Update()override;
	
private:


	// ImGui用の変数
	Vec3 m_fTranslation = {0, 0, 0};
	Vec3 m_fRotation = {0, 0, 0};

};