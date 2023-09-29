#pragma once
// used: vector
#include "vector.h"
#include <xmmintrin.h>
#include <intrin.h>

using matrix3x3_t = float[3][3];
struct matrix3x4_t
{
	matrix3x4_t() = default;

	constexpr matrix3x4_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23)
	{
		arrData[0][0] = m00; arrData[0][1] = m01; arrData[0][2] = m02; arrData[0][3] = m03;
		arrData[1][0] = m10; arrData[1][1] = m11; arrData[1][2] = m12; arrData[1][3] = m13;
		arrData[2][0] = m20; arrData[2][1] = m21; arrData[2][2] = m22; arrData[2][3] = m23;
	}

	constexpr matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin)
	{
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	constexpr void Init(const Vector& vecForward, const Vector& vecLeft, const Vector& vecUp, const Vector& vecOrigin)
	{
		SetForward(vecForward);
		SetLeft(vecLeft);
		SetUp(vecUp);
		SetOrigin(vecOrigin);
	}

	constexpr void SetForward(const Vector& vecForward)
	{
		this->arrData[0][0] = vecForward.x;
		this->arrData[1][0] = vecForward.y;
		this->arrData[2][0] = vecForward.z;
	}

	constexpr Vector GetForward() {

		return { this->arrData[0][0], this->arrData[1][0], this->arrData[2][0] };
	}

	constexpr void SetLeft(const Vector& vecLeft)
	{
		this->arrData[0][1] = vecLeft.x;
		this->arrData[1][1] = vecLeft.y;
		this->arrData[2][1] = vecLeft.z;
	}

	constexpr Vector GetLeft() {

		return { this->arrData[0][1], this->arrData[1][1], this->arrData[2][1] };
	}

	constexpr void SetUp(const Vector& vecUp)
	{
		this->arrData[0][2] = vecUp.x;
		this->arrData[1][2] = vecUp.y;
		this->arrData[2][2] = vecUp.z;
	}

	constexpr Vector GetUp() {

		return { this->arrData[0][2], this->arrData[1][2], this->arrData[2][2] };
	}

	constexpr void SetOrigin(const Vector& vecOrigin)
	{
		this->arrData[0][3] = vecOrigin.x;
		this->arrData[1][3] = vecOrigin.y;
		this->arrData[2][3] = vecOrigin.z;
	}

	constexpr Vector GetOrigin() {

		return { arrData[0][3], arrData[1][3], arrData[2][3] };
	}

	constexpr void Invalidate()
	{
		for (auto& arrSubData : arrData)
		{
			for (auto& flData : arrSubData)
				flData = std::numeric_limits<float>::infinity();
		}
	}

	Vector operator*(const Vector& vec) const {
		Vector result;
		for (int i = 0; i < 3; i++) {
			result[i] = arrData[i][0] * vec.x +
				arrData[i][1] * vec.y +
				arrData[i][2] * vec.z +
				arrData[i][3];
		}
		return result;
	}

	matrix3x4_t operator+(const matrix3x4_t& dataIn) const {

		matrix3x4_t result;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				result.arrData[i][j] = arrData[i][j] + dataIn[i][j];
			}
		}
		return result;
	}

	matrix3x4_t operator+(const matrix3x4_t& dataIn) {

		matrix3x4_t result;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				result.arrData[i][j] = arrData[i][j] + dataIn[i][j];
			}
		}
		return result;
	}

	matrix3x4_t operator*(const matrix3x4_t& dataIn) {

		matrix3x4_t result;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				result.arrData[i][j] = arrData[i][j] * dataIn[i][j];
			}
		}
		return result;
	}

	matrix3x4_t operator*(const float& dataIn) {

		matrix3x4_t result;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				result.arrData[i][j] = arrData[i][j] * dataIn;
			}
		}
		return result;
	}

	float* operator[](const int nIndex)
	{
		return arrData[nIndex];
	}

	const float* operator[](const int nIndex) const
	{
		return arrData[nIndex];
	}

	[[nodiscard]] constexpr Vector at(const int nIndex) const
	{
		return Vector(arrData[0][nIndex], arrData[1][nIndex], arrData[2][nIndex]);
	}

	float* Base()
	{
		return &arrData[0][0];
	}

	[[nodiscard]] const float* Base() const
	{
		return &arrData[0][0];
	}

	__forceinline matrix3x4_t ConcatTransforms( matrix3x4_t in ) const
	{
		matrix3x4_t out;

		out[ 0 ][ 0 ] = arrData[ 0 ][ 0 ] * in[ 0 ][ 0 ] + arrData[ 0 ][ 1 ] * in[ 1 ][ 0 ] + arrData[ 0 ][ 2 ] * in[ 2 ][ 0 ];
		out[ 0 ][ 1 ] = arrData[ 0 ][ 0 ] * in[ 0 ][ 1 ] + arrData[ 0 ][ 1 ] * in[ 1 ][ 1 ] + arrData[ 0 ][ 2 ] * in[ 2 ][ 1 ];
		out[ 0 ][ 2 ] = arrData[ 0 ][ 0 ] * in[ 0 ][ 2 ] + arrData[ 0 ][ 1 ] * in[ 1 ][ 2 ] + arrData[ 0 ][ 2 ] * in[ 2 ][ 2 ];
		out[ 0 ][ 3 ] = arrData[ 0 ][ 0 ] * in[ 0 ][ 3 ] + arrData[ 0 ][ 1 ] * in[ 1 ][ 3 ] + arrData[ 0 ][ 2 ] * in[ 2 ][ 3 ] + arrData[ 0 ][ 3 ];
		out[ 1 ][ 0 ] = arrData[ 1 ][ 0 ] * in[ 0 ][ 0 ] + arrData[ 1 ][ 1 ] * in[ 1 ][ 0 ] + arrData[ 1 ][ 2 ] * in[ 2 ][ 0 ];
		out[ 1 ][ 1 ] = arrData[ 1 ][ 0 ] * in[ 0 ][ 1 ] + arrData[ 1 ][ 1 ] * in[ 1 ][ 1 ] + arrData[ 1 ][ 2 ] * in[ 2 ][ 1 ];
		out[ 1 ][ 2 ] = arrData[ 1 ][ 0 ] * in[ 0 ][ 2 ] + arrData[ 1 ][ 1 ] * in[ 1 ][ 2 ] + arrData[ 1 ][ 2 ] * in[ 2 ][ 2 ];
		out[ 1 ][ 3 ] = arrData[ 1 ][ 0 ] * in[ 0 ][ 3 ] + arrData[ 1 ][ 1 ] * in[ 1 ][ 3 ] + arrData[ 1 ][ 2 ] * in[ 2 ][ 3 ] + arrData[ 1 ][ 3 ];
		out[ 2 ][ 0 ] = arrData[ 2 ][ 0 ] * in[ 0 ][ 0 ] + arrData[ 2 ][ 1 ] * in[ 1 ][ 0 ] + arrData[ 2 ][ 2 ] * in[ 2 ][ 0 ];
		out[ 2 ][ 1 ] = arrData[ 2 ][ 0 ] * in[ 0 ][ 1 ] + arrData[ 2 ][ 1 ] * in[ 1 ][ 1 ] + arrData[ 2 ][ 2 ] * in[ 2 ][ 1 ];
		out[ 2 ][ 2 ] = arrData[ 2 ][ 0 ] * in[ 0 ][ 2 ] + arrData[ 2 ][ 1 ] * in[ 1 ][ 2 ] + arrData[ 2 ][ 2 ] * in[ 2 ][ 2 ];
		out[ 2 ][ 3 ] = arrData[ 2 ][ 0 ] * in[ 0 ][ 3 ] + arrData[ 2 ][ 1 ] * in[ 1 ][ 3 ] + arrData[ 2 ][ 2 ] * in[ 2 ][ 3 ] + arrData[ 2 ][ 3 ];

		return out;
	}

	float arrData[3][4] = { };
};

