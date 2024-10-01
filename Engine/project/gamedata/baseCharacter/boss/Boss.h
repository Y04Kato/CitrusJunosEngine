#pragma once
#include "baseCharacter/BaseCharacter.h"

class Boss : public BaseCharacter {
public:
	void Initialize(Model* model) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

	WorldTransform& GetWorldTransform() { return worldTransform_; }

protected:

};
