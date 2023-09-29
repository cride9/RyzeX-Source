#include "wavparser.h"

void CWavParser::WavHeader_t::ParseWavHeader( unsigned char* wavData )
{
	unsigned char buffer4[ 4 ];
	unsigned char buffer2[ 2 ];

	if (!wavData)
		return;

	unsigned char* walker = wavData;

	// Retreive riff
	memcpy( m_uchRiff, walker, sizeof( m_uchRiff ) );
	walker += sizeof( m_uchRiff );


	// overall_size
	memcpy( buffer4, walker, sizeof( buffer4 ) );
	walker += sizeof( buffer4 );

	// Convert little endian to big endian 4 byte int
	m_uiOverallSize = buffer4[ 0 ] | ( buffer4[ 1 ] << 8 ) | ( buffer4[ 2 ] << 16 ) | ( buffer4[ 3 ] << 24 );

	// wave
	memcpy( m_uchWave, walker, sizeof( m_uchWave ) );
	walker += sizeof( m_uchWave );


	// fmt_chunk_marker
	memcpy( m_uchFmtChunkMarker, walker, sizeof( m_uchFmtChunkMarker ) );
	walker += sizeof( m_uchFmtChunkMarker );


	// length_of_fmt
	memcpy( buffer4, walker, sizeof( buffer4 ) );
	walker += sizeof( buffer4 );

	m_uiLengthOfFmt = buffer4[ 0 ] | ( buffer4[ 1 ] << 8 ) | ( buffer4[ 2 ] << 16 ) | ( buffer4[ 3 ] << 24 );


	// format_type
	memcpy( buffer2, walker, sizeof( buffer2 ) );
	walker += sizeof( buffer2 );

	m_uiFormatType = buffer2[ 0 ] | ( buffer2[ 1 ] << 8 );


	// channels
	memcpy( buffer2, walker, sizeof( buffer2 ) );
	walker += sizeof( buffer2 );

	m_uiChannels = buffer2[ 0 ] | ( buffer2[ 1 ] << 8 );


	// sample_rate
	memcpy( buffer4, walker, sizeof( buffer4 ) );
	walker += sizeof( buffer4 );

	m_uiSampleRate = buffer4[ 0 ] | ( buffer4[ 1 ] << 8 ) | ( buffer4[ 2 ] << 16 ) | ( buffer4[ 3 ] << 24 );


	// byterate
	memcpy( buffer4, walker, sizeof( buffer4 ) );
	walker += sizeof( buffer4 );

	m_uiByterate = buffer4[ 0 ] | ( buffer4[ 1 ] << 8 ) | ( buffer4[ 2 ] << 16 ) | ( buffer4[ 3 ] << 24 );


	// block_align
	memcpy( buffer2, walker, sizeof( buffer2 ) );
	walker += sizeof( buffer2 );

	m_uiBlockAlign = buffer2[ 0 ] | ( buffer2[ 1 ] << 8 );


	// bits_per_sample
	memcpy( buffer2, walker, sizeof( buffer2 ) );
	walker += sizeof( buffer2 );

	m_uiBitsPerSample = buffer2[ 0 ] | ( buffer2[ 1 ] << 8 );


	// data_chunk_header
	memcpy( m_uchDataChunkHeader, walker, sizeof( m_uchDataChunkHeader ) );
	walker += sizeof( m_uchDataChunkHeader );


	// data_size
	memcpy( buffer4, walker, sizeof( buffer4 ) );
	walker += sizeof( buffer4 );

	m_uiDataSize = buffer4[ 0 ] | ( buffer4[ 1 ] << 8 ) | ( buffer4[ 2 ] << 16 ) | ( buffer4[ 3 ] << 24 );

	m_uchpData = walker;
}

bool CWavParser::AdjustWavVolume( WavHeader_t& header, float volume )
{
	// We can only adjust PMC (Uncompressed) WAV files
	if ( header.m_uiFormatType != 1 )
		return false;

	// Adjust volume to correct ratio
	float finalVolume = std::clamp( volume, 0.f, 1.f );

	// Loop through all samples and adjust their volumes
	int numOfSamples = header.m_uiDataSize / header.m_uiChannels;
	unsigned char* walker = header.m_uchpData;

	for ( int i = 0; i < numOfSamples; i++ )
	{
		switch ( header.m_uiChannels )
		{
			case sizeof( unsigned char ) :
				// Mono
				*walker = ( unsigned char )( *walker * finalVolume );
				walker += sizeof( unsigned char );
				break;
				case sizeof( short ) :
					// Stereo
					*( short* )walker = ( short )( *( short* )walker * finalVolume );
					walker += sizeof( short );
					break;
					case sizeof( int ) :
						// Surround?
						*( int* )walker = ( int )( *( int* )walker * finalVolume );
						walker += sizeof( int );
						break;
					default:
						return false;
		}
	}

	return true;
}