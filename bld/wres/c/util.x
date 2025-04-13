/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Utility routines to manipulate certain structures in memory.
*
****************************************************************************/


#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "layer1.h"
#include "wresrtns.h"
#include "util.h"


#define UINT16_MAXDIGITS    5

WResIDName * WResIDNameFromStr( char * string )
/*********************************************/
{
    WResIDName *        newstring;
    int                 stringlen;

    stringlen = strlen( string );
    if (stringlen > UCHAR_MAX) {
        /* truncate the string if it is more that UCHAR_MAX in length */
        stringlen = UCHAR_MAX;
    }

    newstring = WRESALLOC( sizeof(WResIDName) + stringlen - 1 );
    if (newstring != NULL) {
        newstring->NumChars = stringlen;
        /* don't copy the '\0' */
        memcpy( &(newstring->Name), string, stringlen );
    }

    return( newstring );
}

void WResInitIDFromNum( long newnum, WResID * newid )
/****************************************************
 * fill in a WResID
 */
{
    newid->IsName = FALSE;
    newid->ID.Num = newnum;
} /* WResInitIDFromNum */


WResID * WResIDFromNum( long newnum )
/************************************
 * allocate an ID and fill it in
 */
{
    WResID *    newid;

    if (newnum >= 0 && newnum <= USHRT_MAX) {
        newid = WRESALLOC( sizeof(WResID) );
    
        if (newid != NULL) {
            WResInitIDFromNum( newnum, newid );
        }
    } else {
        newid = NULL;
    }

    return( newid );
} /* WResIDFromNum */

WResID * WResIDFromStr( const char * newstr )
/********************************************
 * allocate an ID and fill it in
 */
{
    WResID *    newid;
    unsigned    strsize;

    strsize = strlen( newstr );
    /* check the size of the string:  can it fit in one byte? */
    if (strsize <= 0xff) {
        /* allocate the new ID */
        newid = WRESALLOC( sizeof(WResID) + strsize - 1 );

        if (newid != NULL) {
            newid->IsName = TRUE;
            newid->ID.Name.NumChars = strsize;
            memcpy( newid->ID.Name.Name, newstr, strsize );
        }
    } else {
        newid = NULL;
    }

    return( newid );
} /* WResIDFromStr */

char * WResIDToStr( const WResID * name )
/****************************************
 * return the value in an ID if it is a string, NULL otherwise
 */
{
    char *      string;

    if ( name != NULL && name->IsName) {
        /* alloc space for the string and a \0 char at the end */
        string = WRESALLOC( name->ID.Name.NumChars + 1 );
        if (string != NULL) {
            /* copy the string */
            memcpy( string, name->ID.Name.Name, name->ID.Name.NumChars );
            string[ name->ID.Name.NumChars ] = '\0';
        }
    } else {
        string = WRESALLOC( UINT16_MAXDIGITS );
        itoa( name->ID.Num, string, 10 );
    }

    return( string );
} /* WResIDToStr */

long WResIDToNum( const WResID * num )
/*************************************
 * return the value in a string if it is a number, -1 otherwise
 */
{
    if (num != NULL && !num->IsName) {
        return( num->ID.Num );
    } else {
        return( -1 );
    }
} /* WResIDToNum */

void WResIDFree( WResID * oldid )
/********************************
 * release the space occupied by an ID
 */
{
    if (oldid != NULL) {
        WRESFREE( oldid );
    }
} /* WResIDFree */

int WResIDExtraBytes( const WResID * name )
/*****************************************/
{
    int     extrabytes;

    if (name->IsName) {
        extrabytes = name->ID.Name.NumChars - 1;
    } else {
        extrabytes = 0;
    }

    return( extrabytes );
}

int WResIDNameCmp( const WResIDName * name1, const WResIDName * name2 )
/**********************************************************************
 * Note: don't use stricmp since the names in WResID's are not NULL terminated
 */
{
    int     cmp_rc;

    cmp_rc = memicmp( name1->Name, name2->Name,
                        min( name1->NumChars, name2->NumChars) );
    if (cmp_rc == 0) {
        if (name1->NumChars == name2->NumChars) {
            return( 0 );
        } else if (name1->NumChars > name2->NumChars) {
            /* longer names with the same prefix are greater */
            return( 1 );
        } else {
            return( -1 );
        }
    } else {
        return( cmp_rc );
    }
} /* WResIDNameCmp */

int WResIDCmp( const WResID * name1, const WResID * name2 )
/*********************************************************/
{
    int     same;

    if (name1->IsName && name2->IsName) {
        /* they are both names */
        same = ( WResIDNameCmp( &(name1->ID.Name), &(name2->ID.Name) ) == 0 );
    } else if (!(name1->IsName) && !(name2->IsName)) {
        same = (name1->ID.Num == name2->ID.Num);
    } else {
        /* one is a name, the other is a number:  they are not the same */
        same = FALSE;
    }

    return( same );
}

ResNameOrOrdinal * WResIDToNameOrOrd( WResID * id )
/*************************************************/
{
    ResNameOrOrdinal *  newname;

    if (id->IsName) {
        /* the one char in the ResNameOrOrdinal gives room for the '\0' */
        newname = WRESALLOC( sizeof(ResNameOrOrdinal) + id->ID.Name.NumChars );
        if (newname != NULL) {
            memcpy( newname->name, id->ID.Name.Name, id->ID.Name.NumChars );
            newname->name[ id->ID.Name.NumChars ] = '\0';
        }
    } else {
        newname = WRESALLOC( sizeof(ResNameOrOrdinal) );
        if (newname != NULL) {
            newname->ord.fFlag = 0xff;
            newname->ord.wOrdinalID = id->ID.Num;
        }
    }

    return( newname );
}

WResID * WResIDFromNameOrOrd( ResNameOrOrdinal * name )
/*****************************************************/
{
    if (name->ord.fFlag == 0xff) {
        return( WResIDFromNum( name->ord.wOrdinalID ) );
    } else {
        return( WResIDFromStr( name->name ) );
    }
} /* WResIDFromNameOrOrd */
