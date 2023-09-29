#pragma once
using RadianEuler = float[3];
using Quaternion = float[4];

struct Quaternion_t
{
	constexpr Quaternion_t(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f, const float w = 0.0f) :
		x(x), y(y), z(z), w(w) { }

	[[nodiscard]] bool IsValid() const
	{
		return (std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w));
	}

	/// @param[in] vecOrigin [optional] translation for converted matrix
	/// @returns: matrix converted from quaternion
	[[nodiscard]] matrix3x4a_t ToMatrix(const Vector& vecOrigin = { }) const
	{
		matrix3x4a_t matOut;

#ifdef _DEBUG // precalculate common multiplications
		const float x2 = this->x + this->x, y2 = this->y + this->y, z2 = this->z + this->z;
		const float xx = this->x * x2, xy = this->x * y2, xz = this->x * z2;
		const float yy = this->y * y2, yz = this->y * z2;
		const float zz = this->z * z2;
		const float wx = this->w * x2, wy = this->w * y2, wz = this->w * z2;

		matOut[0][0] = 1.0f - (yy + zz);
		matOut[1][0] = xy + wz;
		matOut[2][0] = xz - wy;

		matOut[0][1] = xy - wz;
		matOut[1][1] = 1.0f - (xx + zz);
		matOut[2][1] = yz + wx;

		matOut[0][2] = xz + wy;
		matOut[1][2] = yz - wx;
		matOut[2][2] = 1.0f - (xx + yy);
#else // let the compiler optimize calculations itself
		matOut[0][0] = 1.0f - 2.0f * this->y * this->y - 2.0f * this->z * this->z;
		matOut[1][0] = 2.0f * this->x * this->y + 2.0f * this->w * this->z;
		matOut[2][0] = 2.0f * this->x * this->z - 2.0f * this->w * this->y;

		matOut[0][1] = 2.0f * this->x * this->y - 2.0f * this->w * this->z;
		matOut[1][1] = 1.0f - 2.0f * this->x * this->x - 2.0f * this->z * this->z;
		matOut[2][1] = 2.0f * this->y * this->z + 2.0f * this->w * this->x;

		matOut[0][2] = 2.0f * this->x * this->z + 2.0f * this->w * this->y;
		matOut[1][2] = 2.0f * this->y * this->z - 2.0f * this->w * this->x;
		matOut[2][2] = 1.0f - 2.0f * this->x * this->x - 2.0f * this->y * this->y;
#endif

		matOut[0][3] = vecOrigin.x;
		matOut[1][3] = vecOrigin.y;
		matOut[2][3] = vecOrigin.z;
		return matOut;
	}

	float x, y, z, w;
};

struct alignas(16) QuaternionAligned_t : Quaternion_t
{
	QuaternionAligned_t& operator=(const Quaternion_t& quatOther)
	{
		this->x = quatOther.x;
		this->y = quatOther.y;
		this->z = quatOther.z;
		this->w = quatOther.w;
		return *this;
	}
};
static_assert(alignof(QuaternionAligned_t) == 16);


#pragma region studio_definitions
#define MAXSTUDIOBONECTRLS			4
#define MAXSTUDIOPOSEPARAM			24
#define MAXSTUDIOSKINS				32		// total textures
#define MAXSTUDIOFLEXCTRL			96		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOBONES				256		// total bones actually used
#define MAXSTUDIOANIMBLOCKS			256
#define MAXSTUDIOFLEXDESC			1024	// maximum number of low level flexes (actual morph targets)

#define BONE_CALCULATE_MASK			0x1F
#define BONE_PHYSICALLY_SIMULATED	0x01
#define BONE_PHYSICS_PROCEDURAL		0x02
#define BONE_ALWAYS_PROCEDURAL		0x04
#define BONE_SCREEN_ALIGN_SPHERE	0x08
#define BONE_SCREEN_ALIGN_CYLINDER	0x10

