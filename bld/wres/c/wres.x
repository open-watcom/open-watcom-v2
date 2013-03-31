/*
*******************************************************************
*	Copyright by WATCOM Systems Inc. 1991. All rights	  *
*	reserved. No part of this software may be reproduced	  *
*	in any form or by any means - graphic, electronic or	  *
*	mechanical, including photocopying, recording, taping	  *
*	or information storage and retrieval systems - except	  *
*	with the written permission of WATCOM Systems Inc.	  *
*******************************************************************

Routines to manipulate WATCOM .RES files. They maintain the directory on disk
and in memory.

Modified	By		Reason
--------	--		------
29 May 91	S.Bosnick	created for WRes library
*/

#include <string.h>
#include "layer1.h"
#include "mem2.h"
#include "wresrtns.h"
#include "util.h"
#include "wres.h"

WResDir WResInitDir( void )
/*************************/
{
    WResDirHead *   newdir;

    newdir = WRESALLOC( sizeof(WResDirHead) );
    if (newdir != NULL) {
	newdir->NumResources = 0;
	newdir->NumTypes = 0;
	newdir->Head = NULL;
	newdir->Tail = NULL;
    }

    return( newdir );
}

int WResIsEmpty( WResDir currdir )
/********************************/
{
    return( currdir->Head == NULL && currdir->Tail == NULL );
}

static void FreeResList( WResTypeNode * currtype )
{
    WResResNode *   oldnode;
    WResResNode *   currnode;

    currnode = currtype->Head;
    while (currnode != NULL) {
	oldnode = currnode;
	currnode = currnode->Next;
	WRESFREE( oldnode );
    }

    currtype->Head = NULL;
    currtype->Tail = NULL;
}

static void FreeTypeList( WResDirHead * currdir )
{
    WResTypeNode *  oldtype;
    WResTypeNode *  currtype;

    currtype = currdir->Head;
    while (currtype != NULL) {
	oldtype = currtype;
	currtype = currtype->Next;
	FreeResList( oldtype );
	WRESFREE( oldtype );
    }

    currdir->Head = NULL;
    currdir->Tail = NULL;
}

void WResFreeDir( WResDir currdir )
/*********************************/
{
    FreeTypeList( currdir );

    WRESFREE( currdir );
}

static WResTypeNode * FindType( const WResID * type, WResDir currdir )
{
    WResTypeNode *  currtype;

    for (currtype = currdir->Head; currtype != NULL; currtype = currtype->Next){
	if (WResIDCmp( type, &(currtype->Info.TypeName) )) {
	    break;
	}
    }

    return( currtype );
}

static WResResNode * FindRes( const WResID * name, WResTypeNode * currtype )
{
    WResResNode *   currres;

    for (currres = currtype->Head; currres != NULL; currres = currres->Next) {
	if (WResIDCmp( name, &(currres->Info.ResName) )) {
	    break;
	}
    }

    return( currres );
}

WResDirWindow WResFindResource( const WResID * type, const WResID * name,
			WResDir currdir )
/***********************************************************************/
{
    WResDirWindow   newwind;

    newwind.CurrType = FindType( type, currdir );
    if (newwind.CurrType != NULL) {
	newwind.CurrRes = FindRes( name, newwind.CurrType );
    } else {
	newwind.CurrRes = NULL;
    }

    return( newwind );
}

WResResInfo * WResGetResInfo( WResDirWindow currwind )
/****************************************************/
{
    if (currwind.CurrRes == NULL) {
	return( NULL );
    } else {
	return( &(currwind.CurrRes->Info) );
    }
}

WResTypeInfo * WResGetTypeInfo( WResDirWindow currwind )
/******************************************************/
{
    if (currwind.CurrType == NULL) {
	return( NULL );
    } else {
	return( &(currwind.CurrType->Info) );
    }
}

