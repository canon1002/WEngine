#pragma once
#include "Object/Base/ObjectBase.h"
#include "Resource/Model/Model.h"
#include "Component/Collider/Collider.h"

class Object3d :
    public ObjectBase
{

public: // -- 公開 メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object3d() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object3d() = default;;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="objectName">オブジェクト名</param>
	virtual void Init(std::string objectName) override;

	/// <summary>
	///  更新処理
	/// </summary>
	virtual void Update() override;

	/// <summary>
	///  描画処理
	/// </summary>
	/// <param name="camera">カメラ</param>
	virtual void Draw(Camera camera) override;

	/// <summary>
	/// デバッグ描画
	/// </summary>
	virtual void Debug() override;

	/// <summary>
	/// モデル取得関数
	/// </summary>
	/// <returns>オブジェクトに登録したモデルを戻り値として渡す</returns>
	Model* GetModel() { return mModel; }

	/// <summary>
	/// モデル登録関数
	/// </summary>
	/// <param name="filepath">モデルのファイルパス</param>
	void SetModel(const std::string& filepath);

private: // -- 非公開 メンバ関数 -- //


	void CreateTransformation();

protected: // -- 限定公開 メンバ変数 -- //

	// Transformation用のResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> mWvpResource = nullptr;
	// データを書き込む
	TransformationMatrix* mWvpData = nullptr;

	// マテリアル
	Material material;
	// モデル
	Model* mModel = nullptr;

public: // -- 公開 メンバ変数 -- //

	// スキニング 
	Skinnig* mSkinning = nullptr;
	// スケルトン
	Skeleton mSkeleton;
	// コライダー
	Collider* mCollider;

};

