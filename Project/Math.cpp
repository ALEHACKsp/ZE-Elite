#include <cassert>
#include "math.h"
#include "SDK.h"

auto RandomSeed(std::uint32_t seed) -> void
{
	static auto oRandomSeed = reinterpret_cast<void(*)(std::uint32_t)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed"));
	oRandomSeed(seed);
}

auto RandomFloat(float min, float max) -> float
{
	static auto oRandomFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
	return oRandomFloat(min, max);
}

void UTIL_ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr)
{
	static auto function = reinterpret_cast<decltype(&UTIL_ClipTraceToPlayers)>(
		Tools::FindPattern("client.dll", "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 18"));

	function(vecAbsStart, vecAbsEnd, mask, filter, tr);
}

float VectorNormalize(Vector& vec)
{
	float radius = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

	// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
	float iradius = 1.f / (radius + FLT_EPSILON);

	vec.x *= iradius;
	vec.y *= iradius;
	vec.z *= iradius;

	return radius;
}


QAngle Math::NormalizeAngle(QAngle& angles)
{
	if (angles.x > 89)
	{
		angles.x = 89;
	}
	else if (-89 > angles.x)
	{
		angles.x = -89;
	}

	if (angles.y > 180)
	{
		angles.y -= 360;
	}
	if (-180 > angles.y)
	{
		angles.y += 360;
	}

	angles.z = 0;

	return angles;
}

inline void NormalizeFloat(float& f)
{
	//f -= floorf(f / 360.0f + 0.5f) * 360.0f;

	f = fmodf(f, 360.0f);
	if (f > 180)
	{
		f -= 360;
	}
	if (f < -180)
	{
		f += 360;
	}
}

float Math::AngleNormalize(float angle)
{
	angle = fmodf(angle, 360.0f);
	if (angle > 180)
	{
		angle -= 360;
	}
	if (angle < -180)
	{
		angle += 360;
	}
	return angle;
}

void Math::NormalizeAngle(float& angle)
{
	NormalizeFloat(angle);
}
void Math::MatrixSetColumn(const Vector &in, int column, matrix3x4& out)
{
	out[0][column] = in.x;
	out[1][column] = in.y;
	out[2][column] = in.z;
}

void Math::SinCos(float a, float* s, float*c)
{
	*s = sin(a);
	*c = cos(a);
}
#define XM_CONST constexpr

XM_CONST float XM_PI = 3.141592654f;
XM_CONST float XM_2PI = 6.283185307f;
XM_CONST float XM_1DIVPI = 0.318309886f;
XM_CONST float XM_1DIV2PI = 0.159154943f;
XM_CONST float XM_PIDIV2 = 1.570796327f;
XM_CONST float XM_PIDIV4 = 0.785398163f;

_Use_decl_annotations_
inline void XMScalarSinCos
(
	float* pSin,
	float* pCos,
	float  Value
)
{
	assert(pSin);
	assert(pCos);

	// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
	float quotient = XM_1DIV2PI * Value;
	if (Value >= 0.0f)
	{
		quotient = static_cast<float>(static_cast<int>(quotient + 0.5f));
	}
	else
	{
		quotient = static_cast<float>(static_cast<int>(quotient - 0.5f));
	}
	float y = Value - XM_2PI * quotient;

	// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
	float sign;
	if (y > XM_PIDIV2)
	{
		y = XM_PI - y;
		sign = -1.0f;
	}
	else if (y < -XM_PIDIV2)
	{
		y = -XM_PI - y;
		sign = -1.0f;
	}
	else
	{
		sign = +1.0f;
	}

	float y2 = y * y;

	// 11-degree minimax approximation
	*pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

	// 10-degree minimax approximation
	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	*pCos = sign * p;
}


