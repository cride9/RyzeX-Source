#include "math.h"
#include "../Interface/interfaces.h"
#include <random>

bool M::Setup()
{
	RandomSeed = reinterpret_cast<RandomSeedFn>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed"));
	if (RandomSeed == nullptr)
		return false;

	RandomFloat = reinterpret_cast<RandomFloatFn>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
	if (RandomFloat == nullptr)
		return false;

	RandomFloatExp = reinterpret_cast<RandomFloatExpFn>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloatExp"));
	if (RandomFloatExp == nullptr)
		return false;

	RandomInt = reinterpret_cast<RandomIntFn>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomInt"));
	if (RandomInt == nullptr)
		return false;

	RandomGaussianFloat = reinterpret_cast<RandomGaussianFloatFn>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomGaussianFloat"));
	if (RandomGaussianFloat == nullptr)
		return false;

	util::Print("Math initialized");
	return true;
}

void M::VectorITransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
{
	out.x = (in1.x - in2[0][3]) * in2[0][0] + (in1.y - in2[1][3]) * in2[1][0] + (in1.z - in2[2][3]) * in2[2][0];
	out.y = (in1.x - in2[0][3]) * in2[0][1] + (in1.y - in2[1][3]) * in2[1][1] + (in1.z - in2[2][3]) * in2[2][1];
	out.z = (in1.x - in2[0][3]) * in2[0][2] + (in1.y - in2[1][3]) * in2[1][2] + (in1.z - in2[2][3]) * in2[2][2];
}

void M::VectorTransform(const Vector& in, const matrix3x4_t& matrix, Vector& out) {

	Vector diff;
	diff = {
		in.x - matrix[0][3],
		in.y - matrix[1][3],
		in.z - matrix[2][3]
	};

	out = {
		diff.x * matrix[0][0] + diff.y * matrix[1][0] + diff.z * matrix[2][0],
		diff.x * matrix[0][1] + diff.y * matrix[1][1] + diff.z * matrix[2][1],
		diff.x * matrix[0][2] + diff.y * matrix[1][2] + diff.z * matrix[2][2]
	};
}

float M::GetVelocityDegree(float flVelocity) {

	float tmp = M_RAD2DEG(atan(30.0f / flVelocity));

	if (CheckIfNonValidNumber(tmp) || tmp > 90.0f)
		return 90.0f;

	else if (tmp < 0.0f)
		return 0.0f;
	else
		return tmp;
}

void MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out) {
	std::memcpy(out.Base(), in.Base(), sizeof(matrix3x4_t));
}

void M::ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out) {

	if (&in1 == &out) {
		matrix3x4_t in1b;
		MatrixCopy(in1, in1b);
		ConcatTransforms(in1b, in2, out);
		return;
	}

	if (&in2 == &out) {
		matrix3x4_t in2b;
		MatrixCopy(in2, in2b);
		ConcatTransforms(in1, in2b, out);
		return;
	}

	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];

	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];

	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}

void M::VectorAngles(const Vector& vecForward, Vector& angView)
{
	float flPitch, flYaw;

	if (vecForward.x == 0.f && vecForward.y == 0.f)
	{
		flPitch = (vecForward.z > 0.f) ? 270.f : 90.f;
		flYaw = 0.f;
	}
	else
	{
		flPitch = std::atan2f(-vecForward.z, vecForward.Length2D()) * 180.f / M_PI;

		if (flPitch < 0.f)
			flPitch += 360.f;

		flYaw = std::atan2f(vecForward.y, vecForward.x) * 180.f / M_PI;

		if (flYaw < 0.f)
			flYaw += 360.f;
	}

	angView.x = flPitch;
	angView.y = flYaw;
	angView.z = 0.f;
}

