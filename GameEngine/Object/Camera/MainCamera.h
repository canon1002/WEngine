#pragma once
#include "Object/Camera/Camera.h"

class MainCamera:
	public Camera
{
public: // -- public メンバ関数 -- //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	MainCamera() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MainCamera() = default;

	/// <summary>
	/// インスタンス取得
	/// </summary>
	static MainCamera* GetInstance();

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
	/// デバッグ描画
	/// </summary>
	virtual void Debug() override;


	// フォローカメラ機能 -- フォロー対象設定 -- 
	inline void SetTarget(const WorldTransform* target) { mTarget = target; }

	// フォローカメラ機能 -- 追加平行移動値の設定 -- 
	inline void SetAddTranslation(const Vector3 translation){
		mAddTranslation = translation;
	}

private: // -- private メンバ関数 -- //

	// コピーコンストラクタと演算子オーバーロードの禁止
	MainCamera(const MainCamera& obj) = delete;
	MainCamera& operator=(const MainCamera& obj) = delete;
	
private: // -- private メンバ変数 -- //

	// インスタンス
	static MainCamera* instance;
	
	// フォロー対象の座標
	const WorldTransform* mTarget;
	// 追加平行移動値
	Vector3 mAddTranslation;

};