#define BONE_USED_MASK				0x0007FF00
#define BONE_USED_BY_ANYTHING		0x0007FF00
#define BONE_USED_BY_HITBOX			0x00000100
#define BONE_USED_BY_ATTACHMENT		0x00000200
#define BONE_USED_BY_VERTEX_MASK	0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0	0x00000400
#define BONE_USED_BY_VERTEX_LOD1	0x00000800
#define BONE_USED_BY_VERTEX_LOD2	0x00001000
#define BONE_USED_BY_VERTEX_LOD3	0x00002000
#define BONE_USED_BY_VERTEX_LOD4	0x00004000
#define BONE_USED_BY_VERTEX_LOD5	0x00008000
#define BONE_USED_BY_VERTEX_LOD6	0x00010000
#define BONE_USED_BY_VERTEX_LOD7	0x00020000
#define BONE_USED_BY_BONE_MERGE		0x00040000
#define BONE_ALWAYS_SETUP			0x00080000

#define BONE_USED_BY_VERTEX_AT_LOD( iLOD ) ( BONE_USED_BY_VERTEX_LOD0 << ( iLOD ) )
#define BONE_USED_BY_ANYTHING_AT_LOD( iLOD ) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD( iLOD ) )

#define MAX_NUM_LODS				8

#define BONE_TYPE_MASK				0x00F00000
#define BONE_FIXED_ALIGNMENT		0x00100000
#define BONE_HAS_SAVEFRAME_POS		0x00200000
#define BONE_HAS_SAVEFRAME_ROT64	0x00400000
#define BONE_HAS_SAVEFRAME_ROT32	0x00800000
#pragma endregion

struct vcollide_t;
struct virtualmodel_t;
struct vertexfileheader_t;

enum EMatrixFlags
{
	Interpolated = (1 << 1),
	BoneUsedByHitbox = (1 << 2),
	VisualAdjustment = (1 << 3)
};

struct mstudiobone_t
{
	int			nNameIndex;
	int			iParent;			// parent bone
	int			iBoneController[6]; // bone controller index, -1 == none

	// default values
	Vector		vecPosition;
	Quaternion_t qWorld;
	RadianEuler radRotation;

	// compression scale
	Vector		vecPositionScale;
	Vector		vecEulerScale;

	matrix3x4_t matPoseToBone;
	Quaternion_t qAlignment;
	int			iFlags;
	int			iProcedureType;
	int			nProcedureIndex;	// procedural rule
	mutable int miPhysicsBone;		// index into physically simulated bone
	int			nSurfacePropIndex;	// index into string table for property name
	int			iContents;			// see bspflags.h for the contents flags
	int			iSurfaceProp;		// this index must be cached by the loader, not saved in the file
	std::byte	pad0[0x1C];

	inline const char* GetName()
	{
		if (!nNameIndex) return nullptr;
		return (char*)this + nNameIndex;
	}

	inline void* GetProcedure() const
	{
		if (!nProcedureIndex) return nullptr;
		return (std::uint8_t*)this + nProcedureIndex;
	}

	inline const char* GetSurfacePropName() const
	{
		if (!nSurfacePropIndex) return nullptr;
		return (char*)this + nSurfacePropIndex;
	}
};

struct mstudiobonecontroller_t
{
	int			iBone; // -1 == 0
	int			nType; // X, Y, Z, XR, YR, ZR, M
	float		flStart;
	float		flEnd;
	int			iRest; // byte index value at rest
	int			iInputField; // 0-3 user set controller, 4 mouth
	std::byte	pad0[0x40];
};

struct mstudiobbox_t
{
	int			iBone;
	int			iGroup; // intersection group
	Vector		vecBBMin; // bounding box
	Vector		vecBBMax;
	int			nHitboxNameIndex; // offset to the name of the hitbox
	Vector		angOffsetOrientation;
	float		flRadius;
	std::byte	pad0[0x10];

	inline const char* GetHitboxName() const
	{
		if (!nHitboxNameIndex) return nullptr;
		return (const char*)this + nHitboxNameIndex;
	}
};

struct mstudiohitboxset_t
{
	int nNameIndex;
	int nHitboxes;
	int nHitboxIndex;

	inline const char* GetName() const
	{
		if (!nNameIndex) return nullptr;
		return (char*)this + nNameIndex;
	}

