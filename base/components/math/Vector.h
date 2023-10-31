#pragma once
#include "Matrix.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <math.h>

struct Vector2 {
	float num[2];
};

struct Vector3 {
	float num[3];
};

struct Vector4 {
	float num[4];
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct OBB {
	Vector3 center;
	Vector3 orientation[3];
	Vector3 size;
};

struct StructSphere {
	Vector3 center;
	float radius;
};