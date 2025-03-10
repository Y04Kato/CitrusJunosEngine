#include "Object3d.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;

    // ワールド座標に変換
    float4 worldPos = mul(input.position, gTransformationMatrix.matWorld);
    output.worldPosition = worldPos.xyz;

    // ビュー・プロジェクション変換
    float4 viewPos = mul(worldPos, gViewProjectionMatrix.view);
    output.position = mul(viewPos, gViewProjectionMatrix.projection);

    // 法線の変換
    output.normal = normalize(mul(input.normal, (float3x3)gTransformationMatrix.WorldInverseTranspose));

    // テクスチャ座標
    output.texcoord = input.texcoord;

    return output;
}
