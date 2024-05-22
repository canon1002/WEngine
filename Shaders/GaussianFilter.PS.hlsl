#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

static const float32_t2 kIndex9x9[9][9] =
{
    { { -4.0f, -4.0f }, { -3.0f, -4.0f }, { -2.0f, -4.0f }, { -1.0f, -4.0f }, { 0.0f, -4.0f }, { 1.0f, -4.0f }, { 2.0f, -4.0f }, { 3.0f, -4.0f }, { 4.0f, -4.0f }, },
    { { -4.0f, -3.0f }, { -3.0f, -3.0f }, { -2.0f, -3.0f }, { -1.0f, -3.0f }, { 0.0f, -3.0f }, { 1.0f, -3.0f }, { 2.0f, -3.0f }, { 3.0f, -3.0f }, { 4.0f, -3.0f }, },
    { { -4.0f, -2.0f }, { -3.0f, -2.0f }, { -2.0f, -2.0f }, { -1.0f, -2.0f }, { 0.0f, -2.0f }, { 1.0f, -2.0f }, { 2.0f, -2.0f }, { 3.0f, -2.0f }, { 4.0f, -2.0f }, },
    { { -4.0f, -1.0f }, { -3.0f, -1.0f }, { -2.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, { 2.0f, -1.0f }, { 3.0f, -1.0f }, { 4.0f, -1.0f }, },
    { { -4.0f,  0.0f }, { -3.0f,  0.0f }, { -2.0f,  0.0f }, { -1.0f,  0.0f }, { 0.0f,  0.0f }, { 1.0f,  0.0f }, { 2.0f,  0.0f }, { 3.0f,  0.0f }, { 4.0f,  0.0f }, },
    { { -4.0f,  1.0f }, { -3.0f,  1.0f }, { -2.0f,  1.0f }, { -1.0f,  1.0f }, { 0.0f,  1.0f }, { 1.0f,  1.0f }, { 2.0f,  1.0f }, { 3.0f,  1.0f }, { 4.0f,  1.0f }, },
    { { -4.0f,  2.0f }, { -3.0f,  2.0f }, { -2.0f,  2.0f }, { -1.0f,  2.0f }, { 0.0f,  2.0f }, { 1.0f,  2.0f }, { 2.0f,  2.0f }, { 3.0f,  2.0f }, { 4.0f,  2.0f }, },
    { { -4.0f,  3.0f }, { -3.0f,  3.0f }, { -2.0f,  3.0f }, { -1.0f,  3.0f }, { 0.0f,  3.0f }, { 1.0f,  3.0f }, { 2.0f,  3.0f }, { 3.0f,  3.0f }, { 4.0f,  3.0f }, },
    { { -4.0f,  4.0f }, { -3.0f,  4.0f }, { -2.0f,  4.0f }, { -1.0f,  4.0f }, { 0.0f,  4.0f }, { 1.0f,  4.0f }, { 2.0f,  4.0f }, { 3.0f,  4.0f }, { 4.0f,  4.0f }, },
};

static const float32_t kKernel9x9[9][9] =
{
    { { 1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f, } },
    { { 1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f, } },
    { { 1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f, } },
    { { 1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f, } },
    { { 1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f, } },
    { { 1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f, } },
    { { 1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f, } },
    { { 1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f, } },
    { { 1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f,1.0f / 81.0f, } }, 
};
// hlslではπが定義されていないので、自分で定義しておく
static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma){
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

float32_t3 f(float32_t2 texcoord)
{
   float32_t3 result = float32_t3(0.0f, 0.0f, 0.0f);
   
    // Kernelを求める
    float32_t weight = 0.0f;
    float32_t kernel9x9[9][9];
    
     // uvStepSizeの算出
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
   // 9x9のループを回す
    for (int32_t x = 0; x < 9; ++x)
    {
        for (int32_t y = 0; y < 9; ++y)
        {
            kernel9x9[x][y] = gauss(kIndex9x9[x][y].x, kIndex9x9[x][y].y, 2.0f);
            weight += kernel9x9[x][y];
            
             // 3.現在のtexcoordを算出
            float32_t2 resultTexcoord = texcoord + kIndex9x9[x][y] * uvStepSize;
            // 4.色に1/9掛けて足す
            float32_t3 fetchColor = gTexture.Sample(gSampler, resultTexcoord).rgb;
            result += fetchColor * kernel9x9[x][y];
        }
    }
     
    result *= rcp(weight);
    return result;

}

PixelShaderOutput main(VertexShaderOutput input){
  
    PixelShaderOutput output;
    output.color.rgb = f(input.texcoord);
    output.color.a = 1.0f;
    
   return output;
}