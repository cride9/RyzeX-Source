#pragma once
#include <cstdint>

class C_VoiceCommunicationData
{
public:
	uint32_t m_nXuidLow{ };
	uint32_t m_nXuidHigh{ };
	int32_t m_nSequenceBytes{ };
	uint32_t m_nSectionNumber{ };
	uint32_t m_nUnCompressedSampleOffset{};

	__forceinline uint8_t* GetRawData()
	{
		return  (uint8_t*)this;
	}

};
class C_CLCMsg_VoiceData
{
public:
	uint32_t INetMessageVTable; //0x0000
	char PAD[4]; //0x0004
	uint32_t CCLCMsg_VoiceData_Vtable; //0x0008
	char PAD0C[8]; //0x000C
	void* pData; //0x0014
	uint32_t m_nXuidLow{ };
	uint32_t m_nXuidHigh{ };
	int32_t m_nFormat; //0x0020
	int32_t m_nSequenceBytes; //0x0024
	uint32_t m_nSectionNumber; //0x0028
	uint32_t m_nUnCompressedSampleOffset; //0x002C
	int32_t m_nCachedSize; //0x0030

	uint32_t iFlags; //0x0034

	uint8_t NoStackOverFlow[0xFF]; // fix clown crashers

	__forceinline void SetData(C_VoiceCommunicationData* pData)
	{
		m_nXuidLow = pData->m_nXuidLow;
		m_nXuidHigh = pData->m_nXuidHigh;
		m_nSequenceBytes = pData->m_nSequenceBytes;
		m_nSectionNumber = pData->m_nSectionNumber;
		m_nUnCompressedSampleOffset = pData->m_nUnCompressedSampleOffset;
	}
};
class C_SVCMsg_GameEvent
{
public:
	char PAD[0x8];
};
class C_SVCMsg_VoiceData
{
public:
	char PAD[0x8];
	int m_iClient;
	int m_nAudibleMask;
	uint32_t m_nXuidLow{ };
	uint32_t m_nXuidHigh{ };
	std::string* m_szVoiceData;
	bool m_bProximity;
	bool m_bCaster;
	int m_nFormat;
	int m_nSequenceBytes;
	uint32_t m_nSectionNumber;
	uint32_t m_nUnCompressedSampleOffset;

	__forceinline C_VoiceCommunicationData GetData()
	{
		C_VoiceCommunicationData cData;
		cData.m_nXuidLow = m_nXuidLow;
		cData.m_nXuidHigh = m_nXuidHigh;
		cData.m_nSequenceBytes = m_nSequenceBytes;
		cData.m_nSectionNumber = m_nSectionNumber;
		cData.m_nUnCompressedSampleOffset = m_nUnCompressedSampleOffset;
		return cData;
	}
};
