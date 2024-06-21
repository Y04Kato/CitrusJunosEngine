#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
//Texture2D<float32_t4> gDepthTexture : register(t1);

SamplerState gSampler : register(s0);
//SamplerState gSamplerPoint : register(s1);

static const float32_t2 kIndex3x3[3][3] = {
	{{-1.0f,-1.0f},{0.0f,-1.0f},{1.0f,-1.0f}},
	{{-1.0f,0.0f},{0.0f,0.0f},{1.0f,0.0f}},
	{{-1.0f,1.0f},{0.0f,1.0f},{1.0f,1.0f}},
};

static const float32_t kPrewittHorizontalKernel[3][3] = {
	{-1.0f / 6.0f,0.0f,1.0f / 6.0f},
	{-1.0f / 6.0f,0.0f,1.0f / 6.0f},
	{-1.0f / 6.0f,0.0f,1.0f / 6.0f},
};

static const float32_t kPrewittVerticalKernel[3][3] = {
	{-1.0f / 6.0f,-1.0f / 6.0f,-1.0f / 6.0f},
	{0.0f,0.0f,0.0f},
	{1.0f / 6.0f,1.0f / 6.0f,1.0f / 6.0f},
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

float32_t Luminance(float32_t3 v) {
	return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

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
			//色に1/9掛けて足す(輝度ベース)
			float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
			float32_t luminance = Luminance(fetchColor);
			
			difference.x += luminance * kPrewittHorizontalKernel[x][y];
			difference.y += luminance * kPrewittVerticalKernel[x][y];

			//深度ベース
			//float32_t ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
			//NDC->View P^{-1}においてxとyはzwに影響を与えないので何でも良い
			//gMaterial.projectionInverseはCBufferを使って渡す
			//float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
			//float32_t viewZ = viewSpace.z * rcp(viewSpace.w);//同次座標系からデカルト座標系へ
		}
	}

	//変化の長さをウェイトとして合成
	float32_t weight = length(difference);
	weight = saturate(weight * 6.0f);//適当に6倍(輝度ベース)
	//weight = saturate(weight);//特に倍率なし(深度ベース)

	output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;

	return output;
}