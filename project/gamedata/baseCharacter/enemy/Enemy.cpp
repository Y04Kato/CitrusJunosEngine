#include "Enemy.h"
#include "GlobalVariables.h"

void Enemy::Initialize(const std::vector<Model*>& models) {
	BaseCharacter::Initialize(models);

	models_[kModelIndexBody] = models[kModelIndexBody];
	models_[kModelIndexHead] = models[kModelIndexHead];
	models_[kModelIndexL_arm] = models[kModelIndexL_arm];
	models_[kModelIndexR_arm] = models[kModelIndexR_arm];
	models_[kModelIndexWeapon] = models[kModelIndexWeapon];
	input_ = Input::GetInstance();

	//worldTransform_.Initialize();
	InitializeFloatGimmick();

	worldTransform_.translation_.num[1] = 5.0f;
	worldTransformBody_.translation_ = { pos_.num[0],pos_.num[1],pos_.num[2] };
	worldTransformHead_.translation_ = { 0.0f, 1.0f, 0.0f };

	RGBA = { 1.0f,1.0f,1.0f,1.0f };

	worldTransform_.Initialize();
	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();

	SetParent(&GetWorldTransformBody());
	SetCollisionAttribute(CollisionConfig::kCollisionAttributeEnemy);
	SetCollisionMask(~CollisionConfig::kCollisionAttributeEnemy);
	move_ = { 0.2f,0.0f,0.0f };

	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "Enemy";
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", 90);
	globalVariables->AddItem(groupName, "Head Translation", worldTransformHead_.translation_);
	globalVariables->AddItem(groupName, "L_Arm Translation", worldTransformL_arm_.translation_);
	globalVariables->AddItem(groupName, "R_Arm Translation", worldTransformR_arm_.translation_);
}

void Enemy::Update() {
	ApplyGlobalVariables();

	if (isFly == true) {
		flyTimer++;
		RGBA.num[3] -= 0.05f;
		worldTransformBody_.translation_.num[0] += velocity_.num[0] * 1.5f;
		worldTransformBody_.translation_.num[1] += velocity_.num[1] + 2.0f * 1.5f;
		worldTransformBody_.translation_.num[2] += velocity_.num[2] * 1.5f;
	}

	if (flyTimer >= 90) {
		flyTimer = 0;
		isFly = false;
		isDead = true;
	}

	if (isDead == true) {
		deadTimer++;
	}

	if (deadTimer >= 120) {
		isDead = false;
		deadTimer = 0;
		worldTransformBody_.translation_ = { pos_.num[0],pos_.num[1],pos_.num[2] };
		RGBA.num[3] = 1.0f;
	}

	structSphere_.center = worldTransformBody_.GetWorldPos();
	structSphere_.radius = 1.5f;

	Obb_.center = worldTransformBody_.GetWorldPos();
	GetOrientations(MakeRotateXYZMatrix(worldTransformBody_.rotation_), Obb_.orientation);
	Obb_.size = worldTransformBody_.scale_;

	UpdateFloatGimmick();
	//Move();
	ModelUpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	if (isDead == false) {
		models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection, RGBA);
		models_[kModelIndexHead]->Draw(worldTransformHead_, viewProjection, RGBA);
		models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection, RGBA);
		models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection, RGBA);
	}
}

void Enemy::OnCollision() {
}

void Enemy::Move() {
	worldTransformBody_.translation_ = Add(worldTransformBody_.translation_, move_);
	if (worldTransformBody_.translation_.num[0] >= 6.0f || worldTransformBody_.translation_.num[0] <= -6.0f) {
		move_ = Multiply(-1.0f, move_);
	}
}

void Enemy::SetParent(const WorldTransform* parent) {
	worldTransformBase_.parent_ = parent;
	worldTransformHead_.parent_ = parent;
	worldTransformR_arm_.parent_ = parent;
	worldTransformL_arm_.parent_ = parent;
	worldTransform_.parent_ = parent;
}

void Enemy::ModelUpdateMatrix() {
	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();

}

void Enemy::InitializeFloatGimmick() {
	floatingParametor_ = 0.0f;
}

void Enemy::UpdateFloatGimmick() {
	uint16_t T = 120;

	float step = 2.0f * (float)M_PI / T;
	float floatingAmplitude = 0.3f;

	floatingParametor_ += step;
	floatingParametor_ = (float)std::fmod(floatingParametor_, 2.0f * M_PI);

	worldTransformBody_.translation_.num[1] = std::sin(floatingParametor_) * floatingAmplitude + 1.0f;

	worldTransformL_arm_.rotation_.num[0] = std::sin(floatingParametor_) * 0.75f;
	worldTransformR_arm_.rotation_.num[0] = std::sin(floatingParametor_) * 0.75f;
}

void Enemy::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Enemy";

	worldTransformHead_.translation_ =
		globalVariables->GetVector3Value(groupName, "Head Translation");
	worldTransformL_arm_.translation_ =
		globalVariables->GetVector3Value(groupName, "L_Arm Translation");
	worldTransformR_arm_.translation_ =
		globalVariables->GetVector3Value(groupName, "R_Arm Translation");
}