Vector M::VectorAngles(const Vector& vecForward)
{
	float flPitch, flYaw;

	if (vecForward.x == 0.f && vecForward.y == 0.f)
	{
		flPitch = (vecForward.z > 0.f) ? 270.f : 90.f;
		flYaw = 0.f;
	}
	else
	{
		flPitch = std::atan2f(-vecForward.z, vecForward.Length2D()) * 180.f / M_PI;

		if (flPitch < 0.f)
			flPitch += 360.f;

		flYaw = std::atan2f(vecForward.y, vecForward.x) * 180.f / M_PI;

		if (flYaw < 0.f)
			flYaw += 360.f;
	}

	return Vector(flPitch, flYaw, 0.f);
}


void M::AngleVectors(const Vector& angView, Vector* pForward, Vector* pRight, Vector* pUp)
{
	float sp, sy, sr, cp, cy, cr;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angView.x));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angView.y));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angView.z));

	if (pForward != nullptr)
	{
		pForward->x = cp * cy;
		pForward->y = cp * sy;
		pForward->z = -sp;
	}

	if (pRight != nullptr)
	{
		pRight->x = -1 * sr * sp * cy + -1 * cr * -sy;
		pRight->y = -1 * sr * sp * sy + -1 * cr * cy;
		pRight->z = -1 * sr * cp;
	}

	if (pUp != nullptr)
	{
		pUp->x = cr * sp * cy + -sr * -sy;
		pUp->y = cr * sp * sy + -sr * cy;
		pUp->z = cr * cp;
	}
}

void AngleMatrixCalculation( const Vector& angles, matrix3x4_t& matrix )
{
	float sr, sp, sy, cr, cp, cy;

	DirectX::XMScalarSinCos( &sp, &cp, M_DEG2RAD( angles.x ) );
	DirectX::XMScalarSinCos( &sy, &cy, M_DEG2RAD( angles.y ) );
	DirectX::XMScalarSinCos( &sr, &cr, M_DEG2RAD( angles.z ) );

	// matrix = (YAW * PITCH) * ROLL
	matrix[ 0 ][ 0 ] = cp * cy;
	matrix[ 1 ][ 0 ] = cp * sy;
	matrix[ 2 ][ 0 ] = -sp;

	float crcy = cr * cy;
	float crsy = cr * sy;
	float srcy = sr * cy;
	float srsy = sr * sy;
	matrix[ 0 ][ 1 ] = sp * srcy - crsy;
	matrix[ 1 ][ 1 ] = sp * srsy + crcy;
	matrix[ 2 ][ 1 ] = sr * cp;

	matrix[ 0 ][ 2 ] = ( sp * crcy + srsy );
	matrix[ 1 ][ 2 ] = ( sp * crsy - srcy );
	matrix[ 2 ][ 2 ] = cr * cp;

	matrix[ 0 ][ 3 ] = 0.0f;
	matrix[ 1 ][ 3 ] = 0.0f;
	matrix[ 2 ][ 3 ] = 0.0f;
}

void M::AngleMatrix( const Vector& ang, const Vector& pos, matrix3x4_t& out ) {
	AngleMatrixCalculation( ang, out );
	out.SetOrigin( pos );
}

void M::AngleMatrix(const Vector& angView, matrix3x4_t& matOutput, const Vector& vecOrigin)
{
	float sp, sy, sr, cp, cy, cr;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angView.x));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angView.y));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angView.z));

	matOutput.SetForward(Vector(cp * cy, cp * sy, -sp));

	const float crcy = cr * cy;
	const float crsy = cr * sy;
	const float srcy = sr * cy;
	const float srsy = sr * sy;

	matOutput.SetLeft(Vector(sp * srcy - crsy, sp * srsy + crcy, sr * cp));
	matOutput.SetUp(Vector(sp * crcy + srsy, sp * crsy - srcy, cr * cp));
	matOutput.SetOrigin(vecOrigin);
}

Vector2D M::AnglePixels(const float flSensitivity, const float flPitch, const float flYaw, const Vector& angBegin, const Vector& angEnd)
{
	Vector angDelta = angBegin - angEnd;
	angDelta.Normalize();

	const float flPixelMovePitch = (-angDelta.x) / (flYaw * flSensitivity);
	const float flPixelMoveYaw = (angDelta.y) / (flPitch * flSensitivity);

	return Vector2D(flPixelMoveYaw, flPixelMovePitch);
}

