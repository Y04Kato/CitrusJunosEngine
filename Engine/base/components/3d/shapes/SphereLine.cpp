#include "SphereLine.h"

void SphereLine::Initialize(uint32_t subdivision, float thickness) {
    kSubdivision_ = subdivision;
    thickness_ = thickness;

    // ゆとりを持って確保
    uint32_t lineCount = subdivision * subdivision * 4;
    lines_.resize(lineCount);

    for (auto& line : lines_) {
        line.Initialize();
        line.SetLineThickness(thickness_);
    }
}


void SphereLine::Draw(const StructSphere& sphere, const ViewProjection& viewProjection, const Vector4& color) {
    const float kLonEvery = std::numbers::pi_v<float> / kSubdivision_;
    const float kLatEvery = 2.0f * std::numbers::pi_v<float> / kSubdivision_;

    uint32_t index = 0;
    for (uint32_t latIndex = 0; latIndex < kSubdivision_; ++latIndex) {
        float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {
            float lon = lonIndex * kLonEvery;

            Vector3 a = { std::cosf(lat) * std::cosf(lon),
                          std::sinf(lat),
                          std::cosf(lat) * std::sinf(lon) };
            Vector3 b = { std::cosf(lat + kLatEvery) * std::cosf(lon),
                          std::sinf(lat + kLatEvery),
                          std::cosf(lat + kLatEvery) * std::sinf(lon) };
            Vector3 c = { std::cosf(lat) * std::cosf(lon + kLonEvery),
                          std::sinf(lat),
                          std::cosf(lat) * std::sinf(lon + kLonEvery) };

            Vector3 worldA = sphere.center + sphere.radius * a;
            Vector3 worldB = sphere.center + sphere.radius * b;
            Vector3 worldC = sphere.center + sphere.radius * c;

            if (index + 1 < lines_.size()) {
                lines_[index++].Draw(worldA, worldB, viewProjection, color);
                lines_[index++].Draw(worldA, worldC, viewProjection, color);
            }
        }
    }
}
