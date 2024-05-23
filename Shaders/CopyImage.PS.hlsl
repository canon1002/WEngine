#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

struct FullScereenEffect
{
    int32_t enableScreenColor;  // Graysceleやセピア調など、画面全体の色を変更する
    int32_t enableGrayScele;    // Graysceleの有無
    float32_t4 screenColor;     // 上記の際に使うfloat[4](RGB+A型)
    int32_t enableVignetting;   // ビネット処理の有無(画面端を暗くする)
    float32_t vigneMultipliier; // ビネット処理の際に使用する乗数
    float32_t vigneIndex;       // ビネット処理の際に使用する指数
    int32_t enableSmooting;     // Smooting(ぼかし)の有無 (ぼかしの種類は以下の変数で決める)
    int32_t enableBoxFilter;    // ぼかしの際にBoxFillterを使用するのか
    int32_t enableGaussianFilter;    // ぼかしをガウスぼかしにするのか
    int32_t kernelSize; // カーネルの大きさ
    float32_t GaussianSigma;    // GaussianFilterの際に使う標準偏差
    
};

ConstantBuffer<FullScereenEffect> gEffects: register(b0);

// hlslではπが定義されていないので、自分で定義しておく
static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // エフェクトなしの場合は最終的にこの色になる
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    // -- 以下 ポストエフェクト処理 -- //
    
    // グレースケール
    if (gEffects.enableGrayScele != 0)
    {
        float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
        output.color.rgb = float32_t3(value, value, value);
        output.color.a = output.color.a * gEffects.screenColor.a;

    }
    // スクリーンオーバーレイ? -- 未完成 --
    if (gEffects.enableScreenColor != 0) 
    {
        float32_t3 baseColor = output.color.rgb * float32_t3(255.0f, 255.0f, 255.0f);
        float32_t3 mixColor = gEffects.screenColor.rgb * float32_t3(255.0f, 255.0f, 255.0f);
        if (true)
        {
            output.color.rgb = (baseColor.rgb * mixColor.rgb * 2.0f * rcp(255.0f));
        }
        else
        {
            
        }
        output.color.a = gEffects.screenColor.a;
    }
    
    // カーネルサイズの上限を設定しておく
    #define MAX_KERNEL_SIZE 255
    // ガウスぼかし
    if (gEffects.enableGaussianFilter != 0 && gEffects.kernelSize > 0)
    {
         // カーネルサイズの上限を設定し、超えないようにする
        int kernelSize = min(gEffects.kernelSize, MAX_KERNEL_SIZE);
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
                kernel[x][y] = gauss(index.x, index.y, gEffects.GaussianSigma);
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
    // ぼかし
    if (gEffects.enableBoxFilter != 0 && gEffects.kernelSize > 0)
    {
        // uvStepSizeの算出
        uint32_t width, height;
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
        float32_t kernelValue = rcp(gEffects.kernelSize * gEffects.kernelSize);
        
        // ループを回す
        for (int32_t x = 0; x < gEffects.kernelSize; ++x)
        {
            for (int32_t y = 0; y < gEffects.kernelSize; ++y)
            {
                // 現在のtexcoordを算出
                float32_t2 resultTexcoord = input.texcoord + float32_t2(
                x - (gEffects.kernelSize * rcp(2.0f)), y - (gEffects.kernelSize * rcp(2.0f))) * uvStepSize;
                // 色に1/n掛けて足す
                float32_t3 fetchColor = gTexture.Sample(gSampler, resultTexcoord).rgb;
                output.color.rgb += fetchColor * kernelValue;
            }
        }
    }
    // ビネット
    if (gEffects.enableVignetting != 0)
    {
        // 周囲を0に、中心に近くなるほど明るくなるように計算で調整
        float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
        // correctだけで計算すると中心の最大値が0.0625と暗すぎるのでScale調整
        float vignette = correct.x * correct.y * gEffects.vigneMultipliier;
        // n乗してみる nはgEffectのvignetIndex
        vignette = saturate(pow(vignette, gEffects.vigneIndex));
        // 係数として乗算
        output.color.rgb *= vignette;
    }
        return output;
}