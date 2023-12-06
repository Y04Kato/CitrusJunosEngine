#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <math.h>

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
	int textureIndex;
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

struct Quaternion {
	float x, y, z, w;
};

struct Particle {
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct ParticleForGPU {
	Matrix4x4 matWorld; // ローカル→ワールド変換行列
	Vector4 color;
};

struct Emitter {
	Transform transform;
	uint32_t count;//発生数
	float frequency;//発生頻度
	float frequencyTime;//頻度用時刻
};

struct AccelerationField {
	Vector3 acceleration;//加速度
	AABB area;//範囲
};