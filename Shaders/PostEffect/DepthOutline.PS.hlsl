#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 調整項目
struct AdjustmentItems
{
    int32_t enable; // ビネットの有無
    float32_t multipliier; // ビネット処理の際に使用する乗数
    float32_t index; // ビネット処理の際に使用する指数
};
ConstantBuffer<AdjustmentItems> gItems : register(b0);


// Depth読み込み用
Texture2D<float32_t> gDepthTexture : register(t1);
// PointSampling用
SamplerState gSamplerPoint : register(s1);


struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    // 周囲を0に、中心に近くなるほど明るくなるように計算で調整
    float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    // correctだけで計算すると中心の最大値が0.0625と暗すぎるのでScale調整
    float vignette = correct.x * correct.y * gItems.multipliier;
    // n乗してみる
    vignette = saturate(pow(vignette, gItems.index));
    // 係数として乗算
    output.color.rgb *= vignette;
    return output;
}