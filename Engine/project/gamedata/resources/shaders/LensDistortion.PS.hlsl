#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;

    // テクスチャ座標を中心からのオフセットに変換
    float2 uv = input.texcoord * 2.0f - 1.0f;

    // バレルディストーション係数
    float k1 = 0.15;  // 歪みの強さ

    // 半径（中心からの距離）を計算
    float r = length(uv);

    // 新しいテクスチャ座標を計算
    float2 distortedUV = uv * (1.0 + k1 * r * r);

    // 元のテクスチャ座標範囲に戻す
    distortedUV = (distortedUV + 1.0f) / 2.0f;

    // テクスチャをサンプル
    output.color = gTexture.Sample(gSampler, distortedUV);

    // アルファ値を1.0に設定
    output.color.a = 1.0f;

    return output;
}
