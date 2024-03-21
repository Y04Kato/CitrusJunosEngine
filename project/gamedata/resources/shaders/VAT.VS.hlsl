#include "VAT.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

uniform int g_Location;
Texture2D g_VATexture;

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	
	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);

	float3 vatPos = g_VATexture.Load(int3(int(input.normal.x), g_Location, 0)).xyz;

    output.position = mul(input.position, mul(gTransformationMatrix.matWorld, (WorldViewProjection + vatPos)));
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));
	output.worldPosition = mul(input.position, gTransformationMatrix.matWorld).xyz;
    output.VATColor = vatPos;
	return output;
}