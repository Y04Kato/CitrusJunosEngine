#pragma once
#include "Vector.h"

struct OBB{
	Vector3 center;
	Vector3 oridentatios[3];
	Vector3 size;
};

void SetOridentatios(OBB& obb, const Matrix4x4& rotateMatrix);
Matrix4x4 GetRotate(const OBB& obb);