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
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/0.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/1.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/2.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/3.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/4.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/5.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/6.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/7.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/8.png"));
    mNumberTex.push_back(DirectXCommon::GetInstance()->mSrv->LoadTexture("number/9.png"));

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

        // 各画像配列ごとに座標を更新する
        PositionUpdate(sprites);
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
	
    // リストにセットする
    RenderSprite(damage, pos,camera);
}

void DamageReaction::RenderSprite(const int32_t value, Vector3 pos,const MainCamera* camera)
{
    // スクリーン座標に変換
    Vector2 screenPos = GetScreenPos(pos, camera);

    // 数値の桁数を取得
    int32_t digits = GetDigits(value);
    // 次のテクスチャのX座標
    float nextSpritePosX = screenPos.x;
    int32_t nextValue = value;
    // 画像データの配列
    std::vector<DamageData*> newSprites;

    for (int32_t i = 0; i < digits; i++) {

        // 宣言
        DamageData* newDamageData = new DamageData;

        // 画像生成
        newDamageData->sprite=new Sprite();
        newDamageData->sprite->Init();
        // 数値のテクスチャをセット
        newDamageData->sprite->SetTexture(mNumberTex[nextValue / GetDigitNumber(digits - i)]);
        // 次項の数値を計算
        nextValue = (int32_t)nextValue % GetDigitNumber(digits-i);
       
        // 座標設定
        newDamageData->world = new WorldTransform();
        newDamageData->world->translation = pos;
        newDamageData->sprite->SetPos(Vector2(nextSpritePosX, screenPos.y));
        nextSpritePosX += 16;

        // サイズ設定
        newDamageData->sprite->SetSpriteSize(Vector2(16.0f, 16.0f));
        newDamageData->sprite->SetScale(Vector2(1.5f, 1.5f));
        
        // 色の変更
        newDamageData->sprite->SetColor({ 1.0f,0.4f,0.0f,1.0f });

        // 表示時間の設定
        newDamageData->activeTime = 60;

        // 配列に追加
        newSprites.push_back(newDamageData);
    }

    // 最終的な結果をリストに追加
    mDataList.push_back(newSprites);
}

void DamageReaction::PositionUpdate(std::vector<DamageData*> sprites){

    int32_t index = 0;

    for (auto& sprite : sprites) {
        // スクリーン座標に変換
        Vector2 screenPos = GetScreenPos(sprite->world->GetWorldPosition(), MainCamera::GetInstance());

        // 座標設定
        sprite->sprite->SetPos(Vector2(screenPos.x + (index * 16.0f), screenPos.y));
        index++;
    }

}

Vector2 DamageReaction::GetScreenPos(const Vector3 pos, const MainCamera* camera)
{

	// 3Dから2Dへの変換を行う
	const static Matrix4x4 viewPort = MakeViewportMatrix(0, 0,
		camera->GetWindowSize().x, camera->GetWindowSize().y, 0.0f, 1.0f);

	Matrix4x4 V = camera->GetViewMatrix();
	Matrix4x4 P = camera->GetProjectionMatrix();
	Matrix4x4 VPV = Multiply(Multiply(V, P), viewPort);

	// スクリーン座標に変換する
	Vector3 screenPos = Transform(pos, VPV);

	return Vector2(screenPos.x,screenPos.y);
}

Vector3 DamageReaction::GetWorldPosForScreen(const Vector2 pos, const float distance, const MainCamera* camera)
{
    // 2Dから3Dへの変換を行う

    // viewPort計算
    const static Matrix4x4 viewPort = MakeViewportMatrix(0, 0,
        camera->GetWindowSize().x, camera->GetWindowSize().y, 0.0f, 1.0f);
    // VP行列をviewPort行列を合成
    Matrix4x4 VPV = Multiply(Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()), viewPort);
    // VPV行列を逆行列にする
    Matrix4x4 inVPV = Inverse(VPV);

    // 近く
    Vector3 posNear = {pos.x,pos.y, 0.0f };
    posNear = Transform(posNear, inVPV);
    // 遠く
    Vector3 posFar = { pos.x, pos.y, 1.0f };
    posFar = Transform(posFar, inVPV);

    // 方向を計算
    Vector3 direction = Subtract(posFar, posNear);

    // ベクトルの長さを整える
    direction = Normalize(direction);
    direction.x *= distance;
    direction.y *= distance;
    direction.z *= distance;

    // ワールド座標を設定
    Vector3 worldPos = Add(posNear, direction);
    return worldPos;
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