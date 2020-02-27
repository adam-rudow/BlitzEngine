#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "Matrix4.h"
#include "Quaternion.h"

//typedef unsigned int uint32;

const static float PI = 3.1415926f;

#define DEG2RAD(i) ((i)*PI/180.0f)
#define RAD2DEG(i) ((i)*180.0f/PI)
#define EPSILON 0.0001f
#define IS_EQUAL(a, b) (((a) >= ((b)-EPSILON)) && ((a) <= ((b)+EPSILON)))
#define CLAMP(i, x, y) if((i) > (y)) (i) = (y); if((i) < (x)) (i) = (x);


inline float ToRadians(float angle)
{
	return angle * (PI / 180.0f);
}
