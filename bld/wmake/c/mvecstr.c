/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Functions for "text vector" handling.
*
****************************************************************************/


#define _IN_MVECSTR
#include "make.h"
#include "mmemory.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mvecstr.h"


#define MIN_TEXT    507     /* minimum length we'll create text */

typedef struct vecEntry FAR *ENTRYPTR;

struct vecEntry {
    ENTRYPTR next;
    size_t   len;
    char     text[1];   /* variable length buffer */
};


typedef union vecHead *OURPTR;

union vecHead {
    OURPTR  next;
    struct {
        ENTRYPTR        head;
        ENTRYPTR        tail;
        size_t          totlen;
    } d;
};


STATIC OURPTR freeVec;


/*
 * When StartVec is called, we return a pointer to a union vecHead.  The
 * initial vecHead contains NULL for head, tail, and 0 for totlen.  (The
 * next pointer is only used to chain unused vecHeads together - it is not
 * used in an active vecHead.)
 *
 * vecEntrys are allocated as required by the calls to WriteVec and CatVec.
 * Each vecEntry contains a character buffer of at least MIN_TEXT characters.
 * cpyTxt() fills up the tail vecEntry buffer before allocating a new
 * buffer.
 *
 * If a string larger than MIN_TEXT is written, first the tail buffer is
 * filled, and then a new buffer is allocated with enough room to hold the
 * remainder of the string.  A buffer is never allocated with less than
 * MIN_TEXT bytes.
 *
 * expandVec simply allocates a chunk large enough to contain the complete
 * string and copies all the individual chunks into place.
 *
 * The strings stored in a vecEntry are not null-terminated.
 */

#ifdef USE_SCARCE
STATIC RET_T vecScarce( void )
/****************************/
{
    OURPTR cur;

    if( freeVec != NULL ) {
        while( freeVec != NULL ) {
            cur = freeVec;
            freeVec = freeVec->next;
            FreeSafe( cur );
        }
        return( RET_SUCCESS );
    }

    return( RET_ERROR );
}
#endif


void VecInit( void )
/*************************/
{
    unsigned count;
    OURPTR   new;

    freeVec = NULL;
    for( count = 0; count < VECSTR_ALLOC_VECSTR; ++count ) {
        new = MallocSafe( sizeof( *new ) );
        new->next = freeVec;
        freeVec = new;
    }
#ifdef USE_SCARCE
    IfMemScarce( vecScarce );
#endif
}


void VecFini( void )
/*************************/
{
}


VECSTR StartVec( void )
/****************************/
{
    OURPTR new;

    if( freeVec != NULL ) {
        new = freeVec;
        freeVec = freeVec->next;
    } else {
        new = MallocSafe( sizeof( *new ) );
    }

    new->d.head = NULL;
    new->d.tail = NULL;
    new->d.totlen = 0U;

    return( (VECSTR)new );
}


void FreeVec( VECSTR vec )
/*******************************/
{
    ENTRYPTR walk;
    ENTRYPTR cur;

    walk = ((OURPTR)vec)->d.head;
    while( walk != NULL ) {
        cur = walk;
        walk = walk->next;
        FarFreeSafe( cur );
    }

    ((OURPTR)vec)->next = freeVec;
    freeVec = (OURPTR)vec;
}


STATIC char *expandVec( VECSTR vec )
/**********************************/
{
    char        *result;
    char        *d;
    ENTRYPTR    cur;

    assert( vec != NULL );

    cur = ((OURPTR)vec)->d.head;
    if( cur == NULL ) {
        return( CallocSafe( 1 ) );
    }

    result = MallocSafe( ((OURPTR)vec)->d.totlen + 1 );

    d = result;
    for( ; cur != NULL; cur = cur->next ) {
        FarMemCpy( d, cur->text, cur->len );
        d += cur->len;
    }
    *d = NULLCHAR;

    assert( (size_t)( d - result ) == ((OURPTR)vec)->d.totlen );

    return( result );
}


