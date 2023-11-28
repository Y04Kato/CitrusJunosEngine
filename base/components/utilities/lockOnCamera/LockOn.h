#pragma once
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "CreateSprite.h"
#include "TextureManager.h"

class Enemy;
class LockOn{
public:
	void Initialize();
	void Update(const std::list<Enemy*>& enemys, const ViewProjection& viewProjection);
	void Draw();
	void Target();
	void Search(const std::list<Enemy*>& enemys, const ViewProjection& viewProjection);
	void Reset();
	Vector3 GetTargetPos() const;
	bool isTarget();
	bool Existtarget()const { return target_ ? true : false; }
	bool isRangeOut(const ViewProjection& viewProjection);

private:
	XINPUT_STATE preInputPad;
	Enemy* target_ = nullptr;

	float minDistance_ = 0.0f;
	float maxDistance_ = 30.0f;
	float angleRange_ = 50.0f * 3.14159265f / 180.0f;
	bool isLockOn_;
	int count_;
	bool isAuto = true;
	std::list<std::pair<float, Enemy*>>targets;
	int iteratornum;
	int max = 0;
	Vector3 WorldToScreen(Vector3 world, const ViewProjection& viewProjection);

	TextureManager* textureManager_;
	std::unique_ptr <CreateSprite> sprite_;
	Transform spriteTransform_;
	Transform SpriteuvTransform_;
	Vector4 spriteMaterial_;
	int tex_;
};
