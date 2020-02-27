#include "Vector4.h"
#include "Matrix4.h"

//namespace Math {

Vector4::Vector4()
{
	x = 0.0f;
	y = 0.0f;
}

Vector4::Vector4(const float& x, const float& y, const float& z, const float& w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4(const Vector3& vec3, const float w)
{
	this->x = vec3.x;
	this->y = vec3.y;
	this->z = vec3.z;
	this->w = w;
}

Vector4& Vector4::add(const Vector4 other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

Vector4& Vector4::subtract(const Vector4 other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

Vector4& Vector4::multiply(const Vector4 other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	w *= other.w;

	return *this;
}

Vector4& Vector4::multiply(const float other)
{
	x *= other;
	y *= other;
	z *= other;
	w *= other;

	return *this;
}

Vector4& Vector4::divide(const Vector4 other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	w /= other.w;

	return *this;
}

Vector4& Vector4::divide(const float other)
{
	x /= other;
	y /= other;
	z /= other;
	w /= other;

	return *this;
}

Vector4 operator+(Vector4 left, const Vector4& right)
{
	return left.add(right);
}

Vector4 operator-(Vector4 left, const Vector4& right)
{
	return left.subtract(right);
}

Vector4 operator*(Vector4 left, const Vector4& right)
{
	return left.multiply(right);
}

Vector4 operator/(Vector4 left, const Vector4& right)
{
	return left.divide(right);
}

Vector4 operator*(Vector4 left, const float& right)
{
	return left.multiply(right);
}

Vector4 operator/(Vector4 left, const float& right)
{
	return left.divide(right);
}

Vector4& Vector4::operator+=(const Vector4& other)
{
	return add(other);
}

Vector4& Vector4::operator-=(const Vector4& other)
{
	return subtract(other);
}

Vector4& Vector4::operator*=(const Vector4& other)
{
	return multiply(other);
}

Vector4& Vector4::operator/=(const Vector4& other)
{
	return divide(other);
}

float Vector4::Length()
{
	return sqrt(Dot(*this, *this));
}

float Vector4::LengthSq()
{
	return Dot(*this, *this);
}

float Vector4::Dot(const Vector4& left, const Vector4& right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
}

bool Vector4::operator==(const Vector4& other)
{
	return (x == other.x && y == other.y && z == other.z && w == other.w);
}

bool Vector4::operator!=(const Vector4& other)
{
	return !(*this == other);
}

std::ostream& operator<<(std::ostream& stream, const Vector4 Vector)
{
	stream << "<" << Vector.x << ", " << Vector.y << ", " << Vector.z << ", " << Vector.w << ">";
	return stream;
}

Vector4 operator*(Vector4 left, const Matrix4& right)
{
	Vector4 res;
	res.x = left.x * right.matrix[0][0] + left.y * right.matrix[1][0] + left.z * right.matrix[2][0] + left.w * right.matrix[3][0];
	res.y = left.x * right.matrix[0][1] + left.y * right.matrix[1][1] + left.z * right.matrix[2][1] + left.w * right.matrix[3][1];
	res.z = left.x * right.matrix[0][2] + left.y * right.matrix[1][2] + left.z * right.matrix[2][2] + left.w * right.matrix[3][2];
	res.w = left.x * right.matrix[0][3] + left.y * right.matrix[1][3] + left.z * right.matrix[2][3] + left.w * right.matrix[3][3];

	return res;
}

const Vector4 Vector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::UnitX(1.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::UnitY(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4 Vector4::UnitZ(0.0f, 0.0f, 1.0f, 0.0f);
const Vector4 Vector4::NegUnitX(-1.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::NegUnitY(0.0f, -1.0f, 0.0f, 0.0f);
const Vector4 Vector4::NegUnitZ(0.0f, 0.0f, -1.0f, 0.0f);

//}