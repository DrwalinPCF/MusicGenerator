
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstdarg>

#include "notesgenerator.h"
#include "generatorutil.h"

void NotesGenerator::Write( const char * fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	vfprintf( file, fmt, args );
	va_end( args );
}

const char * NotesGenerator::GetOutFileName()
{
	return filename;
}

NotesGenerator::NotesGenerator( int argc, char ** argv )
{
	file = NULL;
	if( argc >= 2 )
		sprintf( filename, "%s", argv[1] );
	else
		sprintf( filename, "%s", GetUniqueFileName() );
	
	file = fopen( filename, "w" );
	
	if( file == NULL )
		exit( -5 );
	
	bpm = (rand()%90) + 30;
	
	metrumDown = 4 << ( rand()%100<80 ? 0 : (rand()%100<70 ? 1 : 2) );
	metrumUp = 2 + (rand()%(metrumDown-2));
	
	tactDuration = metrumUp * (128/metrumDown);
}

void NotesGenerator::Generate()
{
	Write( "n%i\n", bpm );
	Write( "m%i/%i\n", metrumUp, metrumDown );
	
	int tacts = rand() % 5*metrumDown + metrumDown*3;
	for( int i=0; i<tacts; ++i )
	{
		GenerateOneTact();
	}
}

void NotesGenerator::GenerateDuration( int duration )
{
	int id;
	for( id=1; (id*2)<duration; id*=2 );
	Write( "p%i", 128/id );
	duration -= id;
	id/=2;
	int count = 0;
	while( duration>=id && count < 3 )
	{
		Write( "." );
		duration-=id;
		id/=2;
		++count;
	}
}

int NotesGenerator::GetRandomOctave()
{
	int value = rand()%100;
	if( value < 70 )
		return 4;
	if( value < 83 )
		return 3;
	if( value < 97 )
		return 5;
	return 6;
}

int NotesGenerator::GetRandomDuration( int & dots )
{
	int value;
	
	value = rand()%100;
	if( value < 80 )
		dots = 0;
	else if( value < 95 )
		dots = 1;
	else
		dots = 2;
	
	value = rand()%100;
	if( value < 40 )
		return 4;
	if( value < 70 )
		return 8;
	if( value < 90 )
		return 2;
	if( value < 96 )
		return 16;
	return 1;
}

int NotesGenerator::GenerateOneNote()
{
	char note = GetRandomNote();
	int octave = GetRandomOctave();
	int dots = 0;
	int duration = GetRandomDuration( dots );
	Write( "%c%i-%i", note, octave, duration );
	int ret = 128/duration;
	for( int i = 0; i < dots; ++i )
	{
		Write( "." );
		ret += 128/(duration<<(i+1));
	}
	return ret;
}

int NotesGenerator::GetStroke( int id )
{
	int value = rand()%100;
	if( id == 0 )
	{
		if( value < 35 )
			return 2;
		if( value < 80 )
			return 3;
		if( value < 95 )
			return 1;
		return 4;
	}
	if( value < 63 )
		return 1;
	if( value < 89 )
		return 2;
	if( value < 98 )
		return 3;
	return 4;
}

void NotesGenerator::GenerateOneTact()
{
	int sumduration = 0;
	
	for( int id=0;; ++id )
	{
		int minduration = 1024, maxduration=0;
		int notesInOne = GetStroke( id );
		Write( "(" );
		for( int i=0; i<notesInOne; ++i )
		{
			int t = GenerateOneNote();
			if( minduration > t )
				minduration = t;
			if( maxduration < t )
				maxduration = t;
		}
		Write( ")" );
		if( sumduration + maxduration > tactDuration )
		{
			if( tactDuration == 0 )
				GenerateDuration( 32 );
			else
				GenerateDuration( minduration );
			break;
		}
		else
		{
			sumduration += minduration;
			GenerateDuration( minduration );
		}
		Write( " " );
	}
	Write( " |\n" );
}

void NotesGenerator::End()
{
	if( file )
		fclose( file );	
	file = NULL;
}

int main( int argc, char ** argv )
{
	srand( time(NULL) );
	NotesGenerator gen( argc, argv );
	gen.Generate();
	
	printf( "\n Output: %s", gen.GetOutFileName() );
	
	gen.End();
	
	char str[1024];
	sprintf( str, "wavgenerate %s", gen.GetOutFileName() );
	system( str );
	
	return 0;
}



