#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

static const float32_t PI = 3.14159265f;

static const float32_t2 kIndex3x3[3][3] = {
	{{-1.0f,-1.0f},{0.0f,-1.0f},{1.0f,-1.0f}},
	{{-1.0f,0.0f},{0.0f,0.0f},{1.0f,0.0f}},
	{{-1.0f,1.0f},{0.0f,1.0f},{1.0f,1.0f}},
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

float gauss(float x, float y, float sigma) {
	float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
	float denominator = 2.0f * PI * sigma * sigma;
	return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

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

	return output;
}