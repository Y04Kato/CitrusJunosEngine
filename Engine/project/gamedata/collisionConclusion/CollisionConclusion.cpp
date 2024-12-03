/**
 * @file CollisionConclusion.cpp
 * @brief 当たり判定全般を纏めて管理
 * @author KATO
 * @date 2024/11/28
 */

#include "collisionConclusion.h"

void CollisionConclusion::Initialize() {
	textureManager_ = TextureManager::GetInstance();

	//接触時パーティションの初期化
	collisionEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
	collisionEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	collisionEmitter_.transform.scale = { 0.5f,0.5f,0.5f };
	collisionEmitter_.count = 0;
	collisionEmitter_.frequency = 0.0f;//0.0秒ごとに発生
	collisionEmitter_.frequencyTime = 0.0f;//発生頻度の時刻

	collisionAccelerationField_.acceleration = { 15.0f,0.0f,0.0f };
	collisionAccelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
	collisionAccelerationField_.area.max = { 1.0f,1.0f,1.0f };

	collisionParticleResource_ = textureManager_->Load("project/gamedata/resources/hit.png");

	collisionParticle_ = std::make_unique <CreateParticle>();
	collisionParticle_->Initialize(100, collisionEmitter_, collisionAccelerationField_, collisionParticleResource_);
	collisionParticle_->SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });
	collisionParticle_->SetisVelocity(true, 10.0f);
	collisionParticle_->SetLifeTime(3.0f);
	collisionParticle_->SetisBillBoard(false);
}

void CollisionConclusion::Update() {

	//パーティクル更新
	collisionParticle_->Update();
	collisionParticle_->SetAccelerationField(collisionAccelerationField_);
}

void CollisionConclusion::DrawParticle(const ViewProjection& viewProjection) {
	collisionParticle_->Draw(viewProjection);
}

template <typename Type1, typename Type2>
std::tuple<Type1, Type2, Vector3, Vector3> CollisionConclusion::Collision(Type1& obj1, Type2& obj2, Vector3& velocity1, Vector3& velocity2, REPELLED repelled) {
	//接触しているか否かを確認
	if (!IsCollision(obj1, obj2)) {
		//してないならそのまま返す
		return { obj1, obj2 , velocity1, velocity2 }
	}

	//押し戻しの処理
	Vector3 correction = ComputePushback(obj1, obj2, repelled);
	ApplyPushback(obj1, obj2, correction, repelled);

	Vector3 [newVel1, newVel2] = ComputeCollisionVelocities(1.0f, velocity1, 1.0f, velocity2, repulsionCoefficient, Normalize(correction));
	ApplyVelocityCorrection(velocity1, velocity2, newVel1, newVel2, repelled);

	return { obj1, obj2 , velocity1, velocity2 }
}

Vector3 CollisionConclusion::ComputePushback(const auto& obj1, const auto& obj2, REPELLED repelled) {
	Vector3 correction;
	if constexpr (std::is_same_v<decltype(obj1), StructSphere> && std::is_same_v<decltype(obj2), StructSphere>) {
		Vector3 direction = obj2.center - obj1.center;
		float distance = Length(direction);
		float overlap = obj1.radius + obj2.radius - distance;
		if (overlap > 0.0f) {
			correction = Normalize(direction) * (overlap / 2) * pushbackMultiplier_;
		}
	}
	else if constexpr (/* 他の型間の押し戻し計算 */) {
		// ここに AABB、OBB、Cylinder、Segment などの計算を追加
	}
	return correction;
}

void CollisionConclusion::ApplyPushback(auto& obj1, auto& obj2, const Vector3& correction, REPELLED repelled) {
	switch (repelled) {
	case LEFT://引数1側のオブジェクトのみ反発する
		obj1.center -= correction;
		break;
	case RIGHT://引数2側のオブジェクトのみ反発する
		obj2.center += correction;
		break;
	case BOTH://両方のオブジェクトが反発する
		obj1.center -= correction * 0.5f;
		obj2.center += correction * 0.5f;
		break;
	}
}

void CollisionConclusion::ContactVolume(Vector3 velocity) {

}

bool CollisionConclusion::IsValidPosition(const Vector3 pos) {

}

Vector3 CollisionConclusion::FindValidPosition() {

}

