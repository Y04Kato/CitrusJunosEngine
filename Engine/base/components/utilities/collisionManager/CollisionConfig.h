#pragma once
#include <cstdint>

namespace CollisionConfig {
	const uint32_t kCollisionAttributePlayer = 0b1;
	const uint32_t kCollisionAttributeEnemy = 0b1 << 1;
};