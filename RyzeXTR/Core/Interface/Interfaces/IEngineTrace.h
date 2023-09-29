#pragma once
#include <cstddef>
#include "../../SDK/DataTyes/Vector.h"
#include "../../SDK/DataTyes/UtlVector.h"
#include "../../SDK/DataTyes/Matrix.h"
#include <functional>
#include <../lua/embedding/sol/sol.hpp>

#define CONTENTS_EMPTY					0
#define CONTENTS_SOLID					0x1
#define CONTENTS_WINDOW					0x2
#define CONTENTS_AUX					0x4
#define CONTENTS_GRATE					0x8
#define CONTENTS_SLIME					0x10
#define CONTENTS_WATER					0x20
#define CONTENTS_BLOCKLOS				0x40
#define CONTENTS_OPAQUE					0x80
#define CONTENTS_TESTFOGVOLUM			0x100
#define CONTENTS_UNUSED					0x200
#define CONTENTS_BLOCKLIGHT				0x400
#define CONTENTS_TEAM1					0x800
#define CONTENTS_TEAM2					0x1000
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000
#define CONTENTS_MOVEABLE				0x4000
#define CONTENTS_AREAPORTAL				0x8000
#define CONTENTS_PLAYERCLIP				0x10000
#define CONTENTS_MONSTERCLIP			0x20000
#define CONTENTS_CURRENT_0				0x40000
#define CONTENTS_CURRENT_90				0x80000
#define CONTENTS_CURRENT_180			0x100000
#define CONTENTS_CURRENT_270			0x200000
#define CONTENTS_CURRENT_UP				0x400000
#define CONTENTS_CURRENT_DOWN			0x800000
#define CONTENTS_ORIGIN					0x1000000
#define CONTENTS_MONSTER				0x2000000
#define CONTENTS_DEBRIS					0x4000000
#define CONTENTS_DETAIL					0x8000000
#define CONTENTS_TRANSLUCENT			0x10000000
#define CONTENTS_LADDER					0x20000000
#define CONTENTS_HITBOX					0x40000000
#define LAST_VISIBLE_CONTENTS			CONTENTS_OPAQUE
#define ALL_VISIBLE_CONTENTS			(LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define SURF_LIGHT						0x0001
#define SURF_SKY2D						0x0002
#define SURF_SKY						0x0004
#define SURF_WARP						0x0008
#define SURF_TRANS						0x0010
#define SURF_NOPORTAL					0x0020
#define SURF_TRIGGER					0x0040
#define SURF_NODRAW						0x0080
#define SURF_HINT						0x0100
#define SURF_SKIP						0x0200
#define SURF_NOLIGHT					0x0400
#define SURF_BUMPLIGHT					0x0800
#define SURF_NOSHADOWS					0x1000
#define SURF_NODECALS					0x2000
#define SURF_NOCHOP						0x4000
#define SURF_HITBOX						0x8000

#define MASK_ALL						(0xFFFFFFFF)
#define MASK_SOLID						(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_PLAYERSOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_NPCSOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define MASK_NPCFLUID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define MASK_WATER						(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define MASK_OPAQUE						(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define MASK_OPAQUE_AND_NPCS			(MASK_OPAQUE|CONTENTS_MONSTER)
#define MASK_BLOCKLOS					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define MASK_BLOCKLOS_AND_NPCS			(MASK_BLOCKLOS|CONTENTS_MONSTER)
#define MASK_VISIBLE					(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define MASK_VISIBLE_AND_NPCS			(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define MASK_SHOT						(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define MASK_SHOT_BRUSHONLY				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define MASK_SHOT_HULL					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define MASK_SHOT_PORTAL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define MASK_SOLID_BRUSHONLY			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define MASK_PLAYERSOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define MASK_NPCSOLID_BRUSHONLY			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC				(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define MASK_NPCWORLDSTATIC_FLUID		(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define MASK_SPLITAREAPORTAL			(CONTENTS_WATER|CONTENTS_SLIME)
#define MASK_CURRENT					(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define MASK_DEADSOLID					(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)

