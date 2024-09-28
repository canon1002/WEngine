#include "ObjectBase.h"
#include "Component/Transform/WorldTransform.h"
#include "Object/Camera/Camera.h"

void ObjectBase::Init(std::string objectName)
{
	// オブジェクト名を設定
	mObjectName = objectName;
}

void ObjectBase::Draw(Camera camera)
{
	camera;
}