	inline mstudiobbox_t* GetHitbox(int iHitBox) const
	{
		if (iHitBox < 0 || iHitBox >= nHitboxes) return nullptr;
		return (mstudiobbox_t*)((std::uint8_t*)this + nHitboxIndex) + iHitBox;
	}
};

class virtualgroup_t
{
public:
	void* pCache;
	CUtlVector<int> vecBoneMap;
	CUtlVector<int> vecMasterBone;
	CUtlVector<int> vecMasterSequence;
	CUtlVector<int> vecMasterAnim;
	CUtlVector<int> vecMasterAttachment;
	CUtlVector<int> vecMasterPose;
	CUtlVector<int> vecMasterNode;
};

struct virtualsequence_t
{
	int	nFlags;
	int iActivity;
	int iGroup;
	int nIndex;
};

struct virtualgeneric_t
{
	int iGroup;
	int nIndex;
};

struct virtualmodel_t
{
	inline virtualgroup_t* GetAnimGroup(const int iAnimation)
	{
		// Note: user must manage mutex for this
		return &vecGroup[vecAnim[iAnimation].iGroup];
	}

	inline virtualgroup_t* GetSequenceGroup(const int iSequence)
	{
		// Note: user must manage mutex for this
		return &vecGroup[vecSequence[iSequence].iGroup];
	}

	std::byte pad0[0x8]; // CThreadFastMutex
	CUtlVector<virtualsequence_t> vecSequence;
	CUtlVector<virtualgeneric_t> vecAnim;
	CUtlVector<virtualgeneric_t> vecAttachment;
	CUtlVector<virtualgeneric_t> vecPose;
	CUtlVector<virtualgroup_t> vecGroup;
	CUtlVector<virtualgeneric_t> vecNode;
	CUtlVector<virtualgeneric_t> vecIKLock;
	CUtlVector<unsigned short> vecAutoplaySequences;
};

struct studiohdr_t;
struct mstudioseqdesc_t
{
	int nBaseStudio;
	inline studiohdr_t* GetStudioHdr() const
	{
		return (studiohdr_t*)((std::uint8_t*)this + nBaseStudio);
	}

	int	nLabelIndex;
	inline const char* GetLabel() const
	{
		return (char*)this + nLabelIndex;
	}

	int	nActivityNameIndex;
	inline const char* GetActivityName() const
	{
		return (char*)this + nActivityNameIndex;
	}

	int	iFlags;

	int	iActivity;
	int	iActivityWeight;

	int nEvents;
	int nEventIndex;
	inline void* GetEvent(int iEvent) const
	{
		if (iEvent < 0 || iEvent >= nEvents) return nullptr;
		return ((std::uint8_t*)this + nEventIndex) + iEvent;
	}

	Vector vecBBMin;
	Vector vecBBMax;

	int	nBlends;
	int	nBlendIndex;

	inline int GetAnimValue(int x, int y) const
	{
		if (x >= iGroupSize[0])
			x = iGroupSize[0] - 1;

		if (y >= iGroupSize[1])
			y = iGroupSize[1] - 1;

		int iOffset = y * iGroupSize[0] + x;
		short* arrBlends = (short*)((std::uint8_t*)this + nBlendIndex);
		return (int)arrBlends[iOffset];
	}

	int	nMovementIndex;		// [blend] float array for blended movement
	int iGroupSize[2];
	int nParamIndex[2];		// X, Y, Z, XR, YR, ZR
	float flParamStart[2];	// local (0..1) starting value
	float flParamEnd[2];	// local (0..1) ending value
	int	iParamParent;

	float flFadeInTime;		// ideal cross fate in time (0.2 default)
	float flFadeOutTime;	// ideal cross fade out time (0.2 default)

	int	iEntryNode;			// transition node at entry
	int	iExitNode;			// transition node at exit
	int	iNodeFlags;			// transition rules

	float flEntryPhase;		// used to match entry gait
	float flExitPhase;		// used to match exit gait

	float flLastFrame;		// frame that should generation EndOfSequence

	int	iNextSequence;		// auto advancing sequences
	int	iPose;				// index of delta animation between end and nextseq

