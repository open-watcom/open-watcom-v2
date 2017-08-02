/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   %       Copyright (C) 1992, by WATCOM Systems Inc. All rights     %
   %       reserved. No part of this software may be reproduced      %
   %       in any form or by any means - graphic, electronic or      %
   %       mechanical, including photocopying, recording, taping     %
   %       or information storage and retrieval systems - except     %
   %       with the written permission of WATCOM Systems Inc.        %
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
   Date		By		Reason
   ----		--		------
   18-jun-92	Craig Eisler	defined, with G.R.Bentz back seat coding,
   				and D.J. Gaudet in the peanut gallery
   19-jun-92	Craig Eisler	fixed some bugs, added environment var
   21-jun-92	Craig Eisler	misc cleanup, added POSIXRX env variable
   25-aug-92	Craig Eisler	did work with invalid options in env var
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "getopt.h"
#include "misc.h"

extern "C" {
    char	*optarg;
    int		optind=1;
};
char		OptChar;
#ifndef __ISVI__
char		AltOptChar='/';
#else
char		AltOptChar='+';
#endif

static int	optOff=0;
static int	testedOptEnvVar;
#ifndef __ISVI__
static int	testedPosixrx;
#endif
static char	*envVar;

static void eatArg( int *argc, char *argv[], int num )
{
    int	i;

    optOff = 0;
    if( envVar != NULL ) {
	envVar = NULL;
	return;
    }

    for( i=optind+num; i<=*argc; i++ ) {
	argv[i-num] = argv[i];
    }
    (*argc) -= num;

} /* eatArg */

static void Quit( char **usage, char *msg, ... )
{
    for( ; *usage != NULL; ++usage ) {
	puts( *usage );
    }
    if( msg != NULL ) {
	puts( msg );
    }
}


extern "C" int getopt( int *argc, char *argv[], char *optstr, char *usage[] )
{
    char	ch;
    char	*ptr;
    char	*currarg;

    optarg = NULL;
#ifndef __ISVI__
    if( !testedPosixrx ) {
	testedPosixrx = 1;
	if( getenv( "POSIXRX" ) != NULL ) {
	    if( strchr( optstr, 'X' ) != NULL ) {
		return( 'X' );
	    }
	}
    }
#endif
    if( !testedOptEnvVar ) {
	testedOptEnvVar = 1;
	envVar = getenv( "RE2C" );
    }

    while( 1 ) {

	if( envVar != NULL ) {
	    currarg = envVar;
	} else {
	    currarg = argv[ optind ];
	    if( currarg == NULL ) {
		return( -1 );
	    }
	}
	while( 1 ) {
	    ch = currarg[ optOff ];
	    if( isspace( ch ) && envVar != NULL ) {
		optOff++;
		continue;
	    }
	    break;
	}
	if( optOff > 1 || ch == '-' || ch == AltOptChar ) {
	    if( optOff > 1 ) {
		ch = currarg[optOff];
		if( ch == '-' || ch == AltOptChar ) {
		    OptChar = ch;
		    optOff++;
		    ch = currarg[optOff];
		}
	    } else {
		OptChar = ch;
		optOff++;
		ch = currarg[optOff];
	    }
	    if( ch == '?' ) {
		Quit( usage, NULL );
	    }
	    if( optstr[0] == '#' && isdigit( ch ) ) {
		optarg = &currarg[optOff];
		eatArg( argc, argv, 1 );
		return( '#' );
	    }
	    ptr = strchr( optstr, ch );
	    if( ptr == NULL || *ptr == ':' ) {
		Quit( usage, "Invalid option '%c'\n", ch );
	    }
	    if( *(ptr+1) == ':' ) {
#ifndef __ISVI__
		if( *(ptr+2) == ':' ) {
		    if( currarg[optOff+1] != 0 ) {
			optarg = &currarg[optOff+1];
		    }
		    eatArg( argc, argv, 1 );
		    return( ch );
		}
#endif
		if( currarg[optOff+1] == 0 ) {
		    if( argv[ optind+1 ] == NULL ) {
			Quit( usage, "Option '%c' requires a parameter\n", ch );
		    }
		    optarg = argv[ optind+1 ];
		    eatArg( argc, argv, 2 );
		    return( ch );
		}
		optarg = &currarg[optOff+1];
		eatArg( argc, argv, 1 );
		return( ch );
	    }
	    optOff++;
	    if( currarg[ optOff ] == 0 ) {
		eatArg( argc, argv, 1 );
	    }
	    return( ch );
	} else {
	    if( envVar != NULL ) {
		envVar = NULL;
	    } else {
		optind++;
	    }
	}
    }

} /* GetOpt */
