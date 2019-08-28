
// This file is part of The Drwalin Sounds project
// Copyright (C) 2019 Marek Zalewski aka Drwalin aka DrwalinPCF

#ifndef NOTES_GENERATOR_H
#define NOTES_GENERATOR_H

#define WRITE( ... ) fprintf( file, __VA_ARGS__ )

#include "generatorutil.h"

class NotesGenerator
{
private:
	
	char filename[512];
	int bpm;
	int metrumUp, metrumDown;
	int tactDuration;		// as number of 1/128 in note
	FILE * file;
	
	void Write( const char * fmt, ... );
	
	void GenerateDuration( int duration );	// as number of 1/128 in note
	int GetRandomOctave();
	int GetRandomDuration( int & dots );	// returns duration as notes
	int GenerateOneNote();		// returns duration as number of 1/128 in note
	int GetStroke( int id );
	
	void GenerateOneTact();
	
public:
	
	void End();
	const char * GetOutFileName();
	void Generate();
	NotesGenerator( int argc, char ** argv );
};

#endif

