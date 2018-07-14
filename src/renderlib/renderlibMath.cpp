#include "renderlib/detail/renderlibMath.h"
using namespace math;

#include <cmath>
#include <algorithm>

const float DEG2RAD = 3.141593f / 180.0f;
const float RAD2DEG = 180.0f / 3.141593f;
const float EPSILON = 0.00001f;

Matrix4::Matrix4()
{
	// initially identity matrix
	identity();
}



Matrix4::Matrix4(const float src[16])
{
	set(src);
}



Matrix4::Matrix4(float xx, float xy, float xz, float xw,
	float yx, float yy, float yz, float yw,
	float zx, float zy, float zz, float zw,
	float wx, float wy, float wz, float ww)
{
	set(xx, xy, xz, xw, yx, yy, yz, yw, zx, zy, zz, zw, wx, wy, wz, ww);
}



void Matrix4::set(const float src[16])
{
	m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
	m[4] = src[4];  m[5] = src[5];  m[6] = src[6];  m[7] = src[7];
	m[8] = src[8];  m[9] = src[9];  m[10] = src[10]; m[11] = src[11];
	m[12] = src[12]; m[13] = src[13]; m[14] = src[14]; m[15] = src[15];
}



void Matrix4::set(float xx, float xy, float xz, float xw,
	float yx, float yy, float yz, float yw,
	float zx, float zy, float zz, float zw,
	float wx, float wy, float wz, float ww)
{
	m[0] = xx;  m[1] = xy;  m[2] = xz;  m[3] = xw;
	m[4] = yx;  m[5] = yy;  m[6] = yz;  m[7] = yw;
	m[8] = zx;  m[9] = zy;  m[10] = zz;  m[11] = zw;
	m[12] = wx;  m[13] = wy;  m[14] = wz;  m[15] = ww;
}



void Matrix4::setRow(int index, const float row[4])
{
	m[index * 4] = row[0];  m[index * 4 + 1] = row[1];  m[index * 4 + 2] = row[2];  m[index * 4 + 3] = row[3];
}



void Matrix4::setRow(int index, const Vector4& v)
{
	m[index * 4] = v.x;  m[index * 4 + 1] = v.y;  m[index * 4 + 2] = v.z;  m[index * 4 + 3] = v.w;
}



void Matrix4::setRow(int index, const Vector3& v)
{
	m[index * 4] = v.x;  m[index * 4 + 1] = v.y;  m[index * 4 + 2] = v.z;
}



void Matrix4::setColumn(int index, const float col[4])
{
	m[index] = col[0];  m[index + 4] = col[1];  m[index + 8] = col[2];  m[index + 12] = col[3];
}



void Matrix4::setColumn(int index, const Vector4& v)
{
	m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;  m[index + 12] = v.w;
}



void Matrix4::setColumn(int index, const Vector3& v)
{
	m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;
}



const float* Matrix4::get() const
{
	return m;
}



Matrix4& Matrix4::identity()
{
	m[0] = m[5] = m[10] = m[15] = 1.0f;
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	return *this;
}



Matrix4 Matrix4::operator+(const Matrix4& rhs) const
{
	return Matrix4(m[0] + rhs[0], m[1] + rhs[1], m[2] + rhs[2], m[3] + rhs[3],
		m[4] + rhs[4], m[5] + rhs[5], m[6] + rhs[6], m[7] + rhs[7],
		m[8] + rhs[8], m[9] + rhs[9], m[10] + rhs[10], m[11] + rhs[11],
		m[12] + rhs[12], m[13] + rhs[13], m[14] + rhs[14], m[15] + rhs[15]);
}



Matrix4 Matrix4::operator-(const Matrix4& rhs) const
{
	return Matrix4(m[0] - rhs[0], m[1] - rhs[1], m[2] - rhs[2], m[3] - rhs[3],
		m[4] - rhs[4], m[5] - rhs[5], m[6] - rhs[6], m[7] - rhs[7],
		m[8] - rhs[8], m[9] - rhs[9], m[10] - rhs[10], m[11] - rhs[11],
		m[12] - rhs[12], m[13] - rhs[13], m[14] - rhs[14], m[15] - rhs[15]);
}