Vector M::PixelsAngle(const float flSensitivity, const float flPitch, const float flYaw, const Vector2D& vecPixels)
{
	const float flAngleMovePitch = (-vecPixels.x) * (flYaw * flSensitivity);
	const float flAngleMoveYaw = (vecPixels.y) * (flPitch * flSensitivity);

	return Vector(flAngleMoveYaw, flAngleMovePitch, 0.f);
}

Vector M::CalcAngle(const Vector& vecStart, const Vector& vecEnd)
{
	Vector angView;
	const Vector vecDelta = vecEnd - vecStart;
	VectorAngles(vecDelta, angView);
	angView.Normalize();

	return angView;
}

Vector M::VectorTransform(const Vector& vecTransform, const matrix3x4_t& matrix)
{
	return Vector(vecTransform.DotProduct(matrix[0]) + matrix[0][3],
		vecTransform.DotProduct(matrix[1]) + matrix[1][3],
		vecTransform.DotProduct(matrix[2]) + matrix[2][3]);
}

Vector M::ExtrapolateTick(const Vector& p0, const Vector& v0)
{
	// position formula: p0 + v0t
	return p0 + (v0 * i::GlobalVars->flIntervalPerTick);
}
	
Vector M::Interpolate(const Vector from, const Vector to, const float percent) {
	return to * percent + from * (1.f - percent);
}

float M::ApproachAngle(float flTarget, float flValue, float flSpeed) {

	float delta = flTarget - flValue;

	// Speed is assumed to be positive
	if (flSpeed < 0)
		flSpeed = -flSpeed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > flSpeed)
		flValue += flSpeed;
	else if (delta < -flSpeed)
		flValue -= flSpeed;
	else
		flValue = flTarget;

	return flValue;
}

// not the same as math::Lerp
float M::Interpolate(const float from, const float to, const float percent) {
	return to * percent + from * (1.f - percent);
}

void M::Extrapolate(CBaseEntity* pEnt , Vector& vecOrigin, Vector& vecVelocity, int& iFlags, bool bOnGround) {

	Vector                start, end, normal;
	CGameTrace            trace;
	CTraceFilter		  filter(nullptr, TRACE_WORLD_ONLY);

	// define trace start.
	start = vecOrigin;

	// move trace end one tick into the future using predicted velocity.
	end = start + (vecVelocity * i::GlobalVars->flIntervalPerTick);

	// trace.
	i::EngineTrace->TraceRay(Ray_t(start, end, pEnt->GetCollideable()->OBBMins(), pEnt->GetCollideable()->OBBMaxs()), CONTENTS_SOLID, &filter, &trace);

	// we hit shit
	// we need to fix shit.
	if (trace.flFraction != 1.f) {

		// fix sliding on planes.
		for (int i{ }; i < 2; ++i) {
			vecVelocity -= trace.plane.vecNormal * vecVelocity.DotProduct(trace.plane.vecNormal);

			float adjust = vecVelocity.DotProduct(trace.plane.vecNormal);
			if (adjust < 0.f)
				vecVelocity -= (trace.plane.vecNormal * adjust);

			start = trace.vecEnd;
			end = start + (vecVelocity * (i::GlobalVars->flIntervalPerTick * (1.f - trace.flFraction)));

			i::EngineTrace->TraceRay(Ray_t(start, end, pEnt->GetCollideable()->OBBMins(), pEnt->GetCollideable()->OBBMaxs()), CONTENTS_SOLID, &filter, &trace);
			if (trace.flFraction == 1.f)
				break;
		}
	}

	// set new final origin.
	start = end = vecOrigin = trace.vecEnd;

	// move endpos 2 units down.
	// this way we can check if we are in/on the ground.
	end.z -= 2.f;

	// trace.
	i::EngineTrace->TraceRay(Ray_t(start, end, pEnt->GetCollideable()->OBBMins(), pEnt->GetCollideable()->OBBMaxs()), CONTENTS_SOLID, &filter, &trace);

	// strip onground flag.
	iFlags &= ~FL_ONGROUND;

	// add back onground flag if we are onground.
	if (trace.flFraction != 1.f && trace.plane.vecNormal.z > 0.7f)
		iFlags |= FL_ONGROUND;
}

