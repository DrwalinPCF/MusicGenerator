
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define WRITE( ... ) fprintf( file, __VA_ARGS__ )

const char * Time()
{
	static char s[16];
	sprintf( s, "%lX", time(NULL) );
	return s;
}

const char * GetUniqueFileName()
{
	static char s[128];
	sprintf( s, "sheets\\note-%lX-%3.3x.note", time(NULL), rand()%(16*16*16) );
	return s;
}

char GetRandomNote()
{
	char n[] = {'c','d','e','f','g','a','h'};
	return n[rand()%sizeof(n)];
}

int GetRandomOctave()
{
	switch( rand() % 11 )
	{
	case 0:
	case 1:
	case 2:
	case 3:
		return 3;
	case 4:
		return 5;
	case 5:
		return 2;
	}
	return 4;
}

int GetRandomDuration()
{
	switch( rand() % 20 )
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		return 8;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		return 2;
	case 10:
	case 11:
	case 12:
		return 16;
	case 13:
		return 1;
	}
	return 4;
}

int GetRandomCount()
{
	switch( rand()%36 )
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 8:
	case 9:
		return 2;
	case 10:
		return 3;
	}
	return 1;
}

char * ConvertToNotableTime( float t )
{
	static char str[1024];
	float b = 1.0f;
	for( ; b>t; b/=2.0f );
	float i, j;
	int k;
	for( k=0, i=b/2, j=0; b+j+i < t && k<3; j+=i, i/=2 );
	
	int bi = int((1.0f/b)+0.5f);
	
	sprintf( str, "%i", bi );
	for( char * s = str; ; ++s )
	{
		if( *s == 0 )
		{
			for( ; k; --k, ++s )
			{
				*s = '.';
			}
			*s = 0;
			break;
		}
	}
	
	return str;
}

int main( int argc, char ** argv )
{
	srand( time(NULL) );
	FILE * file = NULL;
	const char * outfilename = argc >= 2 ? argv[1] : GetUniqueFileName();
	file = fopen( outfilename, "w" );
	if( file == NULL )
		return -2;
	WRITE( "n120 " );
	WRITE( "m2/4 " );
	
	char str[1024];
	
	for( int i = 0; i < 40; ++i )
	{
		WRITE("(");
		int count = GetRandomCount();
		float midduration=0;
		for( int j=0; j<count; ++j )
		{
			char note = GetRandomNote();
			int octave = GetRandomOctave();
			int duration = GetRandomDuration();
			midduration += 1.0f/duration;
			fprintf( file, "%c%i-%i", note, octave, duration );
			float k;
			for( k=1.0f/duration; (rand()%30)==0; k/=2.0f )
				( midduration+=k, WRITE(".") );
		}
		WRITE(")p%s ", ConvertToNotableTime( midduration ) );
	}
	
	fclose( file );
	printf( "\n" );
	
	sprintf( str, "wavgenerate %s", outfilename );
	
	system( str );
	
	return 0;
}



