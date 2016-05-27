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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "error.h"
#include "macro.h"
#include "memory.h"

#define DEFINE          1
#define UNDEFINE        2

#define PURGE_ALL       1
#define PURGE_DEFINE    2
#define PURGE_NOACTION  0x0000
#define PURGE_PURGED    0x0001
#define PURGE_SKIPPED   0x0002

typedef struct ListElem {
    int                 type;       /* DEFINE or UNDEFINE */
    char                *name;      /* the macro's name */
    char                *value;     /* NULL if no expansion */
    struct ListElem     *next;      /* next macro in the list */
} ListElem;


static int      purge_from_list( int purgeOnly, const char *name );
static void     append_list( ListElem *elem );
static void     delete_item( ListElem *elem );
static char     *validate_define_str( const char *str );

static ListElem *macroList;

/*
 * Initialize the macro list.
 */
void InitMacro( void )
/********************/
{
    macroList = NULL;
}


/*
 * Add one more macro definition.  The macro must be of the form NAME[=VALUE].
 * Returns non-zero on success, or zero if the string is incorrectly formed.
 */
int DefineMacro( const char *defineStr )
/**************************************/
{
    ListElem            *newElem;
    const char *        p;
    char *              name;
    char *              value;
    size_t              len;
    const char *        str;
    bool                freeStrFlag = false;

    /*** Validate the string ***/
    str = validate_define_str( defineStr );
    if( str != NULL ) {
        freeStrFlag = true;
    } else {
        str = defineStr;
    }

    /*** Extract the macro name from str ***/
    for( p = str, len = 0; *p != '\0'; p++, len++ ) {
        if( *p == '=' ) {
            break;
        }
    }
    if( len == 0 )
        return( 0 );
    name = AllocMem( len + 1 );
    memcpy( name, str, len );
    name[len] = '\0';
    if( *p != '\0' )
        p++;

    /*** Extract the macro value from str ***/
    while( isspace( *p ) )
        p++;
    if( *p != '\0' ) {
        value = AllocMem( strlen( p ) + 1 );
        strcpy( value, p );
    } else {
        value = NULL;
    }

    /*** Initialize a new list element ***/
    newElem = AllocMem( sizeof( ListElem ) );
    newElem->type = DEFINE;
    newElem->name = name;
    newElem->value = value;
    newElem->next = NULL;

    /*** Add it to the list ***/
    purge_from_list( PURGE_ALL, name ); /* so we don't have any duplicates */
    append_list( newElem );

    if( freeStrFlag )
        FreeMem( (char*)str );
    return( 1 );
}


/*
 * Remove a macro definition.
 */
void UndefineMacro( const char *name )
/************************************/
{
    ListElem    *newElem;

    /*** Add an undefine directive if needed ***/
    if( purge_from_list( PURGE_DEFINE, name )  ==  PURGE_NOACTION ) {
        newElem = AllocMem( sizeof( ListElem ) );
        newElem->type = UNDEFINE;
        newElem->name = (char*)name;
        newElem->value = NULL;
        newElem->next = NULL;
        append_list( newElem );
    }
}


/*
 * Get the next define string in the list.  Returns NULL if nothing to get.
 */
char *GetNextDefineMacro( void )
/******************************/
{
    ListElem    *curElem = macroList;
    char        *str;
    size_t      len;

    while( curElem != NULL ) {
        if( curElem->type == DEFINE ) {
            /*** Allocate a buffer to hold it ***/
            len = strlen( curElem->name );      /* room for NAME */
            if( curElem->value != NULL ) {
                len++;                          /* room for = */
                len += strlen( curElem->value );/* room for VALUE */
            }
            len++;                              /* room for '\0' */
            str = AllocMem( len );

            /*** Fill in the buffer ***/
            strcpy( str, curElem->name );
            if( curElem->value != NULL ) {
                strcat( str, "=" );
                strcat( str, curElem->value );
            }

            /*** Clean up and go home ***/
            delete_item( curElem );
            return( str );
        }
        curElem = curElem->next;
    }

    return( NULL );
}


/*
 * Get the next undefine string in the list.  Returns NULL if nothing to get.
 */
char *GetNextUndefineMacro( void )
/********************************/
{
    ListElem    *curElem = macroList;
    char        *str;
    size_t      len;

    while( curElem != NULL ) {
        if( curElem->type == UNDEFINE ) {
            /*** Copy the macro name ***/
            len = strlen( curElem->name ) + 1;  /* room for NAME\0 */
            str = AllocMem( len );
            strcpy( str, curElem->name );

            /*** Clean up and go home ***/
            delete_item( curElem );
            return( str );
        }
        curElem = curElem->next;
    }

    return( NULL );
}


/*
 * Removes items from the macro list; use purgeOnly to specify which items
 * to purge.  Returns PURGE_NOACTION if nothing was done, PURGE_PURGED if
 * a matching item was purged, or PURGE_SKIPPED if an item with the correct
 * name was found but which wasn't allowed to be purged due to purgeOnly.
 */
static int purge_from_list( int purgeOnly, const char *name )
/***********************************************************/
{
    ListElem    *curElem = macroList;
    int         retcode = PURGE_NOACTION;

    /*** Try to find the item ***/
    while( curElem != NULL ) {
        if( !strcmp( name, curElem->name ) ) {
            if( (purgeOnly==PURGE_ALL)  ||  (purgeOnly==PURGE_DEFINE && curElem->type==DEFINE) ) {
                /*** Found one we're allowed to delete ***/
                delete_item( curElem );
                retcode = PURGE_PURGED;
                break;
            } else {
                /*** Found one we're not allowed to delete ***/
                retcode = PURGE_SKIPPED;
                break;
            }
        }
        curElem = curElem->next;
    }

    return( retcode );
}


/*
 * Append an item to the list.
 */
void append_list( ListElem *elem )
/***************************************/
{
    ListElem    *curElem = macroList;
    ListElem    *prevElem = NULL;

    while( curElem != NULL ) {
        prevElem = curElem;
        curElem = curElem->next;
    }

    if( prevElem == NULL ) {
        macroList = elem;
    } else {
        prevElem->next = elem;
    }
}


/*
 * Remove the specified item from the list, and free all its memory.
 */
static void delete_item( ListElem *elem )
/***************************************/
{
    ListElem    *curElem = macroList;
    ListElem    *prevElem = NULL;

    while( curElem != NULL ) {
        if( curElem == elem ) {
            if( prevElem == NULL ) {
                macroList = curElem->next;
            } else {
                prevElem->next = curElem->next;
            }
            FreeMem( curElem->name );
            if( curElem->value != NULL )  FreeMem( curElem->value );
            FreeMem( curElem );
        }
        prevElem = curElem;
        curElem = curElem->next;
    }
}


/*
 * Ensure that the given define string is valid, changing it if necessary.
 * Returns NULL if no changes must be made, or otherwise a pointer to a new
 * buffer containing the modified string.
 */
static char *validate_define_str( const char *str )
/*************************************************/
{
    char *              buf;
    bool                madeChange = false;
    int                 count;

    /*** Make a new buffer ***/
    buf = AllocMem( strlen( str ) + 1 );
    strcpy( buf, str );

    /*** Transform "NAME=VALUE" to NAME=VALUE ***/
    if( str[0] == '"'  &&  str[strlen(str)-1] == '"' ) {
        for( count=0; count<=strlen(str)-2 ; count++ ) {
            buf[count] = str[count+1];
        }
        buf[count-1] = '\0';
        madeChange = true;
    }

    if( madeChange ) {
        return( buf );
    } else {
        FreeMem( buf );
        return( NULL );
    }
}
