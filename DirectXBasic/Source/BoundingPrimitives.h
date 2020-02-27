#pragma once
#include "Math/Vector3.h"

namespace Primitives
{
	struct BoundingPrimitive
	{
		//// Check if a point is contained inside of this object
		//virtual bool Contains(const Vector3& point) = 0;

		//// Check if this object is intersected by another object
		//virtual bool Intersects(const BoundingPrimitive& other) = 0;
	};

	struct AABB
		: public BoundingPrimitive
	{
		Vector3 max;
		Vector3 min;
	};

	struct OBB
		: public BoundingPrimitive
	{
		// Direction from center to the max point (if not rotated)
		Vector3 maxVec;
		// Direction from center to the min point (if not rotated)
		Vector3 minVec;
	};

	struct Sphere
		: public BoundingPrimitive
	{
		float radius;
	};

}