void M::RotatePoint(const ImVec2& vecIn, const float flAngle, ImVec2* pOutPoint)
{
	if (&vecIn == pOutPoint)
	{
		const ImVec2 vecPoint = vecIn;
		RotatePoint(vecPoint, flAngle, pOutPoint);
		return;
	}

	const float flSin = std::sinf(M_DEG2RAD(flAngle));
	const float flCos = std::cosf(M_DEG2RAD(flAngle));

	pOutPoint->x = vecIn.x * flCos - vecIn.y * flSin;
	pOutPoint->y = vecIn.x * flSin + vecIn.y * flCos;
}

void M::SinCos(float flRadian, float* flSin, float* flCos) {
	*flSin = std::sin(flRadian);
	*flCos = std::cos(flRadian);
}

float M::GenerateRandom(float flMin, float flMax) {

	static std::random_device rand_device;
	static std::mt19937 rand_gen(rand_device());

	std::uniform_real_distribution<float> rand_distributer(flMin, flMax);
	return rand_distributer(rand_gen);
}

float M::NormalizeYaw(float flYaw) {

	float yaww = flYaw;
	while (yaww < -180.0f) yaww += 360.0f;
	while (yaww > 180.0f) yaww -= 360.0f;
	return yaww;
}

void M::CrossProduct(const float* v1, const float* v2, float* cross)
{
	cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
	cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
	cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void M::VectorVectors(const Vector& forward, Vector& right, Vector& up)
{
	Vector tmp;

	if (fabs(forward[0]) < 1e-6 && fabs(forward[1]) < 1e-6)
	{
		// pitch 90 degrees up/down from identity
		right[0] = 0;
		right[1] = -1;
		right[2] = 0;
		up[0] = -forward[2];
		up[1] = 0;
		up[2] = 0;
	}
	else
	{
		tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
		CrossProduct(forward.data(), tmp.data(), right.data());
		right.VectorNormalize();
		CrossProduct(right.data(), forward.data(), up.data());
		up.VectorNormalize();
	}
}

void M::VectorRotate(const float* in1, const matrix3x4_t& in2, float* out)
{
	out[0] = DotProduct(in1, in2[0]);
	out[1] = DotProduct(in1, in2[1]);
	out[2] = DotProduct(in1, in2[2]);
}

void M::VectorIRotate(const Vector& in1, const matrix3x4_t& in2, Vector& out)
{
	out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
	out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
	out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
}

void M::MatrixSetColumn(const Vector& in, int column, matrix3x4_t& out)
{
	out[0][column] = in.x;
	out[1][column] = in.y;
	out[2][column] = in.z;
}

void M::VectorMatrix(const Vector& forward, matrix3x4_t& matrix) {

	Vector right, up; 
	VectorVectors(forward, right, up);

	MatrixSetColumn(forward, 0, matrix);
	MatrixSetColumn((right * -1), 1, matrix);
	MatrixSetColumn(up, 2, matrix);
}

void M::RotateCenter(const ImVec2& vecCenter, const float flAngle, ImVec2* pOutPoint)
{
	const float flSin = std::sinf(M_DEG2RAD(flAngle));
	const float flCos = std::cosf(M_DEG2RAD(flAngle));

	pOutPoint->x -= vecCenter.x;
	pOutPoint->y -= vecCenter.y;

	const float x = pOutPoint->x * flCos - pOutPoint->y * flSin;
	const float y = pOutPoint->x * flSin + pOutPoint->y * flCos;

	pOutPoint->x = x + vecCenter.x;
	pOutPoint->y = y + vecCenter.y;
}

float M::Bias(float x, float biasAmt)
{
	// WARNING: not thread safe
	static float lastAmt = -1;
	static float lastExponent = 0;
	if (lastAmt != biasAmt)
	{
		lastExponent = log(biasAmt) * -1.4427f; // (-1.4427 = 1 / log(0.5))
	}
	return pow(x, lastExponent);
}

float M::Approach(float target, float value, float speed) {
	float delta = target - value;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

Vector M::Approach(Vector target, Vector value, float speed) {

	float delta = target.DistTo(value);

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

float M::AngleDiff(float destAngle, float srcAngle)
{
	float delta = fmodf(destAngle - srcAngle, 360.0f);
	if (destAngle > srcAngle)
	{
		if (delta >= 180)
			delta -= 360;
	}
	else
	{
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

float M::NormalizeAngle(float flAngle)
{
	flAngle = fmod(flAngle, 360.0f);
	if (flAngle > 180.0f)
		flAngle -= 360.0f;
	if (flAngle < -180.0f)
		flAngle += 360.0f;

	return flAngle;
}

float M::DotProduct(Vector v1, Vector v2)
{
	return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
};

template< class T >
void M::Normalize3(T& vec)
{
	for (auto i = 0; i < 2; i++) {
		while (vec[i] < -180.0f) vec[i] += 360.0f;
		while (vec[i] > 180.0f) vec[i] -= 360.0f;
	}

	while (vec[2] < -50.0f) vec[2] += 100.0f;
	while (vec[2] > 50.0f) vec[2] -= 100.0f;
}

void M::ClampAngles(Vector& angAngles)
{
	if (angAngles.x > 89.0f)
		angAngles.x = 89.0f;
	else if (angAngles.x < -89.0f)
		angAngles.x = -89.0f;

	if (angAngles.y > 180.0f)
		angAngles.y = 180.0f;
	else if (angAngles.y < -180.0f)
		angAngles.y = -180.0f;

	angAngles.z = std::clamp(angAngles.z, -45.0f, 45.0f);
}

float M::RemapValClamped(float val, float A, float B, float C, float D)
{
	if (A == B)
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	cVal = std::clamp(cVal, 0.0f, 1.0f);

	return C + (D - C) * cVal;
}

bool M::WorldToScreen( const Vector& in, Vector2D& out )
{
	const auto ScreenTransform = []( const Vector& in, Vector2D& out ) -> bool
	{
		static auto& W2SMatrix = i::EngineClient->WorldToScreenMatrix( );

		out.x = W2SMatrix.arrData[ 0 ][ 0 ] * in.x + W2SMatrix.arrData[ 0 ][ 1 ] * in.y + W2SMatrix.arrData[ 0 ][ 2 ] * in.z + W2SMatrix.arrData[ 0 ][ 3 ];
		out.y = W2SMatrix.arrData[ 1 ][ 0 ] * in.x + W2SMatrix.arrData[ 1 ][ 1 ] * in.y + W2SMatrix.arrData[ 1 ][ 2 ] * in.z + W2SMatrix.arrData[ 1 ][ 3 ];

		const float w = W2SMatrix.arrData[ 3 ][ 0 ] * in.x + W2SMatrix.arrData[ 3 ][ 1 ] * in.y + W2SMatrix.arrData[ 3 ][ 2 ] * in.z + W2SMatrix.arrData[ 3 ][ 3 ];

		if (w < 0.001f)
		{
			out.x *= 100000.0f;
			out.y *= 100000.0f;
			return false;
		}

		out.x /= w;
		out.y /= w;

		return true;
	};

	if (ScreenTransform( in, out ))
	{
		int w = 0, h = 0;
		i::EngineClient->GetScreenSize( w, h );

		out.x = ( w / 2.0f ) + ( out.x * w ) / 2.0f;
		out.y = ( h / 2.0f ) - ( out.y * h ) / 2.0f;

		return true;
	}

	return false;
}

//float M::ApproachAngle(float target, float value, float speed) {
//
//	target = anglemod(target);
//	value = anglemod(value);
//
//	float delta = target - value;
//
//	if (speed < 0)
//		speed = -speed;
//
//	if (delta < -180)
//		delta += 360;
//	else if (delta > 180)
//		delta -= 360;
//
//	if (delta > speed)
//		value += speed;
//	else if (delta < -speed)
//		value -= speed;
//	else
//		value = target;
//
//	return value;
//}