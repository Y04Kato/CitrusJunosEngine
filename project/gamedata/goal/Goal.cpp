#include "Goal.h"

void Goal::Initialize(Vector3 transform, Vector3 scale) {
	worldTransform_.Initialize();
	textureManager_ = TextureManager::GetInstance();
	uvResourceNum_ = textureManager_->Load("project/gamedata/resources/uvChecker.png");
	sphere_ = std::make_unique<CreateSphere>();
	sphere_->Initialize();
	worldTransform_.translation_ = transform;
	worldTransform_.scale_ = scale;
	SetCollisionAttribute(CollisionConfig::kCollisionAttributeEnemy);
	SetCollisionMask(~CollisionConfig::kCollisionAttributeEnemy);
}

void Goal::Update() {
	worldTransform_.UpdateMatrix();
}

void Goal::Draw(const ViewProjection& viewProjection) {
	sphere_->Draw(worldTransform_, viewProjection, Vector4{ 1.0f,1.0f,1.0f,1.0f }, uvResourceNum_);
}

void Goal::OnCollision() {
}
