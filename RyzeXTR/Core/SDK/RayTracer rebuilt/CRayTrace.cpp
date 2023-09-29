#include "CRayTrace.h"

#include <cassert>

RayTracer::Ray::Ray(const Vector& direction) : m_startPoint(), m_direction(direction), m_length(0.f) {}

RayTracer::Ray::Ray(const Vector& startPoint, const Vector& endPoint) : m_startPoint(startPoint)
{
	//assert(startPoint != endPoint);

	auto vectorDiff = endPoint - startPoint;
	m_direction = vectorDiff.Normalized();
	m_length = vectorDiff.Length();
}

RayTracer::Hitbox::Hitbox() : m_mins(), m_maxs(), m_radius() {}

RayTracer::Hitbox::Hitbox(const Vector& mins, const Vector& maxs, const float radius) : m_mins(mins), m_maxs(maxs), m_radius(radius) {}

RayTracer::Hitbox::Hitbox(const std::tuple<Vector, Vector, float>& initTuple) : m_mins(std::get<0>(initTuple)), m_maxs(std::get<1>(initTuple)), m_radius(std::get<2>(initTuple)) {}

RayTracer::Trace::Trace() : m_hit(false), m_fraction(0.f), m_traceEnd() {}

void RayTracer::TraceFromCenter(const Ray& ray, const Hitbox& hitbox, Trace& trace, int flags)
{
	//assert(ray.m_direction.Length() > 0.f);

	// we are treating the cylinder as a cylinder y^2+z^2=r^2, in the x-direction, so we will make it the x direction
	constexpr Vector unitDesired(1.f, 0.f, 0.f);
	constexpr matrix3x4_t identityMatrix(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f
	);

	auto center = (hitbox.m_mins + hitbox.m_maxs) / 2.f;

	auto minsOffset = hitbox.m_mins - center;
	auto maxsOffset = hitbox.m_maxs - center;

	auto vecHitbox = maxsOffset - minsOffset;
	auto hitboxLength = vecHitbox.Length();

	// now we calculate the transformation matrix to get our normalized hitbox to center
	auto unitHitbox = vecHitbox / hitboxLength;

	// dot == cos because they are both unit vectors
	auto dot = unitHitbox.DotProduct(unitDesired);
	auto cross = unitHitbox.CrossProduct(unitDesired);

	Vector rotatedDirection;

	// if cross is 0, then we don't have to rotate because they are parallel
	if (cross.Length() > 0.f)
	{
		matrix3x4_t crossMatrix(
			0.f, -cross.z, cross.y, 0.f,
			cross.z, 0.f, -cross.x, 0.f,
			-cross.y, cross.x, 0.f, 0.f
		);

		auto rotationMatrix = identityMatrix + crossMatrix +
			(crossMatrix * crossMatrix) * (1.f / (1.f + dot));

		rotatedDirection = rotationMatrix * ray.m_direction;
	}
	else
	{
		// cross is 0, they are parallel, if dot is 1.f they are same, else they are opposite
		if (dot == 1.f)
			rotatedDirection = ray.m_direction;
		else
			rotatedDirection = (ray.m_direction * -1);
	}

	auto a = rotatedDirection.y * rotatedDirection.y +
		rotatedDirection.z * rotatedDirection.z;

	if (a == 0)
	{
		// the ray goes through both caps, easy case because we don't actually need to trace the ray because they are circles
		if (rotatedDirection.x > 0)
		{
			// through the right cap, scale by radius and call it a day
			auto newLength = minsOffset.Length() + hitbox.m_radius;
			auto offset = (minsOffset * (newLength) / minsOffset.Length());

			if (flags & Flags_RETURNEND)
				trace.m_traceEnd = center + offset;
			if (flags & Flags_RETURNOFFSET)
				trace.m_traceOffset = offset;
		}
		else
		{
			// through the left cap, scale by radius again
			auto newLength = maxsOffset.Length() + hitbox.m_radius;
			auto offset = (maxsOffset * (newLength) / maxsOffset.Length());

			if (flags & Flags_RETURNEND)
				trace.m_traceEnd = center + offset;
			if (flags & Flags_RETURNOFFSET)
				trace.m_traceOffset = offset;
		}
		trace.m_hit = true;
		return;
	}

	// b is always 0 because we start at the origin

	auto c = -(hitbox.m_radius * hitbox.m_radius);

	// we only care about t1 because we are inside of the surface that we are tracing, so t0 will always be negative. 
	// also -4*a*c will always be positive because c is always negative and a is always positive
	auto t = (sqrtf(-4.f * a * c)) / (2.f * a);

	//assert(t >= 0.f);

	// virutal direction is in the 3-D plane of x
	auto virtualPos = rotatedDirection * t;

	auto minsAdjusted = -hitboxLength / 2.f;
	auto maxsAdjusted = hitboxLength / 2.f;

	auto outOfMinsSide = virtualPos.x < minsAdjusted;
	auto outOfMaxsSide = virtualPos.x > maxsAdjusted;

	// we need to trace the spheres on the ends
	if (outOfMinsSide || outOfMaxsSide)
	{
		// set our position offset to the opposite side to raytrace the sphere
		Vector offsetPosition(
			(outOfMinsSide) ? -minsAdjusted : -maxsAdjusted, 0.f, 0.f
		);

		// we treat a_s as 1 because for a sphere, it is just the entire unit vector's length, always 1

		auto b_s = 2.f * (offsetPosition.x * rotatedDirection.x);

		auto c_s = offsetPosition.x * offsetPosition.x -
			hitbox.m_radius * hitbox.m_radius;

		auto operand = sqrtf(b_s * b_s - 4 * c_s);

		t = (-b_s + operand) / 2.f;

		//assert(t - FLT_EPSILON >= 0.f);
	}

	auto offset = ray.m_direction * t;

	if (flags & Flags_RETURNEND)
		trace.m_traceEnd = center + offset;
	if (flags & Flags_RETURNOFFSET)
		trace.m_traceOffset = offset;

	trace.m_hit = true;
}