void GamePlayScene::CollisionConclusion() {
	//プレイヤーの判定取得
	StructSphere pSphere;
	pSphere = player_->GetStructSphere();

	//プレイヤーとエネミーの当たり判定
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();
		if (IsCollision(pSphere, eSphere)) {
			//押し戻しの処理
			Vector3 direction = eSphere.center - pSphere.center;
			float distance = Length(direction);
			float overlap = pSphere.radius + eSphere.radius - distance;

			if (overlap > 0.0f) {
				Vector3 correction = Normalize(direction) * (overlap / 2) * pushbackMultiplier_;
				pSphere.center = pSphere.center - correction;
				eSphere.center = eSphere.center + correction;

				player_->SetTranslate(pSphere.center);
				enemy->SetWorldTransform(eSphere.center);
			}

			//衝突音を再生
			ContactVolume(player_->GetVelocity());

			//反発処理
			std::pair<Vector3, Vector3> pair = ComputeCollisionVelocities(1.0f, player_->GetVelocity(), 1.0f, enemy->GetVelocity(), repulsionCoefficient_, Normalize(player_->GetWorldTransform().GetWorldPos() - enemy->GetWorldTransform().GetWorldPos()));
			player_->SetVelocity(pair.first);
			enemy->SetVelocity(pair.second);

			collisionParticle_->SetTranslate(MidPoint(player_->GetWorldTransform().translation_, enemy->GetWorldTransform().translation_));
			collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
		}
	}

	//エネミー同士の当たり判定
	for (auto it1 = enemys_.begin(); it1 != enemys_.end(); ++it1) {
		for (auto it2 = std::next(it1); it2 != enemys_.end(); ++it2) {
			Enemy* enemy1 = *it1;
			Enemy* enemy2 = *it2;

			StructSphere eSphere1 = enemy1->GetStructSphere();
			StructSphere eSphere2 = enemy2->GetStructSphere();

			if (IsCollision(eSphere1, eSphere2)) {
				//押し戻しの処理
				Vector3 direction = eSphere2.center - eSphere1.center;
				float distance = Length(direction);
				float overlap = eSphere1.radius + eSphere2.radius - distance;

				if (overlap > 0.0f) {
					Vector3 correction = Normalize(direction) * (overlap / 2) * pushbackMultiplier_;
					eSphere1.center = eSphere1.center - correction;
					eSphere2.center = eSphere2.center + correction;

					enemy1->SetWorldTransform(eSphere1.center);
					enemy2->SetWorldTransform(eSphere2.center);
				}

				//衝突音を再生
				ContactVolume(enemy1->GetVelocity());

				//反発処理
				std::pair<Vector3, Vector3> pair = ComputeCollisionVelocities(1.0f, enemy1->GetVelocity(), 1.0f, enemy2->GetVelocity(), repulsionCoefficient_, Normalize(enemy1->GetWorldTransform().GetWorldPos() - enemy2->GetWorldTransform().GetWorldPos()));
				enemy1->SetVelocity(pair.first);
				enemy2->SetVelocity(pair.second);

				collisionParticle_->SetTranslate(MidPoint(enemy1->GetWorldTransform().translation_, enemy2->GetWorldTransform().translation_));
				collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
			}
		}
	}

	//プレイヤーとオブジェクトの当たり判定
	for (Obj obj : editors_->GetObj()) {
		if (obj.durability > 0) {
			if (obj.type == "Wall") {
				OBB objOBB;
				objOBB = CreateOBBFromEulerTransform(EulerTransform(obj.world.scale_, obj.world.rotation_, obj.world.translation_));
				if (IsCollision(objOBB, pSphere)) {
					//押し戻し処理
					//Sphere から OBB の最近接点を計算
					Vector3 closestPoint = objOBB.center;
					Vector3 d = pSphere.center - objOBB.center;

					for (int i = 0; i < 3; ++i) {
						float dist = Dot(d, objOBB.orientation[i]);
						dist = std::fmax(-objOBB.size.num[i], std::fmin(dist, objOBB.size.num[i]));
						closestPoint += objOBB.orientation[i] * dist;
					}

					//Sphere の中心と最近接点の距離を計算
					Vector3 direction = pSphere.center - closestPoint;
					float distance = Length(direction);
					float overlap = pSphere.radius - distance;

					if (overlap > 0.0f) {
						Vector3 correction = Normalize(direction) * overlap * pushbackMultiplierObj_;
						pSphere.center += correction;

						player_->SetTranslate(pSphere.center);
					}

					//衝突音を再生
					ContactVolume(player_->GetVelocity());

					//反発処理
					Vector3 velocity = ComputeSphereVelocityAfterCollisionWithOBB(pSphere, player_->GetVelocity(), objOBB, repulsionCoefficient_);
					player_->SetVelocity(velocity);

					editors_->Hitobj(obj);
					if (obj.durability <= 1) {
						explosion_->SetWorldTransformBase(obj.world);
						isExplosion_ = true;
						explosion_->ExplosionFlagTrue(obj.material);
						explosionTimer_ = 10;
					}

					collisionParticle_->SetTranslate(closestPoint);
					collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
				}
			}
		}
	}

	//エネミーとオブジェクトの当たり判定
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();
		for (Obj obj : editors_->GetObj()) {
			if (obj.durability > 0) {
				if (obj.type == "Wall") {
					OBB objOBB;
					objOBB = CreateOBBFromEulerTransform(EulerTransform(obj.world.scale_, obj.world.rotation_, obj.world.translation_));
					if (IsCollision(objOBB, eSphere)) {
						//押し戻し処理
						//Sphere から OBB の最近接点を計算
						Vector3 closestPoint = objOBB.center;
						Vector3 d = eSphere.center - objOBB.center;

						for (int i = 0; i < 3; ++i) {
							float dist = Dot(d, objOBB.orientation[i]);
							dist = std::fmax(-objOBB.size.num[i], std::fmin(dist, objOBB.size.num[i]));
							closestPoint += objOBB.orientation[i] * dist;
						}

						//Sphere の中心と最近接点の距離を計算
						Vector3 direction = eSphere.center - closestPoint;
						float distance = Length(direction);
						float overlap = eSphere.radius - distance;

						if (overlap > 0.0f) {
							Vector3 correction = Normalize(direction) * overlap * pushbackMultiplierObj_;
							eSphere.center += correction;

							enemy->SetWorldTransform(eSphere.center);
						}

						//衝突音を再生
						ContactVolume(enemy->GetVelocity());

						//反発処理
						Vector3 velocity = ComputeSphereVelocityAfterCollisionWithOBB(eSphere, enemy->GetVelocity(), objOBB, repulsionCoefficient_);
						enemy->SetVelocity(velocity);

						editors_->Hitobj(obj);
						if (obj.durability <= 1) {
							explosion_->SetWorldTransformBase(obj.world);
							isExplosion_ = true;
							explosion_->ExplosionFlagTrue(obj.material);
							explosionTimer_ = 10;
						}

						collisionParticle_->SetTranslate(closestPoint);
						collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
					}
				}
			}
		}
	}

	//プレイヤーと破片の当たり判定
	for (size_t i = 0; i < explosion_->GetFragments().size(); ++i) {
		OBB fragmentOBB;
		WorldTransform fragmentTransform = explosion_->GetFragmentTransform(i);
		fragmentOBB = CreateOBBFromEulerTransform(EulerTransform(
			fragmentTransform.scale_, fragmentTransform.rotation_, fragmentTransform.translation_
		));

		if (IsCollision(fragmentOBB, pSphere)) {
			//破片の反発処理
			Vector3 velocity = ComputeVelocitiesAfterCollisionWithOBB(pSphere, player_->GetVelocity(), 1.0f, fragmentOBB, explosion_->GetFragmentVelocity(i), 1.0f, repulsionCoefficient_);
			velocity.num[1] = 1.0f;//上方向への反発を調整
			explosion_->SetFragmentVelocity(i, velocity * 0.5f);
			explosion_->SetAngularVelocitie(i, ComputeRotationFromVelocity(velocity * 0.5, 0.5f));
		}
	}

	//エネミーと破片の当たり判定
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();

		for (size_t i = 0; i < explosion_->GetFragments().size(); ++i) {
			OBB fragmentOBB;
			WorldTransform fragmentTransform = explosion_->GetFragmentTransform(i);
			fragmentOBB = CreateOBBFromEulerTransform(EulerTransform(
				fragmentTransform.scale_, fragmentTransform.rotation_, fragmentTransform.translation_
			));

			if (IsCollision(fragmentOBB, eSphere)) {
				//破片の反発処理
				Vector3 velocity = ComputeVelocitiesAfterCollisionWithOBB(eSphere, enemy->GetVelocity(), 1.0f, fragmentOBB, explosion_->GetFragmentVelocity(i), 1.0f, repulsionCoefficient_);
				velocity.num[1] = 1.0f;//上方向への反発を調整
				explosion_->SetFragmentVelocity(i, velocity * 0.5f);
				explosion_->SetAngularVelocitie(i, ComputeRotationFromVelocity(velocity * 0.5, 0.5f));
			}
		}
	}

	//プレイヤーと敵Bodyの当たり判定
	for (Body body : boss_->GetBody()) {
		StructCylinder bCylinder;
		//スケール成分を使って半径を計算 (X軸方向のスケールを円柱の半径に使用する)
		bCylinder.radius = body.world.scale_.num[0];//X軸方向のスケールが円柱の半径

		//ローカル空間での円柱の上端と下端の位置
		Vector3 localTopCenter{ 0, 0.5f, 0 };
		Vector3 localBottomCenter{ 0, -0.5f, 0 };

		//スケールを適用（Y軸方向のスケールを高さに反映）
		localTopCenter = localTopCenter * body.world.scale_.num[1];//Y軸方向のスケール
		localBottomCenter = localBottomCenter * body.world.scale_.num[1];//Y軸方向のスケール

		//平行移動を適用
		bCylinder.topCenter = localTopCenter + body.world.translation_;
		bCylinder.bottomCenter = localBottomCenter + body.world.translation_;

		if (IsCollision(pSphere, bCylinder)) {
			//円柱の高さ方向の単位ベクトルを計算
			Vector3 cylinderAxis = {
				bCylinder.topCenter.num[0] - bCylinder.bottomCenter.num[0],
				bCylinder.topCenter.num[1] - bCylinder.bottomCenter.num[1],
				bCylinder.topCenter.num[2] - bCylinder.bottomCenter.num[2]
			};
			float heightSquared = DistanceSquared(bCylinder.topCenter, bCylinder.bottomCenter);
			float height = std::sqrt(heightSquared);
			Vector3 unitAxis = { cylinderAxis.num[0] / height, cylinderAxis.num[1] / height, cylinderAxis.num[2] / height };

			//球の中心を円柱の高さ方向に投影して最も近い点を取得
			Vector3 sphereToCylinderBase = {
				pSphere.center.num[0] - bCylinder.bottomCenter.num[0],
				pSphere.center.num[1] - bCylinder.bottomCenter.num[1],
				pSphere.center.num[2] - bCylinder.bottomCenter.num[2]
			};
			float projLength = sphereToCylinderBase.num[0] * unitAxis.num[0] +
				sphereToCylinderBase.num[1] * unitAxis.num[1] +
				sphereToCylinderBase.num[2] * unitAxis.num[2];

			//円柱の高さ範囲に限定
			projLength = std::fmax(0.0f, std::fmin(projLength, height));

			//円柱上で球に最も近い点
			Vector3 closestPointOnCylinder = {
				bCylinder.bottomCenter.num[0] + projLength * unitAxis.num[0],
				bCylinder.bottomCenter.num[1] + projLength * unitAxis.num[1],
				bCylinder.bottomCenter.num[2] + projLength * unitAxis.num[2]
			};

			//球の中心と円柱の最も近い点の距離ベクトル
			Vector3 direction = {
				pSphere.center.num[0] - closestPointOnCylinder.num[0],
				pSphere.center.num[1] - closestPointOnCylinder.num[1],
				pSphere.center.num[2] - closestPointOnCylinder.num[2]
			};
			float distance = Length(direction);

			//球が円柱と重なっているかを確認
			float overlap = pSphere.radius + bCylinder.radius - distance;

			if (overlap > 0.0f) {
				//押し戻しの量を計算
				Vector3 correction = Normalize(direction);
				correction.num[0] *= overlap * pushbackMultiplier_;
				correction.num[1] *= overlap * pushbackMultiplier_;
				correction.num[2] *= overlap * pushbackMultiplier_;

				//球を押し戻し
				pSphere.center.num[0] += correction.num[0];
				pSphere.center.num[1] += correction.num[1];
				pSphere.center.num[2] += correction.num[2];

				player_->SetTranslate(pSphere.center);
			}

			//衝突音を再生
			ContactVolume(player_->GetVelocity());

			//反発処理
			Vector3 velocity = ComputeSphereVelocityAfterCollisionWithCylinder(pSphere, player_->GetVelocity(), bCylinder, repulsionCoefficient_);
			player_->SetVelocity(velocity);

			collisionParticle_->SetTranslate(closestPointOnCylinder);
			collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
		}
	}

	//エネミーと敵Bodyの当たり判定
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();
		for (Body body : boss_->GetBody()) {
			StructCylinder bCylinder;
			//スケール成分を使って半径を計算 (X軸方向のスケールを円柱の半径に使用する)
			bCylinder.radius = body.world.scale_.num[0];//X軸方向のスケールが円柱の半径

			//ローカル空間での円柱の上端と下端の位置
			Vector3 localTopCenter{ 0, 0.5f, 0 };
			Vector3 localBottomCenter{ 0, -0.5f, 0 };

			//スケールを適用（Y軸方向のスケールを高さに反映）
			localTopCenter = localTopCenter * body.world.scale_.num[1];//Y軸方向のスケール
			localBottomCenter = localBottomCenter * body.world.scale_.num[1];//Y軸方向のスケール

			//平行移動を適用
			bCylinder.topCenter = localTopCenter + body.world.translation_;
			bCylinder.bottomCenter = localBottomCenter + body.world.translation_;

			if (IsCollision(eSphere, bCylinder)) {
				//円柱の高さ方向の単位ベクトルを計算
				Vector3 cylinderAxis = {
					bCylinder.topCenter.num[0] - bCylinder.bottomCenter.num[0],
					bCylinder.topCenter.num[1] - bCylinder.bottomCenter.num[1],
					bCylinder.topCenter.num[2] - bCylinder.bottomCenter.num[2]
				};
				float heightSquared = DistanceSquared(bCylinder.topCenter, bCylinder.bottomCenter);
				float height = std::sqrt(heightSquared);
				Vector3 unitAxis = { cylinderAxis.num[0] / height, cylinderAxis.num[1] / height, cylinderAxis.num[2] / height };

				//球の中心を円柱の高さ方向に投影して最も近い点を取得
				Vector3 sphereToCylinderBase = {
					eSphere.center.num[0] - bCylinder.bottomCenter.num[0],
					eSphere.center.num[1] - bCylinder.bottomCenter.num[1],
					eSphere.center.num[2] - bCylinder.bottomCenter.num[2]
				};
				float projLength = sphereToCylinderBase.num[0] * unitAxis.num[0] +
					sphereToCylinderBase.num[1] * unitAxis.num[1] +
					sphereToCylinderBase.num[2] * unitAxis.num[2];

				//円柱の高さ範囲に限定
				projLength = std::fmax(0.0f, std::fmin(projLength, height));

				//円柱上で球に最も近い点
				Vector3 closestPointOnCylinder = {
					bCylinder.bottomCenter.num[0] + projLength * unitAxis.num[0],
					bCylinder.bottomCenter.num[1] + projLength * unitAxis.num[1],
					bCylinder.bottomCenter.num[2] + projLength * unitAxis.num[2]
				};

				//球の中心と円柱の最も近い点の距離ベクトル
				Vector3 direction = {
					eSphere.center.num[0] - closestPointOnCylinder.num[0],
					eSphere.center.num[1] - closestPointOnCylinder.num[1],
					eSphere.center.num[2] - closestPointOnCylinder.num[2]
				};
				float distance = Length(direction);

				//球が円柱と重なっているかを確認
				float overlap = eSphere.radius + bCylinder.radius - distance;

				if (overlap > 0.0f) {
					//押し戻しの量を計算
					Vector3 correction = Normalize(direction);
					correction.num[0] *= overlap * pushbackMultiplier_;
					correction.num[1] *= overlap * pushbackMultiplier_;
					correction.num[2] *= overlap * pushbackMultiplier_;

					//球を押し戻し
					eSphere.center.num[0] += correction.num[0];
					eSphere.center.num[1] += correction.num[1];
					eSphere.center.num[2] += correction.num[2];

					enemy->SetWorldTransform(eSphere.center);
				}

				//衝突音を再生
				ContactVolume(enemy->GetVelocity());

				bool isBreakCylinder = false;
				float combinedSpeed = std::abs(enemy->GetVelocity().num[0]) + std::abs(enemy->GetVelocity().num[2]);
				if (combinedSpeed >= bodyBreakSpeed_) {
					isBreakCylinder = true;
				}


				//反発処理
				Vector3 velocity = ComputeSphereVelocityAfterCollisionWithCylinder(eSphere, enemy->GetVelocity(), bCylinder, repulsionCoefficient_);
				enemy->SetVelocity(velocity);

				if (isBreakCylinder == true) {
					explosion_->SetWorldTransformBase(body.world);
					isExplosion_ = true;
					explosion_->ExplosionFlagTrue(body.material);
					explosionTimer_ = 10;

					boss_->HitBody(body);
				}

				collisionParticle_->SetTranslate(closestPointOnCylinder);
				collisionParticle_->OccursOnlyOnce(collisionParticleOccursNum_);
			}
		}
	}
}