enum EDispSurfFlags : int
{
	DISPSURF_FLAG_SURFACE = (1 << 0),
	DISPSURF_FLAG_WALKABLE = (1 << 1),
	DISPSURF_FLAG_BUILDABLE = (1 << 2),
	DISPSURF_FLAG_SURFPROP1 = (1 << 3),
	DISPSURF_FLAG_SURFPROP2 = (1 << 4)
};

enum ETraceType : int
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
	TRACE_FILTERSKY
};

enum EDebugTraceCounterBehavior : int
{
	TRACE_COUNTER_SET = 0,
	TRACE_COUNTER_INC,
};

struct BrushSideInfo_t
{
	VectorAligned	vecPlane;	// the plane of the brush side
	std::uint16_t	uBevel;		// bevel plane?
	std::uint16_t	uThin;		// thin?
};

struct cplane_t
{
	Vector		vecNormal;
	float		flDistance;
	std::byte	dType;
	std::byte	dSignBits;
	std::byte	pad[0x2];
};

struct csurface_t
{
	const char* szName;
	short			nSurfaceProps;
	std::uint16_t	uFlags;
};

class CBaseTrace
{
public:
	CBaseTrace() { }

	Vector			vecStart;		// start position
	Vector			vecEnd;			// final position
	cplane_t		plane;			// surface normal at impact
	float			flFraction;		// time completed, 1.0 = didn't hit anything
	int				iContents;		// contents on other side of surface hit
	std::uint16_t	fDispFlags;		// displacement flags for marking surfaces with data
	bool			bAllSolid;		// if true, plane is not valid
	bool			bStartSolid;	// if true, the initial point was in a solid area
};

class CBaseEntity;
class CGameTrace : public CBaseTrace
{
public:
	CGameTrace() : pHitEntity(nullptr) { }

	float				flFractionLeftSolid;	// time we left a solid, only valid if we started in solid
	csurface_t			surface;				// surface hit (impact surface)
	int					iHitGroup;				// 0 == generic, non-zero is specific body part
	short				sPhysicsBone;			// physics bone hit by trace in studio
	std::uint16_t		uWorldSurfaceIndex;		// index of the msurface2_t, if applicable
	CBaseEntity*		pHitEntity;				// entity hit by trace
	int					iHitbox;				// box hit by trace in studio

	inline bool DidHit() const
	{
		return (flFraction < 1.0f || bAllSolid || bStartSolid);
	}

	inline bool IsVisible() const
	{
		return (flFraction > 0.97f);
	}

