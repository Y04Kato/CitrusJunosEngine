#include "VAT.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<AppData> gAppData : register(b2);

Texture2D<float4> VatPositionTex : register(t0);
Texture2D<float4> VatNormalTex : register(t1);
SamplerState gSampler : register(s0);

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
    
    float32_t vertCoords = gAppData.vertexId;
    float32_t animCoords = 0;
    
    float32_t3 pos1 = VatPositionTex.Load(int3(int(vertCoords), animCoords, 0)).xyz;
    float32_t4 pos2 = VatPositionTex.Sample(gSampler, vertCoords, animCoords);
    
    float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
    output.position = mul(input.position, mul(gTransformationMatrix.matWorld, WorldViewProjection));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.WorldInverseTranspose));
    output.worldPosition = mul(input.position, gTransformationMatrix.matWorld).xyz + pos1;
	return output;
}