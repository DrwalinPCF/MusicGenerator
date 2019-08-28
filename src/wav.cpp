
// This file is part of The Drwalin Sounds project
// Copyright (C) 2019 Marek Zalewski aka Drwalin aka DrwalinPCF

#ifndef WAV_CPP
#define WAV_CPP

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>

#define D_PI (2.0f*3.141592f)

#include "wav.h"

Violin violin;
PureSine pureSine;

float Rand( float a = 1.0f )
{
	return (float(rand())*2.0f*a/float(RAND_MAX)) - a;
}

uint32_t ToInt( const char * s )
{
	return *(uint32_t*)s;
}

void MakeWavHeader( WavHeader * h, uint32_t sampleRate, uint32_t samples, uint16_t bitsPerSample, uint16_t format )
{
	h->ChunkID = ToInt("RIFF");
	h->ChunkSize = 36 + (bitsPerSample*samples/8);
	h->Format = ToInt("WAVE");

	h->Subchunk1Id = ToInt("fmt ");
	h->Subchunk1Size = 16;
	h->AudioFormat = format;
	h->NumChannels = 1;
	h->SampleRate = sampleRate;
	h->BitsPerSample = bitsPerSample;
	h->ByteRate = h->SampleRate * h->NumChannels * h->BitsPerSample / 8;
	h->BlockAlign = h->NumChannels * h->BitsPerSample / 8;

	h->Subchunk2Id = ToInt("data");
	h->Subchunk2Size = bitsPerSample * samples / 8;
}

float & Wav::operator[]( uint32_t id )
{
	if( id >= allocated )
	{
		while( id >= allocated )
		{
			allocated *= 2;
		}
		data = (float*)realloc( data,
				sizeof(float) * allocated );
		for( int i=samples; i<allocated; ++i )
			data[i] = 0.0f;
	}
	if( id >= samples )
		samples = id+1;
	return data[id];
}

Freq::Freq( NOTE note, float octave, float halftones )
{
	float nht = 0;
	switch( note )
	{
	case C: nht=0; break;
	case D: nht=2; break;
	case E: nht=4; break;
	case F: nht=5; break;
	case G: nht=7; break;
	case A: nht=9; break;
	case H: nht=10; break;
	default: frequency = 0; return;
	}
	frequency = 16.35f * pow( 2.0, ((octave*12.0f)+halftones+nht)/12.0f );
}

float Interpolation( float a, float b, float t, INTERPMETHOD method )
{
	switch( method )
	{
	case QUADRATIC:
		return (a-b) * (t-1)*(t-1) + b;
	case TRIGONOMETRIC:
		return (a-b) * (cos(t*3.141592f)/2.0f+0.5f) + b;
	case LINEAR:
		break;
	}
	return (a-b) * t + b;
}

void Violin::Add( Wav & wav, Freq freq, float volume, float start, float time )
{
	uint32_t offset = start * float(wav.sampleRate);
	int count = time * float(wav.sampleRate);
	int i;
	for( i=0; i<count; ++i )
	{
		float t = float(i) * time / float(count-1);
		float p = float(i) / float(count-1);
		float sum = 0.0f, at = 0.0f;
		for( int j=1; j<=7; ++j )
		{
			at += 1/float(j);
			sum +=
				sin
				(
					(
						t *
						freq.frequency *
						D_PI
					) * float(j)
				) / float(j);
		}
		sum = sum / at / 1.2f;
		sum *= 1 + sin(
						D_PI * 6.0f * t
					) * 0.2f;
		sum *= pow( sin( 3.141592f * pow( p, 3.0f/3.0f ) ), 1.0f/2.0f );
		wav[ offset+i ] += sum;
	}
}

void PureSine::Add( Wav & wav, Freq freq, float volume, float start, float time )
{
	uint32_t offset = start * float(wav.sampleRate);
	int count = time * float(wav.sampleRate);
	int i;
	float f = freq.frequency;
	for( i=0; i<count; ++i )
	{
		float t = float(i) * time / float(count-1);
		wav[offset+i] += sin( ( start + t ) * f * 2.0f  );
	}
}

void Wav::AddNoise( float amplitude )
{
	srand( time( NULL ) );
	for( int i=0; i<samples; ++i )
		data[i] += data[i] * Rand(amplitude);
}

void Wav::BestAdjust()
{
	float min = 1000.0f, max = -1000.0f;
	int i;
	for( i=0; i<samples; ++i )
	{
		if( min > data[i] )
			min = data[i];
		if( max < data[i] )
			max = data[i];
	}
	printf("\n data in (%f;%f)", min, max );
	float sub = (min+max)/2;
	float mult = 1.0f/max-sub;
	if( mult < 0.0f )
		mult = -mult;
	printf("\n sub=%f mult=%f", sub, mult );
	for( i=0; i<samples; ++i )
	{
		data[i] -= sub;
		data[i] *= mult;
	}
}

uint8_t ConvertToUint8( float v )
{
	if( v <= -1.0f )
		return 0;
	else if( v >= 1.0f )
		return 255;
	return uint8_t((v+1.0f)*127.5f);
}

int16_t ConvertToInt16( float v )
{
	if( v <= -1.0f )
		return -32768;
	else if( v >= 1.0f )
		return 32767;
	return v * 32766;
}

int Wav::Save( const char * filename )
{
	WavHeader header;
	MakeWavHeader( &header, sampleRate, samples, bitsPerSample, bitsPerSample == 32 ? WAV_HEADER_FORMAT_FLOAT : WAV_HEADER_FORMAT_INT );
	FILE * file = fopen(filename,"wb");
	if( file == NULL )
		return -1;
	fwrite( (char*)&header, 1, sizeof(WavHeader), file );
	
	if( bitsPerSample == 8 )
	{
		uint8_t * d = (uint8_t*)malloc( sizeof(uint8_t) * samples );
		for( int i=0; i<samples; ++i )
		{
			d[i] = ConvertToUint8( data[i] );
		}
		fwrite( (char*)d, 1, samples*sizeof(uint8_t), file );
		free(d);
	}
	else if( bitsPerSample == 16 )
	{
		int16_t * d = (int16_t*)malloc( sizeof(int16_t) * samples );
		for( int i=0; i<samples; ++i )
		{
			d[i] = ConvertToInt16( data[i] );
		}
		fwrite( (char*)d, 1, samples*sizeof(int16_t), file );
		free(d);
	}
	else if( bitsPerSample == 32 )
	{
		fwrite( (char*)data, 1, samples*sizeof(float), file );
	}
	else
	{
		printf("\n cannot use %i bits per sample as format", bitsPerSample );
	}
	
	fclose(file);
	return 0;
}

Wav::Wav( int _sampleRate, int _bitsPerSample, int _allocated )
{
	sampleRate = _sampleRate;
	bitsPerSample = _bitsPerSample;
	allocated = _allocated;
	samples = 0;
	data = (float*)malloc( allocated * sizeof(float) );
	for( int i=0; i<allocated; ++i )
		data[i] = 0.0f;
}

Wav::~Wav()
{
	free(data);
}

#endif
