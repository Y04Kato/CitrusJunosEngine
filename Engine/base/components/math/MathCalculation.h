/**
 * @file MathCalculation.h
 * @brief 数学系の関数や演算子をまとめた部分
 * @author KATO
 * @date 未記録
 */

#pragma once
#include "Structure.h"
#include <assert.h>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h> 
#include <numbers>
#include <algorithm>

#pragma region float
float Length(const Vector3& v);
float Dot(const Vector3& v1, const Vector3& v2);
float Magnitude(const Vector3& v);

float LengthQuaternion(const Quaternion& q);

float Contangent(float b, float a);

float Distance(const Vector3& v1, const Vector3& v2);

float Lerp(float a, float b, float t);
float LerpShortAngle(float a, float b, float t);

//度数からラジアンを計算する関数
float DegreesToRadians(float degrees);

//回転角度を -180度～180度の範囲に正規化する関数
float NormalizeAngle(float angle);

//ベクトルの距離の2乗を計算
float DistanceSquared(const Vector3& a, const Vector3& b);

#pragma endregion

#pragma region Vector2
//演算子オーバーロード
Vector2 operator+(const Vector2&, const Vector2&);
Vector2 operator-(const Vector2&, const Vector2&);
Vector2 operator*(float k, const Vector2& v);
Vector2 operator*(const Vector2& v, float k);

//Vector2の計算
Vector2 Add(const Vector2& v1, const Vector2& v2);
Vector2 Subtruct(const Vector2& v1, const Vector2& v2);
Vector2 Multiply(float scalar, const Vector2& v);

float Cross(Vector2 a, Vector2 b);

Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);

#pragma endregion

#pragma region Vector3
//演算子オーバーロード
Vector3 operator+(const Vector3&, const Vector3&);
Vector3 operator-(const Vector3&, const Vector3&);
Vector3 operator-(const Vector3& v);
Vector3 operator*(float k, const Vector3& v);
Vector3 operator*(const Vector3& v, float k);
Vector3 operator*(const Vector3& v1, const Vector3& v2);
Vector3 operator*(const Vector3& v, const Matrix4x4& matrix);
Vector3 operator*(const Matrix4x4& matrix , const Vector3& v);
Vector3 operator/(const Vector3& v, float k);
Vector3 operator+=(Vector3&, Vector3&);
Vector3 operator+=(Vector3&, const Vector3&);
Vector3 operator-=(Vector3&, const Vector3&);

//Vector3の計算
Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtruct(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(float scalar, const Vector3& v);
Vector3 Multiply(const Vector3& v1, const Vector3& v2);
Vector3 ScalarMultiply(const Vector3& vec, float scalar);
Vector3 Division(float scalar, const Vector3& v);

//TransformNormal
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

//World→Screen
Vector3 TransformN(const Vector3& v, const Matrix4x4& m);

Vector3 Normalize(const Vector3& v);

Vector3 GetXAxis(const Matrix4x4& m);

Vector3 GetYAxis(const Matrix4x4& m);

Vector3 GetZAxis(const Matrix4x4& m);

void GetOrientations(const Matrix4x4& m, Vector3 orientations[3]);

Vector3 Cross(const Vector3& v1, const Vector3& v2);

float Angle(Vector3 from, Vector3 to);

Vector3 matrixToEulerAngles(const Matrix4x4 mat);

Vector3 extractEulerAnglesFromRotationMatrix(const Matrix4x4& rotationMatrix);

Vector3 GetRightVectorFromModelMatrix(const Matrix4x4& modelMatrix);

Vector3 GetUpVectorFromModelMatrix(const Matrix4x4& modelMatrix);

Vector3 GetFrontVectorFromModelMatrix(const Matrix4x4& modelMatrix);

bool CompereVector3(const Vector3& q1, const Vector3& q2);

Vector3 MidPoint(const Vector3& v1, const Vector3& v2);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

//Vector3型に対応したクランプ
Vector3 Clamp(const Vector3& value, const Vector3& min, const Vector3& max);

//射影
Vector3 Project(const Vector3& v, const Vector3 n);

//速度から回転を求める
Vector3 ComputeRotationFromVelocity(const Vector3& velocity, float rotationMultiplier);

//OBBのorientation[3]を算出
std::tuple<Vector3, Vector3, Vector3> ComputeRotationMatrix(const Vector3& rotate);

//衝突＆反発
//2つの速度を持つ物体が衝突した時の反発
std::pair<Vector3, Vector3> ComputeCollisionVelocities(float mass1, const Vector3& velocity1, float mass2, const Vector3& velocity2, float coefficientOfRestitution, const Vector3& normal);

//Obbは反発せず、Sphereだけ反発する処理
Vector3 ComputeSphereVelocityAfterCollisionWithOBB(const StructSphere& sphere, const Vector3& sphereVelocity, const OBB& obb, float restitution);

//Sphereは反発せず、Obbだけ反発する処理
Vector3 ComputeVelocitiesAfterCollisionWithOBB(const StructSphere& sphere, const Vector3& sphereVelocity, float sphereMass, const OBB& obb, const Vector3& obbVelocity, float obbMass, float restitution);

//Cylinderは反発せず、Sphereだけ反発する処理
Vector3 ComputeSphereVelocityAfterCollisionWithCylinder(const StructSphere& sphere, const Vector3& sphereVelocity, const StructCylinder& cylinder, float restitution);

//Cylinder、Sphere両方反発する処理
std::pair<Vector3, Vector3> ComputeSphereCylinderCollision(const StructSphere& sphere, const Vector3& sphereVelocity, float sphereMass, const StructCylinder& cylinder, const Vector3& cylinderVelocity, float cylinderMass, float restitution);

//Cylinder、OBBの反発処理
std::pair<Vector3, Vector3> ComputeOBBCylinderCollisionVelocities(float obbMass, const Vector3& obbVelocity, const OBB& obb, float cylinderMass, const Vector3& cylinderVelocity, const StructCylinder& cylinder, float coefficientOfRestitution);

//Cylinder同士の反発処理
std::pair<Vector3, Vector3> ComputeCollisionVelocities(const StructCylinder& cylinder1, const Vector3& velocity1, const StructCylinder& cylinder2, const Vector3& velocity2, float restitution);

//OBBの最近接点を計算
Vector3 ClosestPointOnOBB(const OBB& obb, const Vector3& point);

//円柱の最近接点を計算
Vector3 ClosestPointOnCylinder(const StructCylinder& cylinder, const Vector3& point);

//任意の時刻の値を取得する
Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);
Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time);