	// exclusive lua only
	sol::object _lua_get( sol::stack_object key, sol::this_state L ) {
		// we use stack_object for the arguments because we
		// know the values from Lua will remain on Lua's stack,
		// so long we we don't mess with it
		auto maybeStringKey
			= key.as< sol::optional< std::string > >( );
		if (maybeStringKey) {
			const std::string& k = *maybeStringKey;
			if (k == XorStr( "vecStart" )) {
				return sol::object(
					L, sol::in_place, this->vecStart );
			}
			else if (k == XorStr( "vecEnd" )) {
				return sol::object(
					L, sol::in_place, this->vecEnd );
			}
			else if (k == XorStr( "flFraction" )) {
				return sol::object(
					L, sol::in_place, this->flFraction );
			}
			else if (k == XorStr( "iContents" )) {
				return sol::object(
					L, sol::in_place, this->iContents );
			}
			else if (k == XorStr( "bAllSolid" )) {
				return sol::object(
					L, sol::in_place, this->bAllSolid );
			}
			else if (k == XorStr( "bStartSolid" )) {
				return sol::object(
					L, sol::in_place, this->bStartSolid );
			}
			/*else if (k == XorStr( "pHitEntity" )) {
				return sol::object(
					L, sol::in_place, this->pHitEntity );
			}*/
			else if (k == XorStr( "iHitGroup" )) {
				return sol::object(
					L, sol::in_place, this->iHitGroup );
			}
			else if (k == XorStr( "iHitbox" )) {
				return sol::object(
					L, sol::in_place, this->iHitbox );
			}
			else if (k == XorStr( "fDispFlags" )) {
				return sol::object(
					L, sol::in_place, this->fDispFlags );
			}
		}

		// No valid key: push nil
		return sol::object( L, sol::in_place, sol::lua_nil );
	}

private:
	CGameTrace(const CGameTrace& other)
	{
		this->vecStart = other.vecStart;
		this->vecEnd = other.vecEnd;
		this->plane = other.plane;
		this->flFraction = other.flFraction;
		this->iContents = other.iContents;
		this->fDispFlags = other.fDispFlags;
		this->bAllSolid = other.bAllSolid;
		this->bStartSolid = other.bStartSolid;
		this->flFractionLeftSolid = other.flFractionLeftSolid;
		this->surface = other.surface;
		this->iHitGroup = other.iHitGroup;
		this->sPhysicsBone = other.sPhysicsBone;
		this->uWorldSurfaceIndex = other.uWorldSurfaceIndex;
		this->pHitEntity = other.pHitEntity;
		this->iHitbox = other.iHitbox;
	}
};

using Trace_t = CGameTrace;

struct Ray_t
{
	Ray_t(const Vector& vecStart, const Vector& vecEnd) :
		vecStart(vecStart), vecDelta(vecEnd - vecStart), matWorldAxisTransform(nullptr), bIsRay(true)
	{
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);
	}

	Ray_t(const Vector& vecStart, const Vector& vecEnd, const Vector& vecMins, const Vector& vecMaxs)
	{
		this->vecDelta = vecEnd - vecStart;

		this->matWorldAxisTransform = nullptr;
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);

		this->vecExtents = vecMaxs - vecMins;
		this->vecExtents *= 0.5f;
		this->bIsRay = (this->vecExtents.LengthSqr() < 1e-6f);

		this->vecStartOffset = vecMins + vecMaxs;
		this->vecStartOffset *= 0.5f;
		this->vecStart = vecStart + this->vecStartOffset;
		this->vecStartOffset *= -1.0f;
	}

	VectorAligned		vecStart;
	VectorAligned		vecDelta;
	VectorAligned		vecStartOffset;
	VectorAligned		vecExtents;
	const matrix3x4a_t* matWorldAxisTransform;
	bool				bIsRay;
	bool				bIsSwept;
};

class IHandleEntity;
class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntity, int fContentsMask) = 0;
	virtual ETraceType GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
	using FilterCallbackFn = std::function<bool(IHandleEntity*, int)>;

public:
	// @todo: sig ctracefiltersimple constructor and use it

	CTraceFilter(const IHandleEntity* pSkipEntity, ETraceType iTraceType = TRACE_EVERYTHING)
		: pSkip(pSkipEntity), iTraceType(iTraceType) { }

	CTraceFilter(FilterCallbackFn&& checkCallback, ETraceType iTraceType = TRACE_EVERYTHING)
		: checkCallback(std::move(checkCallback)), iTraceType(iTraceType) { }

	bool ShouldHitEntity(IHandleEntity* pHandleEntity, int fContentsMask) override
	{
		// if given user defined callback - check it
		if (checkCallback != nullptr)
			return checkCallback(pHandleEntity, fContentsMask);

		// otherwise skip entity if given
		return pHandleEntity != pSkip;
	}

	ETraceType GetTraceType() const override
	{
		return iTraceType;
	}

private:
	const IHandleEntity* pSkip = nullptr;
	FilterCallbackFn checkCallback = nullptr;
	ETraceType iTraceType = TRACE_EVERYTHING;
};

class ITraceListData
{
public:
	virtual			~ITraceListData() { }
	virtual void	Reset() = 0;
	virtual bool	IsEmpty() = 0;
	virtual bool	CanTraceRay(const Ray_t& ray) = 0;
};

