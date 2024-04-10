#pragma once
#include <memory>
#include "Engine/Object/Camera/CameraCommon.h"
#include "Engine/Object/WorldTransform/WorldTransform.h"
#include "Engine/Math/Math.h"
#include "Engine/Object/Texture/Color.h"
#include "Engine/Object/Object3d.h"

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
	void Initialize(Vec3 tr, Vec3 r);
	void Update()override;
	
private:

	// 3次元スプライン曲線の制御点の配列
	std::vector<Vec3> sprineControlPoints_;
	// 始点(0.0) ~ 終点(1.0) のどのあたりにいるか保有する補間用の数値
	float t = 0.0f;
 	/// <summary>
 	/// 始点(0.0) ~ 終点(1.0) のどのあたりにいるか保有する補間用の数値
	/// <para> こっちはカメラの方向を指定する前方の座標を取得するためのもの </para>
 	/// </summary>
 	float forwardT = 0.0f;

	// ImGui用の変数
	Vec3 m_fTranslation = {0, 0, 0};
	Vec3 m_fRotation = {0, 0, 0};

	WorldTransform railWorld_;

	// ビュープロジェクションの計算を先に行い、後々ワールド座標に代入する
	WorldTransform matEye_;
	// ワールド座標の向きを取得するための前方ベクトル
	WorldTransform matTarget_;

};