void RayTracer::TraceHitbox(const Ray& ray, const Hitbox& hitbox, Trace& trace, int flags)
{
	//assert(ray.m_direction.Length() > 0.f);

	trace.m_fraction = 1.f;

	// we are treating the cylinder as a cylinder y^2+z^2=r^2, in the x-direction, so we will make it the x direction
	constexpr Vector unitDesired(1.f, 0.f, 0.f);
	constexpr matrix3x4_t identityMatrix(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f
	);

	auto center = (hitbox.m_mins + hitbox.m_maxs) / 2.f;

	auto minsOffset = hitbox.m_mins - center;
	auto maxsOffset = hitbox.m_maxs - center;

	auto vecHitbox = maxsOffset - minsOffset;
	auto hitboxLength = vecHitbox.Length();

	// now we calculate the transformation matrix to get our normalized hitbox to center
	auto unitHitbox = vecHitbox / hitboxLength;

	// dot == cos because they are both unit vectors
	auto dot = unitHitbox.DotProduct(unitDesired);
	auto cross = unitHitbox.CrossProduct(unitDesired);

	Vector rotatedDirection;
	Vector rotatedStart;

	// offset the position
	auto adjustedStart = ray.m_startPoint - center;

	// if cross is 0, then we don't have to rotate because they are parallel
	if (cross.Length() > 0.f)
	{
		matrix3x4_t crossMatrix(
			0.f, -cross.z, cross.y, 0.f,
			cross.z, 0.f, -cross.x, 0.f,
			-cross.y, cross.x, 0.f, 0.f
		);

		auto rotationMatrix = identityMatrix + crossMatrix +
			(crossMatrix * crossMatrix) * (1.f / (1.f + dot));

		rotatedDirection = rotationMatrix * ray.m_direction;
		rotatedStart = rotationMatrix * adjustedStart;
	}
	else
	{
		// cross is 0, they are parallel, if dot is 1.f they are same, else they are opposite
		if (dot > 0.f)
		{
			rotatedDirection = ray.m_direction;
			rotatedStart = adjustedStart;
		}
		else
		{
			rotatedDirection = (ray.m_direction * -1);
			rotatedStart = (adjustedStart * -1);
		}
	}

	auto a_c = rotatedDirection.y * rotatedDirection.y +
		rotatedDirection.z * rotatedDirection.z;

	/// TODO: Detect the plane!
	// this is INCREDIBLY RARE
	if (a_c == 0.f)
	{
		// the ray goes through both caps, easy case because we don't actually need to trace the ray because they are circles
		if (rotatedDirection.x > 0)
		{
			// through the right cap, scale by radius and call it a day
			auto newLength = minsOffset.Length() + hitbox.m_radius;
			auto offset = (minsOffset * (newLength) / minsOffset.Length());

			if (flags & Flags_RETURNEND)
				trace.m_traceEnd = center + offset;
			if (flags & Flags_RETURNOFFSET)
				trace.m_traceOffset = offset;
		}
		else
		{
			// through the left cap, scale by radius again
			auto newLength = maxsOffset.Length() + hitbox.m_radius;
			auto offset = (maxsOffset * (newLength) / maxsOffset.Length());

			if (flags & Flags_RETURNEND)
				trace.m_traceEnd = center + offset;
			if (flags & Flags_RETURNOFFSET)
				trace.m_traceOffset = offset;
		}
		trace.m_hit = true;
		return;
	}

	constexpr auto a_s = 1.f;

	auto minsAdjusted = -hitboxLength / 2.f;
	auto maxsAdjusted = hitboxLength / 2.f;

	auto minsStart = Vector(rotatedStart.x - minsAdjusted, rotatedStart.y, rotatedStart.z);
	auto maxsStart = Vector(rotatedStart.x - maxsAdjusted, rotatedStart.y, rotatedStart.z);

	auto b_c = 2.f *
		(rotatedStart.y * rotatedDirection.y +
			rotatedStart.z * rotatedDirection.z);

	auto b_smins = 2.f * (minsStart.DotProduct(rotatedDirection));
	auto b_smaxs = 2.f * (maxsStart.DotProduct(rotatedDirection));

	auto c_c = rotatedStart.y * rotatedStart.y +
		rotatedStart.z * rotatedStart.z -
		hitbox.m_radius * hitbox.m_radius;

	auto c_smins = minsStart.DotProduct(minsStart) -
		hitbox.m_radius * hitbox.m_radius;
	auto c_smaxs = maxsStart.DotProduct(maxsStart) -
		hitbox.m_radius * hitbox.m_radius;

	auto cylOperand = b_c * b_c - 4 * a_c * c_c;
	auto sphMinsOperand = b_smins * b_smins - 4 * a_s * c_smins;
	auto sphMaxsOperand = b_smaxs * b_smaxs - 4 * a_s * c_smaxs;

	auto tCyl = 0.f;
	auto tSphMins = 0.f;
	auto tSphMaxs = 0.f;

	auto cylHit = false;
	auto sphMinsHit = false;
	auto sphMaxsHit = false;

	// if we don't hit, operand is negative
	if (cylOperand > 0.f)
	{
		tCyl = (-b_c - sqrtf(cylOperand)) / (2.f * a_c);

		if (tCyl - FLT_EPSILON > 0.f)
		{
			// make sure we hit within our bounds, and not outside of the cylinder's bore
			auto virtualPos = rotatedDirection * tCyl;

			auto outOfMinsSide = virtualPos.x < minsAdjusted;
			auto outOfMaxsSide = virtualPos.x > maxsAdjusted;

			if (!outOfMinsSide &&
				!outOfMaxsSide)
				cylHit = true;
		}
	}

	if (sphMinsOperand > 0.f)
	{
		tSphMins = (-b_smins - sqrtf(sphMinsOperand)) / (2.f * a_s);

		if (tSphMins - FLT_EPSILON > 0.f)
			sphMinsHit = true;
	}

	if (sphMaxsOperand > 0.f)
	{
		tSphMaxs = (-b_smaxs - sqrtf(sphMaxsOperand)) / (2.f * a_s);

		if (tSphMaxs - FLT_EPSILON > 0.f)
			sphMaxsHit = true;
	}

	// see which one hit first, then return accordingly
	if (cylHit)
	{
		trace.m_fraction = tCyl / ray.m_length;
		trace.m_hit = true;
	}
	else if (sphMinsHit)
	{
		trace.m_fraction = tSphMins / ray.m_length;
		trace.m_hit = true;
	}
	else if (sphMaxsHit)
	{
		trace.m_fraction = tSphMaxs / ray.m_length;
		trace.m_hit = true;
	}
}