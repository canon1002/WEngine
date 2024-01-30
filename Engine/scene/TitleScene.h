#pragma once
#include "IScene.h"
#include "../Engine/object/3d/Object3d.h"
#include "../Engine/object/3d/Model.h"
#include "../Engine/primitive/VoxelParticle.h"

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
	//std::unique_ptr<Object3d> eAxis2_ = nullptr;
	std::unique_ptr <VoxelParticle> voxel_ = nullptr;

};

