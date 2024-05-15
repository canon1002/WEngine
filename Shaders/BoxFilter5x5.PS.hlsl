#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

static const float32_t2 kIndex5x5[5][5] =
{
    { { -2.0f, -2.0f }, { -1.0f, -2.0f }, { 0.0f, -2.0f }, { 1.0f, -2.0f },{ 2.0f, -2.0f }, }, 
    { { -2.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f },{ 2.0f, -1.0f }, },
    { { -2.0f,  0.0f }, { -1.0f,  0.0f }, { 0.0f,  0.0f }, { 1.0f,  0.0f },{ 2.0f,  0.0f }, },
    { { -2.0f,  1.0f }, { -1.0f,  1.0f }, { 0.0f,  1.0f }, { 1.0f,  1.0f },{ 2.0f,  1.0f }, },
    { { -2.0f,  2.0f }, { -1.0f,  2.0f }, { 0.0f,  2.0f }, { 1.0f,  2.0f },{ 2.0f,  2.0f }, },

};

static const float32_t kKernel5x5[5][5] =
{
    { { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f} },
    { { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f} },
    { { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f} },
    { { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f} },
    { { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f} },
};


float32_t3 f(float32_t2 texcoord)
{
    float32_t3 result = float32_t3(0.0f, 0.0f, 0.0f);
    
     // 1.uvStepSizeの算出
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
   // 2.3x3のループを回す
    for (int32_t x = 0; x < 5; ++x)
    {
        for (int32_t y = 0; y < 5; ++y)
        {
            // 3.現在のtexcoordを算出
            float32_t2 resultTexcoord = texcoord + kIndex5x5[x][y] * uvStepSize;
            // 4.色に1/9掛けて足す
            float32_t3 fetchColor = gTexture.Sample(gSampler, resultTexcoord).rgb;
            result += fetchColor * kKernel5x5[x][y];
        }
    }
    return result;

}

PixelShaderOutput main(VertexShaderOutput input){
  
    PixelShaderOutput output;
    output.color.rgb = f(input.texcoord);
    output.color.a = 1.0f;
    
   return output;
}