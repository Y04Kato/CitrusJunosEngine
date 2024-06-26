#include "GameDemoScene.h"

void GameDemoScene::Initialize() {
	CJEngine_ = CitrusJunosEngine::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	//テクスチャ
	uvResourceNum_ = textureManager_->Load("project/gamedata/resources/uvChecker.png");

	monsterBallResourceNum_ = textureManager_->Load("project/gamedata/resources/monsterBall.png");

	kaedeResourceNum_ = textureManager_->Load("project/gamedata/resources/kaede.png");

	cjEngineResourceNum_ = textureManager_->Load("project/gamedata/resources/CitrusJunosEngine.png");
	
	ddsResourceNum_ = textureManager_->Load("project/gamedata/resources/rostock_laage_airport_4k.dds");

	//三角形
	for (int i = 0; i < 2; i++) {
		triangle_[i] = std::make_unique <CreateTriangle>();
		triangle_[i]->Initialize();
		worldTransformTriangle_[i].Initialize();
		triangleMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		isTriangleDraw_[i] = false;
	}

	worldTransformTriangle_[1].rotation_.num[1] = 0.7f;

	//スプライト
	for (int i = 0; i < 2; i++) {
		spriteMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		spriteTransform_[i] = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1280 / 2.0f,720 / 2.0f,0.0f} };
		SpriteuvTransform_[i] = {
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
		};
		sprite_[i] = std::make_unique <CreateSprite>();

		isSpriteDraw_[i] = false;
	}
	sprite_[0]->Initialize(Vector2{ 200.0f,200.0f }, uvResourceNum_);
	sprite_[1]->Initialize(Vector2{ 100.0f,100.0f }, monsterBallResourceNum_);
	sprite_[0]->SetTextureLTSize(Vector2{ 0.0f,0.0f }, Vector2{ 50.0f,50.0f });
	sprite_[0]->SetAnchor(Vector2{ 0.5f,0.5f });
	sprite_[1]->SetTextureInitialSize();

	//パーティクル
	for (int i = 0; i < 2; i++) {
		testEmitter_[i].transform.translate = { 0.0f,0.0f,0.0f };
		testEmitter_[i].transform.rotate = { 0.0f,0.0f,0.0f };
		testEmitter_[i].transform.scale = { 1.0f,1.0f,1.0f };
		testEmitter_[i].count = 5;
		testEmitter_[i].frequency = 0.5f;//0.5秒ごとに発生
		testEmitter_[i].frequencyTime = 0.0f;//発生頻度の時刻

		accelerationField_[i].acceleration = { 0.0f,0.0f,-100.0f };
		accelerationField_[i].area.min = { -1.0f,-1.0f,-1.0f };
		accelerationField_[i].area.max = { 1.0f,1.0f,1.0f };

		particle_[i] = std::make_unique <CreateParticle>();
	}
	particle_[0]->Initialize(100, testEmitter_[0], accelerationField_[0], cjEngineResourceNum_);
	particle_[1]->Initialize(100, testEmitter_[1], accelerationField_[1], kaedeResourceNum_);


	//Line
	line_ = std::make_unique <CreateLine>();
	line_->Initialize();
	line_->SetDirectionalLightFlag(false, 0);
	lineMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	linePointMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	for (int i = 0; i < 2; i++) {
		worldTransformLine_[i].Initialize();
		linePoint_[i] = std::make_unique <CreateSphere>();
		linePoint_[i]->Initialize();
	}

	//球体
	for (int i = 0; i < 2; i++) {
		sphere_[i] = std::make_unique <CreateSphere>();
		sphere_[i]->Initialize();
		worldTransformSphere_[i].Initialize();
		sphereMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		sphere_[i]->SetDirectionalLightFlag(true, 3);
		texture_[i] = uvResourceNum_;
	}

	//objモデル
	model_[0].reset(Model::CreateSkinningModel("project/gamedata/resources/flag", "flag.gltf"));
	model_[1].reset(Model::CreateModel("project/gamedata/resources/AnimatedCube", "AnimatedCube.gltf"));
	model_[2].reset(Model::CreateModel("project/gamedata/resources/terrain", "terrain.obj"));
	for (int i = 0; i < 3; i++) {
		worldTransformModel_[i].Initialize();
		modelMaterial_[i] = { 1.0f,1.0f,1.0f,1.0f };
		model_[i]->SetDirectionalLightFlag(true, 3);
	}

	skyBox_ = std::make_unique <CreateSkyBox>();
	skyBox_->Initialize();
	worldTransformSkyBox_.Initialize();
	worldTransformSkyBox_.scale_ = { 1000.0f,1000.0f,1000.0f };
	skyBoxMaterial_ = { 1.0f,1.0f,1.0f,1.0f };
	skyBox_->SetDirectionalLightFlag(true, 3);

	//VAT
	modelVAT_.reset(Model::CreateModel("project/gamedata/resources/vatSphere", "VAT_mesh.fbx"));
	worldTransformModelVAT_.Initialize();
	modelMaterialVAT_ = { 1.0f,1.0f,1.0f,1.0f };
	vatData_.VATTime = 1.0f;
	vatData_.MaxVATTime = 120.0f;
	vatData_.VatPositionTexSize = { 1.0f / 25.0f,1.0f / 120.0f ,25.0f,120.0f };
	vatData_.VatNormalTexSize = { 1.0f / 25.0f,1.0f / 120.0f ,25.0f,120.0f };
	modelVAT_->LoadVATData("project/gamedata/resources/vatSphere", &vatData_);
	modelVAT_->SetDirectionalLightFlag(true, 3);

	//Input
	input_ = Input::GetInstance();

	//Audio
	audio_ = Audio::GetInstance();
	soundData1_ = audio_->SoundLoad("project/gamedata/resources/kamui.mp3");
	soundData2_ = audio_->SoundLoad("project/gamedata/resources/system.mp3");
	//音声再生
	audio_->SoundPlayWave(soundData1_, 0.1f, true);

	// デバッグカメラの初期化
	debugCamera_ = DebugCamera::GetInstance();
	debugCamera_->initialize();

	viewProjection_.Initialize();

	postEffect_ = PostEffect::GetInstance();
	noiseTexture_[0] = textureManager_->Load("project/gamedata/resources/noise0.png");
	noiseTexture_[1] = textureManager_->Load("project/gamedata/resources/noise1.png");
	maskData_.maskThreshold = 1.0f;
	maskData_.maskColor = { 0.0f,1.0f,0.0f };
	maskData_.edgeColor = { 1.0f,0.4f,0.3f };
	postEffect_->SetMaskTexture(noiseTexture_[0]);

	levelDataLoader_ = LevelDataLoader::GetInstance();
	levelDataLoader_->Initialize("project/gamedata/levelEditor", "Transform.json");

	//
	ObjModelData_ = model_[0]->LoadModelFile("project/gamedata/resources/block", "block.obj");
	ObjTexture_ = textureManager_->Load(ObjModelData_.material.textureFilePath);

	for (int i = 0; i < objCountMax_; i++) {
		objNameHolder_[i] = "test" + std::to_string(i);
	}

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	GlobalVariables::GetInstance()->CreateGroup(groupName);

	globalVariables->AddItem(groupName, "ObjCount", objCount_);
}

