//#pragma once
////#include <d3dx9math.h>
//#include "Vector3.h"
////#include "math.h"
//
//class Quaternion;
//
//static float m4Ident[4][4] =
//{
//	{ 1.0f, 0.0f, 0.0f, 0.0f },
//	{ 0.0f, 1.0f, 0.0f, 0.0f },
//	{ 0.0f, 0.0f, 1.0f, 0.0f },
//	{ 0.0f, 0.0f, 0.0f, 1.0f }
//};
//
//class Matrix4
//{
//public:
//	float matrix[4][4];
//
//	Matrix4();
//	~Matrix4();
//
//	Matrix4(float inMat[4][4])
//	{
//		memcpy(matrix, inMat, 16 * sizeof(float));
//	}
//
//	inline Vector3 GetTranslation() const 
//	{ 
//		return Vector3(matrix[3][0], matrix[3][1], matrix[3][2]); 
//	}
//
//	inline Vector3 GetScale() const
//	{
//		Vector3 scaleVec;
//		scaleVec.x = Vector3(matrix[0][0], matrix[0][1], matrix[0][2]).Length();
//		scaleVec.y = Vector3(matrix[1][0], matrix[1][1], matrix[1][2]).Length();
//		scaleVec.z = Vector3(matrix[2][0], matrix[2][1], matrix[2][2]).Length();
//		return scaleVec;
//	}
//
//	static Matrix4 CreateTranslation(const Vector3& trans)
//	{
//		float temp[4][4] =
//		{
//			{ 1.0f, 0.0f, 0.0f, 0.0f },
//			{ 0.0f, 1.0f, 0.0f, 0.0f },
//			{ 0.0f, 0.0f, 1.0f, 0.0f },
//			{ trans.x, trans.y, trans.z, 1.0f }
//		};
//		return Matrix4(temp);
//	}
//
//	inline static Matrix4 CreateScale(float scale)
//	{
//		float temp[4][4] =
//		{
//			{ scale, 0.0f, 0.0f, 0.0f },
//			{ 0.0f, scale, 0.0f, 0.0f },
//			{ 0.0f, 0.0f, scale, 0.0f },
//			{ 0.0f, 0.0f, 0.0f, 1.0f }
//		};
//		return Matrix4(temp);
//	}
//
//	inline static Matrix4 CreateScale(float xScale, float yScale, float zScale)
//	{
//		float temp[4][4] =
//		{
//			{ xScale, 0.0f, 0.0f, 0.0f },
//			{ 0.0f, yScale, 0.0f, 0.0f },
//			{ 0.0f, 0.0f, zScale, 0.0f },
//			{ 0.0f, 0.0f, 0.0f, 1.0f }
//		};
//		return Matrix4(temp);
//	}
//
//	inline static Matrix4 CreateScale(const Vector3& scaleVec)
//	{
//		return Matrix4::CreateScale(scaleVec.x, scaleVec.y, scaleVec.z);
//	}
//
//	inline static Matrix4 CreateRotationX(float angleDegrees)
//	{
//		angleDegrees *= (3.1415926f / 180.0f);
//
//		float mat[4][4] =
//		{
//			{1.0f, 0.0f, 0.0f, 0.0f},
//			{0.0f, cosf(angleDegrees), sinf(angleDegrees), 0.0f},
//			{0.0f, -sinf(angleDegrees), cosf(angleDegrees), 0.0f},
//			{0.0f, 0.0f, 0.0f, 1.0f}
//		};
//		return Matrix4(mat);
//	}
//
//	inline static Matrix4 CreateRotationY(float angle)
//	{
//		angle *= (3.1415926f / 180.0f);
//
//		float mat[4][4] =
//		{
//			{ cosf(angle), 0.0f, -sinf(angle), 0.0f },
//			{ 0.0f, 1.0f, 0.0f, 0.0f },
//			{ sinf(angle), 0.0f, cosf(angle), 0.0f },
//			{ 0.0f, 0.0f, 0.0f, 1.0f }
//		};
//		return Matrix4(mat);
//	}
//
//	inline static Matrix4 CreateRotationZ(float angle)
//	{
//		angle *= (3.1415926f / 180.0f);
//
//		float mat[4][4] =
//		{
//			{ cosf(angle), sinf(angle), 0.0f, 0.0f },
//			{ -sinf(angle), cosf(angle), 0.0f, 0.0f },
//			{ 0.0f, 0.0f, 1.0f, 0.0f },
//			{ 0.0f, 0.0f, 0.0f, 1.0f }
//		};
//		return Matrix4(mat);
//	}
//
//	static Matrix4 CreateFromQuat(const class Quaternion& q);
//
//	inline Vector3 GetForward() const
//	{
//		return GetZAxis();
//	}
//
//	inline Vector3 GetRight() const
//	{
//		return GetXAxis();
//	}
//
//	inline Vector3 GetUp() const
//	{
//		return GetYAxis();
//	}
//
//	// Forward
//	inline Vector3 GetXAxis() const
//	{
//		return Vector3(matrix[0][0], matrix[0][1], matrix[0][2]);
//	}
//
//	// Left
//	inline Vector3 GetYAxis() const
//	{
//		return Vector3(matrix[1][0], matrix[1][1], matrix[1][2]);
//	}
//
//	// Up
//	inline Vector3 GetZAxis() const
//	{
//		return Vector3(matrix[2][0], matrix[2][1], matrix[2][2]);
//	}
//
//	static Matrix4 CreateLookAtMatrix(const Vector3& eye, const Vector3& at, const Vector3& up);
//	
//	static Matrix4 CreateOrtho(float width, float height, float nearVal, float farVal)
//	{
//		float temp[4][4] =
//		{
//			{ 2.0f / width, 0.0f, 0.0f, 0.0f },
//			{ 0.0f, 2.0f / height, 0.0f, 0.0f },
//			{ 0.0f, 0.0f, 1.0f / (farVal - nearVal), 0.0f },
//			{ 0.0f, 0.0f, nearVal / (nearVal - farVal), 1.0f }
//			//{ 0.0f, 0.0f, 0.0f, 1.0f }
//		};
//		return Matrix4(temp);
//	}
//
//	static Matrix4 CreatePerspectiveFOV(float fovY, float width, float height, float nearVal, float farVal)
//	{
//		float yScale = 1.0f / tanf(fovY / 2.0f);
//		float xScale = yScale * height / width;
//		float temp[4][4] =
//		{
//			{ xScale, 0.0f, 0.0f, 0.0f },
//			{ 0.0f, yScale, 0.0f, 0.0f },
//			{ 0.0f, 0.0f, farVal / (farVal - nearVal), 1.0f },
//			{ 0.0f, 0.0f, -nearVal * farVal / (farVal - nearVal), 0.0f }
//		};
//		return Matrix4(temp);
//	}
//
//	static Matrix4 CreateViewMatrix(Vector3& pos, Vector3& target, Vector3&up);
//
//	// Matrix multiplication (a * b)
//	friend Matrix4 operator*(const Matrix4& a, const Matrix4& b)
//	{
//		Matrix4 retVal;
//		// row 0
//		retVal.matrix[0][0] =
//			a.matrix[0][0] * b.matrix[0][0] +
//			a.matrix[0][1] * b.matrix[1][0] +
//			a.matrix[0][2] * b.matrix[2][0] +
//			a.matrix[0][3] * b.matrix[3][0];
//
//		retVal.matrix[0][1] =
//			a.matrix[0][0] * b.matrix[0][1] +
//			a.matrix[0][1] * b.matrix[1][1] +
//			a.matrix[0][2] * b.matrix[2][1] +
//			a.matrix[0][3] * b.matrix[3][1];
//
//		retVal.matrix[0][2] =
//			a.matrix[0][0] * b.matrix[0][2] +
//			a.matrix[0][1] * b.matrix[1][2] +
//			a.matrix[0][2] * b.matrix[2][2] +
//			a.matrix[0][3] * b.matrix[3][2];
//
//		retVal.matrix[0][3] =
//			a.matrix[0][0] * b.matrix[0][3] +
//			a.matrix[0][1] * b.matrix[1][3] +
//			a.matrix[0][2] * b.matrix[2][3] +
//			a.matrix[0][3] * b.matrix[3][3];
//
//		// row 1
//		retVal.matrix[1][0] =
//			a.matrix[1][0] * b.matrix[0][0] +
//			a.matrix[1][1] * b.matrix[1][0] +
//			a.matrix[1][2] * b.matrix[2][0] +
//			a.matrix[1][3] * b.matrix[3][0];
//
//		retVal.matrix[1][1] =
//			a.matrix[1][0] * b.matrix[0][1] +
//			a.matrix[1][1] * b.matrix[1][1] +
//			a.matrix[1][2] * b.matrix[2][1] +
//			a.matrix[1][3] * b.matrix[3][1];
//
//		retVal.matrix[1][2] =
//			a.matrix[1][0] * b.matrix[0][2] +
//			a.matrix[1][1] * b.matrix[1][2] +
//			a.matrix[1][2] * b.matrix[2][2] +
//			a.matrix[1][3] * b.matrix[3][2];
//
//		retVal.matrix[1][3] =
//			a.matrix[1][0] * b.matrix[0][3] +
//			a.matrix[1][1] * b.matrix[1][3] +
//			a.matrix[1][2] * b.matrix[2][3] +
//			a.matrix[1][3] * b.matrix[3][3];
//
//		// row 2
//		retVal.matrix[2][0] =
//			a.matrix[2][0] * b.matrix[0][0] +
//			a.matrix[2][1] * b.matrix[1][0] +
//			a.matrix[2][2] * b.matrix[2][0] +
//			a.matrix[2][3] * b.matrix[3][0];
//
//		retVal.matrix[2][1] =
//			a.matrix[2][0] * b.matrix[0][1] +
//			a.matrix[2][1] * b.matrix[1][1] +
//			a.matrix[2][2] * b.matrix[2][1] +
//			a.matrix[2][3] * b.matrix[3][1];
//
//		retVal.matrix[2][2] =
//			a.matrix[2][0] * b.matrix[0][2] +
//			a.matrix[2][1] * b.matrix[1][2] +
//			a.matrix[2][2] * b.matrix[2][2] +
//			a.matrix[2][3] * b.matrix[3][2];
//
//		retVal.matrix[2][3] =
//			a.matrix[2][0] * b.matrix[0][3] +
//			a.matrix[2][1] * b.matrix[1][3] +
//			a.matrix[2][2] * b.matrix[2][3] +
//			a.matrix[2][3] * b.matrix[3][3];
//
//		// row 3
//		retVal.matrix[3][0] =
//			a.matrix[3][0] * b.matrix[0][0] +
//			a.matrix[3][1] * b.matrix[1][0] +
//			a.matrix[3][2] * b.matrix[2][0] +
//			a.matrix[3][3] * b.matrix[3][0];
//
//		retVal.matrix[3][1] =
//			a.matrix[3][0] * b.matrix[0][1] +
//			a.matrix[3][1] * b.matrix[1][1] +
//			a.matrix[3][2] * b.matrix[2][1] +
//			a.matrix[3][3] * b.matrix[3][1];
//
//		retVal.matrix[3][2] =
//			a.matrix[3][0] * b.matrix[0][2] +
//			a.matrix[3][1] * b.matrix[1][2] +
//			a.matrix[3][2] * b.matrix[2][2] +
//			a.matrix[3][3] * b.matrix[3][2];
//
//		retVal.matrix[3][3] =
//			a.matrix[3][0] * b.matrix[0][3] +
//			a.matrix[3][1] * b.matrix[1][3] +
//			a.matrix[3][2] * b.matrix[2][3] +
//			a.matrix[3][3] * b.matrix[3][3];
//
//		return retVal;
//	}
//
//	// Only use for data being uploaded to renderer
//	void AdjustCoordinatesForRHS();
//
//private:
//
//};
