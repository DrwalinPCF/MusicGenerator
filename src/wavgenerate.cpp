
// This file is part of The Drwalin Sounds project
// Copyright (C) 2019 Marek Zalewski aka Drwalin aka DrwalinPCF

#include "wav.h"
#include "noteparser.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

const char * ConvertFileNameToOutput( char * src ) {
	static char ret[256];
	int i, len = strlen(src);
	for( i=0; i<len && src[i]!='\\' && src[i]!='/'; ++i );
	if( i != len )
		src += i + 1;
	for( i=len-1; i>=0 && src[i]!= '.'; --i );
	if( i > 0 ) {
		sprintf( ret, "music/" );
		len = strlen(ret);
		memmove( ret+len, src, i );
		sprintf( ret+i+len, ".wav" );
	}
	else
		sprintf( ret, "music/%s.wav", src );
	return ret;
}

int main( int argc, char ** argv ) {
	float volume = 0.48f;
	Wav wav( 44100, 8 );
	NoteParser notes;
	notes.Init( &nes_8, &wav, 2, 4, volume );
	
	if( argc == 1 ) {
		printf( "\n Pass path to file with notes to convert to *.wav [second argument a s output file] file" );
		return 1;
	}
	
	notes.ParseFile( argv[1] );
	
	wav.BestAdjust();
	wav *= 0.9;
	
	printf( "\n sounds generated" );
	
	const char * outfile = (argc>=3) ? argv[2] : ConvertFileNameToOutput( argv[1] );
	
	wav.Save( outfile );
	
	fprintf( stdout,"\n file saved\n" );
	
	fflush(stdout);
	system( (std::string("mpv ")+outfile).c_str() );
	
	return 0;
}