char *FinishVec( VECSTR vec )
/**********************************/
{
    char    *result;

    result = expandVec( vec );
    FreeVec( vec );

    return( result );
}


STATIC void cpyTxt( OURPTR vec, const char FAR *text, size_t len )
/*****************************************************************
 * Copy text into vec by filling up the current buffer, and/or
 * allocating a new one.
 */
{
    ENTRYPTR    new;
    ENTRYPTR    tail;
    size_t      clen;
    char FAR    *fptr;      /* we use this to help the optimizer */
    size_t      len1;

    assert( vec != NULL && text != NULL && *text != NULLCHAR );

    clen = vec->d.totlen;             /* hold for overflow check */
    vec->d.totlen += len;
    if( clen > vec->d.totlen ) {      /* check for overflow */
        PrtMsg( FTL | LOC | MAXIMUM_STRING_LENGTH );
        ExitFatal();
    }

    tail = vec->d.tail;

    if( tail != NULL ) {
        fptr = tail->text + tail->len;      /* we'll need this value later */

                        /* we have room in tail for all of text */
        if( tail->len + len <= MIN_TEXT ) {
            FarMemCpy( fptr, text, len );
            tail->len += len;
            return;
        } else if( tail->len < MIN_TEXT ) { /* partially empty buffer */
            clen = MIN_TEXT - tail->len;    /* room remaining */
            FarMemCpy( fptr, text, clen );
            tail->len = MIN_TEXT;
                /* setup to make a new entry with remainder of text */
            text += clen;
            len -= clen;
/**/        assert( len > 0 );  /* the len = 0 case should be handled above */
        } /* else we'll have to make a new buffer */
    }

        /* allocate a new buffer, and copy text into it */
    len1 = len;
    if( len1 < MIN_TEXT )
        len1 = MIN_TEXT;
    new = FarMallocSafe( sizeof( *new ) + len1 );
    FarMemCpy( new->text, text, len );
    new->len = len;
    new->next = NULL;

    if( vec->d.head != NULL ) {
        assert( tail == vec->d.tail );    /* we set this earlier */
        tail->next = new;
    } else {
        vec->d.head = new;
    }
    vec->d.tail = new;
}


void WriteVec( VECSTR vec, const char *text )
/**************************************************/
{
    assert( vec != NULL );

    if( text != NULL && *text != NULLCHAR ) {
        cpyTxt( (OURPTR) vec, text, strlen( text ) );
    }
}


void WriteNVec( VECSTR vec, const char *text, size_t len )
/****************************************************************
 * write vector with input string length len
 */
{
    assert( vec != NULL );

    if( text != NULL && *text != NULLCHAR ) {
        cpyTxt( (OURPTR) vec, text, len);
    }
}


void CatVec( VECSTR dest, VECSTR src )
/********************************************
 * Append the contents of src onto dest, and do a FreeVec(src)
 */
{
    ENTRYPTR walk;
    ENTRYPTR cur;

    walk = ((OURPTR)src)->d.head;
    while( walk != NULL ) {
        cur = walk;
        walk = walk->next;
        cpyTxt( dest, cur->text, cur->len );
        FarFreeSafe( cur );
    }
    ((OURPTR)src)->next = freeVec;
    freeVec = (OURPTR)src;
}

// TODO: If these functions work the way I think they do, all calls to
//       Cat(N)StrToVec should be just replaced with Write(N)Vec - MN

void CatStrToVec( VECSTR dest, const char* str )
/******************************************************
 *  Concatenate the whole string ended by nullchar to the end of dest
 */
{
    WriteVec( dest, str );
}


void CatNStrToVec( VECSTR dest, const char* str, size_t length )
/**********************************************************************
 * Concatenate a string to the end of 'dest' with length 'length'
 */
{
    WriteNVec( dest, str, length );
}
