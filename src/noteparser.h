
// This file is part of The Drwalin Sounds project
// Copyright (C) 2019 Marek Zalewski aka Drwalin aka DrwalinPCF

#ifndef NOTE_PARSER_H
#define NOTE_PARSER_H

#include "wav.h"

class NoteParser
{
private:
	
	Instrument * instrument;
	Wav * wav;
	int metrumUp, metrumDown;
	float tactTime;
	float currentTime;
	float volume;
	float wholeNoteDuration;
	
	int halftonesModification[7];
	
public:
	
	using LPCHAR = char*;
	
	int ParseInt( LPCHAR & str );
	void ParseWholeNoteDurarion( LPCHAR & str );
	void ParsePause( LPCHAR & str );
	void ParseMetrum( LPCHAR & str );
	float ParseNote( LPCHAR & str );
	void ParseBracket( LPCHAR & str );
	void Parse( const LPCHAR str );
	void ParseFile( const LPCHAR str );
	
	void Init( Instrument * instrument, Wav * wav, int metrumUp, int metrumDown, float volume );
	NoteParser();
};

#endif

