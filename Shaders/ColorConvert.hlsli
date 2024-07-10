
// HSVからRGBへの変換を行う
float32_t3 HSVToRGB(float32_t3 hsv)
{
    // 最大値の取得
    float maxValue = hsv.z;
    // 最低値の取得
    float minValue = maxValue - ((hsv.y * rcp(255.0f)) * maxValue);
    
    float32_t3 rgb;
    
    // RGB値を求める
    if (hsv.x <= 60){
        rgb.x = maxValue;
        rgb.y = (hsv.x * rcp(60.0f)) * (maxValue - minValue) + minValue;
        rgb.z = minValue;
    }
    else if (hsv.x <= 120)
    {
        rgb.x = ((120-hsv.x) * rcp(60.0f)) * (maxValue - minValue) + minValue;
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
        if (hue < 0){ hue += 360; }
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
        if (hue < 0){ hue += 360; }
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
        if (hue < 0){ hue += 360; }
        // 彩度を求める
        saturation = (maxValue - minValue) * rcp(maxValue);
        // 明度を求める
        value = maxValue;
    }

    // 求めた値を返す
    return float32_t3(hue, saturation, value);
    
}
