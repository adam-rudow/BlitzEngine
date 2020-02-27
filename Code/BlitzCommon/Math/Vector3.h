#pragma once
#include <iostream>
#include <string>
#include "MathDefines.h"

struct MATH_EXPORT Vector3
{
	float x, y, z;
	Vector3();
	Vector3(const float& x, const float& y, const float& z);

	Vector3& add(const Vector3 other);
	Vector3& subtract(const Vector3 other);
	Vector3& multiply(const Vector3 other);
	Vector3& divide(const Vector3 other);

	Vector3& multiply(const float other);
	Vector3& divide(const float other);

	MATH_EXPORT friend Vector3 operator+(Vector3 left, const Vector3& right);
	MATH_EXPORT friend Vector3 operator-(Vector3 left, const Vector3& right);
	MATH_EXPORT friend Vector3 operator*(Vector3 left, const Vector3& right);
	MATH_EXPORT friend Vector3 operator/(Vector3 left, const Vector3& right);

	MATH_EXPORT friend Vector3 operator*(Vector3 left, const float& right);
	MATH_EXPORT friend Vector3 operator*(float scalar, const Vector3& vec);
	MATH_EXPORT friend Vector3 operator/(Vector3 left, const float& right);
	//friend Vector3 operator/(float scalar, const Vector3& vec);

	float Length();
	float LengthSq();

	void Normalize();

	static float Dot(const Vector3& left, const Vector3& right);
	static Vector3 Cross(const Vector3& left, const Vector3& right);

	bool operator==(const Vector3& other);
	bool operator!=(const Vector3& other);

	Vector3& operator+=(const Vector3& other);
	Vector3& operator-=(const Vector3& other);
	Vector3& operator*=(const Vector3& other);
	Vector3& operator/=(const Vector3& other);

	inline static Vector3 AdjustCoordinateForRHS(Vector3 vec)
	{
		return Vector3();
	}


	MATH_EXPORT friend std::ostream& operator<<(std::ostream& stream, const Vector3 Vector);

	std::string ToString();

	static const Vector3 Zero;
	static const Vector3 UnitX;
	static const Vector3 UnitY;
	static const Vector3 UnitZ;
	static const Vector3 NegUnitX;
	static const Vector3 NegUnitY;
	static const Vector3 NegUnitZ;
};

class Vector3Comparator_Less {
public:
	bool operator()(const Vector3& lhs, const Vector3& rhs) const {
		return lhs.x < rhs.x
			|| (lhs.x == rhs.x && (lhs.y < rhs.y
				|| (lhs.y == rhs.y && lhs.z < rhs.z)));
	}
};

namespace Color
{
	static const Vector3 Black(0.0f, 0.0f, 0.0f);
	static const Vector3 White(1.0f, 1.0f, 1.0f);
	static const Vector3 Red(1.0f, 0.0f, 0.0f);
	static const Vector3 Green(0.0f, 1.0f, 0.0f);
	static const Vector3 Blue(0.0f, 0.0f, 1.0f);
	static const Vector3 Yellow(1.0f, 1.0f, 0.0f);
	static const Vector3 Sun(0.96f, 0.96f, 0.574f);
	static const Vector3 LightYellow(1.0f, 1.0f, 0.88f);
	static const Vector3 LightBlue(0.68f, 0.85f, 0.9f);
	static const Vector3 LightPink(1.0f, 0.71f, 0.76f);
	static const Vector3 LightGreen(0.56f, 0.93f, 0.56f);
}
