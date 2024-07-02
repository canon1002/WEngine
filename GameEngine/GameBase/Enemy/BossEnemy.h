#pragma once
#include "GameEngine/Object/3d/Object3d.h"
#include "GameEngine/GameBase/AI/State/IBossState.h"
#include "GameEngine/GameBase/AI/State/VitalityBossState.h"

// 前方宣言
class Player;

class BossEnemy
{
public: // -- 公開 メンバ関数 -- //

	BossEnemy() = default;
	~BossEnemy() = default;

	// 初期化
	void Init();
	// 初期化 -- ビヘイビアツリー --
	void InitBehavior();
	// 更新
	void Update();
	// 描画
	void Draw();
	// デバッグUI表示
	void DrawGUI();

	// モデルの取得
	Model* GetModel() { return mObject->mModel; }

	// プレイヤーのポインタをセットする
	void SetPlayer(Player* player) { pPlayer = player; }

	// ステートの更新処理
	void UpdateState();

#pragma region コマンド・ステート関連

	inline void MoveForward() { mObject->mWorldTransform->translation.z += mVelocity.z; }
	inline void BackStep(){ mObject->mWorldTransform->translation.z -= mVelocity.z; }

	// -- 変更用関数 -- //

	// 移動させる
	void AddTransform(Vector3 velocity) { mObject->mWorldTransform->translation += velocity; }
	// 回転量を任意の値に変更する
	void SetRotation(Vector3 rotation) { mObject->mWorldTransform->rotation = rotation; }

	// -- 取得関数 -- //

	Vector3 GetWorldPos() { return mObject->GetWorldTransform()->translation; }
	Vector3 GetWorldPosForTarget();

	// -- ActionNodeの実行条件を設定する関数 -- //

	// 距離が近い場合に実行
	bool invokeNearDistance(){
		return true;
	};

	// 距離が遠い場合に実行
	bool invokeFarDistance(){
		return true;
	};

#pragma endregion

private: // -- 非公開 メンバ変数 -- //

	// プレイヤー(ターゲット)のポインタ
	Player* pPlayer;

	// オブジェクトクラス
	std::unique_ptr<Object3d> mObject;

	// 移動量
	Vector3 mVelocity;

	// 重力の影響を受けるか
	bool mIsGravity;
	// 残りジャンプ回数
	int32_t mJumpCount;
	// 最大ジャンプ回数
	const int32_t kJumpCountMax = 2;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.4f;

	// 最大コンボ回数
	const int32_t kComboCountMax = 3;

	// メンバ関数ポインタ
	static void(BossEnemy::*CommandTable[])();

	// ボスのState番号の配列(実行中に増減しないため、長さ固定の配列を宣言)
	std::array<std::unique_ptr<IBossState>, 1> mStateArr;
	int mCurrentStateNo;
	int mPrevStateNo;
};
