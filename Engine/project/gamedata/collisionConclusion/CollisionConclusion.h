/**
 * @file CollisionConclusion.h
 * @brief 当たり判定全般を纏めて管理
 * @author KATO
 * @date 2024/11/28
 */

#pragma once
#include "math/MathCalculation.h"
#include "2d/CreateParticle.h"
#include "TextureManager/TextureManager.h"

enum REPELLED {
	LEFT,//左側（第1引数）が弾かれる
	RIGHT,//右側（第2引数）が弾かれる
	BOTH //両方弾かれる
};

class CollisionConclusion {
public:
	void Initialize();

	void Update();

	void DrawParticle(const ViewProjection& viewProjection);

	//接触及び反発処理
	template <typename Type1, typename Type2>
	//返却する値はobj1、obj2、velocity1、velocity2の順
	std::tuple<Type1, Type2, Vector3, Vector3> Collision(Type1& obj1, Type2& obj2, Vector3& velocity1, Vector3& velocity2, REPELLED repelled);

	//指定した回数、座標が問題ないか確認し座標を返す
	Vector3 FindValidPosition();

private:
	TextureManager* textureManager_;

	// 押し戻し量の計算
	Vector3 ComputePushback(const auto& obj1, const auto& obj2, REPELLED repelled);

	// 押し戻しの適用
	void ApplyPushback(auto& obj1, auto& obj2, const Vector3& correction, REPELLED repelled);

	// 衝突後の速度修正の適用
	void ApplyVelocityCorrection(Vector3& velocity1, Vector3& velocity2, const Vector3& newVel1, const Vector3& newVel2, REPELLED repelled);


	//接触時の音量計算
	void ContactVolume(Vector3 velocity);

	//他オブジェクトに接触しているか確認
	bool IsValidPosition(const Vector3 pos);

	//押し戻しの倍率
	float pushbackMultiplier_ = 2.0f;
	float pushbackMultiplierObj_ = 1.5f;

	//反発係数
	float repulsionCoefficient_ = 1.2f;

	//CollisionParticle
	std::unique_ptr<CreateParticle> collisionParticle_;
	Emitter collisionEmitter_{};
	AccelerationField collisionAccelerationField_;
	uint32_t collisionParticleResource_;//パーティクルリソース
	const int collisionParticleOccursNum_ = 10;//パーティクルの発生数
};
