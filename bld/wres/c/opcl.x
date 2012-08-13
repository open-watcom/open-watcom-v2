/*
*******************************************************************
*	Copyright by WATCOM Systems Inc. 1991. All rights	  *
*	reserved. No part of this software may be reproduced	  *
*	in any form or by any means - graphic, electronic or	  *
*	mechanical, including photocopying, recording, taping	  *
*	or information storage and retrieval systems - except	  *
*	with the written permission of WATCOM Systems Inc.	  *
*******************************************************************

Routines to open and close WatCOM  .RES  files.

Modified	By		Reason
--------	--		------
31 May 91	S.Bosnick	created for WRES library
*/

#include <fcntl.h>
#include <sys/stat.h>
#include "layer0.h"
#include "filefmt.h"
#include "resfmt.h"
#include "mresfmt.h"
#include "read.h"
#include "write.h"
#include "opcl.h"


WResFileID WResOpenNewFile( const char * filename )
/*************************************************/
{
    WResFileID	    newhandle;

    newhandle = (* WRESOPEN) ( filename, O_CREAT | O_WRONLY | O_TRUNC
			| O_BINARY, S_IWRITE | S_IREAD );
    if (newhandle != -1) {
	WResFileInit( newhandle );
    }

    return( newhandle );
}

WResFileID MResOpenNewFile( const char * filename )
/*************************************************/
{
    return( (* WRESOPEN) ( filename, O_CREAT | O_WRONLY | O_TRUNC
			| O_BINARY, S_IWRITE | S_IREAD ) );
}

int WResFileInit( WResFileID handle )
/***********************************/
/* Writes the initial file header out to the file. Later, when WResWriteDir */
/* is called the real header will be written out */
{
    WResHeader	head;
    int 	error;

    head.Magic[0] = WRESMAGIC0;
    head.Magic[1] = WRESMAGIC1;
    head.DirOffset = 0;
    head.NumResources = 0;
    head.NumTypes = 0;
    head.WResVer = WRESVERSION;

    /* write the empty record out at the begining of the file */
    error = (*WRESSEEK) ( handle, 0, SEEK_SET );
    if (!error) {
	error = WResWriteHeaderRecord( &head, handle );
    }

    return( error );
} /* WResInitFile */

WResFileID  ResOpenFileRO( const char * filename )
/*************************************************/
/* use this function to open Microsoft .RES files also */
{
    return( (* WRESOPEN) ( filename, O_RDONLY | O_BINARY ) );
}

WResFileID  ResOpenFileRW( const char * filename )
/*************************************************/
{
    return( (* WRESOPEN) ( filename, O_CREAT | O_RDWR | O_BINARY,
			S_IWRITE | S_IREAD ) );
}

int ResCloseFile( WResFileID handle )
/************************************/
{
    return( (* WRESCLOSE) ( handle ) );
}

int WResCheckWResFile( const char * filename )
/********************************************/
{
    int 	iswresfile;
    WResFileID	handle;

    handle = ResOpenFileRO( filename );

    if (handle == -1) {
	return( FALSE );	/* assume it's not a WRES file */
    }

    iswresfile = WResIsWResFile( handle );

    ResCloseFile( handle );

    return( iswresfile );
}
