#include "Object3d.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
	output.position = mul(input.position, mul(gTransformationMatrix.matWorld, WorldViewProjection));
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float32_t3x3)WorldViewProjection));
	return output;
}