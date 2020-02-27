#pragma once
#include "Matrix4.h"

class Quaternion
{
public:
	Quaternion();
	explicit Quaternion(float X, float Y, float Z, float W);
	explicit Quaternion(const Vector3& axis, float angle);

	void Conjugate();
	float LengthSq();
	float Length();
	void Normalize();

	friend Quaternion Normalize(const Quaternion& quat);
	friend Quaternion Concatenate(const Quaternion& q, const Quaternion& p);

	static const Quaternion Identity;

	float x;
	float y;
	float z;
	float w;

private:
	
};

// Concatenate
// Rotate by q FOLLOWED BY p
