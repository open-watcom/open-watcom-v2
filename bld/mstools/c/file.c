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


#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "error.h"
#include "file.h"
#include "memory.h"
#include "pathconv.h"


/*
 * Define the ListElem structure.
 */
struct ListElem {
    int                 type;       /* file type */
    char *              filename;   /* name of the file to handle */
    struct ListElem *   next;       /* next item in the list */
};

static struct ListElem *fileList = NULL;

static int              defaultType = TYPE_INVALID_FILE;
static const char *     defaultName = NULL;

static int              allowC = 0;
static int              allowCPP = 0;
static int              allowDEF = 0;
static int              allowLIB = 0;
static int              allowOBJ = 0;
static int              allowRC = 0;
static int              allowRES = 0;
static int              allowRBJ = 0;
static int              allowRS = 0;
static int              allowEXP = 0;

static int              languageToForce = FORCE_NONE;

/*
 * Examine a file's extension to determine what type of file it is.  If the
 * file has no extension, a warning is issued and TYPE_ASSUME_FILE is assumed.
 */
static int file_type( const char *filename )
/******************************************/
{
    char *              newfilename;
    char *              tempfilename;
    char                ext[_MAX_EXT];
    int                 type;

    /*** Strip quotes from filename ***/
    newfilename = DupStrMem( filename );
    if( *newfilename == '"' ) {
        tempfilename = newfilename + 1;                 /* skip leading " */
        tempfilename[ strlen(tempfilename)-1 ] = '\0';  /* smite trailing " */
    } else {
        tempfilename = newfilename;
    }

    _splitpath( tempfilename, NULL, NULL, NULL, ext );
    if( allowC  &&  !stricmp( ext, ".c" ) ) {
        type = TYPE_C_FILE;
    } else if( allowCPP  &&  !stricmp( ext, ".cc" ) ) {
        type = TYPE_CPP_FILE;
    } else if( allowCPP  &&  !stricmp( ext, ".cpp" ) ) {
        type = TYPE_CPP_FILE;
    } else if( allowCPP  &&  !stricmp( ext, ".cxx" ) ) {
        type = TYPE_CPP_FILE;
    } else if( allowCPP  &&  !stricmp( ext, ".odl" ) ) {
        type = TYPE_CPP_FILE;
    } else if( allowCPP  &&  !stricmp( ext, ".idl" ) ) {
        type = TYPE_CPP_FILE;
    } else if( allowDEF  &&  !stricmp( ext, ".def" ) ) {
        type = TYPE_DEF_FILE;
    } else if( allowOBJ  &&  !stricmp( ext, ".obj" ) ) {
        type = TYPE_OBJ_FILE;
    } else if( allowLIB  &&  !stricmp( ext, ".lib" ) ) {
        type = TYPE_LIB_FILE;
    } else if( allowRC  &&  !stricmp( ext, ".rc" ) ) {
        type = TYPE_RC_FILE;
    } else if( allowRES  &&  !stricmp( ext, ".res" ) ) {
        type = TYPE_RES_FILE;
    } else if( allowRBJ  &&  !stricmp( ext, ".rbj" ) ) {
        type = TYPE_RBJ_FILE;
    } else if( allowRS  &&  !stricmp( ext, ".rs" ) ) {
        type = TYPE_RS_FILE;
    } else if( allowEXP  &&  !stricmp( ext, ".exp" ) ) {
        type = TYPE_EXP_FILE;
    } else {
        if( defaultType == TYPE_INVALID_FILE ) {
            Zoinks();
        }
        if( languageToForce == FORCE_NONE ) {
            Warning( "Unrecognized file type '%s' -- %s file assumed",
                     filename, defaultName );
        }
        type = defaultType;
    }
    if( type == TYPE_C_FILE && languageToForce == FORCE_CPP_COMPILE ) {
        type = TYPE_CPP_FILE;
    } else if( type == TYPE_CPP_FILE && languageToForce == FORCE_C_COMPILE ) {
        type = TYPE_C_FILE;
    }
    FreeMem( newfilename );
    return( type );
}


/*
 * Add a file to the list.  If its type is TYPE_DEFAULT_FILE, then file_type
 * will be called to determine the file's type.
 */
