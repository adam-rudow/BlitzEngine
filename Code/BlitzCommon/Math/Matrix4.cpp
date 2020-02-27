#include "Matrix4.h"
#include "Quaternion.h"
#include <windows.h>


Matrix4::Matrix4()
{
	memcpy(matrix, m4Ident, sizeof(float) * 16); 
}

Matrix4::~Matrix4()
{
	
}

Matrix4 Matrix4::CreateFromQuat(const class Quaternion& q)
{
	float mat[4][4];

	mat[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
	mat[0][1] = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
	mat[0][2] = 2.0f * q.x * q.z - 2.0f * q.w * q.y;
	mat[0][3] = 0.0f;

	mat[1][0] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
	mat[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
	mat[1][2] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;
	mat[1][3] = 0.0f;

	mat[2][0] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
	mat[2][1] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
	mat[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
	mat[2][3] = 0.0f;

	mat[3][0] = 0.0f;
	mat[3][1] = 0.0f;
	mat[3][2] = 0.0f;
	mat[3][3] = 1.0f;

	return Matrix4(mat);
}

Matrix4 Matrix4::CreateLookAtMatrix(const Vector3& eye, const Vector3& at, const Vector3& up)
{
	Vector3 forward = (at - eye);
	forward.Normalize();
	Vector3 right = Vector3::Cross(up, forward);
	right.Normalize();
	Vector3 realUp = Vector3::Cross(forward, right);
	realUp.Normalize();

	Vector3 trans;
	trans.x = -Vector3::Dot(right, eye);
	trans.y = -Vector3::Dot(realUp, eye);
	trans.z = -Vector3::Dot(forward, eye);

	float temp[4][4] =
	{
		{ right.x, realUp.x, forward.x, 0.0f },
		{ right.y, realUp.y, forward.y, 0.0f },
		{ right.z, realUp.z, forward.z, 0.0f },
		{ trans.x, trans.y, trans.z, 1.0f }
	};

	return Matrix4(temp); 

	/*Vector3 forward = (at - eye);
	forward.Normalize();
	Vector3 left = Vector3::Cross(up, forward);
	left.Normalize();
	Vector3 realUp = Vector3::Cross(forward, left);
	realUp.Normalize();

	Vector3 trans;
	trans.x = -Vector3::Dot(left, eye);
	trans.y = -Vector3::Dot(up, eye);
	trans.z = -Vector3::Dot(forward, eye);

	float temp[4][4] =
	{
		{ left.x, realUp.x, forward.x, 0.0f },
		{ left.y, realUp.y, forward.y, 0.0f },
		{ left.z, realUp.z, forward.z, 0.0f },
		{ trans.x, trans.y, trans.z, 1.0f }
	};
	return Matrix4(temp);*/

}
Matrix4 Matrix4::CreateViewMatrix(Vector3& pos, Vector3& target, Vector3&up)
{
	Matrix4 res2;

	{

		double eyex = pos.x;
		double eyey = pos.y;
		double eyez = pos.z;
		double centerx = target.x;
		double centery = target.y;
		double centerz = target.z;
		double upx = up.x;
		double upy = up.y;
		double upz = up.z;

		double x[3], y[3], z[3], mag;

		/* Difference eye and center vectors to make Z vector. */
		z[0] = eyex - centerx;
		z[1] = eyey - centery;
		z[2] = eyez - centerz;
		/* Normalize Z. */
		mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
		if (mag) {
			z[0] /= mag;
			z[1] /= mag;
			z[2] /= mag;
		}

		/* Up vector makes Y vector. */
		y[0] = upx;
		y[1] = upy;
		y[2] = upz;

		/* X vector = Y cross Z. */
		x[0] = -(y[1] * z[2] - y[2] * z[1]);
		x[1] = -(-y[0] * z[2] + y[2] * z[0]);
		x[2] = -(y[0] * z[1] - y[1] * z[0]);

		/* Recompute Y = Z cross X. */
		y[0] = -(z[1] * x[2] - z[2] * x[1]);
		y[1] = -(-z[0] * x[2] + z[2] * x[0]);
		y[2] = -(z[0] * x[1] - z[1] * x[0]);

		/* Normalize X. */
		mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
		if (mag) {
			x[0] /= mag;
			x[1] /= mag;
			x[2] /= mag;
		}

		/* Normalize Y. */
		mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
		if (mag) {
			y[0] /= mag;
			y[1] /= mag;
			y[2] /= mag;
		}

		/* Build resulting view matrix. */
		res2.matrix[0][0] = float(x[0]);
		res2.matrix[1][0] = float(x[1]);
		res2.matrix[2][0] = float(x[2]);
		res2.matrix[3][0] = float(-x[0] * eyex + -x[1] * eyey + -x[2] * eyez);

		res2.matrix[0][1] = float(y[0]);
		res2.matrix[1][1] = float(y[1]);
		res2.matrix[2][1] = float(y[2]);
		res2.matrix[3][1] = float(-y[0] * eyex + -y[1] * eyey + -y[2] * eyez);

		res2.matrix[0][2] = float(-z[0]);
		res2.matrix[1][2] = float(-z[1]);
		res2.matrix[2][2] = float(-z[2]);
		res2.matrix[3][2] = float(-(-z[0] * eyex + -z[1] * eyey + -z[2] * eyez));

		res2.matrix[0][3] = 0.0f;
		res2.matrix[1][3] = 0.0f;
		res2.matrix[2][3] = 0.0f;
		res2.matrix[3][3] = 1.0f;
		/*res2.matrix[0][0] = float(x[0]);  
		res2.matrix[0][1] = float(x[1]);
		res2.matrix[0][2] = float(x[2]);  
		res2.matrix[0][3] = float(-x[0] * eyex + -x[1] * eyey + -x[2] * eyez);

		res2.matrix[1][0] = float(y[0]);  
		res2.matrix[1][1] = float(y[1]);
		res2.matrix[1][2] = float(y[2]);  
		res2.matrix[1][3] = float(-y[0] * eyex + -y[1] * eyey + -y[2] * eyez);

		res2.matrix[2][0] = float(-z[0]);  
		res2.matrix[2][1] = float(-z[1]);
		res2.matrix[2][2] = float(-z[2]); 
		res2.matrix[2][3] = float(-(-z[0] * eyex + -z[1] * eyey + -z[2] * eyez));

		res2.matrix[3][0] = 0.0f;   
		res2.matrix[3][1] = 0.0f;  
		res2.matrix[3][2] = 0.0f;  
		res2.matrix[3][3] = 1.0f;*/

	}

	return res2;
}

void Matrix4::AdjustCoordinatesForRHS()
{
	// Flip Y values for DirectX system
	matrix[3][1] *= -1;

}