class IEntityEnumerator
{
public:
	// this gets called with each handle
	virtual bool EnumEntity(IHandleEntity* pHandleEntity) = 0;
};

struct virtualmeshlist_t;
struct AABB_t;
class ICollideable;
class CPhysCollide;
class CBrushQuery;
class IEngineTrace
{
public:
	virtual int GetPointContents(const Vector& vecAbsPosition, int fContentsMask = MASK_ALL, IHandleEntity** ppEntity = nullptr) = 0;
	virtual int GetPointContents_WorldOnly(const Vector& vecAbsPosition, int fContentsMask = MASK_ALL) = 0;
	virtual int GetPointContents_Collideable(ICollideable* pCollide, const Vector& vecAbsPosition) = 0;
	virtual void ClipRayToEntity(const Ray_t& ray, unsigned int fMask, IHandleEntity* pEntity, Trace_t* pTrace) = 0;
	virtual void ClipRayToCollideable(const Ray_t& ray, unsigned int fMask, ICollideable* pCollide, Trace_t* pTrace) = 0;
	virtual void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void SetupLeafAndEntityListRay(const Ray_t& ray, ITraceListData* pTraceData) = 0;
	virtual void SetupLeafAndEntityListBox(const Vector& vecBoxMin, const Vector& vecBoxMax, ITraceListData* pTraceData) = 0;
	virtual void TraceRayAgainstLeafAndEntityList(const Ray_t& ray, ITraceListData* pTraceData, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void SweepCollideable(ICollideable* pCollide, const Vector& vecAbsStart, const Vector& vecAbsEnd, const Vector& vecAngles, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void EnumerateEntities(const Ray_t& ray, bool bTriggers, IEntityEnumerator* pEnumerator) = 0;
	virtual void EnumerateEntities(const Vector& vecAbsMins, const Vector& vecAbsMaxs, IEntityEnumerator* pEnumerator) = 0;
	virtual ICollideable* GetCollideable(IHandleEntity* pEntity) = 0;
	virtual int GetStatByIndex(int nIndex, bool bClear) = 0;
	virtual void GetBrushesInAABB(const Vector& vecMins, const Vector& vecMaxs, CUtlVector<int>* pOutput, int fContentsMask = MASK_ALL) = 0;
	virtual CPhysCollide* GetCollidableFromDisplacementsInAABB(const Vector& vecMins, const Vector& vecMaxs) = 0;
	virtual int GetNumDisplacements() = 0;
	virtual void GetDisplacementMesh(int nIndex, virtualmeshlist_t* pMeshTriList) = 0;
	virtual bool GetBrushInfo(int iBrush, CUtlVector<BrushSideInfo_t>* pBrushSideInfoOut, int* pContentsOut) = 0;
	virtual bool PointOutsideWorld(const Vector& vecPoint) = 0;
	virtual int GetLeafContainingPoint(const Vector& vecPoint) = 0;
	virtual ITraceListData* AllocTraceListData() = 0;
	virtual void FreeTraceListData(ITraceListData* pListData) = 0;
	virtual int GetSetDebugTraceCounter(int iValue, EDebugTraceCounterBehavior behavior) = 0;
	virtual int GetMeshesFromDisplacementsInAABB(const Vector& vecMins, const Vector& vecMaxs, virtualmeshlist_t* pOutputMeshes, int nMaxOutputMeshes) = 0;
	virtual void GetBrushesInCollideable(ICollideable* pCollideable, CBrushQuery& BrushQuery) = 0;
	virtual bool IsFullyOccluded(int nOcclusionKey, const AABB_t& aabb1, const AABB_t& aabb2, const Vector& vecShadow) = 0;
	virtual void SuspendOcclusionTests() = 0;
	virtual void ResumeOcclusionTests() = 0;
	virtual void FlushOcclusionQueries() = 0;
};