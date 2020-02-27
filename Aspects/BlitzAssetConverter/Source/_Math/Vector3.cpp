#include "Vector3.h"
#include <sstream>

//namespace Math {

Vector3::Vector3()
{
	x = 0.0f;
	y = 0.0f;
}

Vector3::Vector3(const float& x, const float& y, const float& z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3& Vector3::add(const Vector3 other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

Vector3& Vector3::subtract(const Vector3 other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

Vector3& Vector3::multiply(const Vector3 other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

Vector3& Vector3::multiply(const float other)
{
	x *= other;
	y *= other;
	z *= other;

	return *this;
}


Vector3& Vector3::divide(const Vector3 other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;

	return *this;
}

Vector3& Vector3::divide(const float other)
{
	x /= other;
	y /= other;
	z /= other;

	return *this;
}

Vector3 operator+(Vector3 left, const Vector3& right)
{
	return left.add(right);
}

Vector3 operator-(Vector3 left, const Vector3& right)
{
	return left.subtract(right);
}

Vector3 operator*(Vector3 left, const Vector3& right)
{
	return left.multiply(right);
}

Vector3 operator/(Vector3 left, const Vector3& right)
{
	return left.divide(right);
}

Vector3 operator*(Vector3 left, const float& right)
{
	return left.multiply(right);
}

// Scalar multiplication
 Vector3 operator*(float scalar, const Vector3& vec)
{
	return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

 //// Scalar division
 //Vector3 operator*(float scalar, const Vector3& vec)
 //{
	// return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
 //}

Vector3 operator/(Vector3 left, const float& right)
{
	return left.divide(right);
}

Vector3& Vector3::operator+=(const Vector3& other)
{
	return add(other);
}

Vector3& Vector3::operator-=(const Vector3& other)
{
	return subtract(other);
}

Vector3& Vector3::operator*=(const Vector3& other)
{
	return multiply(other);
}

Vector3& Vector3::operator/=(const Vector3& other)
{
	return divide(other);
}

float Vector3::Length()
{
	return sqrt(Dot(*this, *this));
}

float Vector3::LengthSq()
{
	return Dot(*this, *this);
}

void Vector3::Normalize()
{
	float len = Length();
	x /= len;
	y /= len;
	z /= len;
}

float Vector3::Dot(const Vector3& left, const Vector3& right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

Vector3 Vector3::Cross(const Vector3& left, const Vector3& right)
{
	float rx = left.y * right.z - left.z * right.y;
	float ry = left.z * right.x - left.x * right.z;
	float rz = left.x * right.y - left.y * right.x;
	
	Vector3 ret(rx, ry, rz);
	return ret;
}

bool Vector3::operator==(const Vector3& other)
{
	return (x == other.x && y == other.y && z == other.z);
}

bool Vector3::operator!=(const Vector3& other)
{
	return !(*this == other);
}

std::ostream& operator<<(std::ostream& stream, const Vector3 Vector)
{
	stream << "<" << Vector.x << ", " << Vector.y << ", " << Vector.z << ">";
	return stream;
}

std::string Vector3::ToString()
{
	std::ostringstream ss;
	ss << *this;
	std::string s(ss.str());
	return s;
}

const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitX(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitY(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UnitZ(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::NegUnitX(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::NegUnitY(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::NegUnitZ(0.0f, 0.0f, -1.0f);
//}