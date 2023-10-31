#include "AABB.h"
#include "MatrixCalculation.h"
#include <cmath>

AABB Normalize(const AABB& aabb){
	AABB aabb2;
	aabb2.min.num[0] = (std::min)(aabb.min.num[0], aabb.max.num[0]);
	aabb2.min.num[1] = (std::min)(aabb.min.num[1], aabb.max.num[1]);
	aabb2.min.num[2] = (std::min)(aabb.min.num[2], aabb.max.num[2]);
	aabb2.max.num[0] = (std::max)(aabb.min.num[0], aabb.max.num[0]);
	aabb2.max.num[1] = (std::max)(aabb.min.num[1], aabb.max.num[1]);
	aabb2.max.num[2] = (std::max)(aabb.min.num[2], aabb.max.num[2]);
	return aabb2;
}