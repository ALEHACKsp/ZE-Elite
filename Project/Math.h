#pragma once

#ifndef Math_H
#define Math_H

#include "Vector.h"
#include <iostream>
#include <algorithm>

class CUserCmd;

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

#define FastSqrt(x)			(sqrt)(x)

#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

#define M_PHI		1.61803398874989484820 // golden ratio

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif
#define PI 3.14159265358979323846f
#define TICK_INTERVAL			(Globals->interval_per_tick)
#define TICKS_TO_TIME(t) ( Globals->interval_per_tick * (t) )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

namespace Math
{
	QAngle NormalizeAngle(QAngle& angles);

	float AngleNormalize(float angle);

	void NormalizeAngle(float& angle);

	void MatrixSetColumn(const Vector & in, int column, matrix3x4 & out);
	void SinCos(float a, float* s, float* c);
	void VectorToAngles(const Vector& IN_FORWARD, QAngle& angles);
	void AngleToVectors(const QAngle& angles, Vector& IN_FORWARD);
	void AngleVectors(const Vector &angles, Vector &IN_FORWARD);
	void AngleVectors(const Vector & angles, Vector * IN_FORWARD, Vector * right, Vector * up);
	__forceinline float DotProduct(const Vector & a, const Vector & b);
	__forceinline vec_t DotProduct(const vec_t * v1, const vec_t * v2);
	void VectorTransform(const Vector in1, float in2[3][4], Vector & out);
	void VectorTransform(const float * in1, const matrix3x4 & in2, float * out);
	void VectorTransform(const Vector in1, matrix3x4 in2, Vector & out);
	void VectorAngles(const Vector &IN_FORWARD, Vector &angles);

	void AngleMatrix(const QAngle & angles, matrix3x4 & matrix);

	void AngleMatrix(const QAngle & angles, const Vector & position, matrix3x4 & matrix);

	void ClampAngles(Vector& angle);
	void MatrixAngles(const matrix3x4& matrix, QAngle& angles, Vector& position);
	void CorrectMovement(Vector vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
}

auto RandomSeed(std::uint32_t seed) -> void;

auto RandomFloat(float min, float max) -> float;

float VectorNormalize(Vector& vec);

#endif