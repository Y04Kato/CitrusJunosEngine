#pragma once
#include "CreateLine.h"

class LineShapes {
public:
    void Initialize(uint32_t subdivision = 16, float thickness = 0.05f);

    //ラインの太さ変更
    void SetLineThickness(float thickness);

    //シェイプ描画
    void DrawSphere(const StructSphere& sphere, const ViewProjection& viewProjection, const Vector4& color);
    void DrawPlane(const StructPlane& plane, const ViewProjection& viewProjection, const Vector4& color);
    void DrawAABB(const AABB& aabb, const ViewProjection& viewProjection, const Vector4& color);
    void DrawOBB(const OBB& obb, const ViewProjection& viewProjection, const Vector4& color);
    void DrawCylinder(const StructCylinder& cylinder, const ViewProjection& viewProjection, const Vector4& color);

    //カスタムライン
    void AddCustomLine(const Vector3& a, const Vector3& b, const Vector4& color);
    void ClearCustomLines();
    void DrawCustomLines(const ViewProjection& viewProjection);

private:
    uint32_t kSubdivision_ = 16;
    float thickness_ = 0.05f;
    std::vector<CreateLine> lines_;
    uint32_t lineIndex_ = 0;

    //ライン描画
    void DrawLine(const Vector3& a, const Vector3& b, const ViewProjection& viewProjection, const Vector4& color);

    void ResetLineIndex();

    //登録済みカスタムライン
    struct LineData {
        Vector3 a, b;
        Vector4 color;
    };
    std::vector<LineData> customLines_;
};