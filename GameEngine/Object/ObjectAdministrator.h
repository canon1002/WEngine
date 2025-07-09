#pragma once
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "Object/Base/ObjectBase.h"

#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/Resource/Model/ModelManager.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"

#include <memory>

/// <summary>
/// オブジェクト管理者クラス
/// <para> PostDarwやPreDrawなどの呼び出しや、オブジェクトの登録に必要なDXCommonクラスのポインタ保持、</para>
/// <para> Objectをstd::list(またはstd::vector)で登録してDrawCallするなどの役割をもたせる </para> 
/// </summary>
class ObjectManager{
public: // -- public メンバ関数 -- //

	static std::shared_ptr<Object3d> CreateObject(const std::string& directrypath, const std::string& filename);
	static ObjectManager* GetInstance();

	ObjectManager() = default;
	~ObjectManager() = default;
	void Init();
	void Update();
	void Draw();
	void Final();

private: // -- private メンバ関数 -- //



public: // -- public メンバ変数 -- //

	// Object3Dのリスト
	std::list<std::shared_ptr<ObjectBase>> objectList_;

private: // -- private メンバ変数 -- //

	// DXCommonのポインタ
	
	
	static ObjectManager* instance;
	

};

