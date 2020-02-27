#include "Vector2.h"

//namespace Math {

Vector2::Vector2()
{
	x = 0.0f;
	y = 0.0f;
}

Vector2::Vector2(const float& x, const float& y)
{
	this->x = x;
	this->y = y;
}

Vector2& Vector2::add(const Vector2 other)
{
	x += other.x;
	y += other.y;

	return *this;
}

Vector2& Vector2::subtract(const Vector2 other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

Vector2& Vector2::multiply(const Vector2 other)
{
	x *= other.x;
	y *= other.y;

	return *this;
}

Vector2& Vector2::multiply(const float other)
{
	x *= other;
	y *= other;

	return *this;
}

Vector2& Vector2::divide(const Vector2 other)
{
	x /= other.x;
	y /= other.y;

	return *this;
}

Vector2& Vector2::divide(const float other)
{
	x /= other;
	y /= other;

	return *this;
}

Vector2 operator+(Vector2 left, const Vector2& right)
{
	return left.add(right);
}

Vector2 operator-(Vector2 left, const Vector2& right)
{
	return left.subtract(right);
}

Vector2 operator*(Vector2 left, const Vector2& right)
{
	return left.multiply(right);
}

Vector2 operator/(Vector2 left, const Vector2& right)
{
	return left.divide(right);
}

Vector2 operator*(Vector2 left, const float& right)
{
	return left.multiply(right);
}

Vector2 operator/(Vector2 left, const float& right)
{
	return left.divide(right);
}

float Vector2::Length()
{
	return sqrt(Dot(*this, *this));
}

float Vector2::LengthSq()
{
	return Dot(*this, *this);
}

float Vector2::Dot(const Vector2& left, const Vector2& right)
{
	return left.x * right.x + left.y * right.y;
}

Vector2& Vector2::operator+=(const Vector2& other)
{
	return add(other);
}

Vector2& Vector2::operator-=(const Vector2& other)
{
	return subtract(other);
}

Vector2& Vector2::operator*=(const Vector2& other)
{
	return multiply(other);
}

Vector2& Vector2::operator/=(const Vector2& other)
{
	return divide(other);
}

bool Vector2::operator==(const Vector2& other)
{
	return (x == other.x && y == other.y);
}

bool Vector2::operator!=(const Vector2& other)
{
	return (x != other.x || y != other.y);
}

std::ostream& operator<<(std::ostream& stream, const Vector2 Vector)
{
	stream << "<" << Vector.x << ", " << Vector.y << ">";
	return stream;
}

const Vector2 Vector2::Zero(0.0f, 0.0f);
const Vector2 Vector2::UnitX(1.0f, 0.0f);
const Vector2 Vector2::UnitY(0.0f, 1.0f);
const Vector2 Vector2::NegUnitX(-1.0f, 0.0f);
const Vector2 Vector2::NegUnitY(0.0f, -1.0f);

//}