Matrix4& Matrix4::operator+=(const Matrix4& rhs)
{
	m[0] += rhs[0];    m[1] += rhs[1];    m[2] += rhs[2];    m[3] += rhs[3];
	m[4] += rhs[4];    m[5] += rhs[5];    m[6] += rhs[6];    m[7] += rhs[7];
	m[8] += rhs[8];    m[9] += rhs[9];    m[10] += rhs[10];  m[11] += rhs[11];
	m[12] += rhs[12];  m[13] += rhs[13];  m[14] += rhs[14];  m[15] += rhs[15];
	return *this;
}



Matrix4& Matrix4::operator-=(const Matrix4& rhs)
{
	m[0] -= rhs[0];    m[1] -= rhs[1];    m[2] -= rhs[2];    m[3] -= rhs[3];
	m[4] -= rhs[4];    m[5] -= rhs[5];    m[6] -= rhs[6];    m[7] -= rhs[7];
	m[8] -= rhs[8];    m[9] -= rhs[9];    m[10] -= rhs[10];  m[11] -= rhs[11];
	m[12] -= rhs[12];  m[13] -= rhs[13];  m[14] -= rhs[14];  m[15] -= rhs[15];
	return *this;
}



Vector4 Matrix4::operator*(const Vector4& rhs) const
{
	return Vector4(m[0] * rhs.x + m[1] * rhs.y + m[2] * rhs.z + m[3] * rhs.w,
		m[4] * rhs.x + m[5] * rhs.y + m[6] * rhs.z + m[7] * rhs.w,
		m[8] * rhs.x + m[9] * rhs.y + m[10] * rhs.z + m[11] * rhs.w,
		m[12] * rhs.x + m[13] * rhs.y + m[14] * rhs.z + m[15] * rhs.w);
}



Vector3 Matrix4::operator*(const Vector3& rhs) const
{
	return Vector3(m[0] * rhs.x + m[1] * rhs.y + m[2] * rhs.z,
		m[4] * rhs.x + m[5] * rhs.y + m[6] * rhs.z,
		m[8] * rhs.x + m[9] * rhs.y + m[10] * rhs.z);
}



Matrix4 Matrix4::operator*(const Matrix4& n) const
{
	return Matrix4(m[0] * n[0] + m[1] * n[4] + m[2] * n[8] + m[3] * n[12], m[0] * n[1] + m[1] * n[5] + m[2] * n[9] + m[3] * n[13], m[0] * n[2] + m[1] * n[6] + m[2] * n[10] + m[3] * n[14], m[0] * n[3] + m[1] * n[7] + m[2] * n[11] + m[3] * n[15],
		m[4] * n[0] + m[5] * n[4] + m[6] * n[8] + m[7] * n[12], m[4] * n[1] + m[5] * n[5] + m[6] * n[9] + m[7] * n[13], m[4] * n[2] + m[5] * n[6] + m[6] * n[10] + m[7] * n[14], m[4] * n[3] + m[5] * n[7] + m[6] * n[11] + m[7] * n[15],
		m[8] * n[0] + m[9] * n[4] + m[10] * n[8] + m[11] * n[12], m[8] * n[1] + m[9] * n[5] + m[10] * n[9] + m[11] * n[13], m[8] * n[2] + m[9] * n[6] + m[10] * n[10] + m[11] * n[14], m[8] * n[3] + m[9] * n[7] + m[10] * n[11] + m[11] * n[15],
		m[12] * n[0] + m[13] * n[4] + m[14] * n[8] + m[15] * n[12], m[12] * n[1] + m[13] * n[5] + m[14] * n[9] + m[15] * n[13], m[12] * n[2] + m[13] * n[6] + m[14] * n[10] + m[15] * n[14], m[12] * n[3] + m[13] * n[7] + m[14] * n[11] + m[15] * n[15]);
}