static WResTypeNode * NewTypeNode( const WResID * type )
{
    WResTypeNode *  newnode;
    int 	    extrabytes;

    extrabytes = WResIDExtraBytes( type );
    newnode = WRESALLOC( sizeof(WResTypeNode) + extrabytes );
    if (newnode != NULL) {
	newnode->Next = NULL;
	newnode->Prev = NULL;
	newnode->Head = NULL;
	newnode->Tail = NULL;
	newnode->Info.NumResources = 0;
	memcpy( &(newnode->Info.TypeName), type, sizeof(WResID) + extrabytes );
    }

    return( newnode );
}

static WResResNode * NewResNode( const WResID * name, uint_16 memflags,
			uint_32 offset, uint_32 length )
{
    WResResNode *   newnode;
    int 	    extrabytes;

    extrabytes = WResIDExtraBytes( name );
    newnode = WRESALLOC( sizeof(WResResNode) + extrabytes );
    if (newnode != NULL) {
	newnode->Next = NULL;
	newnode->Prev = NULL;
	newnode->Info.MemoryFlags = memflags;
	newnode->Info.Offset = offset;
	newnode->Info.Length = length;
	memcpy( &(newnode->Info.ResName), name, sizeof(WResID) + extrabytes );
    }

    return( newnode );
}

int WResAddResource( const WResID * type, const WResID * name,
			uint_16 memflags, uint_32 offset, uint_32 length,
			WResDir currdir, int * duplicate )
/************************************************************/
/* Add the new entry to the directory. If the entry is already there don't */
/* add anything, set duplicate TRUE and return an error. Return is TRUE if */
/* any error has occured (including duplicate entry) */
{
    WResTypeNode *  currtype;
    WResResNode *   currres;

    /* set duplicate FALSE so other errors will have it set correctly */
    if (duplicate != NULL) {
	*duplicate = FALSE;
    }

    currtype = FindType( type, currdir );
    if (currtype != NULL) {
	/* if the type is in there already check for a duplicate resource */
	currres = FindRes( name, currtype );
	/* if duplicate entry */
	if (currres != NULL) {
	    if (duplicate != NULL) {
		*duplicate = TRUE;
	    }
	    return( TRUE );
	}
    } else {
	/* otherwise add the type to the list */
	currtype = NewTypeNode( type );
	if (currtype == NULL) {
	    return( TRUE );
	}
	ResAddLLItemAtEnd( &(currdir->Head), &(currdir->Tail), currtype );
	/* adjust the count of the number of types */
	currdir->NumTypes += 1;
    }

    /* add the resource to the current type */
    currres = NewResNode( name, memflags, offset, length );
    if (currres == NULL) {
	return( TRUE );
    }
    ResAddLLItemAtEnd( &(currtype->Head), &(currtype->Tail), currres );
    /* adjust the counts of the number of resources */
    currtype->Info.NumResources += 1;
    currdir->NumResources += 1;

    /* no error has occured */
    return( FALSE );
}

WResDirWindow WResFirstResource( WResDir currdir )
/************************************************/
{
    WResDirWindow   wind;

    wind.CurrType = currdir->Head;
    wind.CurrRes = wind.CurrType->Head;

    return( wind );
}

int WResIsLastResource( WResDirWindow currwind, WResDir currdir )
/***************************************************************/
{
    return( currwind.CurrType == currdir->Tail &&
		currwind.CurrRes == currdir->Tail->Tail );
} /* WResIsLastResource */

#pragma off (unreferenced);
WResDirWindow WResNextResource( WResDirWindow currwind, WResDir currdir )
#pragma on (unreferenced);
/***********************************************************************/
{
    if (!WResIsEmptyWindow( currwind )) {
	if (currwind.CurrRes->Next == NULL) {
	    currwind.CurrType = currwind.CurrType->Next;
	    currwind.CurrRes = currwind.CurrType->Head;
	} else {
	    currwind.CurrRes = currwind.CurrRes->Next;
	}
    }

    return( currwind );
} /* WResNextResource */

