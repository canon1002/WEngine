#include "Object3d.hlsli"

struct Material{
    float32_t4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    // PixelShaderでTextureの宣言を行う
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    PixelShaderOutput output;
    // Samplingしたtextureの色とmaterialの色を乗算して合成する
    output.color = gMaterial.color * textureColor;
    return output;
}