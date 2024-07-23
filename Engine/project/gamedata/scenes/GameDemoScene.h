#pragma once
#include "Iscene.h"
#include "components/audio/Audio.h"
#include "components/input/Input.h"
#include "components/3d/WorldTransform.h"
#include "components/3d/ViewProjection.h"
#include "TextureManager.h"
#include "components/2d/CreateTriangle.h"
#include "components/2d/CreateSprite.h"
#include "components/2d/CreateParticle.h"
#include "components/3d/CreateLine.h"
#include "components/3d/CreateSphere.h"
#include "components/3d/CreateSkyBox.h"
#include "components/3d/Model.h"
#include "components/debugcamera/DebugCamera.h"
#include "components/utilities/collisionManager/CollisionManager.h"
#include "components/utilities/collisionManager/CollisionConfig.h"
#include "levelDataLoader/LevelDataLoader.h"
#include "postEffect/PostEffect.h"
#include "editors/Editors.h"

#include "components/utilities/globalVariables/GlobalVariables.h"

class GameDemoScene :public Iscene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void DrawPostEffect() override;
	void Finalize() override;

	void ApplyGlobalVariables();

	void LevelSetObject();

private:
	CitrusJunosEngine* CJEngine_;
	DirectXCommon* dxCommon_;
	ViewProjection viewProjection_;
	TextureManager* textureManager_;

	int blendCount_;

	std::unique_ptr <CreateTriangle> triangle_[2];
	WorldTransform worldTransformTriangle_[2];
	Vector4 triangleMaterial_[2];

	std::unique_ptr <CreateSprite> sprite_[2];
	EulerTransform spriteTransform_[2];
	EulerTransform SpriteuvTransform_[2];
	Vector4 spriteMaterial_[2];

	std::unique_ptr<CreateParticle> particle_[2];
	Emitter testEmitter_[2] = {};
	AccelerationField accelerationField_[2];
	bool isBillBoard_[2] = {true,true};
	bool isColor_[2];
	Vector4 particleColor_[2] = {0.0f,0.0f,0.0f,0.0f};

	std::unique_ptr<CreateLine> line_;
	WorldTransform worldTransformLine_[2];
	Vector4 lineMaterial_;
	float lineThickness_ = 0.2f;

	std::unique_ptr <CreateSphere> linePoint_[2];
	Vector4 linePointMaterial_;

	std::unique_ptr <CreateSphere> sphere_[2];
	WorldTransform worldTransformSphere_[2];
	Vector4 sphereMaterial_[2];

	std::unique_ptr <CreateSkyBox> skyBox_;
	WorldTransform worldTransformSkyBox_;
	Vector4 skyBoxMaterial_;

	std::unique_ptr<Model> model_[3];
	WorldTransform worldTransformModel_[3];
	Vector4 modelMaterial_[3];

	std::unique_ptr<Model> modelVAT_;
	WorldTransform worldTransformModelVAT_;
	Vector4 modelMaterialVAT_;
	VATData vatData_;

	uint32_t uvResourceNum_;
	uint32_t monsterBallResourceNum_;
	uint32_t kaedeResourceNum_;
	uint32_t cjEngineResourceNum_;
	uint32_t ddsResourceNum_;

	Audio* audio_;
	SoundData soundData1_;
	SoundData soundData2_;

	Input* input_;

	DebugCamera* debugCamera_;


	PostEffect* postEffect_;
	//MaskTexture用Data
	uint32_t noiseTexture_[2];
	int maskTextureNum_;
	MaskData maskData_;

	//Scanline用Data
	ScanlineData scanlineData_;

	//HSV用Material
	HSVMaterial hsvMaterial_;

	int texture_[2];

	bool isTriangleDraw_[2];
	bool isSphereDraw_[2];
	bool isSpriteDraw_[2];
	bool isModelDraw_[3];
	bool isParticleDraw_[2];
	bool isLineDraw_;
	bool isSkyBoxDraw_;
	bool isVATDraw_;

	bool isGrayScaleDraw_;
	bool isVignetteDraw_;
	bool isSmoothingDraw_;
	bool isGaussianDraw_;
	bool isOutlineDraw_;
	bool isRadialBlurDraw_;
	bool isMaskDraw_;
	bool isRandomDraw_;
	bool isLensDistortionDraw_;
	bool isScanlineDraw_;
	bool isHSVDraw_;

	//エディター擬き
	Editors* editors_;

	const char* groupName = "GameDemoScene";

	//LevelEditor
	LevelDataLoader* levelDataLoader_;
	std::list<Obj> levelEditorObjects_;
	ModelData ObjModelData_;
	uint32_t ObjTexture_;
};

