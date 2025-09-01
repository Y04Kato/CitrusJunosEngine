#pragma once
#include "CreateLine.h"

class LineShapes {
public:
    void Initialize(uint32_t subdivision = 16, float thickness = 0.05f);

    void DrawSphere(const StructSphere& sphere, const ViewProjection& viewProjection, const Vector4& color);
    void DrawPlane(const StructPlane& plane, const ViewProjection& viewProjection, const Vector4& color);
    void DrawAABB(const AABB& aabb, const ViewProjection& viewProjection, const Vector4& color);
    void DrawOBB(const OBB& obb, const ViewProjection& viewProjection, const Vector4& color);
    void DrawCylinder(const StructCylinder& cylinder, const ViewProjection& viewProjection, const Vector4& color);

private:
    uint32_t kSubdivision_ = 16;
    float thickness_ = 0.05f;
    std::vector<CreateLine> lines_;
    uint32_t lineIndex_ = 0;

    void ResetLineIndex();
    void DrawLine(const Vector3& a, const Vector3& b, const ViewProjection& viewProjection, const Vector4& color);
};