void GameDemoScene::Update() {

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();
	ApplyGlobalVariables();

	debugCamera_->Update();

	viewProjection_.translation_ = debugCamera_->GetViewProjection()->translation_;
	viewProjection_.rotation_ = debugCamera_->GetViewProjection()->rotation_;
	viewProjection_.UpdateMatrix();

	if (input_->PressKey(DIK_A)) {
		OutputDebugStringA("Press A\n");
		audio_->SoundStopWave(&soundData1_);
	}
	if (input_->ReleaseKey(DIK_S)) {
		OutputDebugStringA("Release S\n");
	}
	if (input_->TriggerKey(DIK_D)) {
		OutputDebugStringA("Trigger D\n");
		audio_->SoundPlayWave(soundData2_, 0.1f, false);
	}

	for (int i = 0; i < 2; i++) {
		worldTransformTriangle_[i].UpdateMatrix();
		worldTransformSphere_[i].UpdateMatrix();
		worldTransformLine_[i].UpdateMatrix();
	}

	for (int i = 0; i < 3; i++) {
		worldTransformModel_[i].UpdateMatrix();
	}

	worldTransformSkyBox_.UpdateMatrix();
	worldTransformModelVAT_.UpdateMatrix();

	for (Obj& obj : objects_) {
		obj.world.UpdateMatrix();
	}

	for (Obj& obj : levelEditorObjects_) {
		obj.world.UpdateMatrix();
	}

	postEffect_->SetMaskTexture(noiseTexture_[maskTextureNum_]);
	postEffect_->SetMaskData(maskData_);

	ImGui::Begin("debug");
	ImGui::Text("GameDemoScene");
	if (ImGui::TreeNode("Triangle")) {//三角形
		ImGui::Checkbox("DrawTriangle1", &isTriangleDraw_[0]);
		ImGui::Checkbox("DrawTriangle2", &isTriangleDraw_[1]);
		if (isTriangleDraw_[0] == true) {
			if (ImGui::TreeNode("Triangle1")) {
				ImGui::DragFloat3("Translate", worldTransformTriangle_[0].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformTriangle_[0].rotation_.num, 0.05f);
				ImGui::DragFloat2("Scale", worldTransformTriangle_[0].scale_.num, 0.05f);
				ImGui::ColorEdit4("", triangleMaterial_[0].num, 0);
				ImGui::TreePop();
			}
		}
		if (isTriangleDraw_[1] == true) {
			if (ImGui::TreeNode("Triangle2")) {
				ImGui::DragFloat3("Translate", worldTransformTriangle_[1].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformTriangle_[1].rotation_.num, 0.05f);
				ImGui::DragFloat2("Scale", worldTransformTriangle_[1].scale_.num, 0.05f);
				ImGui::ColorEdit4("", triangleMaterial_[1].num, 0);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Sphere")) {//球体
		ImGui::Checkbox("DrawSphere1", &isSphereDraw_[0]);
		ImGui::Checkbox("DrawSphere2", &isSphereDraw_[1]);
		if (isSphereDraw_[0] == true) {
			if (ImGui::TreeNode("Sphere1")) {
				ImGui::DragFloat3("Translate", worldTransformSphere_[0].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformSphere_[0].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale", worldTransformSphere_[0].scale_.num, 0.05f);
				ImGui::ColorEdit4("", sphereMaterial_[0].num, 0);
				ImGui::SliderInt("ChangeTexture", &texture_[0], uvResourceNum_, uvResourceNum_ + 1);
				ImGui::TreePop();
			}
		}
		if (isSphereDraw_[1] == true) {
			if (ImGui::TreeNode("Sphere2")) {
				ImGui::DragFloat3("Translate", worldTransformSphere_[1].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformSphere_[1].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale", worldTransformSphere_[1].scale_.num, 0.05f);
				ImGui::ColorEdit4("", sphereMaterial_[1].num, 0);
				ImGui::SliderInt("ChangeTexture", &texture_[1], uvResourceNum_, uvResourceNum_ + 1);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Sprite")) {//スプライト
		ImGui::Checkbox("DrawSprite1", &isSpriteDraw_[0]);
		ImGui::Checkbox("DrawSprite2", &isSpriteDraw_[1]);
		if (isSpriteDraw_[0] == true) {
			if (ImGui::TreeNode("Sprite1")) {
				ImGui::DragFloat2("Translate", spriteTransform_[0].translate.num, 0.05f);
				ImGui::DragFloat3("Rotate", spriteTransform_[0].rotate.num, 0.05f);
				ImGui::DragFloat2("Scale", spriteTransform_[0].scale.num, 0.05f);
				ImGui::ColorEdit4("", spriteMaterial_[0].num, 0);
				ImGui::DragFloat2("uvScale", SpriteuvTransform_[0].scale.num, 0.1f);
				ImGui::DragFloat3("uvTranslate", SpriteuvTransform_[0].translate.num, 0.1f);
				ImGui::DragFloat("uvRotate", &SpriteuvTransform_[0].rotate.num[2], 0.1f);
				ImGui::TreePop();
			}
		}
		if (isSpriteDraw_[1] == true) {
			if (ImGui::TreeNode("Sprite2")) {
				ImGui::DragFloat2("Translate", spriteTransform_[1].translate.num, 0.05f);
				ImGui::DragFloat3("Rotate", spriteTransform_[1].rotate.num, 0.05f);
				ImGui::DragFloat2("Scale", spriteTransform_[1].scale.num, 0.05f);
				ImGui::ColorEdit4("", spriteMaterial_[1].num, 0);
				ImGui::DragFloat2("uvScale", SpriteuvTransform_[1].scale.num, 0.1f);
				ImGui::DragFloat3("uvTranslate", SpriteuvTransform_[1].translate.num, 0.1f);
				ImGui::DragFloat("uvRotate", &SpriteuvTransform_[1].rotate.num[2], 0.1f);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Model")) {//objモデル
		ImGui::Checkbox("DrawModel1", &isModelDraw_[0]);
		ImGui::Checkbox("DrawModel2", &isModelDraw_[1]);
		ImGui::Checkbox("DrawModel3", &isModelDraw_[2]);
		if (isModelDraw_[0] == true) {
			if (ImGui::TreeNode("Model1")) {
				ImGui::DragFloat3("Translate", worldTransformModel_[0].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformModel_[0].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale", worldTransformModel_[0].scale_.num, 0.05f);
				ImGui::TreePop();
			}
		}
		if (isModelDraw_[1] == true) {
			if (ImGui::TreeNode("Model2")) {
				ImGui::DragFloat3("Translate2", worldTransformModel_[1].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate2", worldTransformModel_[1].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale2", worldTransformModel_[1].scale_.num, 0.05f);
				ImGui::TreePop();
			}
		}
		if (isModelDraw_[2] == true) {
			if (ImGui::TreeNode("Model3")) {
				ImGui::DragFloat3("Translate3", worldTransformModel_[2].translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate3", worldTransformModel_[2].rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale3", worldTransformModel_[2].scale_.num, 0.05f);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("VAT")) {//vatモデル
		ImGui::Checkbox("DrawVAT", &isVATDraw_);
		if (isVATDraw_ == true) {
			if (ImGui::TreeNode("ModelVAT")) {
				ImGui::DragFloat3("Translate", worldTransformModelVAT_.translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformModelVAT_.rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale", worldTransformModelVAT_.scale_.num, 0.05f);
				ImGui::DragFloat("AnimTime", &vatData_.VATTime, 1.0f, 0.0f, vatData_.MaxVATTime);
				modelVAT_->SetVATTime(vatData_.VATTime);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Line")) {//ライン
		ImGui::Checkbox("DrawLine", &isLineDraw_);
		if (isLineDraw_ == true) {
			if (ImGui::TreeNode("Line")) {
				ImGui::DragFloat3("Point1", worldTransformLine_[0].translation_.num, 0.05f);
				ImGui::DragFloat3("Point2", worldTransformLine_[1].translation_.num, 0.05f);
				ImGui::DragFloat("LineThickness", &lineThickness_, 0.05f, 0.0f);
				line_->SetLineThickness(lineThickness_);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("SkyBox")) {//SkyBox
		ImGui::Checkbox("DrawSkyBox", &isSkyBoxDraw_);
		if (isSkyBoxDraw_ == true) {
			if (ImGui::TreeNode("SkyBox")) {
				ImGui::DragFloat3("Translate", worldTransformSkyBox_.translation_.num, 0.05f);
				ImGui::DragFloat3("Rotate", worldTransformSkyBox_.rotation_.num, 0.05f);
				ImGui::DragFloat3("Scale", worldTransformSkyBox_.scale_.num, 0.05f);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Particle")) {//パーティクル
		ImGui::Checkbox("DrawParticle1", &isParticleDraw_[0]);
		ImGui::Checkbox("DrawParticle2", &isParticleDraw_[1]);

		if (isParticleDraw_[0] == true) {
			if (ImGui::TreeNode("Particle1")) {
				int amount = testEmitter_[0].count;
				ImGui::Checkbox("UseBillBoard", &isBillBoard_[0]);
				ImGui::DragFloat3("EmitterTranslate", testEmitter_[0].transform.translate.num, 0.1f);
				ImGui::DragFloat3("AccelerationField", accelerationField_[0].acceleration.num, 0.1f);
				ImGui::DragFloat3("OccurrenceRangeMin", accelerationField_[0].area.min.num, 0.1f);
				ImGui::DragFloat3("OccurrenceRangeMax", accelerationField_[0].area.max.num, 0.1f);
				ImGui::DragInt("Amount", &amount, 1, 0, particle_[0]->GetkNumMaxInstance());
				testEmitter_[0].count = amount;
				ImGui::DragFloat("Frequency", &testEmitter_[0].frequency, 0.1f, 0.0f, 100.0f);
				if (ImGui::Button("isSetColor")) {
					if (isColor_[0] == false) {
						isColor_[0] = true;
					}
					else {
						isColor_[0] = false;
					}
				}
				if (isColor_[0]) {
					ImGui::ColorEdit4("Color", particleColor_[0].num, 0);
				}
				ImGui::TreePop();
			}
		}
		if (isParticleDraw_[1] == true) {
			if (ImGui::TreeNode("Particle2")) {
				int amount = testEmitter_[1].count;
				ImGui::Checkbox("UseBillBoard", &isBillBoard_[1]);
				ImGui::DragFloat3("EmitterTranslate", testEmitter_[1].transform.translate.num, 0.1f);
				ImGui::DragFloat3("AccelerationField", accelerationField_[1].acceleration.num, 0.1f);
				ImGui::DragFloat3("OccurrenceRangeMin", accelerationField_[1].area.min.num, 0.1f);
				ImGui::DragFloat3("OccurrenceRangeMax", accelerationField_[1].area.max.num, 0.1f);
				ImGui::DragInt("Amount", &amount, 1, 0, particle_[1]->GetkNumMaxInstance());
				testEmitter_[1].count = amount;
				ImGui::DragFloat("Frequency", &testEmitter_[1].frequency, 0.1f, 0.0f, 100.0f);
				if (ImGui::Button("isSetColor")) {
					if (isColor_[1] == false) {
						isColor_[1] = true;
					}
					else {
						isColor_[1] = false;
					}
				}
				if (isColor_[1]) {
					ImGui::ColorEdit4("Color", particleColor_[1].num, 0);
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("PostEffect")) {//PostEffect
		ImGui::Checkbox("DrawGrayScale", &isGrayScaleDraw_);
		ImGui::Checkbox("DrawVignette", &isVignetteDraw_);
		ImGui::Checkbox("DrawSmoothing", &isSmoothingDraw_);
		ImGui::Checkbox("DrawGaussian", &isGaussianDraw_);
		ImGui::Checkbox("DrawOutline", &isOutlineDraw_);
		ImGui::Checkbox("DrawRadialBlur", &isRadialBlurDraw_);
		ImGui::Checkbox("DrawMaskTexture", &isMaskDraw_);
		if (isMaskDraw_ == true) {
			if (ImGui::TreeNode("MaskTextureData")) {
				ImGui::DragFloat("MaskThreshold", &maskData_.maskThreshold, 0.05f, 0.0f, 1.0f);
				ImGui::DragFloat3("MaskColor", &maskData_.maskColor.num[0], 0.1f, 0.0f, 1.0f);
				ImGui::DragFloat3("EdgeColor", &maskData_.edgeColor.num[0], 0.1f, 0.0f, 1.0f);
				ImGui::DragInt("MaskTexture", &maskTextureNum_, 1.0f, 0, 1);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::Text("%f", ImGui::GetIO().Framerate);

	ImGui::InputText("BlockName", objName_, sizeof(objName_));
	if (ImGui::Button("SpawnBlock")) {
		SetObject(EulerTransform{ { 1.0f,1.0f,1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objName_);
		objCount_++;
		globalVariables->SetValue(groupName, "ObjCount", objCount_);
		for (Obj& obj : objects_) {
			globalVariables->AddItem(groupName, obj.name, (std::string)objName_);
			globalVariables->AddItem(groupName, obj.name + "Translate", obj.world.translation_);
			//globalVariables->AddItem(groupName,obj.name + "Rotate", obj.world.rotation_);
			globalVariables->AddItem(groupName, obj.name + "Scale", obj.world.scale_);
		}
	}
	if (ImGui::Button("DeleteBlock")) {
		SetObject(EulerTransform{ { 1.0f,1.0f,1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objName_);
		for (auto it = objects_.begin(); it != objects_.end();) {
			if (it->name == objName_) {
				globalVariables->RemoveItem(groupName, (std::string)objName_ + "Translate");
				globalVariables->RemoveItem(groupName, (std::string)objName_ + "Scale");
				objCount_--;
				globalVariables->SetValue(groupName, "ObjCount", objCount_);
				it = objects_.erase(it);
			}
			else {
				++it;
			}
		}
	}
	if (ImGui::Button("StartSetBlock")) {
		for (int i = 0; i < objCount_; i++) {
			SetObject(EulerTransform{ { 1.0f,1.0f,1.0f }, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} }, objNameHolder_[i]);
		}
	}

	if (ImGui::Button("LevelEditorLoadScene")) {
		LevelSetObject();
	}

	for (Obj& obj : levelEditorObjects_) {
		ImGui::Text(obj.name.c_str());
	}

	ImGui::End();

	for (int i = 0; i < 2; i++) {
		if (isParticleDraw_[i]) {
			particle_[i]->Update();
			particle_[i]->SetEmitter(testEmitter_[i]);
			particle_[i]->SetAccelerationField(accelerationField_[i]);
			particle_[i]->SetisBillBoard(isBillBoard_[i]);
			particle_[i]->SetisColor(isColor_[i]);
			if (isColor_[i] == true) {
				particle_[i]->SetColor(particleColor_[i]);
			}
		}
	}

	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (input_->PushAButton(joyState)) {
		OutputDebugStringA("A button\n");
	}
}

void GameDemoScene::Draw() {
#pragma region 背景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion

#pragma region SkyBox描画
	CJEngine_->renderer_->Draw(PipelineType::SkyBox);

	if (isSkyBoxDraw_) {//SkyBox描画
		skyBox_->Draw(worldTransformSkyBox_, viewProjection_, skyBoxMaterial_, ddsResourceNum_);
	}
#pragma endregion

#pragma region 3Dオブジェクト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard3D);

	if (isLineDraw_) {//Line描画
		line_->Draw(worldTransformLine_[0], worldTransformLine_[1], viewProjection_, lineMaterial_);

		linePoint_[0]->Draw(worldTransformLine_[0], viewProjection_, lineMaterial_, textureManager_->white);
		linePoint_[1]->Draw(worldTransformLine_[1], viewProjection_, lineMaterial_, textureManager_->white);
	}

	for (int i = 0; i < 2; i++) {
		if (isTriangleDraw_[i]) {//Triangle描画
			triangle_[i]->Draw(worldTransformTriangle_[i], viewProjection_, triangleMaterial_[i], uvResourceNum_);
		}

		if (isSphereDraw_[i]) {//Sphere描画
			sphere_[i]->Draw(worldTransformSphere_[i], viewProjection_, sphereMaterial_[i], texture_[i]);
		}
	}

	if (isModelDraw_[1]) {//Model描画
		model_[1]->Draw(worldTransformModel_[1], viewProjection_, modelMaterial_[1]);
	}
	if (isModelDraw_[2]) {//Model描画
		model_[2]->Draw(worldTransformModel_[2], viewProjection_, modelMaterial_[2]);
	}

	for (Obj& obj : objects_) {
		obj.model.Draw(obj.world, viewProjection_, obj.material);
	}

	for (Obj& obj : levelEditorObjects_) {
		obj.model.Draw(obj.world, viewProjection_, obj.material);
	}

#pragma endregion

#pragma region Skinningモデル描画
	CJEngine_->renderer_->Draw(PipelineType::Skinning);
	if (isModelDraw_[0]) {//Model描画
		model_[0]->SkinningDraw(worldTransformModel_[0], viewProjection_, modelMaterial_[0]);
	}

#pragma endregion

#pragma region VATモデル描画
	CJEngine_->renderer_->Draw(PipelineType::VertexAnimationTexture);
	if (isVATDraw_) {//VATModel描画
		modelVAT_->Draw(worldTransformModelVAT_, viewProjection_, modelMaterialVAT_);
	}

#pragma endregion

#pragma region パーティクル描画
	CJEngine_->renderer_->Draw(PipelineType::Particle);

	for (int i = 0; i < 2; i++) {
		if (isParticleDraw_[i]) {//Particle描画
			particle_[i]->Draw(viewProjection_);
		}
	}

#pragma endregion

#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);

#pragma endregion
}

void GameDemoScene::DrawUI() {
#pragma region 前景スプライト描画
	CJEngine_->renderer_->Draw(PipelineType::Standard2D);
	for (int i = 0; i < 2; i++) {
		if (isSpriteDraw_[i]) {//Sprite描画
			sprite_[i]->Draw(spriteTransform_[i], SpriteuvTransform_[i], spriteMaterial_[i]);
		}
	}

#pragma endregion
}

void GameDemoScene::DrawPostEffect() {
	CJEngine_->renderer_->Draw(PipelineType::PostProcess);
	if (isGrayScaleDraw_ == true) {
		CJEngine_->renderer_->Draw(PipelineType::Grayscale);
	}
	if (isVignetteDraw_ == true) {
		CJEngine_->renderer_->Draw(PipelineType::Vignette);
	}
	if (isSmoothingDraw_ == true) {
		CJEngine_->renderer_->Draw(PipelineType::Smoothing);
	}
	if (isGaussianDraw_ == true) {
		CJEngine_->renderer_->Draw(PipelineType::Gaussian);
	}
	if (isOutlineDraw_ == true) {
		CJEngine_->renderer_->Draw(PipelineType::Outline);
	}
	if (isRadialBlurDraw_ == true) {
		CJEngine_->renderer_->Draw(PipelineType::RadialBlur);
	}
	if (isMaskDraw_ == true) {
		CJEngine_->renderer_->Draw(PipelineType::MaskTexture);
	}
}

void GameDemoScene::Finalize() {
	audio_->SoundUnload(&soundData1_);
	audio_->SoundUnload(&soundData2_);

	objects_.clear();
}

void GameDemoScene::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "GameDemoScene";

	objCount_ = globalVariables->GetIntValue(groupName, "ObjCount");

	for (Obj& obj : objects_) {
		obj.world.translation_ = globalVariables->GetVector3Value(groupName, obj.name + "Translate");
		//obj.world.rotation_ = globalVariables->GetVector3Value(groupName,  obj.name + "Rotate");
		obj.world.scale_ = globalVariables->GetVector3Value(groupName, obj.name + "Scale");
	}
}

void GameDemoScene::SetObject(EulerTransform trans, const std::string& name) {
	Obj obj;
	obj.model.Initialize(ObjModelData_, ObjTexture_);
	obj.model.SetDirectionalLightFlag(true, 3);

	obj.world.Initialize();
	obj.world.translation_ = trans.translate;
	obj.world.rotation_ = trans.rotate;
	obj.world.scale_ = trans.scale;

	obj.material = { 1.0f,1.0f,1.0f,1.0f };

	obj.name = name;
	objects_.push_back(obj);
}

void GameDemoScene::LevelSetObject() {
	//レベルデータからオブジェクトを生成　配置
	for (auto& objectData : levelDataLoader_->GetLevelData()->objectsData_) {
		Obj obj;
		obj.model.Initialize(ObjModelData_, ObjTexture_);
		obj.model.SetDirectionalLightFlag(true, 3);

		obj.world.Initialize();
		obj.world.translation_ = objectData.transform.translate;
		obj.world.rotation_ = objectData.transform.rotate;
		obj.world.scale_ = objectData.transform.scale;

		obj.material = { 1.0f,1.0f,1.0f,1.0f };

		obj.name = objectData.name;
		levelEditorObjects_.push_back(obj);
	}
}