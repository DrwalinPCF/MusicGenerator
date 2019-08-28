
// This file is part of The Drwalin Sounds project
// Copyright (C) 2019 Marek Zalewski aka Drwalin aka DrwalinPCF

#ifndef GENERATOR_UTIL_CPP
#define GENERATOR_UTIL_CPP

#include "generatorutil.h"

#include <ctime>
#include <cstdio>
#include <cstdlib>

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

#endif