class alignas(16) matrix3x4a_t : public matrix3x4_t
{
public:
	matrix3x4a_t() = default;

	constexpr matrix3x4a_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23)
	{
		arrData[0][0] = m00; arrData[0][1] = m01; arrData[0][2] = m02; arrData[0][3] = m03;
		arrData[1][0] = m10; arrData[1][1] = m11; arrData[1][2] = m12; arrData[1][3] = m13;
		arrData[2][0] = m20; arrData[2][1] = m21; arrData[2][2] = m22; arrData[2][3] = m23;
	}

	constexpr matrix3x4a_t(const matrix3x4_t& matSource)
	{
		*this = matSource;
	}

	constexpr matrix3x4a_t& operator=(const matrix3x4_t& matSource)
	{
		arrData[0][0] = matSource.arrData[0][0]; arrData[0][1] = matSource.arrData[0][1]; arrData[0][2] = matSource.arrData[0][2]; arrData[0][3] = matSource.arrData[0][3];
		arrData[1][0] = matSource.arrData[1][0]; arrData[1][1] = matSource.arrData[1][1]; arrData[1][2] = matSource.arrData[1][2]; arrData[1][3] = matSource.arrData[1][3];
		arrData[2][0] = matSource.arrData[2][0]; arrData[2][1] = matSource.arrData[2][1]; arrData[2][2] = matSource.arrData[2][2]; arrData[2][3] = matSource.arrData[2][3];
		return *this;
	}

	/// concatenate transformations of two aligned matrices into one
	/// @returns: aligned matrix with concatenated transformations
	//[[nodiscard]] matrix3x4a_t ConcatTransforms(const matrix3x4a_t& matOther) const
	//{
	//	matrix3x4a_t matOutput;
	//	assert((reinterpret_cast<std::uintptr_t>(this) & 15U) == 0 && (reinterpret_cast<std::uintptr_t>(&matOther) & 15U) == 0 && (reinterpret_cast<std::uintptr_t>(&matOutput) & 15U) == 0); // matrices aren't aligned

	//	__m128 thisRow0 = _mm_load_ps(this->arrData[0]);
	//	__m128 thisRow1 = _mm_load_ps(this->arrData[1]);
	//	__m128 thisRow2 = _mm_load_ps(this->arrData[2]);

	//	__m128 otherRow0 = _mm_load_ps(matOther.arrData[0]);
	//	__m128 otherRow1 = _mm_load_ps(matOther.arrData[1]);
	//	__m128 otherRow2 = _mm_load_ps(matOther.arrData[2]);

	//	__m128 outRow0 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow0, thisRow0, _MM_SHUFFLE(0, 0, 0, 0)), otherRow0), _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow0, thisRow0, _MM_SHUFFLE(1, 1, 1, 1)), otherRow1), _mm_mul_ps(_mm_shuffle_ps(thisRow0, thisRow0, _MM_SHUFFLE(2, 2, 2, 2)), otherRow2)));
	//	__m128 outRow1 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow1, thisRow1, _MM_SHUFFLE(0, 0, 0, 0)), otherRow0), _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow1, thisRow1, _MM_SHUFFLE(1, 1, 1, 1)), otherRow1), _mm_mul_ps(_mm_shuffle_ps(thisRow1, thisRow1, _MM_SHUFFLE(2, 2, 2, 2)), otherRow2)));
	//	__m128 outRow2 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow2, thisRow2, _MM_SHUFFLE(0, 0, 0, 0)), otherRow0), _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow2, thisRow2, _MM_SHUFFLE(1, 1, 1, 1)), otherRow1), _mm_mul_ps(_mm_shuffle_ps(thisRow2, thisRow2, _MM_SHUFFLE(2, 2, 2, 2)), otherRow2)));

	//	// add in translation vector
	//	constexpr std::uint32_t arrComponentMask[4] = { 0x0, 0x0, 0x0, 0xFFFFFFFF };
	//	outRow0 = _mm_add_ps(outRow0, _mm_and_ps(thisRow0, std::bit_cast<__m128>(arrComponentMask)));
	//	outRow1 = _mm_add_ps(outRow1, _mm_and_ps(thisRow1, std::bit_cast<__m128>(arrComponentMask)));
	//	outRow2 = _mm_add_ps(outRow2, _mm_and_ps(thisRow2, std::bit_cast<__m128>(arrComponentMask)));

	//	_mm_store_ps(matOutput.arrData[0], outRow0);
	//	_mm_store_ps(matOutput.arrData[1], outRow1);
	//	_mm_store_ps(matOutput.arrData[2], outRow2);
	//	return matOutput;
	//}
};
static_assert(alignof(matrix3x4a_t) == 16);

