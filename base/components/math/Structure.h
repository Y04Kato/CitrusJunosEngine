#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <math.h>
#include <map>

struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct Quaternion {
	float x, y, z, w;
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
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
	Matrix4x4 WorldInverseTranspose;
};

struct VATData {
	float VATTime;
	float MaxVATTime;
	Vector4 VatPositionTexSize;//(1.0/width, 1.0/height, width, height)
	Vector4 VatNormalTexSize;//(1.0/width, 1.0/height, width, height)
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
};

//Node情報格納用構造体
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

//Keyframe構造体
template <typename tValue>
struct Keyframe {
	float time;//キーフレームの時間(単位：秒)
	tValue value;//キーフレームの値
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

//KeyframeをNodeごとにまとめる
template <typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};

struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

struct Animation {
	float duration;//アニメーションの全体の長さ(単位：秒)
	//NodeAnimationの集合、Node名で引けるようにする
	std::map<std::string, NodeAnimation> nodeAnimations;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	int textureIndex;
	Node rootNode;
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

#pragma region Particle
struct Particle {
	EulerTransform transform;
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
	EulerTransform transform;
	uint32_t count;//発生数
	float frequency;//発生頻度
	float frequencyTime;//頻度用時刻
};

struct AccelerationField {
	Vector3 acceleration;//加速度
	AABB area;//範囲
};

#pragma endregion

#pragma region Lighting
struct CameraForGPU {
	Vector3 worldPosition;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct PointLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	float radius;
	float decay;
};

#pragma endregion