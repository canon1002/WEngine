#include "Particle.hlsli"

struct Material{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
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
   
    if (textureColor.a <= 0.5f)
    {
        discard;
    }
    // Samplingしたtextureの色とmaterialの色を乗算して合成する
    output.color = input.color * textureColor;
    
    return output;
}