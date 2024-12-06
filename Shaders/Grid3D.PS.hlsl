#include "Grid3D.hlsli"

float32_t4 main(VSOutput input) : SV_TARGET
{
    // VSで入力された色をそのまま返す
    return input.color;
}