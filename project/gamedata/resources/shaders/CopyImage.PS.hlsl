#include "CopyImage.hlsli"

ConstantBuffer<PostEffectList> gPostEffectList : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

float gauss(float x, float y, float sigma) {
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

float32_t Luminance(float32_t3 v) {
    return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);

    if (gPostEffectList.isGrayScale == true) {
        float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
        output.color.rgb = float32_t3(value, value, value);
    }

    if (gPostEffectList.isVignetting == true) {
        //周囲を0に、中心になるほど明るくなるように計算で調整
        float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
        //correctだけで計算すると中心の最大値が0.0625で暗すぎるのでscaleで調整、今回は16倍して1に
        float32_t vignette = correct.x * correct.y * 16.0f;
        //とりあえず0.8乗でそれっぽく
        vignette = saturate(pow(vignette, 0.8f));
        //係数として乗算
        output.color.rgb *= vignette;
    }

    if (gPostEffectList.isSmoothing == true) {
        uint32_t width, height;//uvStepSizeの算出
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

        output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
        output.color.a = 1.0f;

        for (int32_t x = 0; x < 3; ++x) {
            for (int32_t y = 0; y < 3; ++y) {
                //TexCoordの算出
                float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
                //色に1/9掛けて足す
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                output.color.rgb += fetchColor * kKernel3x3[x][y];
            }
        }
    }

    if (gPostEffectList.isGaussian == true) {
        float32_t weight = 0.0f;
        float32_t kernel3x3[3][3];
        for (int32_t x = 0; x < 3; ++x) {
            for (int32_t y = 0; y < 3; ++y) {
                kernel3x3[x][y] = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, 2.0f);
                weight += kernel3x3[x][y];
            }
        }

        uint32_t width, height;//uvStepSizeの算出
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

        output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
        output.color.a = 1.0f;

        for (int32_t i = 0; i < 3; ++i) {
            for (int32_t j = 0; j < 3; ++j) {
                //TexCoordの算出
                float32_t2 texcoord = input.texcoord + kIndex3x3[i][j] * uvStepSize;
                //色に1/9掛けて足す
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                output.color.rgb += fetchColor * kernel3x3[i][j];
            }
        }

        output.color.rgb *= rcp(weight);
    }

    if (gPostEffectList.isOutline == true) {
                float32_t2 difference = float32_t2(0.0f, 0.0f);//縦横それぞれ畳み込みの結果を格納する

        uint32_t width, height;//uvStepSizeの算出
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

        output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
        output.color.a = 1.0f;

        //色を輝度に変換して畳み込みを行う
        for (int32_t x = 0; x < 3; ++x) {
            for (int32_t y = 0; y < 3; ++y) {
                //TexCoordの算出
                float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
                //色に1/9掛けて足す
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                float32_t luminance = Luminance(fetchColor);

                difference.x += luminance * kPrewittHorizontalKernel[x][y];
                difference.y += luminance * kPrewittVerticalKernel[x][y];
            }
        }

        //変化の長さをウェイトとして合成
        float32_t weight = length(difference);
        weight = saturate(weight * 6.0f);//適当に6倍

        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    }
    
    output.color.a = 1.0f;

    return output;
}