/**
 * @file CreateLine.h
 * @brief 3Dで2点を結ぶ線の初期化及び描画を行う
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "DirectXCommon.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include <wrl.h>

class CitrusJunosEngine;

class CreateLine {
public:
    void Initialize();
    void Draw(const Vector3& start, const Vector3& end, const ViewProjection& viewProjection, const Vector4& material);
    void Finalize();

    void SetDirectionalLightFlag(bool isDirectionalLight, int lightNum);
    void SetLineThickness(float thickness) { size_ = thickness; };
    void SetEnvironmentTexture(const uint32_t& envTex) { environmentTexture_ = envTex; isSetEnviromentTexture_ = true; }

private:
    void SettingVertex();
    void SettingColor();
    void SettingLight();

private:
    DirectXCommon* dxCommon_;
    CitrusJunosEngine* CJEngine_;
    TextureManager* textureManager_;
    DirectionalLights* directionalLights_;
    PointLights* pointLights_;

    // バッファ
    Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    VertexData* vertexData_;

    Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
    uint32_t* indexData_;

    Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
    Material* materialData_;

    Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource_;
    DirectionalLight* directionalLight_;

    Microsoft::WRL::ComPtr <ID3D12Resource> pointLightResource_;
    PointLight* pointLight_;

    Microsoft::WRL::ComPtr <ID3D12Resource> cameraResource_;
    CameraForGPU* cameraData_;

    WorldTransform worldTransform_;

    uint32_t vertexCount_ = 36; // 立方体
    float size_ = 0.05f;        // 線の太さ

    bool isDirectionalLight_ = false;
    int lightNum_ = 0;

    bool isSetEnviromentTexture_ = false;
    uint32_t environmentTexture_;
};
