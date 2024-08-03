#include "Object3d.hlsli"
//#include "ColorConvert.hlsli"


// HSVからRGBへの変換を行う
float32_t3 HSVToRGB(float32_t3 hsv)
{
    // 最大値の取得
    float maxValue = hsv.z;
    // 最低値の取得
    float minValue = maxValue - ((hsv.y * rcp(255.0f)) * maxValue);
    
    float32_t3 rgb;
    
    // RGB値を求める
    if (hsv.x <= 60)
    {
        rgb.x = maxValue;
        rgb.y = (hsv.x * rcp(60.0f)) * (maxValue - minValue) + minValue;
        rgb.z = minValue;
    }
    else if (hsv.x <= 120)
    {
        rgb.x = ((120 - hsv.x) * rcp(60.0f)) * (maxValue - minValue) + minValue;
        rgb.y = maxValue;
        rgb.z = minValue;
    }
    else if (hsv.x <= 180)
    {
        rgb.x = minValue;
        rgb.y = maxValue;
        rgb.z = ((hsv.x - 120) * rcp(60.0f)) * (maxValue - minValue) + minValue;
    }
    else if (hsv.x <= 240)
    {
        rgb.x = minValue;
        rgb.y = ((240 - hsv.x) * rcp(60.0f)) * (maxValue * minValue) + minValue;
        rgb.z = maxValue;
    }
    else if (hsv.x <= 300)
    {
        rgb.x = ((hsv.x - 240) * rcp(60.0f)) * (maxValue * minValue) + minValue;
        rgb.y = minValue;
        rgb.z = maxValue;
    }
    else if (hsv.x <= 360)
    {
        rgb.x = maxValue;
        rgb.y = minValue;
        rgb.z = ((360 - hsv.x) * rcp(60.0f)) * (maxValue * minValue) + minValue;
    }
    
    // 求めた値を返す
    return rgb;
    
}

// RGBからHSVへの変換を行う
float32_t3 RGBToHSV(float32_t3 rgb)
{
    // 最低値の取得
    float minValue = min(rgb.x, min(rgb.y, rgb.z));
    // 最大値の取得
    float maxValue = max(rgb.x, max(rgb.y, rgb.z));
    
    // 色相の値を計算する
    float hue = 0;
    // 彩度を求める
    float saturation = (maxValue - minValue) * rcp(maxValue);
    // 明度を求める
    float value = maxValue;
    
    // RGBいずれかの最も大きな値に応じた計算を行う
    if (rgb.x == rgb.y == rgb.z) // RGBの値がすべて同じ値だった場合
    {
        // 色相の値を計算する
        hue = 0;
         // 彩度を求める
        saturation = (maxValue - minValue) * rcp(maxValue);
        // 明度を求める
        value = maxValue;
        
    }
    else if (maxValue == rgb.x) // Rの値が最大値だった場合
    {
        // 色相を求める
        hue = 60 * ((rgb.y - rgb.z) * rcp(maxValue - minValue));
        // 色相の値がマイナスの場合、360を加算し、0~360の値にする
        if (hue < 0)
        {
            hue += 360;
        }
        // 彩度を求める
        saturation = (maxValue - minValue) * rcp(maxValue);
        // 明度を求める
        value = maxValue;
    }
    else if (maxValue == rgb.y) // Gの値が最大値だった場合
    {
        // 色相を求める
        hue = 60 * ((rgb.z - rgb.x) * rcp(maxValue - minValue)) + 120.0f;
        // 色相の値がマイナスの場合、360を加算し、0~360の値にする
        if (hue < 0)
        {
            hue += 360;
        }
        // 彩度を求める
        saturation = (maxValue - minValue) * rcp(maxValue);
        // 明度を求める
        value = maxValue;
    }
    else if (maxValue == rgb.z) // Bの値が最大値だった場合
    {
        // 色相を求める
        hue = 60 * ((rgb.x - rgb.y) * rcp(maxValue - minValue));
        // 色相の値がマイナスの場合、360を加算し、0~360の値にする
        if (hue < 0)
        {
            hue += 360;
        }
        // 彩度を求める
        saturation = (maxValue - minValue) * rcp(maxValue);
        // 明度を求める
        value = maxValue;
    }

    // 求めた値を返す
    float32_t3 output;
    output.x = hue;
    output.y = saturation;
    output.z = value;
    
    return output;
    
}

float32_t WrapValue(float32_t value, float32_t minRange, float32_t maxRange){
    float32_t range = maxRange - minRange;
    float32_t modValue = fmod(value - minRange, minRange);
    if (modValue < 0)
    {
        modValue += range;
    }
    return minRange + modValue;
}

// マテリアル
struct Material{
    float32_t4 color;
    float32_t4x4 uvTransform;
    int32_t enableLighting;
    float32_t shininess;
    float32_t environmentCoefficient;
};
ConstantBuffer<Material> gMaterial : register(b0);

// 平行光源
struct DirectionalLight{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

// カメラ
struct Camera {
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

// テクスチャ
Texture2D<float32_t4> gTexture : register(t0);
// 環境マップテクスチャ
TextureCube<float32_t4> gEnvironmentTexture : register(t1);

// サンプラー
SamplerState gSampler : register(s0);


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    
    // PixelShaderでTextureの宣言を行う
    PixelShaderOutput output;    
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    //RGB - > HSV変換を行う
    float32_t3 hsv = RGBToHSV(textureColor.rgb);
    
    //HSV - > RGB変換を行う
    float32_t3 rgb = HSVToRGB(hsv);
    
    // Samplingしたtextureの色とmaterialの色を乗算して合成する
    output.color.rgb = rgb;
    output.color.a = textureColor.a;
    
    if (gMaterial.enableLighting != 0)// Lightingする場合
    {
        // 鏡面反射
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        float RdotE = dot(reflectLight, toEye);
    
        // Blinn Phong Reflection
        float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float NDoH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NDoH), gMaterial.shininess);
        
        // half lambert
        // Ndotlは法線とライト方向(ライトへの向き)の内積
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
       
        // 拡散反射
        float32_t3 diffuse =
        gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
        // 鏡面反射
        float32_t3 specular =
        gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * 
        float32_t3(1.0f, 1.0f, 1.0f); // 最後のfloat32_t3は物体の鏡面反射色
        
        // 拡散反射+鏡面反射
        output.color.rgb = diffuse + specular;
        // アルファは今まで通り
        output.color.a = gMaterial.color.a * textureColor.a;
        
        // 環境マッピングを行う
        float32_t3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
        float32_t3 refrectedVector = reflect(cameraToPosition, normalize(input.normal));
        float32_t4 enviromentColor = gEnvironmentTexture.Sample(gSampler, refrectedVector);
        // 計算結果を適用する
        output.color.rgb += enviromentColor.rgb * gMaterial.environmentCoefficient;
    }
    else// Lightingしない場合
    {
        // Samplingしたtextureの色とmaterialの色を乗算して合成する
        output.color = gMaterial.color * textureColor;
    }
    
    return output;
}