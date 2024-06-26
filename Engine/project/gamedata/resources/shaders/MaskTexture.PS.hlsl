#include "CopyImage.hlsli"

ConstantBuffer<MaskData> gMaskData : register(b2);

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);

SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);

	//maskの値が閾値以下の場合、discardして元のcolorを与える
	if (mask <= gMaskData.maskThreshold) {
		output.color = gTexture.Sample(gSampler, input.texcoord);
	}
	else {
		output.color.rgb = gMaskData.maskColor;
		//Edgeっぽさを算出
		float32_t edge = 1.0f - smoothstep(gMaskData.maskThreshold, gMaskData.maskThreshold + 0.03f, mask);
		output.color.rgb += edge * gMaskData.edgeColor;
	}

	output.color.a = 1.0f;

	return output;
}