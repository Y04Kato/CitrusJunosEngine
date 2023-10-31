#include "OBB.h"
#include "MatrixCalculation.h"

void SetOridentatios(OBB& obb, const Matrix4x4& rotateMatrix){
	obb.oridentatios[0].num[0] = rotateMatrix.m[0][0];
	obb.oridentatios[0].num[1] = rotateMatrix.m[0][1];
	obb.oridentatios[0].num[2] = rotateMatrix.m[0][2];

	obb.oridentatios[1].num[0] = rotateMatrix.m[1][0];
	obb.oridentatios[1].num[1] = rotateMatrix.m[1][1];
	obb.oridentatios[1].num[2] = rotateMatrix.m[1][2];

	obb.oridentatios[2].num[0] = rotateMatrix.m[2][0];
	obb.oridentatios[2].num[1] = rotateMatrix.m[2][1];
	obb.oridentatios[2].num[2] = rotateMatrix.m[2][2];

}


Matrix4x4 GetRotate(const OBB& obb){
	Matrix4x4 rotateMatrix;
	rotateMatrix = MakeIdentity4x4();
	rotateMatrix.m[0][0] = obb.oridentatios[0].num[0];
	rotateMatrix.m[0][1] = obb.oridentatios[0].num[1];
	rotateMatrix.m[0][2] = obb.oridentatios[0].num[2];

	rotateMatrix.m[1][0] = obb.oridentatios[1].num[0];
	rotateMatrix.m[1][1] = obb.oridentatios[1].num[1];
	rotateMatrix.m[1][2] = obb.oridentatios[1].num[2];

	rotateMatrix.m[2][0] = obb.oridentatios[2].num[0];
	rotateMatrix.m[2][1] = obb.oridentatios[2].num[1];
	rotateMatrix.m[2][2] = obb.oridentatios[2].num[2];

	return rotateMatrix;
}