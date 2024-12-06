#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 調整項目
struct AdjustmentItems {
    int32_t enable; // Fillterの有無
    int32_t kernelSize; // カーネルの大きさ
    float sigma; // 標準偏差
};
ConstantBuffer<AdjustmentItems> gItems : register(b0);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

// hlslではπが定義されていないので、自分で定義しておく
static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma){
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input){
  
    // 戻り値
    PixelShaderOutput output;
    // エフェクトなしの場合は最終的にこの色になる
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    if (gItems.enable != 0 && gItems.kernelSize > 0)
    {
        // カーネルサイズの上限を設定し、超えないようにする
        #define MAX_KERNEL_SIZE 255
        int kernelSize = min(gItems.kernelSize, MAX_KERNEL_SIZE);
        float32_t kernel[MAX_KERNEL_SIZE][MAX_KERNEL_SIZE];
        float32_t weight = 0.0f;

        uint32_t width, height;
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

        // カーネルを生成し、その重みを計算
        for (int32_t x = 0; x < kernelSize; ++x)
        {
            for (int32_t y = 0; y < kernelSize; ++y)
            {
                float32_t2 index = float32_t2(x - (kernelSize * 0.5f), y - (kernelSize * 0.5f));
                kernel[x][y] = gauss(index.x, index.y, gItems.sigma);
                weight += kernel[x][y];
            }
        }

        float32_t3 blurredColor = float32_t3(0.0f, 0.0f, 0.0f);

        // カーネルを使い、入力テクスチャの色をぼかす
        for (int32_t i = 0; i < kernelSize; ++i)
        {
            for (int32_t j = 0; j < kernelSize; ++j)
            {
                float32_t2 offset = float32_t2(i - (kernelSize * 0.5f), j - (kernelSize * 0.5f)) * uvStepSize;
                float32_t3 fetchColor = gTexture.Sample(gSampler, input.texcoord + offset).rgb;
                blurredColor += fetchColor * kernel[i][j];
            }
        }

        // ぼかしカラーを正規化
        output.color.rgb = blurredColor * rcp(weight);
    }
    
   return output;
}