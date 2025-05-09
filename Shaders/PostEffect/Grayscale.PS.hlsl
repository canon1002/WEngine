#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 調整項目
struct AdjustmentItems{
    int32_t enable; // Graysceleの有無
};
ConstantBuffer<AdjustmentItems> gItems : register(b0);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    // 有効時 色を変更する
    if (gItems.enable == 1)
    {
        output.color = gTexture.Sample(gSampler, input.texcoord);
        float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
        output.color.rgb = float32_t3(value, value, value);
    }
    return output;
}