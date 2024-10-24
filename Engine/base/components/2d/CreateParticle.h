/**
 * @file CreateParticle.h
 * @brief 2DParticleの初期化及び描画を行う
 * @author KATO
 * @date 未記録
 */

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
	void Initialize(int kNumInstance, Emitter emitter, AccelerationField accelerationField, uint32_t textureIndex);
	void Update();
	void Finalize();
	void Draw(const ViewProjection& viewProjection);

	//新しいパーティクルを指定地点に発生させる
	Particle MakeNewParticle(std::mt19937& randomEngine, const EulerTransform transform);
	
	//エミッターの設定を基に、パーティクルを発生させる
	std::list<Particle> Emission(const Emitter& emitter, std::mt19937& randomEngine);

	//エミッターの設定
	void SetEmitter(const Emitter& emitter) {
		emitter_.count = emitter.count;
		emitter_.frequency = emitter.frequency;
		emitter_.transform = emitter.transform;
	};
	
	//パーティクルの流れる方向を設定
	void SetAccelerationField(const AccelerationField& accelerationField) { accelerationField_ = accelerationField; };
	
	//ビルボードを行うか否か(視点に方向を向くか否か)
	void SetisBillBoard(const bool isBillBoard) { isBillBoard_ = isBillBoard; };
	
	//パーティクルカラーの手動設定
	void SetisColor(const bool isColor) { isColor_ = isColor; };
	
	//Velocityをランダムにするか否かFalseでOFF、True+1以上ならランダムな値に値を掛ける
	void SetisVelocity(const bool isVelocity, const float boost) { isVelocity_ = isVelocity; velocityBoost_ = boost; };
	
	//パーティクル発生座標のみを設定
	void SetTranslate(Vector3 transform) { emitter_.transform.translate = transform; }
	
	//パーティクル発生頻度のみを設定
	void SetFrequency(float frequency) { emitter_.frequency = frequency; }

	//パーティクルの色をランダムから指定した色に変更出来る
	void SetColor(Vector4 color) {
		color_ = color;
		isColor_ = true;
	}

	//パーティクル生きてる時間を変更出来る
	void SetLifeTime(float lifeTime) {
		lifeTime_ = lifeTime;
		isLifeTimer = true;
	}

	int GetkNumMaxInstance() { return kNumMaxInstance_; }

	//指定した数、1回だけパーティクルを生成する
	void OccursOnlyOnce(int occursNum);

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

	Vector3 velocity_;
	bool isVelocity_ = false;
	float velocityBoost_ = 0.0f;

	float lifeTime_ = 0.0f;
	bool isLifeTimer = false;
};