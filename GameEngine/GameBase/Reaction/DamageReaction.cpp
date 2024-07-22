#include "DamageReaction.h"
#include "GameEngine/Object/Camera/MainCamera.h"
#include "GameEngine/Object/Sprite/SpriteAdministrator.h"

DamageReaction* DamageReaction::instance = nullptr;

DamageReaction* DamageReaction::GetInstance(){

	if (instance == nullptr) {
		instance = new DamageReaction();
	}

	return instance;
}

void DamageReaction::Init(){
    
    // 数値用の画像を設定
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/0.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/1.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/2.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/3.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/4.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/5.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/6.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/7.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/8.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->srv_->LoadTexture("number/9.png"));

}

void DamageReaction::UpdateSprite(){

    // スプライト配列のリストを更新する
    for (auto& sprites : mDataList) {
        for (auto& sprite : sprites) {
            // 更新処理
            sprite->sprite->Update();
            // 表示時間の減少
            sprite->activeTime--;
        }

        // ベクトル内の要素をチェックして削除
        sprites.erase(std::remove_if(sprites.begin(), sprites.end(),
            [](DamageData* ptr) {
                if (ptr->activeTime == 0) {
                    delete ptr; // メモリを解放
                    return true; // 削除対象
                }
                return false; // 削除しない
            }), sprites.end());
    }

    // 空のベクトルを持つリストの要素を削除
    mDataList.remove_if([](const std::vector<DamageData*>& vec) {
        return vec.empty();
        });


    

}

void DamageReaction::DrawSprite() {

    // 描画前処理
    SpriteAdministrator::GetInstance()->PreDraw();

    // スプライト配列のリストを描画する
    for (auto& sprites : mDataList) {
        for (auto& sprite : sprites) {
            sprite->sprite->Draw();
        }
    }


}

void DamageReaction::Reaction(const Vector3 pos,int32_t damage, const MainCamera* camera){
	
	// ワールド座標をスクリーン座標に変換する
	Vec2 screenPos = GetScreenPos(pos, camera);
    // リストにセットする
    RenderSprite(damage, screenPos);

}

void DamageReaction::RenderSprite(const int32_t value, Vec2 pos)
{
    // 数値の桁数を取得
    int32_t digits = GetDigits(value);
    // 次のテクスチャのX座標
    float nextSpritePosX = pos.x;
    int32_t nextValue = value;

    std::vector<DamageData*> newSprites;

    for (int32_t i = 0; i < digits; i++) {

        // 宣言
        DamageData* newDamageData = new DamageData;

        // 画像関連
       
        newDamageData->sprite=new Sprite();
        newDamageData->sprite->Init();
        // 数値のテクスチャをセット
        newDamageData->sprite->SetTexture(mNumberTex[nextValue / GetDigitNumber(digits - i)]);
        // 次項の数値を計算
        nextValue = (int32_t)nextValue % GetDigitNumber(digits-i);
       
        // 座標設定
        newDamageData->sprite->SetPos(Vec2(nextSpritePosX, pos.y));
        nextSpritePosX += 16;

        // サイズ設定
        newDamageData->sprite->SetSpriteSize(Vec2(16.0f, 16.0f));

        // 表示時間の設定
        newDamageData->activeTime = 120;

        newSprites.push_back(newDamageData);
    }

    // 最終的な結果をリストに追加
    mDataList.push_back(newSprites);
}

Vec2 DamageReaction::GetScreenPos(const Vector3 pos, const MainCamera* camera)
{

	// 3Dから2Dへの変換を行う
	const static Matrix4x4 viewPort = MakeViewportMatrix(0, 0,
		camera->GetWindowSize().x, camera->GetWindowSize().y, 0.0f, 1.0f);

	Matrix4x4 V = camera->GetViewMatrix();
	Matrix4x4 P = camera->GetProjectionMatrix();
	Matrix4x4 VPV = Multiply(Multiply(V, P), viewPort);

	// スクリーン座標に変換する
	Vector3 screenPos = Transform(pos, VPV);

	return Vec2(screenPos.x,screenPos.y);
}

int32_t DamageReaction::GetDigits(int32_t num){
    
    // 桁数
    int32_t result = 0;
    // 除算後の数値を保持する変数
    int32_t value = num;

    while (value > 0)
    {
        // 10で割る
        value /= 10;
        // 桁数を加算
        result++;
    }
    return result;
}

int32_t DamageReaction::GetDigitNumber(int32_t digits)
{
    // 除算後の数値を保持する変数
    int32_t result = 1;
    for (int32_t i = 1; i < digits; i++) {
        result *= 10;
    }

    return result;
}



void DamageReaction::RenderText(const std::wstring& text, float x, float y)
{
    text; x; y;
    //// テキストフォーマットの作成
    //ComPtr<IDWriteTextFormat> textFormat;
    //dwriteFactory->CreateTextFormat(
    //    L"Segoe UI",               // フォントファミリー名
    //    nullptr,                   // フォントコレクション（nullの場合はシステムフォントを使用）
    //    DWRITE_FONT_WEIGHT_REGULAR,   // フォントの太さ
    //    DWRITE_FONT_STYLE_NORMAL,      // フォントスタイル
    //    DWRITE_FONT_STRETCH_NORMAL,  // フォントストレッチ
    //    24.0f,                         // フォントサイズ
    //    L"en-us",                     // ロケール
    //    &textFormat);

    //// テキストのレイアウトを計算
    //D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + 300, y + 50);

    //// テキストブラシの作成
    //ComPtr<ID2D1SolidColorBrush> textBrush;
    //d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &textBrush);

    //// テキストの描画
    //d2dContext->BeginDraw();
    //d2dContext->DrawText(
    //    text.c_str(),                            // 描画するテキスト
    //    static_cast<UINT32>(text.length()), // テキストの長さ
    //    textFormat.Get(),                              // テキストフォーマット
    //    &layoutRect,                                   // レイアウトの範囲
    //    textBrush.Get()                  // テキストブラシ
    //);
    //d2dContext->EndDraw();
}