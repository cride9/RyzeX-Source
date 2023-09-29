#pragma once
#include <DirectXMath.h>
#include "DataTyes/Vector.h"
#include "../../Dependecies/ImGui/imgui.h"
#include "DataTyes/Matrix.h"
#include "Entity.h"
#pragma region math_definitions
#define M_HPI			DirectX::XM_PIDIV2	// half pi
#define M_QPI			DirectX::XM_PIDIV4	// quarter pi
#define M_PI			DirectX::XM_PI		// pi const
#define M_2PI			DirectX::XM_2PI		// pi double
#define M_GPI			1.6180339887498f	// golden ratio
#define M_RADPI			57.295779513082f	// pi in radians

#define M_METRE2INCH( x )	( ( x ) / 0.0254f )
#define M_INCH2METRE( x )	( ( x ) * 0.0254f )
#define M_METRE2FOOT( x )	( ( x ) * 3.28f )
#define M_FOOT2METRE( x )	( ( x ) / 3.28f )
#define M_RAD2DEG( x )		DirectX::XMConvertToDegrees( x )
#define M_DEG2RAD( x )		DirectX::XMConvertToRadians( x )
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)
#pragma endregion

extern "C" using RandomSeedFn = int(__cdecl*)(int iSeed);
extern "C" using RandomFloatFn = float(__cdecl*)(float flMinVal, float flMaxVal);
extern "C" using RandomFloatExpFn = float(__cdecl*)(float flMinVal, float flMaxVal, float flExponent);
extern "C" using RandomIntFn = int(__cdecl*)(int iMinVal, int iMaxVal);
extern "C" using RandomGaussianFloatFn = float(__cdecl*)(float flMean, float flStdDev);

/*
 * MATHEMATICS
 */
namespace M
{
	// Get
	/* export specific functions addresses from libraries handles to get able call them */
	bool	Setup();
	void VectorITransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	/* convert vector to angles */
	void	VectorAngles(const Vector& vecForward, Vector& angView);
	Vector	VectorAngles(const Vector& vecForward);
	/* convert angles to x, y, z vectors */
	void	AngleVectors(const Vector& angView, Vector* pForward, Vector* pRight = nullptr, Vector* pUp = nullptr);
	/* convert angles to matrix */
	void	AngleMatrix( const Vector& ang, const Vector& pos, matrix3x4_t& out );
	/* convert angles to matrix */
	void	AngleMatrix(const Vector& angView, matrix3x4_t& matOutput, const Vector& vecOrigin = Vector(0.0f, 0.0f, 0.0f));
	/* convert angle to screen pixels by sensivity, pitch and yaw */
	// @note: could be useful at mouse event aimbot
	Vector2D AnglePixels(const float flSensitivity, const float flPitch, const float flYaw, const Vector& angBegin, const Vector& angEnd);
	/* convert screen pixels to angles by sensivity, pitch and yaw */
	Vector	PixelsAngle(const float flSensitivity, const float flPitch, const float flYaw, const Vector2D& vecPixels);
	/* calculate angles by source and destination vectors */
	Vector	CalcAngle(const Vector& vecStart, const Vector& vecEnd);
	/* transform vector by matrix and return it */
	Vector	VectorTransform(const Vector& vecTransform, const matrix3x4_t& matrix);
	/* calculate next tick position */
	Vector	ExtrapolateTick(const Vector& p0, const Vector& v0);
	/* rotates given point and outputs to given outpoint by given angle */
	void	RotatePoint(const ImVec2& vecIn, const float flAngle, ImVec2* pOutPoint);
	/* rotates given center point and outputs to given outpoint by given angle */
	void	RotateCenter(const ImVec2& vecCenter, const float flAngle, ImVec2* pOutPoint);

	void VectorMatrix(const Vector& forward, matrix3x4_t& matrix);

	void ConcatTransforms(const matrix3x4_t&, const matrix3x4_t&, matrix3x4_t&);

	void VectorTransform(const Vector&, const matrix3x4_t&, Vector&);

	float Bias(float x, float biasAmt);

	float	GenerateRandom(float, float);

	void SinCos(float, float*, float*);

	float NormalizeYaw(float);

	void CrossProduct(const float* v1, const float* v2, float* cross);

	void VectorVectors(const Vector& forward, Vector& right, Vector& up);

	void VectorRotate(const float* in1, const matrix3x4_t& in2, float* out);

    void VectorIRotate(const Vector& in1, const matrix3x4_t& in2, Vector& out);

	void MatrixSetColumn(const Vector& in, int column, matrix3x4_t& out);

	float NormalizeAngle(float flAngle);

	float GetVelocityDegree(float flVelocity);

	template< class T >
	void Normalize3(T& vec);

	template < typename t >
	t Lerp( const t& t1, const t& t2, float progress )
	{
		return t1 + ( t2 - t1 ) * progress;
	}

	template < class T >
	T AnimationLerp(const T& flCurrent, const T& flTarget, const int iProgress, const int iMaximum)
	{
		return flCurrent + (((flTarget - flCurrent) / iMaximum) * iProgress);
	}

	void ClampAngles(Vector& angAngles);

	float RemapValClamped(float val, float A, float B, float C, float D);

	bool WorldToScreen( const Vector& in, Vector2D& out );

	float AngleDiff(float destAngle, float srcAngle);

	float DotProduct(Vector v1, Vector v2);

	Vector Interpolate(const Vector, const Vector, const float);
	float Interpolate(const float, const float, const float);
	void Extrapolate(CBaseEntity* pEnt, Vector& vecOrigin, Vector& vecVelocity, int& iFlags, bool bOnGround);

	float Approach(float, float, float);
	Vector Approach(Vector target, Vector value, float speed);
	float ApproachAngle(float, float, float);

	inline matrix3x4_t ToMatrix( const Vector& vecIn, const Vector& vecOrigin )
	{
		float flPitchSin, flPitchCos, flYawSin, flYawCos, flRollSin, flRollCos;
		DirectX::XMScalarSinCos( &flPitchSin, &flPitchCos, M_DEG2RAD( vecIn.x ) );
		DirectX::XMScalarSinCos( &flYawSin, &flYawCos, M_DEG2RAD( vecIn.y ) );
		DirectX::XMScalarSinCos( &flRollSin, &flRollCos, M_DEG2RAD( vecIn.z ) );

		return
		{
			( flPitchCos * flYawCos ), ( flRollSin * flPitchSin * flYawCos + flRollCos * -flYawSin ), ( flRollCos * flPitchSin * flYawCos + -flRollSin * -flYawSin ), vecOrigin.x,
			( flPitchCos * flYawSin ), ( flRollSin * flPitchSin * flYawSin + flRollCos * flYawCos ), ( flRollCos * flPitchSin * flYawSin + -flRollSin * flYawCos ), vecOrigin.y,
			( -flPitchSin ), ( flRollSin * flPitchCos ), ( flRollCos * flPitchCos ), vecOrigin.z
		};
	}

	// Exports
	inline RandomSeedFn				RandomSeed;
	inline RandomFloatFn			RandomFloat;
	inline RandomFloatExpFn			RandomFloatExp;
	inline RandomIntFn				RandomInt;
	inline RandomGaussianFloatFn	RandomGaussianFloat;
}