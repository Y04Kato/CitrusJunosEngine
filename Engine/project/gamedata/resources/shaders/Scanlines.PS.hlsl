#include "CopyImage.hlsli"

ConstantBuffer<ScanlineData> gLineData : register(b0);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    float4 texColor = gTexture.Sample(gSampler, input.texcoord);

    // スキャンラインのY座標を時間に基づいて上下に動かす
    float movingScanlinePosition = input.texcoord.y + sin(gLineData.time) * 0.5;

    // スキャンライン効果の適用
    float scanline = sin(movingScanlinePosition * gLineData.scanlineFrequency) * gLineData.scanlineIntensity;
    texColor.rgb *= 1.0 - scanline;

    output.color = texColor;
    output.color.a = 1.0f;

    return output;
}

