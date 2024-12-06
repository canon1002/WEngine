#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// 調整項目
struct AdjustmentItems {
    int32_t enable; // BoxFillterの有無
    int32_t kernelSize; // カーネルの大きさ
};
ConstantBuffer<AdjustmentItems> gItems : register(b0);

struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input){ 
    
    // 戻り値
    PixelShaderOutput output;
    // エフェクトなしの場合は最終的にこの色になる
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    if (gItems.enable != 0 && gItems.kernelSize > 0)
    {
        // uvStepSizeの算出
        uint32_t width, height;
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
        float32_t weight = 0.0f;
        
        float32_t kernelValue = rcp(gItems.kernelSize * gItems.kernelSize);
        
        // ループを回す
        for (int32_t x = 0; x < gItems.kernelSize; ++x)
        {
            for (int32_t y = 0; y < gItems.kernelSize; ++y)
            {
                // 現在のtexcoordを算出
                float32_t2 resultTexcoord = input.texcoord + float32_t2(
                x - (gItems.kernelSize * rcp(2.0f)), y - (gItems.kernelSize * rcp(2.0f))) * uvStepSize;
                // 色に1/n掛けて足す
                float32_t3 fetchColor = gTexture.Sample(gSampler, resultTexcoord).rgb;
                output.color.rgb += fetchColor * kernelValue;
            }
        }
    }
    
    //output.color.rgb = f(input.texcoord);
    //output.color.a = 1.0f;
  
   return output;
}