#pragma endregion

#pragma region Vector4
Vector4 operator*(const Vector4& v, float k);

Vector4 Multiply(float scalar, const Vector4& v);

Vector4 MultiplyMatrixVector(const Matrix4x4& matrix, const Vector4& vector);

#pragma endregion

#pragma region Matrix4x4
//演算子オーバーロード
Matrix4x4 operator+(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 operator-(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator+=(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 operator-=(Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*=(Matrix4x4& m1, const Matrix4x4& m2);

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
//行列の減法
Matrix4x4 Subtruct(const Matrix4x4& m1, const Matrix4x4& m2);
//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

//X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

//Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

//Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

//軸回転行列
Matrix4x4 MakeRotateMatrix(Vector3 theta);

Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotate);

//平行移動
Matrix4x4 MakeTranslateMatrix(Vector3 translate);

//拡大縮小
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// アフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m1);

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

//単位行列
Matrix4x4 MakeIdentity4x4();

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRadio, float nearClip, float farClip);

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

Matrix4x4& SetTranslate(Matrix4x4& m, const Vector3& v);

Matrix4x4 MakeInverseMatrix(const Matrix4x4& rotate, const Vector3& translate);

Matrix4x4 MakeRotateMatrixFromOrientations(const Vector3 orientations[3]);

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, const float cos, const float sin);

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

#pragma endregion

#pragma region Quaternion
//演算子オーバーロード
Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const float t, const Quaternion& q);
Quaternion operator/(const Quaternion& q, const float t);

Vector4 MakeQuaternion(Vector3 axis, float radian);

//任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(Vector3 axis, float Radian);

//クォータニオンからオイラー角への変換
Vector3 QuaternionToEulerAngles(const Quaternion& quat);

Matrix4x4 MakeQuatAffineMatrix(const Vector3& scale, const Matrix4x4& rotate, const Vector3& translate);

//Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& quat);

//ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Quaternion& q, const Vector3& v);

Vector3 RotateVectorAndQuaternion(const Quaternion& q, const Vector3& v);

//積
Quaternion Multiply(const Quaternion& q1, const Quaternion& q2);

//単位Quaternion
Quaternion IdentityQuaternion();

//共役Quaternion
Quaternion Conjugate(const Quaternion& q);

//norm
float Norm(const Quaternion& q);

//正規化Quaternion
Quaternion Normalize(const Quaternion& q);

//逆Quaternion
Quaternion Inverse(const Quaternion& q);

bool CompereQuaternion(const Quaternion& q1, const Quaternion& q2);

Quaternion Lerp(float t, const Quaternion& s, const Quaternion& e);
Quaternion Slerp(float t, const Quaternion& s, const Quaternion& e);

#pragma endregion

EulerTransform operator+(const EulerTransform& v1, const EulerTransform& v2);
EulerTransform operator-(const EulerTransform& v1, const EulerTransform& v2);

//EulerTransformからOBBを算出
OBB CreateOBBFromEulerTransform(const EulerTransform& transform);

bool IsCollision(const AABB& aabb, const StructSphere& sphere);
bool IsCollision(const AABB& aabb, const Vector3& point);
bool IsCollision(const AABB& aabb, const Segment& segment);
bool IsCollision(const AABB& aabb1, const AABB& aabb2);

bool IsCollision(const OBB& obb, const StructSphere& sphere);
bool IsCollision(const OBB& obb, const Segment& segment);

bool IsCollision(const StructSphere& sphere1, const StructSphere& sphere2);

bool IsCollision(const StructSphere& sphere, const StructCylinder& cylinder);
bool IsCollision(const AABB& aabb, const StructCylinder& cylinder);
bool IsCollision(const OBB& obb, const StructCylinder& cylinder);
bool IsCollision(const StructCylinder& cylinder1, const StructCylinder& cylinder2);