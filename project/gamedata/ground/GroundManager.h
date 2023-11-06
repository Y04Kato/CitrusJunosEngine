#pragma once
#include "Ground.h"
#include "MovingGround.h"

#include "Model.h"
#include "worldTransform.h"
#include "ViewProjection.h"
#include <memory>

class GroundManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	OBB GetOBB(int num) { return Obb_[num]; }
	MovingGround* GetMoveGround() { return moveGround_.get(); }
	Ground* GetGround(int num) { return ground_->get(); }
private:

	std::unique_ptr<Ground>ground_[2];
	std::unique_ptr<MovingGround> moveGround_;
	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> movemodel_;
	OBB Obb_[3];
};

