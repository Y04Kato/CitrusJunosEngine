/**
 * @file CreateLine.cpp
 * @brief 3Dで2点を結ぶ線の初期化及び描画を行う
 * @author KATO
 * @date 未記録
 */

#include "CreateLine.h"
#include "CJEngine.h"

void CreateLine::Initialize() {
    dxCommon_ = DirectXCommon::GetInstance();
    CJEngine_ = CitrusJunosEngine::GetInstance();
    textureManager_ = TextureManager::GetInstance();
    directionalLights_ = DirectionalLights::GetInstance();
    pointLights_ = PointLights::GetInstance();

    worldTransform_.Initialize();
    environmentTexture_ = textureManager_->ddsSample;

    SettingVertex();
    SettingColor();
    SettingLight();
}

void CreateLine::Draw(const Vector3& start, const Vector3& end, const ViewProjection& viewProjection, const Vector4& material) {
    Vector3 dir = end - start;
    float length = Length(dir);
    dir = Normalize(dir);

    // 行列計算
    Matrix4x4 S = MakeScaleMatrix({ length, size_, size_ });
    Matrix4x4 R = DirectionToDirection({ 1,0,0 }, dir);
    Matrix4x4 T = MakeTranslateMatrix(start);
    worldTransform_.matWorld_ = S * R * T;
    worldTransform_.TransferMatrix();

    // マテリアル
    if (!isDirectionalLight_) {
        *materialData_ = { material, 0 };
    }
    else {
        *materialData_ = { material, lightNum_ };
    }

    *directionalLight_ = directionalLights_->GetDirectionalLight();
    *pointLight_ = pointLights_->GetPointLight();
    cameraData_->worldPosition = viewProjection.translation_;

    // VB/IB 設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
    dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 定数バッファ
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform_.constBuff_->GetGPUVirtualAddress());
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraResource_->GetGPUVirtualAddress());
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightResource_->GetGPUVirtualAddress());

    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(textureManager_->white));
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(7,
        textureManager_->GetGPUHandle(isSetEnviromentTexture_ ? environmentTexture_ : textureManager_->ddsSample));

    dxCommon_->GetCommandList()->DrawIndexedInstanced(vertexCount_, 1, 0, 0, 0);
}

void CreateLine::Finalize() {}

void CreateLine::SettingVertex() {
    vertexCount_ = 36; // 6面 × 2三角形 × 3頂点
    vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * vertexCount_);
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexCount_;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    indexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * vertexCount_);
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * vertexCount_;
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

    for (uint32_t i = 0; i < vertexCount_; ++i) indexData_[i] = i;

    // 単位直方体：X ∈ [0,1], Y,Z ∈ [-0.5,0.5]
    const float x0 = 0.0f, x1 = 1.0f;
    const float y0 = -0.5f, y1 = 0.5f;
    const float z0 = -0.5f, z1 = 0.5f;

    auto V = [&](float x, float y, float z)->Vector4 { return { x,y,z,1.0f }; };
    auto N = [&](float x, float y, float z)->Vector3 { return { x,y,z }; };

    // 6面 × 2tri × 3vert（法線は面の向き、UVはダミー）
    uint32_t i = 0;

    // +X 面（右）
    Vector3 n = N(1, 0, 0);
    vertexData_[i++] = { V(x1,y0,z0), {0,0}, n };
    vertexData_[i++] = { V(x1,y1,z0), {1,0}, n };
    vertexData_[i++] = { V(x1,y1,z1), {1,1}, n };
    vertexData_[i++] = { V(x1,y0,z0), {0,0}, n };
    vertexData_[i++] = { V(x1,y1,z1), {1,1}, n };
    vertexData_[i++] = { V(x1,y0,z1), {0,1}, n };

    // -X 面（左）
    n = N(-1, 0, 0);
    vertexData_[i++] = { V(x0,y0,z1), {0,0}, n };
    vertexData_[i++] = { V(x0,y1,z1), {1,0}, n };
    vertexData_[i++] = { V(x0,y1,z0), {1,1}, n };
    vertexData_[i++] = { V(x0,y0,z1), {0,0}, n };
    vertexData_[i++] = { V(x0,y1,z0), {1,1}, n };
    vertexData_[i++] = { V(x0,y0,z0), {0,1}, n };

    // +Y 面（上）
    n = N(0, 1, 0);
    vertexData_[i++] = { V(x0,y1,z0), {0,0}, n };
    vertexData_[i++] = { V(x1,y1,z0), {1,0}, n };
    vertexData_[i++] = { V(x1,y1,z1), {1,1}, n };
    vertexData_[i++] = { V(x0,y1,z0), {0,0}, n };
    vertexData_[i++] = { V(x1,y1,z1), {1,1}, n };
    vertexData_[i++] = { V(x0,y1,z1), {0,1}, n };

    // -Y 面（下）
    n = N(0, -1, 0);
    vertexData_[i++] = { V(x0,y0,z1), {0,0}, n };
    vertexData_[i++] = { V(x1,y0,z1), {1,0}, n };
    vertexData_[i++] = { V(x1,y0,z0), {1,1}, n };
    vertexData_[i++] = { V(x0,y0,z1), {0,0}, n };
    vertexData_[i++] = { V(x1,y0,z0), {1,1}, n };
    vertexData_[i++] = { V(x0,y0,z0), {0,1}, n };

    // +Z 面（前）
    n = N(0, 0, 1);
    vertexData_[i++] = { V(x0,y0,z1), {0,0}, n };
    vertexData_[i++] = { V(x0,y1,z1), {1,0}, n };
    vertexData_[i++] = { V(x1,y1,z1), {1,1}, n };
    vertexData_[i++] = { V(x0,y0,z1), {0,0}, n };
    vertexData_[i++] = { V(x1,y1,z1), {1,1}, n };
    vertexData_[i++] = { V(x1,y0,z1), {0,1}, n };

    // -Z 面（後）
    n = N(0, 0, -1);
    vertexData_[i++] = { V(x1,y0,z0), {0,0}, n };
    vertexData_[i++] = { V(x1,y1,z0), {1,0}, n };
    vertexData_[i++] = { V(x0,y1,z0), {1,1}, n };
    vertexData_[i++] = { V(x1,y0,z0), {0,0}, n };
    vertexData_[i++] = { V(x0,y1,z0), {1,1}, n };
    vertexData_[i++] = { V(x0,y0,z0), {0,1}, n };

    // ここまでで i==36 になるはず
}

void CreateLine::SettingColor() {
    materialResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void CreateLine::SettingLight() {
    directionalLightResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));

    pointLightResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(PointLight));
    pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLight_));

    cameraResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice(), sizeof(CameraForGPU));
    cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
    cameraData_->worldPosition = { 0,0,0 };
}

void CreateLine::SetDirectionalLightFlag(bool isDirectionalLight, int lightNum) {
    isDirectionalLight_ = isDirectionalLight;
    lightNum_ = lightNum;
}