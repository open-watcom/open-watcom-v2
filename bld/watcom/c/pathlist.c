/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2013-2013 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  Path list processing.
*
****************************************************************************/

#include <stddef.h>
#include "bool.h"
#include "iopath.h"
#include "pathlist.h"

/*
 * In rc/rc/c/rcio.c code function GetPathElementLen is derived from this code.
 * Hold rcio.c in sync with this code.
 */

char *GetPathElement( const char *path_list, const char *end, char **path )
{
    bool    is_blank;
    char    c;

    is_blank = true;
    while( path_list != end && (c = *path_list) != '\0' ) {
        path_list++;
        if( IS_INCLUDE_LIST_SEP( c ) ) {
            if( !is_blank ) {
                break;
            }
        } else if( IS_DIR_SEP( c ) ) {
            is_blank = false;
            *(*path)++ = DIR_SEP;
        } else if( !is_blank ) {
            *(*path)++ = c;
        } else if( c != ' ' ) {
            is_blank = false;
            *(*path)++ = c;
        }
    }
    return( (char *)path_list );
}
