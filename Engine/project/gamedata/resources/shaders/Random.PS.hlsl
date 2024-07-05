#include "CopyImage.hlsli"

ConstantBuffer<RandomData> gRandomData : register(b1);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233)) {
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);

    float32_t random = rand2dTo1d(input.texcoord * gRandomData.time);

    output.color = output.color * float32_t4(random, random, random, 1.0f);

    output.color.a = 1.0f;

    return output;
}