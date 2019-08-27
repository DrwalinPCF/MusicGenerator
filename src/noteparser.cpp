
#ifndef NOTE_PARSER_CPP
#define NOTE_PARSER_CPP

#include "noteparser.h"

#include <cstdio>
#include <cstdlib>

int NoteParser::ParseInt( LPCHAR & str )
{
	int sum=0;
	for( ; *str >= '0' && *str <= '9'; ++str )
		sum = sum*10 + (*str-'0');
	return sum;
}

void NoteParser::ParseWholeNoteDurarion( LPCHAR & str )
{
	int bpm = ParseInt(str);
	wholeNoteDuration = 4.0f * 60.0f / float(bpm);
}

void NoteParser::ParsePause( LPCHAR & str )
{
	int v = ParseInt( str );
	float t = wholeNoteDuration / float(v);
	for( float d=t/2.0f; *str == '.'; d/=2.0f, ++str )
		t += d;
	currentTime += t;
}

void NoteParser::ParseMetrum( LPCHAR & str )
{
	metrumUp = ParseInt( str );
	++str;
	metrumDown = ParseInt( str );
	tactTime = float(metrumUp) * wholeNoteDuration / float(metrumDown);
}

float NoteParser::ParseNote( LPCHAR & str )
{
	NOTE note = Note( *str ); ++str;
	if( note == NONE )
		return 0.0f;
	int octave = ParseInt( str );
	Freq freq( note, octave, halftonesModification[note] );
	++str;
	float notedur = ParseInt(str);
	if( notedur == 0 )
	{
		printf("\n int = 0" );
		return 0.0f;
	}
	float duration = wholeNoteDuration / float( notedur );
	float ret = duration;
	for( float d=duration/2.0f; *str == '.'; ++str, d/=2.0f )
		duration += d;
	instrument->Add( *wav, freq, volume, currentTime, duration );
	//printf("\n note: %i:%i added at time: %f->%f", note, octave, currentTime, duration );
	return ret;
}

void NoteParser::ParseBracket( LPCHAR & str )
{
	float t = 0.0f;
	for( ; *str && *str != ')'; )
	{
		float f = ParseNote( str );
		if( t < f )
			t = f;
	}
}

void NoteParser::Parse( const LPCHAR _str )
{
	LPCHAR str = (LPCHAR)_str;
	NOTE n;
	for( ; *str; ++str )
	{
		switch( *(str++) )
		{
		case '(': ParseBracket( str ); break;
		case 'p': ParsePause( str ); break;
		case '|': currentTime = float(1+(int((currentTime-0.001f)/tactTime)))*tactTime; break;
		case 'b': n = Note(*str); if( n != NONE ) halftonesModification[n] = -1; break;
		case '#': n = Note(*str); if( n != NONE ) halftonesModification[n] = 1; break;
		case 'k': n = Note(*str); if( n != NONE ) halftonesModification[n] = 0; break;
		case 'm': ParseMetrum( str ); break;
		case 'n': ParseWholeNoteDurarion( str ); break;
		default:
			--str;
			ParseNote( str );
			--str;
		}
	}
}

void NoteParser::ParseFile( const LPCHAR str )
{
	FILE * file = fopen( str, "rb" );
	if( file == NULL )
		return;
	fseek( file, 0, SEEK_END );
	int fileSize = ftell( file );
	fseek( file, 0, SEEK_SET );
	char * buf = (char*)malloc(fileSize+1);
	buf[fileSize] = 0;
	fread( buf, 1, fileSize, file );
	fclose( file );
	Parse( buf );
	free( buf );
}

void NoteParser::Init( Instrument * instrument, Wav * wav, int metrumUp, int metrumDown, float volume )
{
	this->instrument = instrument;
	this->wav = wav;
	this->metrumUp = metrumUp;
	this->metrumDown = metrumDown;
	this->currentTime = 0;
	this->tactTime = float(metrumUp) * 2.0f / float(metrumDown);
	this->volume = volume;
	for( int i=0; i<7; ++i )
		halftonesModification[i] = 0;
	wholeNoteDuration = 2.0f;
}

NoteParser::NoteParser()
{
	instrument = NULL;
	wav = NULL;
}


#endif


