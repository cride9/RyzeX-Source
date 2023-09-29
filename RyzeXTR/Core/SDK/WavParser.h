#pragma once
#include "memory.h"
#include <algorithm>

class CWavParser
{
public:
	struct WavHeader_t
	{
		unsigned char m_uchRiff[ 4 ];                    // RIFF string
		unsigned int m_uiOverallSize;					// overall size of file in bytes
		unsigned char m_uchWave[ 4 ];                    // WAVE string
		unsigned char m_uchFmtChunkMarker[ 4 ];        // fmt string with trailing null char
		unsigned int m_uiLengthOfFmt;					// length of the format data
		unsigned int m_uiFormatType;                   // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
		unsigned int m_uiChannels;                      // num of channels
		unsigned int m_uiSampleRate;                   // sampling rate (blocks per second)
		unsigned int m_uiByterate;                      // SampleRate * NumChannels * BitsPerSample/8
		unsigned int m_uiBlockAlign;                   // NumChannels * BitsPerSample/8
		unsigned int m_uiBitsPerSample;               // bits per sample, 8- 8bits, 16- 16 bits etc
		unsigned char m_uchDataChunkHeader[ 4 ];       // DATA string or FLLR string
		unsigned int m_uiDataSize;                     // NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
		unsigned char* m_uchpData;						// The actual sound data

		void ParseWavHeader( unsigned char* m_uchpWavData );
	};

	bool AdjustWavVolume( WavHeader_t& m_header, float m_flVolume );
};
inline CWavParser wavparser;