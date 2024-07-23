#pragma once
#include "Vector.h"
#include "Matrix.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <math.h>
#include <map>
#include <optional>
#include <span>
#include <array>
#include <utility>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <wrl.h>
#include "SRVManager/SRVManager.h"

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

struct Joint {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix;//SkeltonSpaceでの変換行列
	std::string name;//名前
	std::vector<int32_t> children;//子JointのIndexリスト
	int32_t index;//自身のIndex
	std::optional<int32_t> parent;//親JointのIndex
};

struct Skeleton {
	int32_t root;//RootJointのIndex
	std::map<std::string, int32_t> jointMap;//Joint名とIndexの辞書
	std::vector<Joint> joints;//所持しているジョイント
};

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array <float, kNumMaxInfluence> weight;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix;//位置用
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;//法線用
};

struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;
	Microsoft::WRL::ComPtr <ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	Microsoft::WRL::ComPtr <ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	DESCRIPTERHANDLE paletteSrvHandle;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	std::map<std::string, JointWeightData> skinClusterData;
	MaterialData material;
	uint32_t textureIndex;
	Node rootNode;
	std::string directoryPath;
	std::string filename;
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

struct Segment {
	Vector3 origin;
	Vector3 diff;
};

struct StructSphere {
	Vector3 center;
	float radius;
};

#pragma region PostEffect
struct MaskData {
	float maskThreshold;//閾値
	Vector3 maskColor;//色
	Vector3 edgeColor;//境界線の色
};

struct RandomData {
	float time;
};

struct ScanlineData{
	float scanlineIntensity;//スキャンラインの強度
	float scanlineFrequency;//スキャンラインの頻度
	float time;//時間（秒単位）
};

struct HSVMaterial {
	float hue;
	float saturation;
	float value;
};

#pragma endregion

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