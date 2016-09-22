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
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "wio.h"
#include "bool.h"
#include "demangle.h"           /* from lib_misc project */
#include "dlltool.h"
#include "error.h"
#include "fuzzy.h"
#include "hash.h"
#include "memory.h"
#include "orl.h"                /* from riscdev project */

#define HASH_TABLE_SIZE         2111
#define MAX_SYMBOL_LEN          512

#define MATCH_MODE_EXACT                        1
#define MATCH_MODE_UNDERBAR_SYMBOL              2
#define MATCH_MODE_SYMBOL_UNDERBAR              3
#define MATCH_MODE_UNDERBAR_SYMBOL_AT_NUMBER    4


/*
 * Types.
 */
typedef struct _ListElem {
    struct _ListElem *  next;
    char                buff[1];
} ListElem;

typedef struct _MatchingInfo {
    char *              basename;
    int                 findmode;
    ListElem *          found;
} MatchingInfo;


/*
 * Static data.
 */
static HashTable        hashtable = NULL;
static ListElem *       bufflist = NULL;
static void *           dllhandle = NULL;


/*
 * Determine if a symbol's name is mangled.
 */
static int is_name_mangled( const char *symbol )
/**********************************************/
{
    if( __is_mangled( symbol, strlen( symbol ) ) ) {
        return( 1 );
    }
    /* what about MS mangling? */

    return( 0 );
}


/*
 * Hash comparison callback function.
 */
static int hash_compare( const void *item1, const void *item2 )
/*************************************************************/
{
    return( !strcmp( (const char*)item1, (const char*)item2 ) );
}


/*
 * Hash a symbol name.
 */
static unsigned hash_symbol_name( const void *symbol )
/****************************************************/
{
    char *              namecopy;
    char *              namestart;
    char *              p;
    unsigned *          s;
    unsigned            len;
    unsigned            mask;
    unsigned            c;
    unsigned            g;
    unsigned            h;
    unsigned const      namecmpmask[5] = {
        0x00000000, 0x000000FF, 0x0000FFFF, 0x00FFFFFF, 0xFFFFFFFF
    };

    /*
     * We want 'foo', '_foo', 'foo_', '_foo@8', and so on all to hash to
     * the same value, so strip off stuff from each end if necessary to
     * get the root name.
     */
    namecopy = DupStrMem( symbol );
    namestart = namecopy;
    while( *namestart == '_' )  namestart++;
    len = (unsigned)strlen( namestart );
    if( len > 0 ) {
        p = namestart + len - 1;
        while( p >= namestart  &&  ( isdigit(*p) || *p=='@' || *p=='_' ) ) {
            *p = '\0';
            p--;
        }
    }

    /*
     * Ok, now do the actual hashing.
     */
    s = (unsigned *)namestart;
    len = (unsigned)strlen( namestart );
    h = len;
    c = len;
    if( len > sizeof( unsigned ) ) {
        do {
            c ^= *s;
            h = ( h << 4 ) + c;
            g = h & ~0x00FFFFFF;
            h ^= g;
            h ^= g >> (4+4+4+4+4);
            ++s;
            len -= sizeof( unsigned );
        } while( len > sizeof( unsigned ) );
    }
    mask = namecmpmask[ len ];
    c ^= *s & mask;
    h = ( h << 4 ) + c;
    g = h & ~0x00FFFFFF;
    h ^= g;
    h ^= g >> (4+4+4+4+4);
    g = h & ~0x0FFF;
    h ^= g;
    h ^= g >> (4+4+4);
    h ^= h >> (2+4);

    FreeMem( namecopy );
    return( h );
}


/*
 * Used by ORL.
 */
static void *obj_read( void *hdl, size_t len )
/********************************************/
{
    ListElem *          newelem;

    newelem = AllocMem( sizeof( ListElem ) + len - 1 );
    newelem->next = bufflist;
    bufflist = newelem;
    if( (unsigned)read( (int)(pointer_int)hdl, newelem->buff, (unsigned)len ) != (unsigned)len ) {
        FreeMem( newelem );
        return( NULL );
    }
    return( newelem->buff );
}


/*
 * Used by ORL.
 */
static long obj_seek( void *hdl, long pos, int where )
/****************************************************/
{
    return( lseek( (int)(pointer_int)hdl, pos, where ) );
}


/*
 * Free a list of ListElem's.
 */
