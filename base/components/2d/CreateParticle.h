#pragma once
#include "CJEngine.h"
#include "DirectXCommon.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "TextureManager.h"
#include "DirectionalLight.h"
#include <wrl.h>
#include <random>
#include "SRVManager/SRVManager.h"

class CreateParticle {
public:
	void Initialize(int kNumInstance,Emitter emitter,AccelerationField accelerationField, uint32_t textureIndex);
	void Update();
	void Finalize();
	void Draw(const ViewProjection& viewProjection);

	Particle MakeNewParticle(std::mt19937& randomEngine,const EulerTransform transform);
	std::list<Particle> Emission(const Emitter& emitter, std::mt19937& randomEngine);

	void SetEmitter(const Emitter& emitter) {
		emitter_.count = emitter.count; 
		emitter_.frequency = emitter.frequency;
		emitter_.transform = emitter.transform;
	};
	void SetAccelerationField(const AccelerationField& accelerationField) { accelerationField_ = accelerationField; };
	void SetisBillBoard(const bool isBillBoard) { isBillBoard_ = isBillBoard; };
	void SetisColor(const bool isColor) { isColor_ = isColor; };

	//パーティクルの色をランダムから指定した色に変更出来る
	void SetColor(Vector4 color) {
		color_ = color;
		isColor_ = true;
	}

	int GetkNumMaxInstance() { return kNumMaxInstance_; }

private:
	void SettingVertex();

	void SettingColor();

	void SettingDictionalLight();

	uint32_t LoadBuffer(int kNumInstance);
	void LoadBuffer(uint32_t index, int kNumInstance);

private:
	DirectXCommon* dxCommon_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	SRVManager* srvManager_;

	ModelData modelData_;

	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource_ = nullptr;
	VertexData* vertexData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	Material* materialData_ = nullptr;

	//パーティクルの数
	int kNumMaxInstance_;
	int numInstance_;
	//パーティクルの数と同じ数のTransform
	std::list<Particle> particles_;
	//Δtime 現在は60FPS固定
	const float kDeltaTime = 1.0f / 60.0f;

	DirectionalLights* directionalLights_;
	DirectionalLight* directionalLight_;
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource_;

	uint32_t bufferIndex_;
	ParticleForGPU* instancingData_ = nullptr;

	bool isBillBoard_ = true;

	std::random_device seedGenerator;

	Emitter emitter_{};
	AccelerationField accelerationField_;

	Vector4 color_;
	bool isColor_ = false;
};

