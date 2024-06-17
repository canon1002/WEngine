#include "Object3d.hlsli"

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