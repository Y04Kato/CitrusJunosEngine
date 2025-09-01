#pragma once
#include "CreateLine.h"

class SphereLine {
public:
    // 初期化（分割数と線の太さを指定）
    void Initialize(uint32_t subdivision = 16, float thickness = 0.05f);

    // 球を描画
    void Draw(const StructSphere& sphere, const ViewProjection& viewProjection, const Vector4& color);

private:
    uint32_t kSubdivision_ = 16;
    float thickness_ = 0.05f;
    std::vector<CreateLine> lines_; // 球を構成するライン群
};