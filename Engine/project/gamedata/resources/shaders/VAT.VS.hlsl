#include "VAT.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<VATData> gVATData : register(b2);

Texture2D<float4> VatPositionTex : register(t0);
Texture2D<float4> VatRotTex : register(t1);
SamplerState gSampler : register(s0);

VertexShaderOutput main(VertexShaderInput input, uint index : SV_VertexID) {
    VertexShaderOutput output;

    // テクスチャ座標の計算
    float32_t vertCoords = (index % int32_t(gVATData.VatPositionTexSize.z)) * gVATData.VatPositionTexSize.x;
    float32_t animCoords = gVATData.VATTime * gVATData.VatPositionTexSize.y;
    float32_t2 texCoords = float2(vertCoords, animCoords);

    // VATテクスチャから位置データをサンプリング
    float32_t4 sampledPosition = VatPositionTex.SampleLevel(gSampler, texCoords, 0);
    float32_t4 animatedPosition = input.position + sampledPosition;

    // ワールド座標への変換
    float32_t4 worldPosition = mul(animatedPosition, gTransformationMatrix.matWorld);

    // ビュー・プロジェクションマトリックスを適用
    float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
    output.position = mul(worldPosition, WorldViewProjection);

    // テクスチャ座標
    output.texcoord = input.texcoord;

    // 法線データのサンプリングと変換
    float32_t4 sampledNormal = VatRotTex.SampleLevel(gSampler, texCoords, 0);
    float32_t3 animatedNormal = normalize(input.normal + sampledNormal.xyz);
    output.normal = normalize(mul(animatedNormal, (float3x3)gTransformationMatrix.WorldInverseTranspose));

    // ワールド座標
    output.worldPosition = worldPosition.xyz;

    return output;
}