struct ViewMatrix_t
{
	ViewMatrix_t() = default;

	constexpr ViewMatrix_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33)
	{
		arrData[0][0] = m00; arrData[0][1] = m01; arrData[0][2] = m02; arrData[0][3] = m03;
		arrData[1][0] = m10; arrData[1][1] = m11; arrData[1][2] = m12; arrData[1][3] = m13;
		arrData[2][0] = m20; arrData[2][1] = m21; arrData[2][2] = m22; arrData[2][3] = m23;
		arrData[3][0] = m30; arrData[3][1] = m31; arrData[3][2] = m32; arrData[3][3] = m33;
	}

	constexpr ViewMatrix_t(const matrix3x4_t& matFrom, const Vector4D& vecAdditionalColumn = { })
	{
		arrData[0][0] = matFrom.arrData[0][0]; arrData[0][1] = matFrom.arrData[0][1]; arrData[0][2] = matFrom.arrData[0][2]; arrData[0][3] = matFrom.arrData[0][3];
		arrData[1][0] = matFrom.arrData[1][0]; arrData[1][1] = matFrom.arrData[1][1]; arrData[1][2] = matFrom.arrData[1][2]; arrData[1][3] = matFrom.arrData[1][3];
		arrData[2][0] = matFrom.arrData[2][0]; arrData[2][1] = matFrom.arrData[2][1]; arrData[2][2] = matFrom.arrData[2][2]; arrData[2][3] = matFrom.arrData[2][3];
		arrData[3][0] = vecAdditionalColumn.x; arrData[3][1] = vecAdditionalColumn.y; arrData[3][2] = vecAdditionalColumn.z; arrData[3][3] = vecAdditionalColumn.w;
	}

	float* operator[](const int nIndex)
	{
		return arrData[nIndex];
	}

	const float* operator[](const int nIndex) const
	{
		return arrData[nIndex];
	}

	[[nodiscard]] constexpr Vector4D at(const int nIndex) const
	{
		return Vector4D(arrData[0][nIndex], arrData[1][nIndex], arrData[2][nIndex], arrData[3][nIndex]);
	}

	constexpr ViewMatrix_t& operator+=(const ViewMatrix_t& matAdd)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int n = 0; n < 4; n++)
				this->arrData[i][n] += matAdd[i][n];
		}

		return *this;
	}

	constexpr ViewMatrix_t& operator-=(const ViewMatrix_t& matSubtract)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int n = 0; n < 4; n++)
				this->arrData[i][n] -= matSubtract[i][n];
		}

		return *this;
	}

	ViewMatrix_t operator*(const ViewMatrix_t& matMultiply) const
	{
		return ViewMatrix_t(
			arrData[0][0] * matMultiply.arrData[0][0] + arrData[0][1] * matMultiply.arrData[1][0] + arrData[0][2] * matMultiply.arrData[2][0] + arrData[0][3] * matMultiply.arrData[3][0],
			arrData[0][0] * matMultiply.arrData[0][1] + arrData[0][1] * matMultiply.arrData[1][1] + arrData[0][2] * matMultiply.arrData[2][1] + arrData[0][3] * matMultiply.arrData[3][1],
			arrData[0][0] * matMultiply.arrData[0][2] + arrData[0][1] * matMultiply.arrData[1][2] + arrData[0][2] * matMultiply.arrData[2][2] + arrData[0][3] * matMultiply.arrData[3][2],
			arrData[0][0] * matMultiply.arrData[0][3] + arrData[0][1] * matMultiply.arrData[1][3] + arrData[0][2] * matMultiply.arrData[2][3] + arrData[0][3] * matMultiply.arrData[3][3],

			arrData[1][0] * matMultiply.arrData[0][0] + arrData[1][1] * matMultiply.arrData[1][0] + arrData[1][2] * matMultiply.arrData[2][0] + arrData[1][3] * matMultiply.arrData[3][0],
			arrData[1][0] * matMultiply.arrData[0][1] + arrData[1][1] * matMultiply.arrData[1][1] + arrData[1][2] * matMultiply.arrData[2][1] + arrData[1][3] * matMultiply.arrData[3][1],
			arrData[1][0] * matMultiply.arrData[0][2] + arrData[1][1] * matMultiply.arrData[1][2] + arrData[1][2] * matMultiply.arrData[2][2] + arrData[1][3] * matMultiply.arrData[3][2],
			arrData[1][0] * matMultiply.arrData[0][3] + arrData[1][1] * matMultiply.arrData[1][3] + arrData[1][2] * matMultiply.arrData[2][3] + arrData[1][3] * matMultiply.arrData[3][3],

			arrData[2][0] * matMultiply.arrData[0][0] + arrData[2][1] * matMultiply.arrData[1][0] + arrData[2][2] * matMultiply.arrData[2][0] + arrData[2][3] * matMultiply.arrData[3][0],
			arrData[2][0] * matMultiply.arrData[0][1] + arrData[2][1] * matMultiply.arrData[1][1] + arrData[2][2] * matMultiply.arrData[2][1] + arrData[2][3] * matMultiply.arrData[3][1],
			arrData[2][0] * matMultiply.arrData[0][2] + arrData[2][1] * matMultiply.arrData[1][2] + arrData[2][2] * matMultiply.arrData[2][2] + arrData[2][3] * matMultiply.arrData[3][2],
			arrData[2][0] * matMultiply.arrData[0][3] + arrData[2][1] * matMultiply.arrData[1][3] + arrData[2][2] * matMultiply.arrData[2][3] + arrData[2][3] * matMultiply.arrData[3][3],

			arrData[3][0] * matMultiply.arrData[0][0] + arrData[3][1] * matMultiply.arrData[1][0] + arrData[3][2] * matMultiply.arrData[2][0] + arrData[3][3] * matMultiply.arrData[3][0],
			arrData[3][0] * matMultiply.arrData[0][1] + arrData[3][1] * matMultiply.arrData[1][1] + arrData[3][2] * matMultiply.arrData[2][1] + arrData[3][3] * matMultiply.arrData[3][1],
			arrData[3][0] * matMultiply.arrData[0][2] + arrData[3][1] * matMultiply.arrData[1][2] + arrData[3][2] * matMultiply.arrData[2][2] + arrData[3][3] * matMultiply.arrData[3][2],
			arrData[3][0] * matMultiply.arrData[0][3] + arrData[3][1] * matMultiply.arrData[1][3] + arrData[3][2] * matMultiply.arrData[2][3] + arrData[3][3] * matMultiply.arrData[3][3]);
	}

	constexpr void Identity()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int n = 0; n < 4; n++)
				this->arrData[i][n] = i == n ? 1.0f : 0.0f;
		}
	}

	const matrix3x4_t& As3x4() const
	{
		return *reinterpret_cast<const matrix3x4_t*>(this);
	}

	matrix3x4_t& As3x4()
	{
		return *reinterpret_cast<matrix3x4_t*>(this);
	}

	float arrData[4][4] = { };
};
