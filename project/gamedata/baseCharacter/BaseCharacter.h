#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <Model.h>
#include <vector>

enum ModelParts {
	kModelIndexBody,
	kModelIndexHead,
	kModelIndexL_arm,
	kModelIndexR_arm,
	kModelIndexWeapon
};

class BaseCharacter {
public:
	virtual void Initialize(const std::vector<Model*>& models);

	virtual void Update();

	virtual void Draw(const ViewProjection& viewProjection);

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

protected:
	std::vector<Model*> models_;
	WorldTransform worldTransform_;
};