void Math::VectorToAngles(const Vector& IN_FORWARD, QAngle& angles)
{
	float yaw, pitch;

	if (IN_FORWARD[1] == 0 && IN_FORWARD[0] == 0) {
		yaw = 0;
		if (IN_FORWARD[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else {
		yaw = (atan2(IN_FORWARD[1], IN_FORWARD[0]) * 180 / XM_PI);
		if (yaw < 0)
			yaw += 360;

		const auto tmp = sqrt(IN_FORWARD[0] * IN_FORWARD[0] + IN_FORWARD[1] * IN_FORWARD[1]);
		pitch = (atan2(-IN_FORWARD[2], tmp) * 180 / XM_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void Math::AngleToVectors(const QAngle& angles, Vector& IN_FORWARD)
{
	float	sp, sy, cp, cy;

	XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
	XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

	IN_FORWARD.x = cp * cy;
	IN_FORWARD.y = cp * sy;
	IN_FORWARD.z = -sp;
}


void Math::AngleVectors(const Vector &angles, Vector& IN_FORWARD)
{
	Assert(IN_FORWARD);

	float sp, sy, cp, cy;

	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);

	IN_FORWARD.x = cp * cy;
	IN_FORWARD.y = cp * sy;
	IN_FORWARD.z = -sp;
}

void Math::AngleVectors(const Vector &angles, Vector *IN_FORWARD, Vector *right, Vector *up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
	SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);

	if (IN_FORWARD)
	{
		IN_FORWARD->x = cp * cy;
		IN_FORWARD->y = cp * sy;
		IN_FORWARD->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr * -sy);
		up->y = (cr*sp*sy + -sr * cy);
		up->z = cr * cp;
	}
}

__forceinline float Math::DotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

__forceinline vec_t Math::DotProduct(const vec_t *v1, const vec_t *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void Math::VectorTransform(const Vector in1, float in2[3][4], Vector &out)
{
	out[0] = DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}

void Math::VectorTransform(const float *in1, const matrix3x4& in2, float *out)
{
	out[0] = DotProduct(in1, in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}

void Math::VectorTransform(const Vector in1, matrix3x4 in2, Vector &out)
{
	out[0] = DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}

void Math::VectorAngles(const Vector& IN_FORWARD, Vector &angles)
{
	if (IN_FORWARD[1] == 0.0f && IN_FORWARD[0] == 0.0f)
	{
		angles[0] = (IN_FORWARD[2] > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
		angles[1] = 0.0f;  //yaw left/right
	}
	else
	{
		angles[0] = atan2(-IN_FORWARD[2], IN_FORWARD.Length2D()) * -180 / M_PI_F;
		angles[1] = atan2(IN_FORWARD[1], IN_FORWARD[0]) * 180 / M_PI_F;

		if (angles[1] > 90)
			angles[1] -= 180;
		else if (angles[1] < 90)
			angles[1] += 180;
		else if (angles[1] == 90)
			angles[1] = 0;
	}

	angles[2] = 0.0f;
}

void Math::AngleMatrix(const QAngle &angles, matrix3x4& matrix)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
	SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;

	float crcy = cr * cy;
	float crsy = cr * sy;
	float srcy = sr * cy;
	float srsy = sr * sy;
	matrix[0][1] = sp * srcy - crsy;
	matrix[1][1] = sp * srsy + crcy;
	matrix[2][1] = sr * cp;

	matrix[0][2] = (sp*crcy + srsy);
	matrix[1][2] = (sp*crsy - srcy);
	matrix[2][2] = cr * cp;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

void MatrixGetColumn(const matrix3x4& in, int column, Vector& out)
{
	out.x = in[0][column];
	out.y = in[1][column];
	out.z = in[2][column];
}

void Math::AngleMatrix(const QAngle &angles, const Vector &position, matrix3x4& matrix)
{
	AngleMatrix(angles, matrix);
	MatrixSetColumn(position, 3, matrix);
}

void MatrixAngles(const matrix3x4& matrix, QAngle angles)
{
	float forward[3];
	float left[3];
	float up[3];

	//
	// Extract the basis vectors from the matrix. Since we only need the Z
	// component of the up vector, we don't get X and Y.
	//
	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

	// enough here to get angles?
	if (xyDist > 0.001f)
	{
		// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
		angles.y = RAD2DEG(atan2f(forward[1], forward[0]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles.x = RAD2DEG(atan2f(-forward[2], xyDist));

		// (roll)	z = ATAN( left.z, up.z );
		angles.z = RAD2DEG(atan2f(left[2], up[2]));
	}
	else	// forward is mostly Z, gimbal lock-
	{
		// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
		angles.y = RAD2DEG(atan2f(-left[0], left[1]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles.x = RAD2DEG(atan2f(-forward[2], xyDist));

		// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
		angles.z = 0;
	}
}

void  Math::MatrixAngles(const matrix3x4& matrix, QAngle& angles, Vector& position)
{
	MatrixGetColumn(matrix, 3, position);
	MatrixAngles(matrix, angles);
}

void Math::CorrectMovement(Vector vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	// side/IN_FORWARD move correction
	float deltaView;
	float f1;
	float f2;

	if (vOldAngles.y < 0.f)
		f1 = 360.0f + vOldAngles.y;
	else
		f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
}

void Math::ClampAngles(Vector& angle)
{
	angle.x = std::clamp(angle.x, -89.f, 89.f);
	angle.y = std::remainderf(angle.y, 360.f);
}