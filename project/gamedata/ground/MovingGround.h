#pragma once
#include "Model.h"
#include "worldTransform.h"
#include "ViewProjection.h"
#include <memory>

class MovingGround {
public:
	void Initialize(Model* model, Vector3 translation, Vector3 Scale);
	void Update();
	void Draw(const ViewProjection& viewProjection);
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	void Move();
private:
	WorldTransform worldTransform_;
	Model* groundModel_;

	Vector3 move_;
};

