/**
 * @file DebugScene.h
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Iscene.h"

#include "dataReceipt/DataReceipt.h"

#include "Receipt3D.h"

#include "LineShapes.h"

class DebugScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

private:
	//テクスチャ
	uint32_t testTex_;
	uint32_t skyboxTex_;
	uint32_t particleTex_;

	//サウンドリソース
	SoundData testSoundData_;

	//2DSprite
	std::unique_ptr <CreateSprite> sprite_;
	EulerTransform spriteTransform_;
	EulerTransform spriteUVTransform_;
	Vector4 spriteMaterial_;

	//3DModel(NoAnimation)
	std::unique_ptr<Model> model_;
	WorldTransform modelWorldTransform_;
	Vector4 modelMaterial_;

	//3DModel(Animation)
	std::unique_ptr<Model> animationModel_;
	WorldTransform animationModelWorldTransform_;
	Vector4 animationModelMaterial_;

	//SkyBox
	std::unique_ptr <CreateSkyBox> skyBox_;
	WorldTransform skyBoxWorldTransform_;
	Vector4 skyBoxMaterial_;

	//Particle
	std::unique_ptr<CreateParticle> particle_;
	Emitter particleEmitter_{};
	AccelerationField accelerationField;

	//DataReceipt
	DataReceipt datareceipt_;
	std::unique_ptr<Receipt3D> receipt3D_;

	float testData_ = 0.0f;

	LineShapes lineShapes_;
	StructSphere sphere_ = { {0.0f, 0.0f, 0.0f}, 10.0f };
	StructPlane plane_ = { {0.0f,1.0f,0.0f},0.0f };
	AABB aabb_ = {
	{ -0.5f, -0.5f, -0.5f }, // min
	{  0.5f,  0.5f,  0.5f }  // max
	};
	OBB obb_ = {
	{0.0f, 0.0f, 0.0f},           // center
	{ {1,0,0}, {0,1,0}, {0,0,1} }, // orientation: XYZ軸に沿う
	{0.5f, 0.5f, 0.5f}             // size: 半長
	};
	StructCylinder cylinder_ = {
		{0.0f, 0.5f, 0.0f},  // topCenter
		{0.0f, -0.5f, 0.0f}, // bottomCenter
		2.0f                  // radius
	};
};