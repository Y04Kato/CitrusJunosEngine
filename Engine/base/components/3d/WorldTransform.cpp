/**
 * @file WorldTransform.cpp
 * @brief WorldTransformの初期化及び管理を行う
 * @author KATO
 * @date 未記録
 */

#include "WorldTransform.h"

void WorldTransform::Initialize(){
	matWorld_ = MakeIdentity4x4();
	CreateConstBuffer(1);
	Map();
	TransferMatrix();
}

void WorldTransform::Initialize(uint32_t numInstance) {
	matWorld_ = MakeIdentity4x4();
	CreateConstBuffer(numInstance);
	Map();
	TransferMatrix();
}

void WorldTransform::CreateConstBuffer(uint32_t numInstance){
	constBuff_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice().Get(), sizeof(ConstBufferDataWorldTransform) * numInstance);

}

void WorldTransform::Map(){
	constBuff_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&constMap));

}

void WorldTransform::TransferMatrix(){
	Matrix4x4 rotationScaleMatrix = matWorld_;
	rotationScaleMatrix.m[0][3] = 0.0f;
	rotationScaleMatrix.m[1][3] = 0.0f;
	rotationScaleMatrix.m[2][3] = 0.0f;
	rotationScaleMatrix.m[3][3] = 1.0f;

	constMap->matWorld = matWorld_;
	constMap->inverseTranspose = Transpose(Inverse(rotationScaleMatrix));
}

void WorldTransform::UpdateMatrix(){
	Matrix4x4 AffineMatrix = MakeAffineMatrix(scale_, rotation_, translation_);
	matWorld_ = AffineMatrix;
	//親があれば親のワールド行列を掛ける
	if (parent_) {
		matWorld_ = Multiply(matWorld_, parent_->matWorld_);
	}

	TransferMatrix();
}

void WorldTransform::UpdateQuaternionMatrix() {
	Matrix4x4 quart_ = MakeRotateMatrix(quaternion_);
	Matrix4x4 Afine = MakeQuatAffineMatrix(scale_, quart_, translation_);
	matWorld_ = Afine;
	if (parent_) {
		matWorld_ = Multiply(matWorld_, parent_->matWorld_);
	}
	TransferMatrix();
}

Vector3 WorldTransform::GetWorldPos(){
	return { matWorld_.m[3][0],matWorld_.m[3][1],matWorld_.m[3][2] };
}