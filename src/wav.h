
#ifndef WAV_H
#define WAV_H

#include <cinttypes>

struct WavHeader
{
	uint32_t ChunkID;		// "RIFF"
	uint32_t ChunkSize;		// 36 + Subchunk2Size
	uint32_t Format;		// "WAVE"

	uint32_t Subchunk1Id;	// "fmt "
	uint32_t Subchunk1Size;	// 16
	uint16_t AudioFormat;	// 1
	uint16_t NumChannels;	// 1
	uint32_t SampleRate;	// 8000, 44100, ...
	uint32_t ByteRate;		// SampleRate * NumChannels * BitsPerSample/8
	uint16_t BlockAlign;	// NumChannels * BitsPerSample/8
	uint16_t BitsPerSample;	// 16

	uint32_t Subchunk2Id;	// "data"
	uint32_t Subchunk2Size;	// ...
};

const uint16_t WAV_HEADER_FORMAT_INT = 1;
const uint16_t WAV_HEADER_FORMAT_FLOAT = 3;

void MakeWavHeader( WavHeader * header, uint32_t sampleRate, uint32_t samples, uint16_t bitsPerSample, uint16_t format = WAV_HEADER_FORMAT_INT );

enum NOTE
{
	C=0,
	D=1,
	E=2,
	F=3,
	G=4,
	A=5,
	H=6,
	NONE
};

inline NOTE Note( char n )
{
	switch( n )
	{
	case 'c': return C;
	case 'd': return D;
	case 'e': return E;
	case 'f': return F;
	case 'g': return G;
	case 'a': return A;
	case 'h': return H;
	}
	return NONE;
}

enum INTERPMETHOD
{
	QUADRATIC,
	TRIGONOMETRIC,
	LINEAR
};

float Interpolation( float a, float b, float t, INTERPMETHOD method );

class Freq
{
public:
	float frequency;
	Freq( NOTE note, float octaves, float halftones );
};

class Wav
{
public:
	
	int32_t samples, allocated;
	float * data;
	int sampleRate, bitsPerSample;

	float & operator[]( uint32_t id );
	
	void BestAdjust();
	int Save( const char * filename );
	
	void AddNoise( float amplitude );
	
	Wav( int _sampleRate = 44100, int _bitsPerSample = 16, int _allocated = 1024 );
	~Wav();
};

class Instrument
{
public:
	virtual void Add( Wav & wav, Freq freq, float volume, float start, float time ) = 0;
};

class Violin : public Instrument
{
public:
	virtual void Add( Wav & wav, Freq freq, float volume, float start, float time ) override;
};

extern "C" Violin violin;

class PureSine : public Instrument
{
public:
	virtual void Add( Wav & wav, Freq freq, float volume, float start, float time ) override;
};

extern "C" PureSine pureSine;

#endif

