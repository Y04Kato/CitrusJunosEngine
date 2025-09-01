/**
 * @file DebugScene.cpp
 * @brief ゲームの動作テストを行うシーンを管理
 * @author KATO
 * @date 未記録
 */

#include "DebugScene.h"

#include "GlobalVariablesGroup.h"

void DebugScene::Initialize() {
	Iscene::Initialize();

	//テクスチャ
	testTex_ = textureManager_->Load("project/gamedata/resources/default/uvChecker.png");
	skyboxTex_ = textureManager_->Load("project/gamedata/resources/default/vz_empty_space_cubemap_ue.dds");
	particleTex_ = textureManager_->Load("project/gamedata/resources/default/circle.png");

	//サウンドリソース
	testSoundData_ = audio_->SoundLoad("project/gamedata/resources/default/click.mp3");

	//2DSprite
	spriteTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280.0f / 2.0f,720.0f / 2.0f,0.0f} };
	spriteUVTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	spriteMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

	sprite_ = std::make_unique <CreateSprite>();
	sprite_->Initialize(Vector2{ 100.0f,100.0f }, testTex_);
	sprite_->SetTextureInitialSize();//これがない場合、Initialize第一引数sizeで指定したサイズになる
	sprite_->SetAnchor(Vector2{ 0.5f,0.5f });//アンカーは授業で作った通り

	//3DModel(NoAnimation)
	model_.reset(Model::CreateModel("project/gamedata/resources/block", "block.obj"));
	model_->SetDirectionalLightFlag(true, 3);
	modelWorldTransform_.Initialize();
	modelMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

	//3DModel(Animation)
	animationModel_.reset(Model::CreateSkinningModel("project/gamedata/resources/flag", "flag.gltf"));
	animationModel_->SetDirectionalLightFlag(true, 3);
	animationModelWorldTransform_.Initialize();

	//SkyBox
	skyBox_ = std::make_unique <CreateSkyBox>();
	skyBox_->Initialize();
	skyBox_->SetDirectionalLightFlag(true, 3);
	skyBoxWorldTransform_.Initialize();
	skyBoxWorldTransform_.scale_ = { 1000.0f,1000.0f,1000.0f };
	skyBoxMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

	//Particle
	particleEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	particleEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	particleEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	particleEmitter_.count = 5;
	particleEmitter_.frequency = 0.2f;//0.2秒ごとに発生
	particleEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	accelerationField.acceleration = { 10.0f,15.0f,10.0f };
	accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };

	particle_ = std::make_unique <CreateParticle>();
	particle_->Initialize(100, particleEmitter_, accelerationField, particleTex_);
	particle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	//DataReceipt
	datareceipt_.Initialize(50001);
	datareceipt_.start();

	lineShapes_.Initialize(16, 0.1f);

	//GlobalVariablesGroup
	std::unique_ptr<GVariGroup>gvg = std::make_unique<GVariGroup>("DebugScene");
	gvg->SetValue("Test", &testData_);
}

void DebugScene::Update() {
	Iscene::Update();
	ImGui::Begin("DebugOperate");
	if (ImGui::Button("TitleScene")) {
		sceneNumber_->SetSceneNumber(TITLE_SCENE);
	}
	if (ImGui::Button("TestSound")) {
		audio_->SoundPlayWave(testSoundData_, 1.0f, false);
	}
	ImGui::Text("CameraReset:Q key");
	ImGui::End();

	//3DModel(NoAnimation)
	modelWorldTransform_.UpdateMatrix();

	//3DModel(Animation)
	animationModelWorldTransform_.UpdateMatrix();

	//SkyBox
	skyBoxWorldTransform_.UpdateMatrix();

	//DataReceipt
	datareceipt_.receiveMessage();

	//Particle
	particle_->Update();

	//カメラリセット
	if (input_->TriggerKey(DIK_Q)) {
		debugCamera_->MovingCamera(Vector3{ 0.0f,0.0f,-20.0f }, Vector3{ 0.0f,0.0f,0.0f }, 0.05f);
	}
}

void DebugScene::Draw() {
#pragma region SkyBox描画
	CJEngine_->renderer_->Draw(PipelineType::SkyBox);

	skyBox_->Draw(skyBoxWorldTransform_, viewProjection_, skyBoxMaterial_, skyboxTex_);
#pragma endregion

#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

	//3DModel(NoAnimation)
	//model_->Draw(modelWorldTransform_, viewProjection_, modelMaterial_);

	//Line
	//lineShapes_.DrawSphere(sphere_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });
	//lineShapes_.DrawPlane(plane_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });
	//lineShapes_.DrawAABB(aabb_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });
	//lineShapes_.DrawOBB(obb_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });
	lineShapes_.DrawCylinder(cylinder_, viewProjection_, { 1.0f, 0.0f, 0.0f, 1.0f });

	//Editors
	editors_->Draw(viewProjection_);

	//Houdini
	datareceipt_.Draw(viewProjection_);

#pragma endregion

#pragma region 3DSkinningオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Skinning);

	//3DModel(Animation)
	//animationModel_->SkinningDraw(animationModelWorldTransform_, viewProjection_, animationModelMaterial_);

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

	//Particle
	//particle_->Draw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void DebugScene::DrawUI() {//ここで描画するとポストエフェクトの影響を受けないよ
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

	//2DSprite
	//sprite_->Draw(spriteTransform_, spriteUVTransform_, spriteMaterial_);

#pragma endregion
}

void DebugScene::DrawPostEffect() {
	Iscene::DrawPostEffect();
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
}

void DebugScene::Finalize() {
	Iscene::Finalize();
	datareceipt_.stop();
	audio_->SoundUnload(&testSoundData_);
}