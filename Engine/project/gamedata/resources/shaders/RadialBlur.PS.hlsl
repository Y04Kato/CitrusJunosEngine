#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    //中心点、ここを基準にブラーがかかる
    const float32_t2 kCenter = float32_t2(0.5f, 0.5f);
    //サンプリング数、多いほど重くなるので注意
    const int32_t kNumSamples = 10;
    //ぼかしの幅、大きいほど大きくなる
    const float32_t kBlurWidth = 0.01f;

    //中心から現在のUVに対しての方向を計算
    //普段方向は単位ベクトルだが、今回は敢えて正規化せずに遠いほど遠くをサンプリングする
    float32_t2 direction = input.texcoord - kCenter;
    float32_t3 outputColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (int32_t sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex) {
        //現在のUVから先程計算した方向にサンプリング点を進めながらサンプリングする
        float32_t2 texcoord = input.texcoord + direction * kBlurWidth * float32_t(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }

    //平均化する
    outputColor.rgb *= rcp(kNumSamples);

    output.color.rgb = outputColor.rgb;

    output.color.a = 1.0f;

    return output;
}