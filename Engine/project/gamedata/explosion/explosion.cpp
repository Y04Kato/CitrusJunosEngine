#include "explosion.h"

void Explosion::Initialize() {
	input_ = Input::GetInstance();
	for (int i = 0; i < Max; i++) {
		model_[i] = Model::CreateModel("project/gamedata/resources/block", "block.obj");
		worldTransform_[i].Initialize();
		worldTransform_[i].scale_ = { 0.2f,0.2f,0.2f };
	}
	modelMaterial_ = { 1.0f,1.0f,1.0f,1.0f };

	worldTransformBase_.Initialize();

	audio_ = Audio::GetInstance();

	soundData1_ = audio_->SoundLoad("project/gamedata/resources/break.mp3");
}

void Explosion::Update() {
	for (int i = 0; i < Max; i++) {
		transform_[i].translate.num[1] += bounceSpeed_;
	}
	bounceSpeed_ += acceleration_;

	if (startFlag == true) {
		time_ += 0.05f;
		if (time_ >= 5.0f) {
			startFlag = false;
			drawFlag = false;
		}
	}

	if (testFlag == true) {
		for (int i = 0; i < Max; i++) {
			worldTransform_[i].translation_ = worldTransformBase_.translation_;
			transform_[i].translate.num[0] = worldTransformBase_.translation_.num[0] + rand() % 6 - 3 + rand() / (float)RAND_MAX;
			transform_[i].translate.num[1] = worldTransformBase_.translation_.num[1];
			transform_[i].translate.num[2] = worldTransformBase_.translation_.num[2] + rand() % 4 - 2 + rand() / (float)RAND_MAX;
		}
		testFlag = false;
	}

	for (int i = 0; i < Max; i++) {
		worldTransform_[i].translation_.num[0] = (1.0f - time_) * worldTransformBase_.translation_.num[0] + time_ * transform_[i].translate.num[0];
		worldTransform_[i].translation_.num[1] = transform_[i].translate.num[1];
		worldTransform_[i].translation_.num[2] = (1.0f - time_) * worldTransformBase_.translation_.num[2] + time_ * transform_[i].translate.num[2];

		worldTransform_[i].UpdateMatrix();
	}
}

void Explosion::Draw(const ViewProjection& viewProjection) {
	if (drawFlag == true) {
		for (int i = 0; i < Max; i++) {
			model_[i]->Draw(worldTransform_[i], viewProjection, modelMaterial_);
		}
	}
}

void Explosion::ExplosionFlagTrue() {
	audio_->SoundPlayWave(soundData1_, 0.1f, false);
	bounceSpeed_ = 1.2f;
	time_ = 0.0f;
	startFlag = true;
	testFlag = true;
	drawFlag = true;
	for (int i = 0; i < Max; i++) {
		worldTransform_[i].scale_.num[0] = (worldTransformBase_.scale_.num[0] + worldTransformBase_.scale_.num[1] + worldTransformBase_.scale_.num[2]) / 3.0f / 4.0f;
		worldTransform_[i].scale_.num[1] = (worldTransformBase_.scale_.num[0] + worldTransformBase_.scale_.num[1] + worldTransformBase_.scale_.num[2]) / 3.0f / 4.0f;
		worldTransform_[i].scale_.num[2] = (worldTransformBase_.scale_.num[0] + worldTransformBase_.scale_.num[1] + worldTransformBase_.scale_.num[2]) / 3.0f / 4.0f;
	}
}