static int WriteResList( WResFileID handle, WResResNode * currres )
{
    int error;

    for (error = FALSE; currres != NULL && !error; currres = currres->Next) {
	error = WResWriteResRecord( &(currres->Info), handle );
    }

    return( error );
}

static int WriteTypeList( WResFileID handle, WResTypeNode * currtype )
{
    int error;

    for (error = FALSE; currtype != NULL && !error; currtype = currtype->Next) {
	error = WResWriteTypeRecord( &(currtype->Info), handle );
	if (!error) {
	    error = WriteResList( handle, currtype->Head );
	}
    }

    return( error );
}

int WResWriteDir( WResFileID handle, WResDir currdir )
/****************************************************/
{
    WResHeader	head;
    int 	error;
    uint_32	diroffset;
    uint_32	seekpos;

    /* get the offset of the start of the directory */
    diroffset = WRESTELL( handle );
    error = (diroffset == -1);

    if (!error) {
	error = WriteTypeList( handle, currdir->Head );
    }
    /* write out the file header */
    if (!error) {
	head.Magic[0] = WRESMAGIC0;
	head.Magic[1] = WRESMAGIC1;
	head.DirOffset = diroffset;
	head.NumResources = currdir->NumResources;
	head.NumTypes = currdir->NumTypes;
	head.WResVer = WRESVERSION;
	error = WResWriteHeaderRecord( &head, handle );
    }
    /* leave the handle at the start of the file */
    if (!error) {
	seekpos = WRESSEEK( handle, 0L, SEEK_SET );
	error = (seekpos == -1);
    }

    return( error );
}

static int ReadResList( WResFileID handle, WResTypeNode * currtype )
{
    WResResNode *   newnode;
    WResResInfo     newres;
    int 	    error;
    int 	    resnum;
    int 	    extrabytes;

    /* loop through the list of resources of this type */
    for (resnum = 0, error = FALSE; resnum < currtype->Info.NumResources &&
	    !error; resnum++) {
	/* read a resource record from disk */
	error = WResReadFixedResRecord( &newres, handle );
	if (!error) {
	    /* allocate a new node */
	    extrabytes = WResIDExtraBytes( &(newres.ResName) );
	    newnode = WRESALLOC( sizeof(WResResNode) + extrabytes );
	    error = (newnode == NULL);
	}
	if (!error) {
	    /* copy the new resource info into the new node */
	    memcpy( &(newnode->Info), &newres, sizeof(WResResInfo) );

	    /* read the extra bytes (if any) */
	    if (extrabytes > 0) {
		error = WResReadExtraWResID( &(newnode->Info.ResName), handle );
	    }
	}
	if (!error) {
	    /* add the resource node to the linked list */
	    ResAddLLItemAtEnd( &(currtype->Head), &(currtype->Tail), newnode );
	}
    }

    return( error );
}

static int ReadTypeList( WResFileID handle, WResDirHead * currdir )
{
    WResTypeNode *  newnode;
    WResTypeInfo    newtype;
    int 	    error;
    int 	    typenum;
    int 	    extrabytes;

    /* loop through the list of types */
    for (error = FALSE, typenum = 0; typenum < currdir->NumTypes && !error;
		    typenum++) {
	/* read a type record from disk */
	error = WResReadFixedTypeRecord( &newtype, handle );
	if (!error) {
	    /* allocate a new node */
	    extrabytes = WResIDExtraBytes( &(newtype.TypeName) );
	    newnode = WRESALLOC( sizeof(WResTypeNode) + extrabytes );
	    error = (newnode == NULL);
	}
	if (!error) {
	    /* initialize the linked list of resources */
	    newnode->Head = NULL;
	    newnode->Tail = NULL;
	    /* copy the new type info into the new node */
	    memcpy( &(newnode->Info), &newtype, sizeof(WResTypeInfo) );

	    /* read the extra bytes (if any) */
	    if (extrabytes > 0) {
		error = WResReadExtraWResID( &(newnode->Info.TypeName),
			handle );
	    }
	}
	if (!error) {
	    /* add the type node to the linked list */
	    ResAddLLItemAtEnd( &(currdir->Head), &(currdir->Tail), newnode );
	    /* read in the list of resources of this type */
	    error = ReadResList( handle, newnode );
	}
    }

    return( error );
}

