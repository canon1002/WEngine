#pragma once
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/Object/Model/ModelManager.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"

#include <memory>

/// <summary>
/// オブジェクト管理者クラス
/// <para> PostDarwやPreDrawなどの呼び出しや、オブジェクトの登録に必要なDXCommonクラスのポインタ保持、</para>
/// <para> Objectをstd::list(またはstd::vector)で登録してDrawCallするなどの役割をもたせる </para> 
/// </summary>
class ObjectAdministrator{
public: // -- public メンバ関数 -- //

	static std::shared_ptr<Object3d> CreateObject(const std::string& directrypath, const std::string& filename);
	static ObjectAdministrator* GetInstance();

	ObjectAdministrator() = default;
	~ObjectAdministrator() = default;
	void Init(DirectXCommon* dxCommon);
	void Update();
	void Draw();
	void Finalize();

private: // -- private メンバ関数 -- //



public: // -- public メンバ変数 -- //

	// Object3Dのリスト
	std::list<std::shared_ptr<Object3d>> objectList_;

	// モデル管理者クラス
	ModelManager* modelAdmin_ = nullptr;
	// スプライト管理者クラス
	std::unique_ptr<SpriteAdministrator> spriteAdmin_ = nullptr;
	// パーティクル管理者クラス
	//std::unique_ptr<> particleAdmin_ = nullptr;


private: // -- private メンバ変数 -- //

	// DXCommonのポインタ
	DirectXCommon* mDxCommon = nullptr;
	
	static ObjectAdministrator* instance;
	

};

