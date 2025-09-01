#include "LineShapes.h"

void LineShapes::Initialize(uint32_t subdivision, float thickness) {
	kSubdivision_ = subdivision;
	thickness_ = thickness;

	// 球や円柱で大量にラインを使うため、多めに確保
	uint32_t lineCount = subdivision * subdivision * 8;
	lines_.resize(lineCount);

	for (auto& line : lines_) {
		line.Initialize();
		line.SetLineThickness(thickness_);
	}
}

void LineShapes::ResetLineIndex() {
	lineIndex_ = 0;
}

void LineShapes::DrawLine(const Vector3& a, const Vector3& b, const ViewProjection& viewProjection, const Vector4& color) {
	if (lineIndex_ < lines_.size()) {
		lines_[lineIndex_++].Draw(a, b, viewProjection, color);
	}
}

// ================= Sphere =================
void LineShapes::DrawSphere(const StructSphere& sphere, const ViewProjection& viewProjection, const Vector4& color) {
	ResetLineIndex();

	const float kLonEvery = std::numbers::pi_v<float> / kSubdivision_;
	const float kLatEvery = 2.0f * std::numbers::pi_v<float> / kSubdivision_;

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

			DrawLine(worldA, worldB, viewProjection, color);
			DrawLine(worldA, worldC, viewProjection, color);
		}
	}
}

// ================= Plane =================
void LineShapes::DrawPlane(const StructPlane& plane, const ViewProjection& viewProjection, const Vector4& color) {
	ResetLineIndex();

	// 平面は可視化のため四角形で描画
	Vector3 center = plane.normal * plane.distance;
	Vector3 tangent = Normalize(Cross(plane.normal, { 0,1,0 }));
	if (Length(tangent) < 1e-5f) tangent = Normalize(Cross(plane.normal, { 1,0,0 }));
	Vector3 bitangent = Cross(plane.normal, tangent);

	float size = 5.0f;
	Vector3 p[4] = {
		center + (tangent * size) + (bitangent * size),
		center - (tangent * size) + (bitangent * size),
		center - (tangent * size) - (bitangent * size),
		center + (tangent * size) - (bitangent * size),
	};

	for (int i = 0;i < 4;i++) {
		DrawLine(p[i], p[(i + 1) % 4], viewProjection, color);
	}
}

// ================= AABB =================
void LineShapes::DrawAABB(const AABB& aabb, const ViewProjection& viewProjection, const Vector4& color) {
	ResetLineIndex();

	Vector3 p[8] = {
	{ aabb.min.num[0], aabb.min.num[1], aabb.min.num[2] },
	{ aabb.max.num[0], aabb.min.num[1], aabb.min.num[2] },
	{ aabb.max.num[0], aabb.max.num[1], aabb.min.num[2] },
	{ aabb.min.num[0], aabb.max.num[1], aabb.min.num[2] },
	{ aabb.min.num[0], aabb.min.num[1], aabb.max.num[2] },
	{ aabb.max.num[0], aabb.min.num[1], aabb.max.num[2] },
	{ aabb.max.num[0], aabb.max.num[1], aabb.max.num[2] },
	{ aabb.min.num[0], aabb.max.num[1], aabb.max.num[2] },
	};


	int edges[12][2] = {
		{0,1},{1,2},{2,3},{3,0},
		{4,5},{5,6},{6,7},{7,4},
		{0,4},{1,5},{2,6},{3,7},
	};

	for (auto& e : edges) {
		DrawLine(p[e[0]], p[e[1]], viewProjection, color);
	}
}

// ================= OBB =================
void LineShapes::DrawOBB(const OBB& obb, const ViewProjection& viewProjection, const Vector4& color) {
	ResetLineIndex();

	Vector3 axis[3] = {
		Normalize(obb.orientation[0]) * obb.size.num[0] * 0.5f,
		Normalize(obb.orientation[1]) * obb.size.num[1] * 0.5f,
		Normalize(obb.orientation[2]) * obb.size.num[2] * 0.5f,
	};

	Vector3 p[8];
	int idx = 0;
	for (int x = -1;x <= 1;x += 2)
		for (int y = -1;y <= 1;y += 2)
			for (int z = -1;z <= 1;z += 2) {
				p[idx++] = obb.center + axis[0] * float(x) + axis[1] * float(y) + axis[2] * float(z);
			}

	int edges[12][2] = {
		{0,1},{1,3},{3,2},{2,0},
		{4,5},{5,7},{7,6},{6,4},
		{0,4},{1,5},{2,6},{3,7},
	};

	for (auto& e : edges) {
		DrawLine(p[e[0]], p[e[1]], viewProjection, color);
	}
}

// ================= Cylinder =================
void LineShapes::DrawCylinder(const StructCylinder& cylinder, const ViewProjection& viewProjection, const Vector4& color) {
	ResetLineIndex();

	Vector3 axis = Normalize(cylinder.topCenter - cylinder.bottomCenter);
	Vector3 tangent = Normalize(Cross(axis, { 0,1,0 }));
	if (Length(tangent) < 1e-5f) tangent = Normalize(Cross(axis, { 1,0,0 }));
	Vector3 bitangent = Cross(axis, tangent);

	for (uint32_t i = 0;i < kSubdivision_;i++) {
		float t0 = (float)i / kSubdivision_ * 2.0f * std::numbers::pi_v<float>;
		float t1 = (float)(i + 1) / kSubdivision_ * 2.0f * std::numbers::pi_v<float>;

		Vector3 dir0 = tangent * std::cosf(t0) + bitangent * std::sinf(t0);
		Vector3 dir1 = tangent * std::cosf(t1) + bitangent * std::sinf(t1);

		Vector3 top0 = cylinder.topCenter + dir0 * cylinder.radius;
		Vector3 top1 = cylinder.topCenter + dir1 * cylinder.radius;
		Vector3 bot0 = cylinder.bottomCenter + dir0 * cylinder.radius;
		Vector3 bot1 = cylinder.bottomCenter + dir1 * cylinder.radius;

		// 上円と下円
		DrawLine(top0, top1, viewProjection, color);
		DrawLine(bot0, bot1, viewProjection, color);

		// 縦の辺
		DrawLine(top0, bot0, viewProjection, color);
	}
}
