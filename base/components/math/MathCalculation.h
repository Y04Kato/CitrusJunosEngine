#pragma once
#include "Structure.h"
#include <assert.h>
#include <cmath>
#include <numbers>
#include <algorithm>

#pragma region float
float Length(const Vector3& v);
float Dot(const Vector3& v1, const Vector3& v2);

float LengthQuaternion(const Quaternion& q);

float contangent(float b, float a);

float Lerp(float a, float b, float t);
float LerpShortAngle(float a, float b, float t);

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
Vector3 operator+=(Vector3&, Vector3&);
Vector3 operator+=(Vector3&, const Vector3&);
Vector3 operator-=(const Vector3&, const Vector3&);

//Vector3の計算
Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtruct(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(float scalar, const Vector3& v);
Vector3 Multiply(const Vector3& v1, const Vector3& v2);

//TransformNormal
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

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

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

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
Quaternion operator/(const Quaternion& q,const float t);

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

bool IsCollision(const AABB& aabb, const StructSphere& sphere);
bool IsCollision(const AABB& aabb, const Vector3& point);
bool IsCollision(const AABB& aabb1, const AABB& aabb2);

bool IsCollision(const OBB& obb, const StructSphere& sphere);