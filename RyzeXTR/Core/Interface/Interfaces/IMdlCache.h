#pragma once

#pragma region mdlcache_enumerations
enum
{
	MDLHANDLE_INVALID = static_cast<MDLHandle_t>(~0)
};

enum MDLCacheDataType_t : int
{
	MDLCACHE_STUDIOHDR = 0,
	MDLCACHE_STUDIOHWDATA,
	MDLCACHE_VCOLLIDE,
	MDLCACHE_ANIMBLOCK,
	MDLCACHE_VIRTUALMODEL,
	MDLCACHE_VERTEXES,
	MDLCACHE_DECODEDANIMBLOCK,
};

enum MDLCacheFlush_t : unsigned int
{
	MDLCACHE_FLUSH_STUDIOHDR = 0x01,
	MDLCACHE_FLUSH_STUDIOHWDATA = 0x02,
	MDLCACHE_FLUSH_VCOLLIDE = 0x04,
	MDLCACHE_FLUSH_ANIMBLOCK = 0x08,
	MDLCACHE_FLUSH_VIRTUALMODEL = 0x10,
	MDLCACHE_FLUSH_AUTOPLAY = 0x20,
	MDLCACHE_FLUSH_VERTEXES = 0x40,
	MDLCACHE_FLUSH_COMBINED_DATA = 0x80,
	MDLCACHE_FLUSH_IGNORELOCK = 0x80000000,
	MDLCACHE_FLUSH_ALL = 0xFFFFFFFF
};
#pragma endregion

class IMDLCacheNotify
{
public:
	virtual void OnDataLoaded(MDLCacheDataType_t nType, MDLHandle_t hModel) = 0;
	virtual void OnCombinerPreCache(MDLHandle_t hOldHandle, MDLHandle_t hNewHandle) = 0;
	virtual void OnDataUnloaded(MDLCacheDataType_t nType, MDLHandle_t hModel) = 0;
	virtual bool ShouldSupressLoadWarning(MDLHandle_t hModel) = 0;
};

class IMDLCache
{
public:
	void SetCacheNotify(IMDLCacheNotify* pNotify)
	{
		util::CallVFunc<void>(this, 9, pNotify);
	}

	[[nodiscard]] MDLHandle_t FindMDL(const char* szMDLRelativePath)
	{
		return util::CallVFunc<MDLHandle_t>(this, 1, szMDLRelativePath);
	}

	[[nodiscard]] int AddReference(MDLHandle_t hModel)
	{
		return util::CallVFunc<int>(this, 11U, hModel);
	}

	[[nodiscard]] int Release(MDLHandle_t hModel)
	{
		return util::CallVFunc<int>(this, 12U, hModel);
	}

	[[nodiscard]] int GetReference(MDLHandle_t hModel)
	{
		return util::CallVFunc<int>(this, 13U, hModel);
	}

	[[nodiscard]] studiohdr_t* GetStudioHdr(MDLHandle_t hModel)
	{
		return util::CallVFunc<studiohdr_t*>(this, 14U, hModel);
	}

	[[nodiscard]] studiohwdata_t* GetHardwareData(MDLHandle_t hModel)
	{
		return util::CallVFunc<studiohwdata_t*>(this, 15U, hModel);
	}

	[[nodiscard]] vcollide_t* GetVCollide(MDLHandle_t hModel)
	{
		return util::CallVFunc<vcollide_t*>(this, 16U, hModel);
	}

	[[nodiscard]] vcollide_t* GetVCollide(MDLHandle_t hModel, float flScale)
	{
		return util::CallVFunc<vcollide_t*>(this, 17U, hModel, flScale);
	}

	[[nodiscard]] unsigned char* GetAnimBlock(MDLHandle_t hModel, int nBlock, bool bPreloadIfMissing)
	{
		return util::CallVFunc<unsigned char*>(this, 18U, hModel, nBlock, bPreloadIfMissing);
	}

