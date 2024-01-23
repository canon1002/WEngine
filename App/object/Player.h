#pragma once
#include <stdint.h>
#include "../worldTransform/WorldTransform.h"
#include "../math/Math.h"
#include "../color/Color.h"
#include "../3d/Model.h"

class Player
{
public:

	Player();
	~Player();
	void Init();
	void Update();
	void Draw();

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

private:

	std::unique_ptr<Model> model_;
	 Vec3 pos_;
	 Vec3 rad_;
	 Vec3 vel_;
	 Vec3 speed_;
	 Color color_;

};

