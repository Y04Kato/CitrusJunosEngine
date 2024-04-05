#include "VAT.hlsli"
#include "ShaderStructures.h"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<AppData> gAppData : register(b2);

Texture2D<float4> VatPositionTex : register(t0);
Texture2D<float4> VatNormalTex : register(t1);

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
	output.position = mul(input.position, mul(gTransformationMatrix.matWorld, WorldViewProjection));
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));
	output.worldPosition = mul(input.position, gTransformationMatrix.matWorld).xyz;
	return output;
}