	int	nIKRules;

	int	nAutoLayers;
	int	nAutoLayerIndex;
	inline void* GetAutoLayer(int iAutoLayer) const
	{
		if (iAutoLayer < 0 || iAutoLayer >= nAutoLayers) return nullptr;
		return ((std::uint8_t*)this + nAutoLayerIndex) + iAutoLayer;
	}

	int	nWeightListIndex;
	inline float* GetBoneWeight(int iBone) const
	{
		return ((float*)((std::uint8_t*)this + nWeightListIndex) + iBone);
	}

	int	nPoseKeyIndex;
	inline float* GetPoseKey(int iParameter, int iAnimation) const
	{
		return (float*)((std::uint8_t*)this + nPoseKeyIndex) + iParameter * iGroupSize[0] + iAnimation;
	}

	int	nIKLocks;
	int	nIKLockIndex;
	inline void* GetIKLock(int iIK) const
	{
		if (iIK < 0 || iIK >= nIKLocks) return nullptr;
		return ((std::uint8_t*)this + nIKLockIndex) + iIK;
	}

	int	nKeyValueIndex;
	int	iKeyValueSize;
	inline const char* KeyValueText() const
	{
		if (!iKeyValueSize) return nullptr;
		return (char*)this + nKeyValueIndex;
	}

	int	nCyclePoseIndex;

	int nActivityModifierIndex;
	int	nActivityModifiers;

	int	nAnimTagIndex;
	int	nAnimTags;

	int	nRootDriverIndex;
	std::byte pad[0x8];
};

struct mstudioposeparamdesc_t
{
	int nNameIndex;
	int nFlags;
	float flStart; // starting value
	float flEnd; // ending value
	float flLoop; // looping range, 0 for no looping, 360 for rotations, etc.

	inline const char* GetName() const
	{
		return (char*)this + nNameIndex;
	}
};

struct studiohwdata_t;
struct studiohdr_t
{
	int	nID;
	int	iVersion;
	int iChecksum;
	char szName[64];
	int	iLength;

	Vector vecEyePosition;
	Vector vecIllumPosition;
	Vector vecHullMin;
	Vector vecHullMax;
	Vector vecViewBBMin;
	Vector vecViewBBMax;

	int	iFlags;

	int	nBones;
	int	nBoneIndex;
	inline mstudiobone_t* GetBone(int iBone) const
	{
		if (iBone < 0 || iBone >= nBones) return nullptr;
		return (mstudiobone_t*)((std::uint8_t*)this + nBoneIndex) + iBone;
	}

	int	nBoneControllers;
	int	nBoneControllerIndex;
	inline void* GetBoneController(int iBoneController) const
	{
		if (iBoneController < 0 || iBoneController >= nBoneControllers) return nullptr;
		return ((std::uint8_t*)this + nBoneControllerIndex) + iBoneController;
	}

	int	nHitboxSets;
	int	nHitboxSetIndex;
	inline mstudiohitboxset_t* GetHitboxSet(int iSet) const
	{
		if (iSet < 0 || iSet >= nHitboxSets) return nullptr;
		return (mstudiohitboxset_t*)((std::uint8_t*)this + nHitboxSetIndex) + iSet;
	}

	inline mstudiobbox_t* GetHitbox(int iHitBox, int iSet) const
	{
		const mstudiohitboxset_t* pHitboxSet = GetHitboxSet(iSet);

		if (pHitboxSet == nullptr)
			return nullptr;

		return pHitboxSet->GetHitbox(iHitBox);
	}

	inline int GetHitboxCount(int iSet) const
	{
		const mstudiohitboxset_t* pHitboxSet = GetHitboxSet(iSet);

		if (pHitboxSet == nullptr)
			return 0;

		return pHitboxSet->nHitboxes;
	}

	int	nLocalAnimations;
	int	nLocalAnimationIndex;
	inline void* GetAnimDescription(int iAnimation) const
	{
		if (iAnimation < 0 || iAnimation >= nLocalAnimations) iAnimation = 0;
		return ((std::uint8_t*)this + nLocalAnimationIndex) + iAnimation;
	}

