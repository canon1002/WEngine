#include "Collider3d.hlsli"

float32_t4 main(VSOutput input) : SV_TARGET
{
    // VSで入力された色をそのまま返す
    VSOutput output = input;
    output.color.r = 0.8f;
    output.color.a = 1.0f;
    return output.color;
}