static void free_list( ListElem *liststart )
/******************************************/
{
    ListElem *          curelem;
    ListElem *          nextelem;

    curelem = liststart;
    while( curelem != NULL ) {
        nextelem = curelem->next;
        FreeMem( curelem );
        curelem = nextelem;
    }
    bufflist = NULL;
}


/*
 * Add one more symbol.
 */
static orl_return do_orl_symbol( orl_symbol_handle o_symbol )
/***********************************************************/
{
    char *              name;
    char *              namecopy;

    name = ORLSymbolGetName( o_symbol );

    if( !is_name_mangled( name ) ) {
        namecopy = DupStrMem( name );
        InsertHash( hashtable, namecopy );
    }

    return( ORL_OKAY );
}


/*
 * Collect all external symbols from an object file.  Returns 0 on error.
 */
static int handle_obj_file( const char *filename, orl_handle o_hnd )
/******************************************************************/
{
    orl_file_handle     o_fhnd;
    orl_file_format     o_format;
    orl_file_type       o_filetype;
    orl_sec_handle      o_symtab;
    orl_return          o_rc;
    int                 fileh;

    /*** Make ORL interested in the file ***/
    fileh = open( filename, O_BINARY | O_RDONLY );
    if( fileh == -1 ) {
        return( 0 );
    }
    o_format = ORLFileIdentify( o_hnd, (void *)(pointer_int)fileh );
    if( o_format == ORL_UNRECOGNIZED_FORMAT ) {
        close( fileh );
        return( 0 );
    }
    o_fhnd = ORLFileInit( o_hnd, (void *)(pointer_int)fileh, o_format );
    if( o_fhnd == NULL ) {
        close( fileh );
        return( 0 );
    }
    o_filetype = ORLFileGetType( o_fhnd );
    if( o_filetype != ORL_FILE_TYPE_OBJECT ) {
        close( fileh );
        return( 0 );
    }

    /*** Scan the file's symbol table ***/
    o_symtab = ORLFileGetSymbolTable( o_fhnd );
    if( o_symtab == NULL ) {
        close( fileh );
        return( 0 );
    }
    o_rc = ORLSymbolSecScan( o_symtab, &do_orl_symbol );
    if( o_rc != ORL_OKAY ) {
        close( fileh );
        return( 0 );
    }
    o_rc = ORLFileFini( o_fhnd );
    if( o_rc != ORL_OKAY ) {
        close( fileh );
        return( 0 );
    }

    close( fileh );
    return( 1 );
}


/*
 * Returns the actual name of 'filename', searching 'libpaths' if necessary.
 * The caller is responsible for freeing the returned memory.  If the file
 * cannot be found, NULL is returned.
 */
static char *find_file( const char *filename, const char *libpaths[] )
/********************************************************************/
{
    int                 rc;
    unsigned            count;
    char *              tryme;
    size_t              len;
    const char *        p;
    bool                hasbackslash;

    /*** We might not have to go searching for it ***/
    rc = access( filename, F_OK );
    if( rc == 0 ) {
        return( DupStrMem( filename ) );
    }

    /*** Not found, so check any directories in 'libpaths' ***/
    if( libpaths != NULL ) {
        for( count=0; libpaths[count]!=NULL; count++ ) {
            /*** Determine if we need to add a backslash to the path ***/
            len = strlen( libpaths[count] );
            if( len == 0 )  Zoinks();
            p = libpaths[count] + len - 1;
            if( *p == '\\' ) {
                hasbackslash = true;
            } else {
                hasbackslash = false;
            }

            /*** See if the file exists here ***/
            len = strlen( libpaths[count] )  +
                  ( hasbackslash ? 0 : 1 )  +
                  strlen( filename ) + 1;
            tryme = AllocMem( len );
            sprintf( tryme, "%s%s%s", libpaths[count],
                     hasbackslash ? "" : "\\", filename );
            rc = access( tryme, F_OK );
            if( rc == 0 ) {
                return( tryme );
            } else {
                FreeMem( tryme );
            }
        }
    }

    return( NULL );
}


/*
 * Collect all external symbols from a library file.  Returns 0 on error.
 */
static int handle_lib_file( const char *filename, const char *libpaths[] )
/************************************************************************/
{
    char *              realpath;
    size_t              len;
    char *              options;
    const size_t        leeway = 10;    /* for "-tl" and such */
    int                 rc;

    realpath = find_file( filename, libpaths );
    if( realpath == NULL ) {
        Warning( "Cannot locate '%s'; skipping -- fuzzy name matching may not work",
                 filename );
        return( 0 );
    }

    len = strlen( realpath );
    options = AllocMem( leeway + len + 1 );
    sprintf( options, "-tl %s", realpath );
    rc = RunDllTool( dllhandle, options );
    FreeMem( options );
    if( rc ) {
        return( 1 );
    } else {
        return( 0 );
    }
}


