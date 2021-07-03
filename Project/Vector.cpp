
#include "Vector.h"
#include "Defintions.h"

void Vector::Init(vec_t ix, vec_t iy, vec_t iz)
{
	x = ix; y = iy; z = iz;
}

bool Vector::IsZero(float tolerance) const
{
	return (x > -tolerance && x < tolerance&&
		y > -tolerance && y < tolerance&&
		z > -tolerance && z < tolerance);
}

vec_t Vector::Length2D(void) const
{
	return sqrt(x * x + y * y);
}

Vector Vector::Lengthed(void) const
{
	static Vector Temp{};

	Temp.x = MakeItPositive(x);

	Temp.y = MakeItPositive(y);

	Temp.z = MakeItPositive(z);

	return Temp;
}

bool Vector::InSameDirection(const Vector& dst, bool Yaw) {

	if (Yaw)
	{
		return (y > 0 && dst.y > 0) || (y < 0 && dst.y < 0);
	}

	return (x > 0 && dst.x > 0) || (x < 0 && dst.x < 0);
}

vec_t Vector::Length(void) const
{
	return sqrt(x * x + y * y + z * z);
}

vec_t Vector::SpecifiedLength(int i) const
{
	if (i == 4)
		return sqrt(x * x + y * y + z * z);
	else if (i == 0)
		return sqrt(x * x);
	else if (i == 1)
		return sqrt(y * y);
	else if (i == 2)
		return sqrt(z * z);

	return 0;
}

vec_t Vector::DistToThis(const Vector& vOther, int i) const
{
	Vector delta;
	delta = *this - vOther;
	return delta.SpecifiedLength(i);
}

vec_t Vector::DistTo(const Vector& vOther) const
{
		Vector delta;
		delta = *this - vOther;
		return delta.Length();
}

Vector::Vector(void)
{
	Invalidate();
}

Vector::Vector(vec_t X, vec_t Y, vec_t Z)
{
	x = X;
	y = Y;
	z = Z;
}
Vector::Vector(vec_t* clr)
{
	x = clr[0];
	y = clr[1];
	z = clr[2];
}

void Vector::Invalidate()
{
	x = y = z = std::numeric_limits<float>::infinity();
}

void Vector::Random(vec_t minVal, vec_t maxVal)
{
#define VALVE_RAND_MAX 0x7fff

	x = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	y = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	z = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
}

vec_t Vector::Dot(const Vector& b) const
{
	return (x * b.x + y * b.y + z * b.z);
}

Vector Vector::Transform(const matrix3x4& mat) const noexcept
{
	return Vector{ Dot({ mat[0][0], mat[0][1], mat[0][2] }) + mat[0][3],
				   Dot({ mat[1][0], mat[1][1], mat[1][2] }) + mat[1][3],
				   Dot({ mat[2][0], mat[2][1], mat[2][2] }) + mat[2][3] };
}

void CrossProduct
(
	const Vector& a,
	const Vector& b,
	Vector& result
)
{
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
}

Vector Vector::Cross(const Vector& vOther) const
{
	Vector res;
	CrossProduct(*this, vOther, res);
	return res;
}

void Vector::VectorSubtract(CONST Vector* pV1, CONST Vector* pV2)
{
	x = pV1->x - pV2->x;
	y = pV1->y - pV2->y;
	z = pV1->z - pV2->z;
}

void Vector::Cross(CONST Vector* pV1, CONST Vector* pV2)
{
	Vector v;

	v.x = pV1->y * pV2->z - pV1->z * pV2->y;
	v.y = pV1->z * pV2->x - pV1->x * pV2->z;
	v.z = pV1->x * pV2->y - pV1->y * pV2->x;

	*this = v;
}


static vec_t VectorNormalize0(Vector& v)
{
	vec_t l = v.Length();

	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f; v.z = 1.0f;
	}

	return l;
}

FORCEINLINE float VectorNormalizer(float* v)
{
	return VectorNormalize0(*(reinterpret_cast<Vector*>(v)));
}
vec_t Vector::NormalizeInPlace()
{
	return VectorNormalize0(*this);
}


// operators

vec_t& Vector::operator[](int i)
{
	return ((vec_t*)this)[i];
}

vec_t Vector::operator[](int i) const
{
	return ((vec_t*)this)[i];
}

Vector Vector::operator-(void) const
{
	return Vector(-x, -y, -z);
}

Vector Vector::operator+(const Vector& v) const
{
	return Vector(x + v.x, y + v.y, z + v.z);
}

Vector Vector::operator-(const Vector& v) const
{
	return Vector(x - v.x, y - v.y, z - v.z);
}

Vector Vector::operator*(float fl) const
{
	return Vector(x * fl, y * fl, z * fl);
}

Vector Vector::operator*(const Vector& v) const
{
	return Vector(x * v.x, y * v.y, z * v.z);
}

Vector Vector::operator/(float fl) const
{
	return Vector(x / fl, y / fl, z / fl);
}

Vector Vector::operator/(const Vector& v) const
{
	return Vector(x / v.x, y / v.y, z / v.z);
}

Vector operator*(float fl, const Vector& v)
{
	return v * fl;
}