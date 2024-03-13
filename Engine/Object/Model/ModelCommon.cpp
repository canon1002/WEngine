#include "ModelCommon.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "../camera/MatrixCamera.h"


// staticメンバ変数で宣言したインスタンスを初期化
ModelCommon* ModelCommon::instance = nullptr;

ModelCommon::ModelCommon() {}

ModelCommon::~ModelCommon() {}

// インスタンスを取得
ModelCommon* ModelCommon::GetInstance() {
	// 関数内staticは初めて通ったときのみ実行される
	//static DirectXCommon* instance = nullptr;
	if (instance == nullptr) {
		instance = new ModelCommon;
	}
	return instance;
}


void ModelCommon::Init()
{
	//dx_ = DirectXCommon::GetInstance();
}

void ModelCommon::Update()
{

}

void ModelCommon::Draw()
{

}
