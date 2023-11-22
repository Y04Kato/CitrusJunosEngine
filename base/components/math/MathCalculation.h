#pragma once
#include "Structure.h"
#include <assert.h>
#include <cmath>
#include <numbers>
#include <algorithm>

float Length(const Vector3& v);
float Dot(const Vector3& v1, const Vector3& v2);

float LengthQuaternion(const Quaternion& q);

//演算子オーバーロード
Matrix4x4 operator+(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 operator-(Matrix4x4 m1, Matrix4x4 m2);
Vector3 operator*(const Vector3& v, const Matrix4x4& matrix);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator+=(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 operator-=(Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*=(Matrix4x4& m1, const Matrix4x4& m2);

Vector2 operator+(const Vector2&, const Vector2&);
Vector2 operator-(const Vector2&, const Vector2&);
Vector2 operator*(float k, const Vector2& v);
Vector2 operator*(const Vector2& v, float k);

Vector3 operator+(const Vector3&, const Vector3&);
Vector3 operator-(const Vector3&, const Vector3&);
Vector3 operator*(float k, const Vector3& v);
Vector3 operator*(const Vector3& v, float k);

Vector3 operator-(const Vector3& v);

Vector3 operator+=(Vector3&, Vector3&);
Vector3 operator+=(Vector3&, const Vector3&);
Vector3 operator-=(const Vector3&, const Vector3&);

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

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

//行列の減法
Matrix4x4 Subtruct(const Matrix4x4& m1, const Matrix4x4& m2);

//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m1);

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

//単位行列
Matrix4x4 MakeIdentity4x4();

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRadio, float nearClip, float farClip);

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// TransformNormal
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

Vector3 TransformN(const Vector3& v, const Matrix4x4& m);

Vector3 Normalize(const Vector3& v);

Vector3 GetXAxis(const Matrix4x4& m);

Vector3 GetYAxis(const Matrix4x4& m);

Vector3 GetZAxis(const Matrix4x4& m);

void GetOrientations(const Matrix4x4& m, Vector3 orientations[3]);

Matrix4x4& SetTranslate(Matrix4x4& m, const Vector3& v);

Matrix4x4 MakeInverseMatrix(const Matrix4x4& rotate, const Vector3& translate);

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);

Matrix4x4 MakeRotateMatrixFromOrientations(const Vector3 orientations[3]);

bool IsCollision(const AABB& aabb, const StructSphere& sphere);
bool IsCollision(const AABB& aabb, const Vector3& point);
bool IsCollision(const AABB& aabb1, const AABB& aabb2);

bool IsCollision(const OBB& obb, const StructSphere& sphere);

Vector4 MakeQuaternion(Vector3 axis, float radian);

Quaternion createQuaternion(float Radian, Vector3 axis);

//クォータニオンからオイラー角への変換
Vector3 quaternionToEulerAngles(const Quaternion& quat);

Matrix4x4 MakeQuatAffineMatrix(const Vector3& scale, const Matrix4x4& rotate, const Vector3& translate);

Matrix4x4 quaternionToMatrix(const Quaternion& quat);

Vector3 matrixToEulerAngles(const Matrix4x4 mat);

Quaternion Normalize(const Quaternion& q);

Vector3 extractEulerAnglesFromRotationMatrix(const Matrix4x4& rotationMatrix);

Vector3 GetRightVectorFromModelMatrix(const Matrix4x4& modelMatrix);

Vector3 GetUpVectorFromModelMatrix(const Matrix4x4& modelMatrix);

Vector3 GetFrontVectorFromModelMatrix(const Matrix4x4& modelMatrix);

Vector3 rotateVectorAndQuaternion(const Quaternion& q, const Vector3& v);

Quaternion Multiply(const Quaternion& q1, const Quaternion& q2);

bool CompereQuaternion(const Quaternion& q1, const Quaternion& q2);

bool CompereVector3(const Vector3& q1, const Vector3& q2);

//Vector2の計算
Vector2 Add(const Vector2& v1, const Vector2& v2);
Vector2 Subtruct(const Vector2& v1, const Vector2& v2);
Vector2 Multiply(float scalar, const Vector2& v);

//Vector3の計算
Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtruct(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(float scalar, const Vector3& v);

Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

float Lerp(float a, float b, float t);
float LerpShortAngle(float a, float b, float t);

Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const float t, const Quaternion& q);

Quaternion Lerp(float t, const Quaternion& s, const Quaternion& e);
Quaternion Slerp(float t, const Quaternion& s, const Quaternion& e);