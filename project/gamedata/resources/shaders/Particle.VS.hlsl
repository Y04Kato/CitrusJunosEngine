#include"Particle.hlsli"

StructuredBuffer<TransformationMatrix> gTransformationMatrices : register(t0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

VertexShaderOutput main(VertexShaderInput input,uint32_t instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
	output.position = mul(input.position, mul(gTransformationMatrices[instanceId].matWorld, WorldViewProjection));
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float32_t3x3)WorldViewProjection));

	return output;
}
