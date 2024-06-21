#pragma once
#include "Model.h"
#include "worldTransform.h"
#include "ViewProjection.h"
#include <memory>

class Ground {
public:
	void Initialize(Model* model, Vector3 translation, Vector3 Scale);
	
	void Update();
	
	void Draw(const ViewProjection& viewProjection);
	
	WorldTransform& GetWorldTransform() { return worldTransform_; }

private:
	WorldTransform worldTransform_;
	Model* groundModel_;
};
