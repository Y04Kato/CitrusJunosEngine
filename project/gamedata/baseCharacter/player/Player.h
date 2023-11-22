#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "baseCharacter/BaseCharacter.h"
#include <optional>
#include "Collider.h"
#include "CollisionConfig.h"

class Player : public BaseCharacter, public Collider {
public:
	void Initialize(const std::vector<Model*>& models) override;

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

	WorldTransform GetWorldTransform()override { return worldTransformBody_; }
	const WorldTransform& GetWorldtransformBase() { return worldTransformBase_; }
	const WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

	void InitializeFloatingGimmick();
	void UpdateFloatingGimmick();

	void BehaviorRootInitialize();
	void BehaviorRootUpdate();

	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	void BehaviorDashInitialize();
	void BehaviorDashUpdate();

	void BehaviorJumpInitialize();
	void BehaviorJumpUpdate();

	void ApplyGlobalVariables();

	void SetParent(const WorldTransform* parent);
	void SetWorldTransform(const Vector3 translation);

	void IsFallStart();
	StructSphere GetStructSphere() { return structSphere_; }
	bool isHit_;
	bool isGameover() { return gameOver; }
	void OnCollision() override;
	void Setparent(const WorldTransform* parent);
	void IsCollision(const WorldTransform& worldtransform);
	void DeleteParent();
	void SetObjectPos(const WorldTransform& worldtransform);

	bool GetIsAttack() { return isAttack; }
	
	OBB GetOBB() { return obb_; }

	struct ConstAttack {
		uint32_t anticipationTime;//振りかぶりの時間
		uint32_t chargeTime;//溜めの時間
		uint32_t swingTime;//振る時間
		uint32_t recoveryTime;//硬直時間
		float anticipationSpeed;//振りかぶりの移動速さ
		float chargeSpeed;//溜めの早さ
		float swingSpeed;//振りの早さ
	};

	//コンボ数
	static const int conboNum_ = 3;
	static const std::array<ConstAttack, conboNum_>kConstAttacks_;

private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	WorldTransform worldTransformWeapon_;

	Model* modelBody_ = nullptr;
	Model* modelHead_ = nullptr;
	Model* modelL_arm_ = nullptr;
	Model* modelR_arm_ = nullptr;
	Model* modelWeapon_ = nullptr;

	Input* input_ = nullptr;

	const ViewProjection* viewProjection_ = nullptr;

	float floatingParameter_ = 0.0f;

	int32_t floatingCycle_ = 120;

	float floatingAmplitude_ = 0.3f;

	enum class Behavior { kRoot, kAttack, kDash , kJump};

	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	int animationFrame_;

	StructSphere structSphere_;
	bool gameOver = false;
	WorldTransform objectPos_;

	OBB obb_;

	bool isAttack = false;

	float angle;

	struct WorkDash {
		uint32_t dashParameter_ = 0;
	};

	WorkDash workDash_;
	
	Quaternion quaternion_;
	Vector3 preMove_;
	Quaternion preQuaternion_;

	bool isMove_ = false;

	float dashSpeed = 1.8f;

	Vector3 velocity_ = {};

	bool aButtonPressed = false;
};