/*
 * Process output from the WLIB DLL.
 */
static int wlib_output( const char *text )
/****************************************/
{
    bool                badness = false;

    if( ( strncmp( text, "Error!", 6 ) == 0 ) || ( strncmp( text, "Warning!", 8 ) == 0 ) ) {
        badness = true;
    }

    if( !badness ) {
        if( strchr( text, ' ' ) == NULL ) {
            InsertHash( hashtable, (void*)DupStrMem( (char*)text ) );
        }
        return( 1 );
    } else {
        Warning( "Message from WLIB DLL: %s", text );
        return( 0 );
    }
}


/*
 * WLIB DLL output callback functions.
 */
static IDEBool __stdcall print_message_crlf( IDECBHdl hdl, const char *text )
{
    hdl = hdl;
    return( (IDEBool)wlib_output( text ) );
}
static IDEBool __stdcall print_with_info2( IDECBHdl hdl, IDEMsgInfo2 *info )
{
    hdl = hdl;
    return( (IDEBool)wlib_output( info->msg ) );
}
static IDEBool __stdcall print_with_info( IDECBHdl hdl, IDEMsgInfo *info )
{
    hdl = hdl;
    return( (IDEBool)wlib_output( info->msg ) );
}


/*
 * Given a set of object and library files, prepare to use fuzzy linking.
 * If symbols cannot be collected from a file, 'callback' will be called
 * with the name of the offending file.
 */
void InitFuzzy( const char *objs[], const char *libs[],
                const char *libpaths[], FuzzyInitCallback callback )
/******************************************************************/
{
    unsigned            count;
    orl_handle          o_hnd;
    int                 rc = 1;
    DllToolCallbacks    dllcallbacks;
    OrlSetFuncs( orl_cli_funcs, obj_read, obj_seek, AllocMem, FreeMem );

    /*** Create a hash table ***/
    hashtable = InitHash( HASH_TABLE_SIZE, hash_symbol_name, hash_compare );

    /*** Collect all external symbols from the specified object files ***/
    if( objs != NULL ) {
        /*** Initialize ORL ***/
        o_hnd = ORLInit( &orl_cli_funcs );
        if( o_hnd == NULL ) {
            FatalError( "Got NULL orl_handle." );
        }

        /*** Scan the file(s) ***/
        for( count=0; objs[count]!=NULL; count++ ) {
            if( !handle_obj_file( objs[count], o_hnd ) ) {
                rc = (*callback)( objs[count] );
                if( !rc )  break;
            }
        }

        /*** Tell ORL to go away ***/
        if( ORLFini( o_hnd ) != ORL_OKAY ) {
            Warning( "Error calling ORLFini." );
        }
        free_list( bufflist );
    }
    if( !rc )  return;

    /*** Collect all external symbols from the specified library files ***/
    if( libs != NULL ) {
        /*** Load the WLIB DLL ***/
        dllcallbacks.printmessage = NULL;
        dllcallbacks.printmessageCRLF = print_message_crlf;
        dllcallbacks.printwithinfo2 = print_with_info2;
        dllcallbacks.printwithinfo = print_with_info;
        dllcallbacks.cookie = NULL;
        dllhandle = InitDllTool( DLLTOOL_WLIB, &dllcallbacks );
        if( dllhandle == NULL ) {
            Warning( "Cannot load WLIB DLL -- fuzzy name matching may not work" );
        } else {
            /*** Scan the file(s) ***/
            for( count=0; libs[count]!=NULL; count++ ) {
                if( !handle_lib_file( libs[count], libpaths ) ) {
                    rc = (*callback)( libs[count] );
                    if( !rc )  break;
                }
            }
            FiniDllTool( dllhandle );
        }
    }
}


/*
 * Disable the fuzzy linking module, freeing any allocated resources.
 */
void FiniFuzzy( void )
/********************/
{
    if( hashtable == NULL )  Zoinks();
    FiniHash( hashtable, 1 );
    hashtable = NULL;
}


/*
 * Check a hash table element during a walk of a hash table bucket.
 */
