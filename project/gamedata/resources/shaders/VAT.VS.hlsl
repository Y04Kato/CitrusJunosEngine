#include "VAT.hlsli"
#include "ShaderStructures.h"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

Texture2D<float4> VatPositionTex : register(t0);
Texture2D<float4> VatNormalTex : register(t1);

inline float CalcVatAnimationTime(float time) {
	return fmod(time, VATData.VatAnimLength) * VATData.VatAnimFps;
}

inline float4 CalcVatTexCoord(uint vertexId, float animationTime) {
	float x = vertexId + 0.5f;
	float y = animationTime + 0.5f;

	return float4(x, y, 0.0f, 0.0f) * VATData.VatPositionTexTexelSize;
}

inline float3 GetVatPosition(uint vertexId, float animationTime) {
	return tex2Dlod(VatPositionTex, CalcVatTexCoord(vertexId, animationTime));
}

inline float3 GetVatNormal(uint vertexId, float animationTime) {
	return tex2Dlod(VatNormalTex, CalcVatTexCoord(vertexId, animationTime));
}

VertexShaderOutput main(VertexShaderInput input, uint vertexId) {
	VertexShaderOutput output;

	float animTime = CalcVatAnimationTime(Time.y + VATData.AnimationTimeOffset);
	float3 pos = GetVatPosition(vertexId, animTime);

	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);

	output.position = mul(input.position, mul(gTransformationMatrix.matWorld, mul(float4(pos, 1.0f), WorldViewProjection)));
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));
	output.worldPosition = mul(input.position, gTransformationMatrix.matWorld).xyz;
	return output;
}

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<AppData> gAppData : register(b2);

Texture2D<float32_t4> gVatPositionTex : register(t0);
Texture2D<float32_t4> gVatNormalTex : register(t1);

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;

	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
	output.position = mul(input.position, mul(gTransformationMatrix.matWorld, WorldViewProjection));
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));
	output.worldPosition = mul(input.position, gTransformationMatrix.matWorld).xyz;
	
	float3 w_pos = mul(In.Pos, g_WXf).xyz;
	float3 vat_pos = g_VATexture.Load(int3(int(In.TextureUV.x), g_Location, 0)).xyz;
	w_pos = w_pos + vat_pos;
	Out.HPos = mul(mul(float4(w_pos, 1.0), g_VXf), g_PXf);
	Out.VATColor = vat_pos;
	
	return output;
}