	int nLocalSequences;
	int nLocalSequenceIndex;
	inline mstudioseqdesc_t* GetLocalSequenceDescription(int iSequence) const
	{
		if (iSequence < 0 || iSequence >= nLocalSequences) iSequence = 0;
		return (mstudioseqdesc_t*)((std::uint8_t*)this + nLocalSequenceIndex) + iSequence;
	}

	inline mstudioseqdesc_t& GetSequenceDescription(int iSequence, virtualmodel_t* pVirtualModel = nullptr, const studiohdr_t* pStudioHdr = nullptr) const
	{
		if (nIncludeModels == 0 || pVirtualModel == nullptr || pStudioHdr == nullptr)
			return *GetLocalSequenceDescription(iSequence);

		return *pStudioHdr->GetLocalSequenceDescription(pVirtualModel->vecSequence[iSequence].nIndex);
	}

	mutable int miActivityListVersion;
	mutable int miEventsIndexed;

	int	nTextures;
	int	nTextureIndex;
	inline void* GetTexture(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nTextures) return nullptr;
		return ((std::uint8_t*)this + nTextureIndex) + nIndex;
	}

	int	nCdTextures;
	int	nCdTextureIndex;
	inline char* GetCdTexture(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nCdTextures) return nullptr;
		return (((char*)this) + *((int*)((std::uint8_t*)this + nCdTextureIndex) + nIndex));
	}

	int	nSkinRefs;
	int	nSkinFamilies;
	int	nSkinIndex;
	inline short* GetSkinRef(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nSkinRefs) return nullptr;
		return (short*)((std::uint8_t*)this + nSkinIndex) + nIndex;
	}

	int	nBodyParts;
	int	nBodyPartIndex;
	inline void* GetBodyPart(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nBodyParts) return nullptr;
		return ((std::uint8_t*)this + nBodyPartIndex) + nIndex;
	}

	int	nAttachments;
	int	nAttachmentIndex;
	inline void* GetAttachment(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nAttachments) return nullptr;
		return ((std::uint8_t*)this + nAttachmentIndex) + nIndex;
	}

	int	nTransitions;
	int	nTransitionIndex;
	int	nTransitionNameIndex;
	inline std::uint8_t* GetTransition(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= (nTransitions * nTransitions)) return nullptr;
		return (std::uint8_t*)((std::uint8_t*)this + nTransitionIndex) + nIndex;
	}

	int	nFlexDescs;
	int	nFlexDescIndex;
	inline void* GetFlexDescription(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nFlexDescs) return nullptr;
		return ((std::uint8_t*)this + nFlexDescIndex) + nIndex;
	}

	int	nFlexControllers;
	int	nFlexControllerIndex;
	inline void* GetFlexController(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nFlexControllers) return nullptr;
		return ((std::uint8_t*)this + nFlexControllerIndex) + nIndex;
	}

	int	nFlexRules;
	int	nFlexRuleIndex;
	inline void* GetFlexRule(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nFlexRules) return nullptr;
		return ((std::uint8_t*)this + nFlexRuleIndex) + nIndex;
	}

	int	nIkChains;
	int	nIkChainIndex;
	inline void* GetIKChain(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nIkChains) return nullptr;
		return ((std::uint8_t*)this + nIkChainIndex) + nIndex;
	}

	int	nMouths;
	int	nMouthIndex;
	inline void* GetMouth(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nMouths) return nullptr;
		return ((std::uint8_t*)this + nMouthIndex) + nIndex;
	}

	int	nPoseParameters;
	int	nPoseParameterIndex;
	inline void* GetPoseParameter(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nPoseParameters) return nullptr;
		return ((std::uint8_t*)this + nPoseParameterIndex) + nIndex;
	}

	int	nSurfacePropIndex;
	inline const char* GetSurfaceProp() const
	{
		if (!nSurfacePropIndex) return nullptr;
		return (char*)this + nSurfacePropIndex;
	}

	int	nKeyValueIndex;
	int	nKeyValueSize;
	inline const char* KeyValueText() const
	{
		if (!nKeyValueSize) return nullptr;
		return (char*)this + nKeyValueIndex;
	}

	int	nIkAutoplayLocks;
	int	nIkAutoplayLockIndex;
	inline void* GetIKAutoplayLock(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nIkAutoplayLocks) return nullptr;
		return ((std::uint8_t*)this + nIkAutoplayLockIndex) + nIndex;
	}

	float flMass;
	int	iContents;

	int	nIncludeModels;
	int	nIncludeModelIndex;
	inline void* GetModelGroup(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nIncludeModels) return nullptr;
		return ((std::uint8_t*)this + nIncludeModelIndex) + nIndex;
	}

	int	iVirtualModel;

	int	nAnimBlockNameIndex;
	inline const char* GetAnimBlockName() const
	{
		return ((char*)this) + nAnimBlockNameIndex;
	}

	int	nAnimBlocks;
	int	nAnimBlockIndex;
	inline void* GetAnimBlock(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= nAnimBlocks) return nullptr;
		return (((std::uint8_t*)this) + nAnimBlockIndex) + nIndex;
	}

	int	iAnimBlockModel;

	int	nBoneTableByNameIndex;
	inline const std::uint8_t* GetBoneTableSortedByName() const
	{
		return (std::uint8_t*)this + nBoneTableByNameIndex;
	}

	int iVertexBase;
	int nIndexBase;

	std::uint8_t uDirectionalLightDot;
	std::uint8_t uRootLOD;
	std::uint8_t nAllowedRootLODs;
	std::byte pad0[0x5];

	int	nFlexControllerUI;
	int	nFlexcontrollerUIIndex;

	float flVertAnimFixedPointScale;
	mutable int miSurfacePropLookup;
	int	nStudioHdr2Index;

	std::byte pad1[0x4];
};

