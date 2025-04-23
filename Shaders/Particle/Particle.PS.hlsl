#include "Particle.hlsli"

struct Material{
    float32_t4 color;
    float32_t4x4 uvTransform;
    int32_t enableLighting;
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
    PixelShaderOutput output;
    
    // PixelShaderでTextureの宣言を行う
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
  
    if (textureColor.a == 0.0f)
    {
        discard;
    }
    
    // Samplingしたtextureの色とmaterialの色を乗算して合成する
    output.color.rgb = gMaterial.color.rgb * textureColor.rgb * input.color.rgb;
    output.color.a = input.color.a * textureColor.a;
    
    // 最終的な色が透明な場合は描画しない
    if (output.color.a == 0.0f)
    {
        discard;
    }
    
    return output;
}