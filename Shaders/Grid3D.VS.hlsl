#include "Grid3D.hlsli"

struct TransformationMatrix {
    float32_t4x4 WVP;
    float32_t4x4 World;
};
ConstantBuffer<TransformationMatrix> gTransfomationMatrix : register(b0);

struct VSInput
{
    float32_t4 position : POSITION0;
    float32_t4 color : COLOR0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = mul(input.position, gTransfomationMatrix.WVP);
    output.color = input.color;
    return output;
}