void AddFile( int type, const char *filename )
/********************************************/
{
    struct ListElem *   newElem;
    struct ListElem *   curElem;
    char *              newfilename;

    /*** Initialize a new ListElem structure ***/
    newElem = AllocMem( sizeof(struct ListElem) );
    newfilename = PathConvert( filename, '"' );
    newElem->filename = newfilename;
    if( type == TYPE_DEFAULT_FILE ) {
        newElem->type = file_type( newfilename );
    } else {
        newElem->type = type;
    }
    newElem->next = NULL;

    /*** Add it to the end of the list (to maintain order) ***/
    if( fileList == NULL ) {
        fileList = newElem;
    } else {
        curElem = fileList;
        while( curElem->next != NULL )  curElem = curElem->next;
        curElem->next = newElem;
    }
}


/*
 * Searches the file list for the next file of the given type.  If such a
 * file is found, a pointer to its filename is returned (this memory can
 * be freed by the caller) and its entry is removed from the file list.  If
 * no such file is found, NULL is returned.  Any additional types to match
 * may optionally be specified after type, in which case TYPE_INVALID_FILE
 * should be the last item passed.  If typeBuf is non-NULL, the file's type
 * is stored there.
 */
char *GetNextFile( int *typeBuf, int type, ... )
/**********************************************/
{
    int                 found = 0;
    struct ListElem *   curElem = fileList;
    struct ListElem *   prevElem = NULL;
    char *              filename;
    va_list             args;
    int                 curType;

    /*** Search the list for the file ***/
    while( curElem != NULL ) {
        if( curElem->type == type ) {   /* check the first type */
            found = 1;
            break;
        } else {                        /* check any other types */
            va_start( args, type );
            while( !found ) {
                curType = va_arg( args, int );
                if( curType == TYPE_INVALID_FILE )  break;
                if( curElem->type == curType ) {
                    found = 1;
                }
            }
            va_end( args );
            if( found )  break;
        }
        prevElem = curElem;
        curElem = curElem->next;
    }

    /*** Extract info if found, or return NULL if not ***/
    if( found ) {
        filename = curElem->filename;
        if( prevElem != NULL ) {
            prevElem->next = curElem->next;
        } else {
            fileList = curElem->next;
        }
        if( typeBuf != NULL )  *typeBuf = curElem->type;
        FreeMem( curElem );
    } else {
        filename = NULL;
    }
    return( filename );
}


/*
 * Set the default file type and default type description.
 */
void SetDefaultFile( int type, const char *name )
/***********************************************/
{
    defaultType = type;
    defaultName = name;
}


/*
 * Add the specified types to the list of those which are allowed.  The
 * last item must be TYPE_INVALID_FILE.  Use TYPE_DEFAULT_FILE to disallow
 * all file types.  Arguments are processed from left to right.
 */
void AllowTypeFile( int type, ... )
/*********************************/
{
    va_list             args;
    int                 alive = 1;
    int                 curType;
    int                 gotFirst = 0;

    va_start( args, type );
    while( alive ) {
        if( !gotFirst ) {
            curType = type;
            gotFirst = 1;
        } else {
            curType = va_arg( args, int );
        }
        switch( curType ) {
          case TYPE_DEFAULT_FILE:
            allowC = 0;
            allowCPP = 0;
            allowDEF = 0;
            allowOBJ = 0;
            allowLIB = 0;
            allowRC = 0;
            allowRES = 0;
            allowRBJ = 0;
            allowRS = 0;
            allowEXP = 0;
            break;
          case TYPE_C_FILE:
            allowC = 1;
            break;
          case TYPE_CPP_FILE:
            allowCPP = 1;
            break;
          case TYPE_DEF_FILE:
            allowDEF = 1;
            break;
          case TYPE_OBJ_FILE:
            allowOBJ = 1;
            break;
          case TYPE_LIB_FILE:
            allowLIB = 1;
            break;
          case TYPE_RC_FILE:
            allowRC = 1;
            break;
          case TYPE_RES_FILE:
            allowRES = 1;
            break;
          case TYPE_RBJ_FILE:
            allowRBJ = 1;
            break;
          case TYPE_RS_FILE:
            allowRS = 1;
            break;
          case TYPE_EXP_FILE:
            allowEXP = 1;
            break;
          case TYPE_INVALID_FILE:
            alive = 0;
            break;
          default:
            Zoinks();
        }
    }
    va_end( args );
}


/*
 * Force compilation using the specified language.
 */
void ForceLanguage( int language )
{
    languageToForce = language;
    if( language == FORCE_C_COMPILE ) {
        defaultType = TYPE_C_FILE;
    } else if( language == FORCE_CPP_COMPILE ) {
        defaultType = TYPE_CPP_FILE;
    }
}
