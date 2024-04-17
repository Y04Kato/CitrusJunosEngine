#include "VAT.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<VATData> gVATData : register(b2);

Texture2D<float4> VatPositionTex : register(t0);
Texture2D<float4> VatNormalTex : register(t1);
SamplerState gSampler : register(s0);

VertexShaderOutput main(VertexShaderInput input, uint32_t index : SV_VertexID) {
	VertexShaderOutput output;

    float32_t vertCoords = index * gVATData.VatPositionTexSize.x;
	float32_t animCoords = gVATData.VATTime * gVATData.VatPositionTexSize.y;
	float32_t2 texCoords = { vertCoords, animCoords };

	float32_t4 pos = VatPositionTex.SampleLevel(gSampler, texCoords, 0);

	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
	float32_t4 pos2 = input.position + pos;
	output.position = mul(pos2, mul(gTransformationMatrix.matWorld, WorldViewProjection));
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));
	output.worldPosition = mul(input.position, gTransformationMatrix.matWorld).xyz;
	return output;
}