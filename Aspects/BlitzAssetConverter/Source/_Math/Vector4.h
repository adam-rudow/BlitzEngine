#pragma once
#include <iostream>
#include "Vector3.h"

class Matrix4;
//namespace Math{

struct Vector4
{
	float x, y, z, w;
	Vector4();
	Vector4(const float& x, const float& y, const float& z, const float& w);
	Vector4(const Vector3& vec3, const float w);

	Vector4& add(const Vector4 other);
	Vector4& subtract(const Vector4 other);
	Vector4& multiply(const Vector4 other);
	Vector4& divide(const Vector4 other);

	Vector4& multiply(const float other);
	Vector4& divide(const float other);

	friend Vector4 operator+(Vector4 left, const Vector4& right);
	friend Vector4 operator-(Vector4 left, const Vector4& right);
	friend Vector4 operator*(Vector4 left, const Vector4& right);
	friend Vector4 operator/(Vector4 left, const Vector4& right);

	friend Vector4 operator*(Vector4 left, const class Matrix4& right);

	friend Vector4 operator*(Vector4 left, const float& right);
	friend Vector4 operator/(Vector4 left, const float& right);

	float Length();
	float LengthSq();

	static float Dot(const Vector4& left, const Vector4& right);

	bool operator==(const Vector4& other);
	bool operator!=(const Vector4& other);

	Vector4& operator+=(const Vector4& other);
	Vector4& operator-=(const Vector4& other);
	Vector4& operator*=(const Vector4& other);
	Vector4& operator/=(const Vector4& other);


	friend std::ostream& operator<<(std::ostream& stream, const Vector4 Vector);

	static const Vector4 Zero;
	static const Vector4 UnitX;
	static const Vector4 UnitY;
	static const Vector4 UnitZ;
	static const Vector4 NegUnitX;
	static const Vector4 NegUnitY;
	static const Vector4 NegUnitZ;
};

//}
