#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 調整項目
struct AdjustmentItems {
    int32_t enable; // アウトラインの有無
    float32_t multipliier; // アウトライン生成時の差を大きくするための数値  
};
ConstantBuffer<AdjustmentItems> gItems : register(b0);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

// アウトライン用に縦横それぞれの畳み込み用の配列を作成しておく
static const float32_t kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};
static const float32_t kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

// RGBを輝度に変換する
float32_t Luminance(float32_t3 v)
{
    return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
     // 輝度で検出したアウトラインの有無
    if (gItems.enable)
    {
        // uvStepSizeの算出
        uint32_t width, height;
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
        
        // 縦横それぞれの畳み込みの結果を格納する
        float32_t2 difference = float32_t2(0.0f, 0.0f);

        // 重み
        float32_t weight = 0.0f;
        
        // 色を輝度に変換して、畳み込みを行っていく。微分Filter用のKernelになっているので、
        // やること自体は今までの畳み込みと同じ
        
        // ループを回す
        for (int32_t x = 0; x < 3; ++x)
        {
            for (int32_t y = 0; y < 3; ++y)
            {
                // 現在のtexcoordを算出
                float32_t2 resultTexcoord = input.texcoord + float32_t2(x - (3 * rcp(2.0f)), y - (3 * rcp(2.0f))) * uvStepSize;
                float32_t3 fetchColor = gTexture.Sample(gSampler, resultTexcoord).rgb;
                float32_t luminance = Luminance(fetchColor);
                difference.x += luminance * kPrewittHorizontalKernel[x][y];
                difference.y += luminance * kPrewittVerticalKernel[x][y];
                
            }
        }
        
        // 変化の長さをウェイトとして合成 // CBufferで持ってきた数値を掛けて差を大きくする
        weight = saturate(length(difference) * gItems.multipliier);
        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
        output.color.a = 1.0f;
    }
    
    return output;
}