#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <Model.h>
#include <vector>

class BaseCharacter {
public:
	virtual void Initialize(Model* model);

	virtual void Update();

	virtual void Draw(const ViewProjection& viewProjection);

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

protected:
	Model* model_;
	WorldTransform worldTransform_;
};
