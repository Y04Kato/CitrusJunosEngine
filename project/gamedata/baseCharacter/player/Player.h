#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "baseCharacter/BaseCharacter.h"
#include <optional>
#include "Collider.h"
#include "CollisionConfig.h"

class LockOn;
struct ConstAttack {
	uint32_t anticipationTime;//振りかぶりの時間
	uint32_t chargeTime;//溜めの時間
	uint32_t swingTime;//振る時間
	uint32_t recoveryTime;//硬直時間
	float anticipationSpeed;//振りかぶりの移動速さ
	float chargeSpeed;//溜めの早さ
	float swingSpeed;//振りの早さ
};

static const float anticipationRotate = 3.642f;
static const float anticipationRotateHammer = 0.5f;

static const float swingRotate = 1.542f;
static const float swingRotateHammer = -1.6f;

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
	
	OBB GetOBB() { return obb_; };

	//コンボ数
	static const int comboNum_ = 3;
	static const std::array<ConstAttack, comboNum_>kConstAttacks_;

	Vector3 GetVelocity() { return preMove_; }
	int32_t GetComboIndex() { return workAtack_.comboIndex; }

	void SetLockOn(LockOn* lock) { LockOn_ = lock; }

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

	float animationFrame_;

	StructSphere structSphere_;
	bool gameOver = false;
	WorldTransform objectPos_;

	OBB obb_;

	bool isAttack = false;
	bool isAttack2 = true;

	float angle;

	struct WorkDash {
		uint32_t dashParameter_ = 0;
	};

	WorkDash workDash_;

	struct WorkAtack {
		uint32_t Time;
		float rotate;
		float hammerRotate;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
	};

	WorkAtack workAtack_;
	
	Quaternion quaternion_;
	Vector3 preMove_;
	Quaternion preQuaternion_;

	bool isMove_ = false;

	float dashSpeed = 1.8f;

	Vector3 velocity_ = {};

	bool aButtonPressed = false;

	LockOn* LockOn_;
	XINPUT_STATE prejoy;
};
