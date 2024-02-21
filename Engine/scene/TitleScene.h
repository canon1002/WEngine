#pragma once
#include "IScene.h"
#include "../Engine/object/3d/Object3d.h"
#include "../Engine/object/3d/Model.h"
#include "../Engine/primitive/VoxelParticle.h"
#include "../Engine/primitive/Sprite.h"
#include "../Engine/object/2d/SpriteCommon.h"

// 前方宣言
class Input;

class TitleScene :
    public IScene
{
public:
	~TitleScene() {}

	void Finalize()override;

	//　継承した関数
	void Init() override;
	void Update() override;
	void Draw() override;

private:

	// 外部ポインタ
	Input* input_ = nullptr;

	std::unique_ptr<Object3d> eAxis_ = nullptr;
	std::unique_ptr<Object3d> ball_ = nullptr;
	std::unique_ptr <VoxelParticle> voxel_ = nullptr;
	std::unique_ptr<Sprite> sprite_ = nullptr;
	
	float alpth_ = 0.0f;
	bool isSceneChange = false;
	int32_t fadeTimer_ = 60;
};

