#pragma once
#include <iostream>

//namespace Math{

struct Vector2
{
	float x, y;
	Vector2();
	Vector2(const float& x, const float& y);

	Vector2& add(const Vector2 other);
	Vector2& subtract(const Vector2 other);
	Vector2& multiply(const Vector2 other);
	Vector2& divide(const Vector2 other);

	Vector2& multiply(const float other);
	Vector2& divide(const float other);

	friend Vector2 operator+(Vector2 left, const Vector2& right);
	friend Vector2 operator-(Vector2 left, const Vector2& right);
	friend Vector2 operator*(Vector2 left, const Vector2& right);
	friend Vector2 operator/(Vector2 left, const Vector2& right);

	friend Vector2 operator*(Vector2 left, const float& right);
	friend Vector2 operator/(Vector2 left, const float& right);

	float Length();
	float LengthSq();

	static float Dot(const Vector2& left, const Vector2& right);

	bool operator==(const Vector2& other);
	bool operator!=(const Vector2& other);

	Vector2& operator+=(const Vector2& other);
	Vector2& operator-=(const Vector2& other);
	Vector2& operator*=(const Vector2& other);
	Vector2& operator/=(const Vector2& other);


	friend std::ostream& operator<<(std::ostream& stream, const Vector2 Vector);

	static const Vector2 Zero;
	static const Vector2 UnitX;
	static const Vector2 UnitY;
	static const Vector2 NegUnitX;
	static const Vector2 NegUnitY;
};


class Vector2Comparator_Less {
public:
	bool operator()(const Vector2& lhs, const Vector2 rhs) const {
		return lhs.x < rhs.x
			|| (lhs.x == rhs.x && (lhs.y < rhs.y));
	}
};
