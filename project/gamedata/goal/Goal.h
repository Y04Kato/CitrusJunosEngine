#pragma once
#include "Collider.h"
#include "CollisionConfig.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "CreateSphere.h"
#include "components/manager/TextureManager.h"

class Goal :public Collider {
public:
	void Initialize(Vector3 transform, Vector3 scale);
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void OnCollision() override;
	WorldTransform GetWorldTransform()override { return worldTransform_; }
private:
	WorldTransform worldTransform_;
	TextureManager* textureManager_;
	std::unique_ptr<CreateSphere> sphere_;

	uint32_t uvResourceNum_;
};