Matrix4& Matrix4::operator*=(const Matrix4& rhs)
{
	*this = *this * rhs;
	return *this;
}

Matrix4 operator-(const Matrix4& rhs)
{
	return Matrix4(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8], -rhs[9], -rhs[10], -rhs[11], -rhs[12], -rhs[13], -rhs[14], -rhs[15]);
}



Matrix4 operator*(float s, const Matrix4& rhs)
{
	return Matrix4(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8], s*rhs[9], s*rhs[10], s*rhs[11], s*rhs[12], s*rhs[13], s*rhs[14], s*rhs[15]);
}



Vector4 operator*(const Vector4& v, const Matrix4& m)
{
	return Vector4(v.x*m[0] + v.y*m[1] + v.z*m[2] + v.w*m[3], v.x*m[4] + v.y*m[5] + v.z*m[6] + v.w*m[7], v.x*m[8] + v.y*m[9] + v.z*m[10] + v.w*m[11], v.x*m[12] + v.y*m[13] + v.z*m[14] + v.w*m[15]);
}



Vector3 operator*(const Vector3& v, const Matrix4& m)
{
	return Vector3(v.x*m[0] + v.y*m[1] + v.z*m[2], v.x*m[4] + v.y*m[5] + v.z*m[6], v.x*m[8] + v.y*m[9] + v.z*m[10]);
}




bool Matrix4::operator==(const Matrix4& n) const
{
	return (m[0] == n[0]) && (m[1] == n[1]) && (m[2] == n[2]) && (m[3] == n[3]) &&
		(m[4] == n[4]) && (m[5] == n[5]) && (m[6] == n[6]) && (m[7] == n[7]) &&
		(m[8] == n[8]) && (m[9] == n[9]) && (m[10] == n[10]) && (m[11] == n[11]) &&
		(m[12] == n[12]) && (m[13] == n[13]) && (m[14] == n[14]) && (m[15] == n[15]);
}



bool Matrix4::operator!=(const Matrix4& n) const
{
	return (m[0] != n[0]) || (m[1] != n[1]) || (m[2] != n[2]) || (m[3] != n[3]) ||
		(m[4] != n[4]) || (m[5] != n[5]) || (m[6] != n[6]) || (m[7] != n[7]) ||
		(m[8] != n[8]) || (m[9] != n[9]) || (m[10] != n[10]) || (m[11] != n[11]) ||
		(m[12] != n[12]) || (m[13] != n[13]) || (m[14] != n[14]) || (m[15] != n[15]);
}



float Matrix4::operator[](int index) const
{
	return m[index];
}



float& Matrix4::operator[](int index)
{
	return m[index];
}

