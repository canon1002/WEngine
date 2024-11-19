#include "ObjectBase.h"

ObjectBase::ObjectBase(const std::string& objname){
	// 名前をセット
	mObjname = objname;
}

ObjectBase::~ObjectBase(){}

void ObjectBase::Init(){
	// ワールド座標を初期化
	mWorldTransform->Init();
	// 描画フラグの初期化(一律true)
	mDrawFlags.isActiveModel = true;
	mDrawFlags.isActiveSkinning = true;
	mDrawFlags.isActiveSprite = true;
	
}

void ObjectBase::Update(){}

void ObjectBase::Draw(){

	// フラグに応じて早期リターン
	if (!mDrawFlags.isActiveModel) {
		return;
	}

}

void ObjectBase::DrawSkin(){

	// フラグに応じて早期リターン
	if (!mDrawFlags.isActiveSprite) {
		return;
	}

}

void ObjectBase::DrawUI(){

	// フラグに応じて早期リターン
	if (!mDrawFlags.isActiveSprite) {
		return;
	}


}

void ObjectBase::DrawGUI(){
#ifdef _DEBUG
#endif // _DEBUG
}