void GamePlayScene::ContactVolume(Vector3 velocity) {
	//接触速度に基づく音量の調整
	float collisionSpeed = Length(velocity);
	float maxSpeed = 0.95f;//最大速度の仮定
	float minVolume = 0.0005f;//最小音量
	float maxVolume = 0.12f;//最大音量

	//速度に基づく音量の計算 (速度が最大時には maxVolume、速度が0の時には minVolume)
	float volume = minVolume + (maxVolume - minVolume) * (collisionSpeed / maxSpeed);
	volume = std::clamp(volume, minVolume, maxVolume);//音量の範囲を制限

	audio_->SoundPlayWave(soundData2_, volume, false);
}

bool GamePlayScene::IsValidPosition(const Vector3 pos) {
	StructSphere sphere;
	sphere.center = pos;
	sphere.radius = 1.5f;//大きさは仮決め

	StructSphere pSphere;
	pSphere = player_->GetStructSphere();

	//各種オブジェクトと衝突しているか確認
	if (IsCollision(pSphere, sphere)) {
		return false;
	}
	for (Enemy* enemy : enemys_) {
		StructSphere eSphere;
		eSphere = enemy->GetStructSphere();
		if (IsCollision(eSphere, sphere)) {
			return false;
		}
	}
	for (Obj obj : editors_->GetObj()) {
		OBB objOBB;
		objOBB = CreateOBBFromEulerTransform(EulerTransform(obj.world.scale_, obj.world.rotation_, obj.world.translation_));
		if (IsCollision(objOBB, sphere)) {
			return false;
		}
	}
	for (Body body : boss_->GetBody()) {
		StructCylinder bCylinder;
		//スケール成分を使って半径を計算 (X軸方向のスケールを円柱の半径に使用する)
		bCylinder.radius = body.world.scale_.num[0];//X軸方向のスケールが円柱の半径

		//ローカル空間での円柱の上端と下端の位置
		Vector3 localTopCenter{ 0, 0.5f, 0 };
		Vector3 localBottomCenter{ 0, -0.5f, 0 };

		//スケールを適用（Y軸方向のスケールを高さに反映）
		localTopCenter = localTopCenter * body.world.scale_.num[1];//Y軸方向のスケール
		localBottomCenter = localBottomCenter * body.world.scale_.num[1];//Y軸方向のスケール

		//平行移動を適用
		bCylinder.topCenter = localTopCenter + body.world.translation_;
		bCylinder.bottomCenter = localBottomCenter + body.world.translation_;

		if (IsCollision(sphere, bCylinder)) {
			return false;
		}
	}

	//衝突していなければTrueを返す
	return true;
}

Vector3 GamePlayScene::FindValidPosition() {
	for (int i = 0; i < maxAttempts_; ++i) {
		Vector3 pos = GenerateRandomPosition();
		if (IsValidPosition(pos)) {
			return pos;
		}
	}
	return Vector3{ 0.0f,0.0f,0.0f };
}