class CStudioHdr
{
public:
	studiohdr_t* pStudioHdr;
	virtualmodel_t* pVirtualModel;
	void* pSoftbody;

	mutable CUtlVector<const studiohdr_t*> pStudioHdrCache;
	mutable int		mnFrameUnlockCounter;
	int* pFrameUnlockCounter;
	std::byte		pad0[ 0x8 ];
	CUtlVector<int>	vecBoneFlags;
	CUtlVector<int>	vecBoneParent;
	void* pActivityToSequence;

	__forceinline const studiohdr_t* GroupStudioHdr( int i )
	{
		auto pStudioHdr = this->pStudioHdrCache[ i ];
		assert( pStudioHdr );
		return pStudioHdr;
	}

	__forceinline int GetNumSeq( void )
	{
		if (!this->pVirtualModel)
			return this->pStudioHdr->nLocalSequences;
		return GetNumSeq_Internal( );
	}

	__forceinline int GetNumSeq_Internal( void )
	{
		return this->pVirtualModel->vecSequence.Count( );
	}

	inline mstudioseqdesc_t& pSeqdesc( int iSequence )
	{
		if (!this->pVirtualModel)
			return *this->pStudioHdr->GetLocalSequenceDescription( iSequence );

		return pSeqdesc_Internal( iSequence );
	}

	__forceinline mstudioseqdesc_t& pSeqdesc_Internal( int i )
	{
		assert( i >= 0 && i < GetNumSeq( ) );
		if (i < 0 || i >= GetNumSeq( ))
		{
			// Avoid reading random memory.
			i = 0;
		}

		const studiohdr_t* pStudioHdr = GroupStudioHdr( this->pVirtualModel->vecSequence[ i ].iGroup );
		return *pStudioHdr->GetLocalSequenceDescription( this->pVirtualModel->vecSequence[ i ].nIndex );
	}

	__forceinline mstudioposeparamdesc_t& GetPoseParameterDescription( int iParameter )
	{
		static auto fnpPoseParameter = reinterpret_cast< mstudioposeparamdesc_t & ( __thiscall* )( CStudioHdr*, int ) >( MEM::FindPattern( CLIENT_DLL, XorStr( "55 8B EC 8B 45 08 57 8B F9 8B 4F 04 85 C9 75 15" ) ) );
		return fnpPoseParameter( this, iParameter );
	}
};