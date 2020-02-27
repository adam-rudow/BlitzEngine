#include "Quaternion.h"
#include "math.h"

const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion()
{
	*this = Quaternion::Identity;
}

Quaternion::Quaternion(float X, float Y, float Z, float W)
{
	x = X;
	y = Y;
	z = Z;
	w = W;
}

// Angle in degrees
Quaternion::Quaternion(const Vector3& axis, float angle)
{
	angle *= (PI / 180.0f);// convert to radians
	float sine = sinf(angle / 2.0f);
	x = axis.x * sine;
	y = axis.y * sine;
	z = axis.z * sine;
	w = cosf(angle / 2.0f);
}

void Quaternion::Conjugate()
{
	x *= -1;
	y *= -1;
	z *= -1;
}

Quaternion Concatenate(const Quaternion& q, const Quaternion& p)
{
	Quaternion retVal;

	// Vector component is:
	// ps * qv + qs * pv + pv x qv
	Vector3 qv(q.x, q.y, q.z);
	Vector3 pv(p.x, p.y, p.z);
	Vector3 newVec = p.w * qv + q.w * pv + Vector3::Cross(pv, qv);
	retVal.x = newVec.x;
	retVal.y = newVec.y;
	retVal.z = newVec.z;

	// Scalar component is:
	// ps * qs - pv . qv
	retVal.w = p.w * q.w - Vector3::Dot(pv, qv);

	return retVal;
}

float Quaternion::LengthSq()
{
	return (x*x + y*y + z*z + w*w);
}

float Quaternion::Length()
{
	return sqrtf(LengthSq());
}

void Quaternion::Normalize()
{
	float len = Length();
	x /= len;
	y /= len;
	z /= len;
	w /= len;
}


Quaternion Normalize(const Quaternion& quat)
{
	Quaternion q = quat;
	q.Normalize();
	return q;
}
