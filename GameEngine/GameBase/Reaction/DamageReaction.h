#pragma once
#include "GameEngine/Append/Transform/WorldTransform.h"
#include "GameEngine/Math/Vec2.h"
#include <d2d1_3.h>
#include <dwrite.h>
#include <wrl.h>
#include <string>
#include "GameEngine/Base/DirectX/DirectXCommon.h"
#include "GameEngine/Object/Sprite/Sprite.h"


using namespace Microsoft::WRL;

struct DamageData {
	// 画像データ
	Sprite* sprite;
	// 3Dワールド座標
	WorldTransform* world;
	// 表示時間
	int32_t activeTime;
};

class MainCamera;

class DamageReaction{
public: // -- 公開 メンバ関数 -- //

	// インスタンス取得
	static DamageReaction* GetInstance();

	void Init();
	// 更新処理
	void UpdateSprite();

	// 画像を表示
	void DrawSprite();

	// 
	void Reaction(const Vector3 pos, int32_t damage, const MainCamera* camera);

	// テキスト画像の表示
	void RenderSprite(const int32_t value, Vector3 pos, const MainCamera* camera);
	// テキスト画像の座標更新
	void PositionUpdate(std::vector<DamageData*> sprites);

	// テキストの表示
	void RenderText(const std::wstring& text, float x, float y);

	// ワールド座標をスクリーン座標に変換する
	static Vector2 GetScreenPos(const Vector3 pos,const MainCamera* camera);

	// int型の桁数を計算する関数
	int32_t GetDigits(int32_t num);
	// 指定した桁数の数値に変換(例 引数が3 -> 100を返す)
	int32_t GetDigitNumber(int32_t digits);




	

private: // -- 非公開 メンバ変数 -- //

	// インスタンス
	static DamageReaction* instance;

	
	// 0~9までの数値画像の配列
	std::vector<int32_t> mNumberTex;

	// スプライト配列のリスト
	std::list<std::vector<DamageData*>> mDataList;

};