static int ReadWResDir( WResFileID handle, WResDir currdir )
/**********************************************************/
{
    WResHeader	head;
    int 	error;
    int 	seekpos;

    /* read the header and check that it is valid */
    error = WResReadHeaderRecord( &head, handle );
    if (!error) {
	error = (head.Magic[0] != WRESMAGIC0 || head.Magic[1] != WRESMAGIC1);
    }
    /* set up the initial info for the directory and seek to it's start */
    if (!error) {
	currdir->NumResources = head.NumResources;
	currdir->NumTypes = head.NumTypes;
	seekpos = WRESSEEK( handle, head.DirOffset, SEEK_SET );
	error = (seekpos == -1);
    }
    /* read in the list of types (and the resources) */
    if (!error) {
	error = ReadTypeList( handle, currdir );
    }

    return( error );
} /* ReadWResDir */

static int ReadMResDir( WResFileID handle, WResDir currdir,
                        int * dup_discarded )
/**********************************************************/
{
    MResResourceHeader *    head;
    int 		    error;
    long		    seek_rc;
    WResID *		    name;
    WResID *		    type;
    int                     dup;

    head = MResReadResourceHeader( handle );
    error = FALSE;
    if ( dup_discarded != NULL ) {
        *dup_discarded = FALSE;
    }
    /* assume that a NULL head is the EOF which is the only way of detecting */
    /* the end of a MS .RES file */
    while (head != NULL && !error) {
	name = WResIDFromNameOrOrd( head->Name );
	type = WResIDFromNameOrOrd( head->Type );
	error = (name == NULL || type == NULL);

	/* MResReadResourceHeader leaves the file at the start of the resource*/
	if (!error) {
	    error = WResAddResource( type, name, head->MemoryFlags,
			WRESTELL( handle ), head->Size, currdir, &dup );
	    if ( error && dup ) {
		error = FALSE;
		if ( dup_discarded != NULL ) {
		    *dup_discarded = TRUE;
		}
	    }
	}

	if (!error) {
	    seek_rc = WRESSEEK( handle, head->Size, SEEK_CUR );
	    error = (seek_rc == -1);
	}

	if (name != NULL) {
	    WRESFREE( name );
	    name = NULL;
	}
	if (type != NULL) {
	    WRESFREE( type );
	    type = NULL;
	}
	MResFreeResourceHeader( head );

	if (!error) {
	    head = MResReadResourceHeader( handle );
	}
    }

    return( error );
} /* ReadMResDir */

int WResReadDir( WResFileID handle, WResDir currdir,
                 int * dup_discarded )
/***************************************************/
{
    int 	error;
    int 	seekpos;

    /* var representing whether or not a duplicate dir entry was
     * discarded is set to FALSE.
     * NOTE: duplicates are not discarded by calls to ReadWResDir.
     */
    if ( dup_discarded == NULL ) {
        *dup_discarded = FALSE;
    }

    /* get rid of any directory info that is already in memory */
    if (currdir->Head != NULL) {
	FreeTypeList( currdir );
    }

    /* seek to the start of the file */
    seekpos = WRESSEEK( handle, 0, SEEK_SET );
    error = (seekpos == -1);

    if (!error) {
	if (WResIsWResFile( handle )) {
	    error = ReadWResDir( handle, currdir );
	} else {
	    error = ReadMResDir( handle, currdir, dup_discarded );
	}
    }

    return( error );
} /* WResReadDir */