static int matching_callback( const void *name_, void *info_ )
/************************************************************/
{
    const char          *name = name_;
    MatchingInfo        *info = info_;
    char                matchstr[MAX_SYMBOL_LEN+1];
    bool                addit = false;
    const char *        p;
    ListElem *          newelem;
    ListElem *          nextelem;

    if( name == NULL )  Zoinks();
    if( strlen(info->basename)+2 > MAX_SYMBOL_LEN )  Zoinks();

    /*** Try to match this symbol ***/
    switch( info->findmode ) {
        case MATCH_MODE_EXACT:
            if( !strcmp( name, info->basename ) ) {
                addit = true;
            }
            break;
        case MATCH_MODE_UNDERBAR_SYMBOL:
            sprintf( matchstr, "_%s", info->basename );
            if( !strcmp( name, matchstr ) ) {
                addit = true;
            }
            break;
        case MATCH_MODE_SYMBOL_UNDERBAR:
            sprintf( matchstr, "%s_", info->basename );
            if( !strcmp( name, matchstr ) ) {
                addit = true;
            }
            break;
        case MATCH_MODE_UNDERBAR_SYMBOL_AT_NUMBER:
            sprintf( matchstr, "_%s@", info->basename );
            if( strstr( name, matchstr ) == name ) {
                p = name + strlen( matchstr );
                while( isdigit( *p ) ) {
                    p++;
                }
                if( *p == '\0' ) {
                    addit = true;
                }
            }
            break;
        default:
            Zoinks();
    }

    /*** If it matches, add it to the found list ***/
    if( addit ) {
        nextelem = info->found;
        newelem = AllocMem( sizeof( ListElem ) + strlen( name ) );
        strcpy( newelem->buff, name );
        newelem->next = nextelem;
        info->found = newelem;

        /*** Keep searching only if multiple matches are possible ***/
        if( info->findmode == MATCH_MODE_UNDERBAR_SYMBOL_AT_NUMBER ) {
            return( 1 );
        } else {
            return( 0 );
        }
    } else {
        return( 1 );
    }
}


/*
 * Perform fuzzy matching on the given symbol.  If exactly one match is
 * found, a pointer to the real symbol name (in ' quotes) is returned;
 * the caller is responsible for freeing this memory.  In all other cases,
 * NULL is returned.
 */
char *MatchFuzzy( const char *entryname )
/***************************************/
{
    MatchingInfo        info;
    char *              retval;
    char *              tmp;
    unsigned            numfound = 0;
    ListElem *          curelem;

    if( hashtable == NULL )  Zoinks();

    /*** Strip quotes from 'entryname' ***/
    tmp = DupStrMem( entryname );
    if( *tmp == '\'' ) {
        info.basename = DupStrMem( tmp + 1 );
        info.basename[ strlen(info.basename) - 1 ] = '\0';
        FreeMem( tmp );
    } else {
        info.basename = tmp;
    }

    /*** Check for an exact match ***/
    info.findmode = MATCH_MODE_EXACT;
    info.found = NULL;
    WalkBucketHash( hashtable, info.basename, matching_callback, &info );

    /*** Check for _symbol ***/
    if( info.found == NULL ) {
        info.findmode = MATCH_MODE_UNDERBAR_SYMBOL;
        WalkBucketHash( hashtable, info.basename, matching_callback, &info );
    }

    /*** Check for symbol_ ***/
    if( info.found == NULL ) {
        info.findmode = MATCH_MODE_SYMBOL_UNDERBAR;
        WalkBucketHash( hashtable, info.basename, matching_callback, &info );
    }

    /*** Check for _symbol@number ***/
    if( info.found == NULL ) {
        info.findmode = MATCH_MODE_UNDERBAR_SYMBOL_AT_NUMBER;
        WalkBucketHash( hashtable, info.basename, matching_callback, &info );
    }

    /*** Count how many matches we got ***/
    numfound = 0;
    curelem = info.found;
    while( curelem != NULL ) {
        numfound++;
        curelem = curelem->next;
    }

    /*** If there was more than one match, complain ***/
    if( numfound > 1 ) {
        Warning( "Symbol with entry name %s has %u matching internal names:",
                 entryname, numfound );
        curelem = info.found;
        while( curelem != NULL ) {
            Information( "%s", curelem->buff );
            curelem = curelem->next;
        }
        Information( "Symbol will be ignored!" );
    }

    /*** Return an appropriate value ***/
    if( numfound == 1 ) {
        retval = DupQuoteStrMem( info.found->buff, '\'' );
    } else {
        retval = NULL;
    }
    free_list( info.found );
    return( retval );
}
