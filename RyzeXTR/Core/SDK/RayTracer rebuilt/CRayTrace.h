#pragma once
#include <tuple>
#include "../DataTyes/Matrix.h"
#include "../DataTyes/Vector.h"

/*
	RayTracer provides the same functionality as ClipRayToEntity, only
	it allows to trace for a specific hitbox through all others and provides
	exceptional performance compared to the game's RayTracing engine due to its
	inherent specialization
*/

class RayTracer
{
public:
	struct Ray
	{
		Ray(const Vector& direction);
		Ray(const Vector& startPoint, const Vector& endPoint);
		Vector m_startPoint;
		Vector m_direction;
		float m_length;
	};

	struct Hitbox
	{
		Hitbox();
		Hitbox(const Vector& mins, const Vector& maxs, const float radius);
		Hitbox(const std::tuple<Vector, Vector, float>& initTuple);
		Vector m_mins;
		Vector m_maxs;
		float m_radius;
	};

	struct Trace
	{
		Trace();
		bool m_hit;
		float m_fraction;
		Vector m_traceEnd;
		Vector m_traceOffset;
	};

	enum Flags
	{
		Flags_NONE = 0,
		Flags_RETURNEND = (1 << 0),
		Flags_RETURNOFFSET = (1 << 1)
	};

	// This is a specialization that starts from the center, as calculations are much simpler from the center of the hitbox
	static void TraceFromCenter(const Ray& ray, const Hitbox& hitbox, Trace& trace, int flags = 0);
	// This is for the general case, tracing against the hitbox
	static void TraceHitbox(const Ray& ray, const Hitbox& hitbox, Trace& trace, int flags = 0);
};