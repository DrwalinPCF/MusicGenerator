
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
NES_1 nes_1;
NES_2 nes_2;
NES_3 nes_3;
NES_4 nes_4;
NES_5 nes_5;
NES_6 nes_6;
NES_7 nes_7;
NES_8 nes_8;

float Rand( float a = 1.0f ) {
	return (float(rand())*2.0f*a/float(RAND_MAX)) - a;
}

uint32_t ToInt( const char * s ) {
	return *(uint32_t*)s;
}

void MakeWavHeader( WavHeader * h, uint32_t sampleRate, uint32_t samples, uint16_t bitsPerSample, uint16_t format ) {
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

float & Wav::operator[]( uint32_t id ) {
	if( id >= allocated ) {
		if(allocated < 256)
			allocated = 256;
		while( id >= allocated ) {
			allocated *= 2;
			allocated += 16;
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

void Wav::operator*=(float v) {
	for(int i=0; i<samples; ++i)
		data[i] *= v;
}

void Wav::operator+=(float v) {
	for(int i=0; i<samples; ++i)
		data[i] += v;
}

Freq::Freq( NOTE note, float octave, float halftones ) {
	float nht = 0;
	switch( note ) {
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

float Interpolation( float a, float b, float t, INTERPMETHOD method ) {
	switch( method ) {
	case QUADRATIC:
		return (a-b) * (t-1)*(t-1) + b;
	case TRIGONOMETRIC:
		return (a-b) * (cos(t*3.141592f)/2.0f+0.5f) + b;
	case LINEAR:
		break;
	}
	return (a-b) * t + b;
}
void Wav::AddNoise( float amplitude ) {
	srand( time( NULL ) );
	for( int i=0; i<samples; ++i )
		data[i] += data[i] * Rand(amplitude);
}

void Wav::BestAdjust() {
	float min = data[0], max = data[0];
	int i;
	for( i=0; i<samples; ++i ) {
		if( min > data[i] )
			min = data[i];
		if( max < data[i] )
			max = data[i];
	}
	printf("\n data in (%f;%f)", min, max );
	float sub = (min+max)/2;
	float mult = 1.0f/(max-sub);
	if( mult < 0.0f )
		mult = -mult;
	printf("\n sub=%f mult=%f", sub, mult );
	(*this) -= sub;
	(*this) *= mult;
}

uint8_t ConvertToUint8( float v ) {
	if( v <= -1.0f )
		return 0;
	else if( v >= 1.0f )
		return 255;
	return uint8_t((v+1.0f)*127.5f);
}

int16_t ConvertToInt16( float v ) {
	if( v <= -1.0f )
		return -32768;
	else if( v >= 1.0f )
		return 32767;
	return v * 32766;
}

int Wav::Save( const char * filename ) {
	WavHeader header;
	MakeWavHeader( &header, sampleRate, samples, bitsPerSample, bitsPerSample == 32 ? WAV_HEADER_FORMAT_FLOAT : WAV_HEADER_FORMAT_INT );
	FILE * file = fopen(filename,"wb");
	if( file == NULL )
		return -1;
	fwrite( (char*)&header, 1, sizeof(WavHeader), file );
	
	if( bitsPerSample == 8 ) {
		uint8_t * d = (uint8_t*)malloc( sizeof(uint8_t) * samples );
		for( int i=0; i<samples; ++i ) {
			d[i] = ConvertToUint8( data[i] );
		}
		fwrite( (char*)d, 1, samples*sizeof(uint8_t), file );
		free(d);
	}
	else if( bitsPerSample == 16 ) {
		int16_t * d = (int16_t*)malloc( sizeof(int16_t) * samples );
		for( int i=0; i<samples; ++i ) {
			d[i] = ConvertToInt16( data[i] );
		}
		fwrite( (char*)d, 1, samples*sizeof(int16_t), file );
		free(d);
	}
	else if( bitsPerSample == 32 ) {
		fwrite( (char*)data, 1, samples*sizeof(float), file );
	}
	else {
		printf("\n cannot use %i bits per sample as format", bitsPerSample );
	}
	
	fclose(file);
	return 0;
}

Wav::Wav( int _sampleRate, int _bitsPerSample, int _allocated ) {
	sampleRate = _sampleRate;
	bitsPerSample = _bitsPerSample;
	allocated = _allocated;
	samples = 0;
	data = (float*)malloc( allocated * sizeof(float) );
	for( int i=0; i<allocated; ++i )
		data[i] = 0.0f;
}

Wav::~Wav() {
	free(data);
}




void Violin::Add( Wav & wav, Freq freq, float volume, float start, float time ) {
	uint32_t offset = start * float(wav.sampleRate);
	int count = time * float(wav.sampleRate);
	int i;
	for( i=0; i<count; ++i ) {
		float t = float(i) * time / float(count-1);
		float p = float(i) / float(count-1);
		float sum = 0.0f, at = 0.0f;
		for( int j=1; j<=7; ++j ) {
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

void PureSine::Add( Wav & wav, Freq freq, float volume, float start, float time ) {
	uint32_t offset = start * float(wav.sampleRate);
	int count = time * float(wav.sampleRate);
	int i;
	float f = freq.frequency;
	for( i=0; i<count; ++i ) {
		float t = float(i) * time / float(count-1);
		wav[offset+i] += sin( ( start + t ) * f * 2.0f / M_PI );
	}
}

void NES::Add( Wav & wav, Freq freq, float volume, float start, float time ) {
	uint32_t offset = start * float(wav.sampleRate);
	int count = time * float(wav.sampleRate);
	int i;
	float f = freq.frequency;
	for( i=0; i<count; ++i ) {
		float t = float(i) * time / float(count-1);
		float x = (/*start + */t) * f;
		wav[offset+i] += Func(x) * volume;
	}
}

float NES_1::Func(float x) {
	x = Norm(x);
	if(x < 0.5f) {
		return 1.0f-x;
	} else {
		return -1.5f+x;
	}
}

float NES_2::Func(float x) {
	x = Norm(x);
	if(x < 0.5f) {
		return x+0.5f;
	} else {
		return -x;
	}
}

float NES_3::Func(float x) {
	x = Norm(x);
	x *= 2.0f;
	float mult = x > 1.0f ? -1.0f : 1.0f;	
	if(x > 1.0f)
		x -= 1.0f;
	if(x > 0.5f)
		x = 1.0f - x;
	
	float sum = 0;
	if(x > 0.4)
		sum += 0.25;	
	if(x > 0.3)
		sum += 0.25;	
	if(x > 0.2)
		sum += 0.25;	
	if(x > 0.1)
		sum += 0.25;	
	return sum * mult;
}

float NES_4::Func(float x) {
	return NES_1::Func(x)*2.0f/3.0f +  NES_1::Func(x*2.0f)/3.0f;
}

float NES_5::Func(float x) {
	x = Norm(x)*4;
	if(x < 1)
		return x;
	else if (x < 3)
		return 2-x;
	else
		return x-4;
}

float NES_6::Func(float x) {
	return NES_5::Func(x)*2.0f/3.0f +  NES_5::Func(x*2.0f+0.2f)/3.0f;
}

float NES_7::Func(float x) {
	x = Norm(x)*2.0*M_PI;
	return (sin(x) + sin(x*2 + M_PI/2.0)) * 0.5f;
}

float NES_8::NES_8_::Func(float x) {
	float _x = NES_7::Norm(x);
	if(_x < 0.25)
		return NES_2::Func(x);
	if(_x < 0.5)
		return NES_2::Func(0.25);
	if(_x < 0.75)
		return NES_5::Func(x);
	return NES_7::Func(x);
}

float NES_8::Func(float x) {
	return n.Func(x);
}


#endif