///////////////////////////////////////////////////////////////////////////////
// transpose 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::transpose()
{
	std::swap(m[1], m[4]);
	std::swap(m[2], m[8]);
	std::swap(m[3], m[12]);
	std::swap(m[6], m[9]);
	std::swap(m[7], m[13]);
	std::swap(m[11], m[14]);

	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// inverse 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::invert()
{

		this->invertGeneral();
		/*@@ invertProjective() is not optimized (slower than generic one)
		if(fabs(m[0]*m[5] - m[1]*m[4]) > EPSILON)
		this->invertProjective();   // inverse using matrix partition
		else
		this->invertGeneral();      // generalized inverse
		*/
	

	return *this;
}



///////////////////////////////////////////////////////////////////////////////
// compute the inverse of 4x4 Euclidean transformation matrix
//
// Euclidean transformation is translation, rotation, and reflection.
// With Euclidean transform, only the position and orientation of the object
// will be changed. Euclidean transform does not change the shape of an object
// (no scaling). Length and angle are reserved.
//
// Use inverseAffine() if the matrix has scale and shear transformation.
//
// M = [ R | T ]
//     [ --+-- ]    (R denotes 3x3 rotation/reflection matrix)
//     [ 0 | 1 ]    (T denotes 1x3 translation matrix)
//
// y = M*x  ->  y = R*x + T  ->  x = R^-1*(y - T)  ->  x = R^T*y - R^T*T
// (R is orthogonal,  R^-1 = R^T)
//
//  [ R | T ]-1    [ R^T | -R^T * T ]    (R denotes 3x3 rotation matrix)
//  [ --+-- ]   =  [ ----+--------- ]    (T denotes 1x3 translation)
//  [ 0 | 1 ]      [  0  |     1    ]    (R^T denotes R-transpose)
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::invertEuclidean()
{
	// transpose 3x3 rotation matrix part
	// | R^T | 0 |
	// | ----+-- |
	// |  0  | 1 |
	float tmp;
	tmp = m[1];  m[1] = m[4];  m[4] = tmp;
	tmp = m[2];  m[2] = m[8];  m[8] = tmp;
	tmp = m[6];  m[6] = m[9];  m[9] = tmp;

	// compute translation part -R^T * T
	// | 0 | -R^T x |
	// | --+------- |
	// | 0 |   0    |
	float x = m[12];
	float y = m[13];
	float z = m[14];
	m[12] = -(m[0] * x + m[4] * y + m[8] * z);
	m[13] = -(m[1] * x + m[5] * y + m[9] * z);
	m[14] = -(m[2] * x + m[6] * y + m[10] * z);

	// last row should be unchanged (0,0,0,1)

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
// compute the inverse of a general 4x4 matrix using Cramer's Rule
// If cannot find inverse, return indentity matrix
// M^-1 = adj(M) / det(M)
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::invertGeneral()
{
	// get cofactors of minor matrices
	float cofactor0 = getCofactor(m[5], m[6], m[7], m[9], m[10], m[11], m[13], m[14], m[15]);
	float cofactor1 = getCofactor(m[4], m[6], m[7], m[8], m[10], m[11], m[12], m[14], m[15]);
	float cofactor2 = getCofactor(m[4], m[5], m[7], m[8], m[9], m[11], m[12], m[13], m[15]);
	float cofactor3 = getCofactor(m[4], m[5], m[6], m[8], m[9], m[10], m[12], m[13], m[14]);

	// get determinant
	float determinant = m[0] * cofactor0 - m[1] * cofactor1 + m[2] * cofactor2 - m[3] * cofactor3;
	if (fabs(determinant) <= EPSILON)
	{
		return identity();
	}

	// get rest of cofactors for adj(M)
	float cofactor4 = getCofactor(m[1], m[2], m[3], m[9], m[10], m[11], m[13], m[14], m[15]);
	float cofactor5 = getCofactor(m[0], m[2], m[3], m[8], m[10], m[11], m[12], m[14], m[15]);
	float cofactor6 = getCofactor(m[0], m[1], m[3], m[8], m[9], m[11], m[12], m[13], m[15]);
	float cofactor7 = getCofactor(m[0], m[1], m[2], m[8], m[9], m[10], m[12], m[13], m[14]);

	float cofactor8 = getCofactor(m[1], m[2], m[3], m[5], m[6], m[7], m[13], m[14], m[15]);
	float cofactor9 = getCofactor(m[0], m[2], m[3], m[4], m[6], m[7], m[12], m[14], m[15]);
	float cofactor10 = getCofactor(m[0], m[1], m[3], m[4], m[5], m[7], m[12], m[13], m[15]);
	float cofactor11 = getCofactor(m[0], m[1], m[2], m[4], m[5], m[6], m[12], m[13], m[14]);

	float cofactor12 = getCofactor(m[1], m[2], m[3], m[5], m[6], m[7], m[9], m[10], m[11]);
	float cofactor13 = getCofactor(m[0], m[2], m[3], m[4], m[6], m[7], m[8], m[10], m[11]);
	float cofactor14 = getCofactor(m[0], m[1], m[3], m[4], m[5], m[7], m[8], m[9], m[11]);
	float cofactor15 = getCofactor(m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10]);

	// build inverse matrix = adj(M) / det(M)
	// adjugate of M is the transpose of the cofactor matrix of M
	float invDeterminant = 1.0f / determinant;
	m[0] = invDeterminant * cofactor0;
	m[1] = -invDeterminant * cofactor4;
	m[2] = invDeterminant * cofactor8;
	m[3] = -invDeterminant * cofactor12;

	m[4] = -invDeterminant * cofactor1;
	m[5] = invDeterminant * cofactor5;
	m[6] = -invDeterminant * cofactor9;
	m[7] = invDeterminant * cofactor13;

	m[8] = invDeterminant * cofactor2;
	m[9] = -invDeterminant * cofactor6;
	m[10] = invDeterminant * cofactor10;
	m[11] = -invDeterminant * cofactor14;

	m[12] = -invDeterminant * cofactor3;
	m[13] = invDeterminant * cofactor7;
	m[14] = -invDeterminant * cofactor11;
	m[15] = invDeterminant * cofactor15;

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
// return determinant of 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
float Matrix4::getDeterminant() const
{
	return m[0] * getCofactor(m[5], m[6], m[7], m[9], m[10], m[11], m[13], m[14], m[15]) -
		m[1] * getCofactor(m[4], m[6], m[7], m[8], m[10], m[11], m[12], m[14], m[15]) +
		m[2] * getCofactor(m[4], m[5], m[7], m[8], m[9], m[11], m[12], m[13], m[15]) -
		m[3] * getCofactor(m[4], m[5], m[6], m[8], m[9], m[10], m[12], m[13], m[14]);
}



///////////////////////////////////////////////////////////////////////////////
// compute cofactor of 3x3 minor matrix without sign
// input params are 9 elements of the minor matrix
// NOTE: The caller must know its sign.
///////////////////////////////////////////////////////////////////////////////
float Matrix4::getCofactor(float m0, float m1, float m2,
	float m3, float m4, float m5,
	float m6, float m7, float m8) const
{
	return m0 * (m4 * m8 - m5 * m7) -
		m1 * (m3 * m8 - m5 * m6) +
		m2 * (m3 * m7 - m4 * m6);
}



///////////////////////////////////////////////////////////////////////////////
// translate this matrix by (x, y, z)
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::translate(const Vector3& v)
{
	return translate(v.x, v.y, v.z);
}

Matrix4& Matrix4::translate(float x, float y, float z)
{
	m[0] += m[3] * x;   m[4] += m[7] * x;   m[8] += m[11] * x;   m[12] += m[15] * x;
	m[1] += m[3] * y;   m[5] += m[7] * y;   m[9] += m[11] * y;   m[13] += m[15] * y;
	m[2] += m[3] * z;   m[6] += m[7] * z;   m[10] += m[11] * z;   m[14] += m[15] * z;

	return *this;
}



///////////////////////////////////////////////////////////////////////////////
// uniform scale
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::scale(float s)
{
	return scale(s, s, s);
}

Matrix4& Matrix4::scale(float x, float y, float z)
{
	m[0] *= x;   m[4] *= x;   m[8] *= x;   m[12] *= x;
	m[1] *= y;   m[5] *= y;   m[9] *= y;   m[13] *= y;
	m[2] *= z;   m[6] *= z;   m[10] *= z;   m[14] *= z;
	return *this;
}



///////////////////////////////////////////////////////////////////////////////
// build a rotation matrix with given angle(degree) and rotation axis, then
// multiply it with this object
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::rotate(float angle, const Vector3& axis)
{
	return rotate(angle, axis.x, axis.y, axis.z);
}

Matrix4& Matrix4::rotate(float angle, float x, float y, float z)
{
	float c = cosf(angle * DEG2RAD);    // cosine
	float s = sinf(angle * DEG2RAD);    // sine
	float c1 = 1.0f - c;                // 1 - c
	float m0 = m[0], m4 = m[4], m8 = m[8], m12 = m[12],
		m1 = m[1], m5 = m[5], m9 = m[9], m13 = m[13],
		m2 = m[2], m6 = m[6], m10 = m[10], m14 = m[14];

	// build rotation matrix
	float r0 = x * x * c1 + c;
	float r1 = x * y * c1 + z * s;
	float r2 = x * z * c1 - y * s;
	float r4 = x * y * c1 - z * s;
	float r5 = y * y * c1 + c;
	float r6 = y * z * c1 + x * s;
	float r8 = x * z * c1 + y * s;
	float r9 = y * z * c1 - x * s;
	float r10 = z * z * c1 + c;

	// multiply rotation matrix
	m[0] = r0 * m0 + r4 * m1 + r8 * m2;
	m[1] = r1 * m0 + r5 * m1 + r9 * m2;
	m[2] = r2 * m0 + r6 * m1 + r10 * m2;
	m[4] = r0 * m4 + r4 * m5 + r8 * m6;
	m[5] = r1 * m4 + r5 * m5 + r9 * m6;
	m[6] = r2 * m4 + r6 * m5 + r10 * m6;
	m[8] = r0 * m8 + r4 * m9 + r8 * m10;
	m[9] = r1 * m8 + r5 * m9 + r9 * m10;
	m[10] = r2 * m8 + r6 * m9 + r10 * m10;
	m[12] = r0 * m12 + r4 * m13 + r8 * m14;
	m[13] = r1 * m12 + r5 * m13 + r9 * m14;
	m[14] = r2 * m12 + r6 * m13 + r10 * m14;

	return *this;
}

Matrix4& Matrix4::rotateX(float angle)
{
	float c = cosf(angle * DEG2RAD);
	float s = sinf(angle * DEG2RAD);
	float m1 = m[1], m2 = m[2],
		m5 = m[5], m6 = m[6],
		m9 = m[9], m10 = m[10],
		m13 = m[13], m14 = m[14];

	m[1] = m1 * c + m2 * -s;
	m[2] = m1 * s + m2 * c;
	m[5] = m5 * c + m6 * -s;
	m[6] = m5 * s + m6 * c;
	m[9] = m9 * c + m10 * -s;
	m[10] = m9 * s + m10 * c;
	m[13] = m13 * c + m14 * -s;
	m[14] = m13 * s + m14 * c;

	return *this;
}

Matrix4& Matrix4::rotateY(float angle)
{
	float c = cosf(angle * DEG2RAD);
	float s = sinf(angle * DEG2RAD);
	float m0 = m[0], m2 = m[2],
		m4 = m[4], m6 = m[6],
		m8 = m[8], m10 = m[10],
		m12 = m[12], m14 = m[14];

	m[0] = m0 * c + m2 * s;
	m[2] = m0 * -s + m2 * c;
	m[4] = m4 * c + m6 * s;
	m[6] = m4 * -s + m6 * c;
	m[8] = m8 * c + m10 * s;
	m[10] = m8 * -s + m10 * c;
	m[12] = m12 * c + m14 * s;
	m[14] = m12 * -s + m14 * c;

	return *this;
}

Matrix4& Matrix4::rotateZ(float angle)
{
	float c = cosf(angle * DEG2RAD);
	float s = sinf(angle * DEG2RAD);
	float m0 = m[0], m1 = m[1],
		m4 = m[4], m5 = m[5],
		m8 = m[8], m9 = m[9],
		m12 = m[12], m13 = m[13];

	m[0] = m0 * c + m1 * -s;
	m[1] = m0 * s + m1 * c;
	m[4] = m4 * c + m5 * -s;
	m[5] = m4 * s + m5 * c;
	m[8] = m8 * c + m9 * -s;
	m[9] = m8 * s + m9 * c;
	m[12] = m12 * c + m13 * -s;
	m[13] = m12 * s + m13 * c;

	return *this;
}



///////////////////////////////////////////////////////////////////////////////
// rotate matrix to face along the target direction
// NOTE: This function will clear the previous rotation and scale info and
// rebuild the matrix with the target vector. But it will keep the previous
// translation values.
// NOTE: It is for rotating object to look at the target, NOT for camera
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::lookAt(const Vector3& target)
{
	// compute forward vector and normalize
	Vector3 position = Vector3(m[12], m[13], m[14]);
	Vector3 forward = target - position;
	forward.normalize();
	Vector3 up;             // up vector of object
	Vector3 left;           // left vector of object

							// compute temporal up vector
							// if forward vector is near Y-axis, use up vector (0,0,-1) or (0,0,1)
	if (fabs(forward.x) < EPSILON && fabs(forward.z) < EPSILON)
	{
		// forward vector is pointing +Y axis
		if (forward.y > 0)
			up = Vector3(0, 0, -1);
		// forward vector is pointing -Y axis
		else
			up = Vector3(0, 0, 1);
	}
	else
	{
		// assume up vector is +Y axis
		up = Vector3(0, 1, 0);
	}

	// compute left vector
	left = up.cross(forward);
	left.normalize();

	// re-compute up vector
	up = forward.cross(left);
	//up.normalize();

	// NOTE: overwrite rotation and scale info of the current matrix
	this->setColumn(0, left);
	this->setColumn(1, up);
	this->setColumn(2, forward);

	return *this;
}

Matrix4& Matrix4::lookAt(const Vector3& target, const Vector3& upVec)
{
	// compute forward vector and normalize
	Vector3 position = Vector3(m[12], m[13], m[14]);
	Vector3 forward = target - position;
	forward.normalize();

	// compute left vector
	Vector3 left = upVec.cross(forward);
	left.normalize();

	// compute orthonormal up vector
	Vector3 up = forward.cross(left);
	up.normalize();

	// NOTE: overwrite rotation and scale info of the current matrix
	this->setColumn(0, left);
	this->setColumn(1, up);
	this->setColumn(2, forward);

	return *this;
}

Matrix4& Matrix4::lookAt(float tx, float ty, float tz)
{
	return lookAt(Vector3(tx, ty, tz));
}

Matrix4& Matrix4::lookAt(float tx, float ty, float tz, float ux, float uy, float uz)
{
	return lookAt(Vector3(tx, ty, tz), Vector3(ux, uy, uz));
}

/*@@
///////////////////////////////////////////////////////////////////////////////
// skew with a given angle on the axis
///////////////////////////////////////////////////////////////////////////////
Matrix4& Matrix4::skew(float angle, const Vector3& axis)
{
float t = tanf(angle * DEG2RAD);    // tangent
m[0] += m[1] * t;
m[4] += m[5] * t;
m[8] += m[9] * t;
m[12]+= m[13]* t;
return *this;
}
*/



///////////////////////////////////////////////////////////////////////////////
// retrieve angles in degree from rotation matrix, M = Rx*Ry*Rz
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
// Rz: rotation about Z-axis, roll
//    Rx           Ry          Rz
// |1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
// |0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
// |0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
//
// Pitch: atan(-m[9] / m[10]) = atan(SxCy/CxCy)
// Yaw  : asin(m[8]) = asin(Sy)
// Roll : atan(-m[4] / m[0]) = atan(SzCy/CzCy)
///////////////////////////////////////////////////////////////////////////////
Vector3 Matrix4::getAngle() const
{
	float pitch, yaw, roll;         // 3 angles

									// find yaw (around y-axis) first
									// NOTE: asin() returns -90~+90, so correct the angle range -180~+180
									// using z value of forward vector
	yaw = RAD2DEG * asinf(m[8]);
	if (m[10] < 0)
	{
		if (yaw >= 0) yaw = 180.0f - yaw;
		else         yaw = -180.0f - yaw;
	}

	// find roll (around z-axis) and pitch (around x-axis)
	// if forward vector is (1,0,0) or (-1,0,0), then m[0]=m[4]=m[9]=m[10]=0
	if (m[0] > -EPSILON && m[0] < EPSILON)
	{
		roll = 0;  //@@ assume roll=0
		pitch = RAD2DEG * atan2f(m[1], m[5]);
	}
	else
	{
		roll = RAD2DEG * atan2f(-m[4], m[0]);
		pitch = RAD2DEG * atan2f(-m[9], m[10]);
	}

	return Vector3(pitch, yaw, roll);
}
