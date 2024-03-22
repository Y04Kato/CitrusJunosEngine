#include "VAT.hlsli"
#include "ShaderStructures.h"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<VATData> vatData : register(b2)

Texture2D<float4> VatPositionTex : register(t1);
Texture2D<float4> VatNormalTex : register(t2);

inline float CalcVatAnimationTime(float time){
    return fmod(time, VatAnimLength) * VatAnimFps;
}

inline float4 CalcVatTexCoord(uint vertexId, float animationTime){
    float x = vertexId + 0.5f;
    float y = animationTime + 0.5f;

    return float4(x, y, 0.0f, 0.0f);
}

inline float3 GetVatPosition(uint vertexId, float animationTime){
    return tex2Dlod(VatPositionTex, CalcVatTexCoord(vertexId, animationTime));
}

inline float3 GetVatNormal(uint vertexId, float animationTime){
    return tex2Dlod(VatNormalTex, CalcVatTexCoord(vertexId, animationTime));
}

VertexShaderOutput main(VertexShaderInput input, uint vertexId) {
	VertexShaderOutput output;

    float animTime = CalcVatAnimationTime(Time.y + AnimationTimeOffset);
    float3 pos = GetVatPosition(vertexId, animTime);
	
	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);

    output.position = mul(input.position, mul(gTransformationMatrix.matWorld, mul(float4(pos, 1.0f),WorldViewProjection)));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));
    output.worldPosition = mul(input.position, gTransformationMatrix.matWorld).xyz;
	return output;
}