	[[nodiscard]] bool HasAnimBlockBeenPreloaded(MDLHandle_t hModel, int nBlock)
	{
		return util::CallVFunc<bool>(this, 19U, hModel, nBlock);
	}

	[[nodiscard]] virtualmodel_t* GetVirtualModel(MDLHandle_t hModel)
	{
		return util::CallVFunc<virtualmodel_t*>(this, 20U, hModel);
	}

	[[nodiscard]] int GetAutoplayList(MDLHandle_t hModel, unsigned short** pOut)
	{
		return util::CallVFunc<int>(this, 21U, hModel, pOut);
	}

	[[nodiscard]] vertexfileheader_t* GetVertexData(MDLHandle_t hModel)
	{
		return util::CallVFunc<vertexfileheader_t*>(this, 22U, hModel);
	}

	[[nodiscard]] bool IsErrorModel(MDLHandle_t hModel)
	{
		return util::CallVFunc<bool>(this, 26U, hModel);
	}

	void Flush(int nFlushFlags = MDLCACHE_FLUSH_ALL)
	{
		util::CallVFunc<void>(this, 28U, nFlushFlags);
	}

	void Flush(MDLHandle_t hModel, int nFlushFlags = MDLCACHE_FLUSH_ALL)
	{
		util::CallVFunc<void>(this, 29U, hModel, nFlushFlags);
	}

	[[nodiscard]] const char* GetModelName(MDLHandle_t hModel)
	{
		return util::CallVFunc<const char*>(this, 30U, hModel);
	}

	[[nodiscard]] void* GetCacheSection(int nType)
	{
		return util::CallVFunc<void*>(this, 31U, nType);
	}

	[[nodiscard]] virtualmodel_t* GetVirtualModelFast(const studiohdr_t* pStudioHdr, MDLHandle_t hModel)
	{
		return util::CallVFunc<virtualmodel_t*>(this, 32U, pStudioHdr, hModel);
	}

	void BeginLock()
	{
		util::CallVFunc<void>(this, 33U);
	}

	void EndLock()
	{
		util::CallVFunc<void>(this, 34U);
	}

	[[nodiscard]] vcollide_t* GetVCollideEx(MDLHandle_t hModel, bool bSynchronousLoad = true)
	{
		return util::CallVFunc<vcollide_t*>(this, 37U, hModel, bSynchronousLoad);
	}

	[[nodiscard]] bool GetVCollideSize(MDLHandle_t hModel, int* pVCollideSize)
	{
		return util::CallVFunc<bool>(this, 38U, hModel, pVCollideSize);
	}

	[[nodiscard]] studiohdr_t* LockStudioHdr(MDLHandle_t hModel)
	{
		return util::CallVFunc<studiohdr_t*>(this, 48U, hModel);
	}

	void UnlockStudioHdr(MDLHandle_t hModel)
	{
		util::CallVFunc<void>(this, 49U, hModel);
	}

	[[nodiscard]] bool PreloadModel(MDLHandle_t hModel)
	{
		return util::CallVFunc<bool>(this, 50U, hModel);
	}

	void BeginCoarseLock()
	{
		util::CallVFunc<void>(this, 53U);
	}

	void EndCoarseLock()
	{
		util::CallVFunc<void>(this, 54U);
	}

	void ReloadVCollide(MDLHandle_t hModel)
	{
		util::CallVFunc<void>(this, 55U, hModel);
	}

	void DisableVCollideLoad()
	{
		util::CallVFunc<void>(this, 58U);
	}

	void EnableVCollideLoad()
	{
		util::CallVFunc<void>(this, 59U);
	}
};

class CMDLCacheCriticalSection
{
public:
	CMDLCacheCriticalSection(IMDLCache* pCache)
		: pCache(pCache)
	{
		this->pCache->BeginLock();
	}

	~CMDLCacheCriticalSection()
	{
		this->pCache->EndLock();
	}
private:
	IMDLCache* pCache;
};