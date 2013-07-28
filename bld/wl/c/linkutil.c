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
* Description:  Utility routines for wlink.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <sys/types.h>
#include "linkstd.h"
#include "pcobj.h"
#include "newmem.h"
#include "alloc.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "linkutil.h"
#include "fileio.h"
#include "ideentry.h"
#include "ring.h"
#include "overlays.h"
#include "strtab.h"
#include "loadfile.h"
#include "permdata.h"
#include "objio.h"
#include "mapio.h"


void WriteNulls( f_handle file, unsigned_32 len, char *name )
/*******************************************************************/
/* copy nulls for uninitialized data */
{
    static unsigned NullArray[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for( ; len > sizeof( NullArray ); len -= sizeof( NullArray ) ) {
        QWrite( file, NullArray, sizeof( NullArray ), name );
    }
    if( len > 0 ) {
        QWrite( file, NullArray, len, name );
    }
}

void CheckErr( void )
/**************************/
{
    if( LinkState & ( LINK_ERROR | STOP_WORKING ) ) {
        WriteLibsUsed();
        Suicide();
    }
}

void CheckStop( void )
/***************************/
{
    if( LinkState & STOP_WORKING ) {
        Suicide();
    }
}

void LnkFatal( char *msg )
/********************************/
{
    LnkMsg( FTL+MSG_INTERNAL, "s", msg );
}

bool TestBit( byte *array, unsigned num )
/***********************************************/
/* return TRUE if the specified bit is on */
{
    byte        mask;

    mask = 1 << ( num % 8 );
    num /= 8;
    return( *( array + num ) & mask );
}

void ClearBit( byte *array, unsigned num )
/************************************************/
/* make sure a bit is turned off */
{
    byte        mask;

    mask = 1 << ( num % 8 );
    num /= 8;
    array += num;
    *array &= ~mask;
}

char *ChkStrDup( char *str )
/**************************/
{
    size_t      len;
    char        *copy;

    len = strlen( str ) + 1;
    _ChkAlloc( copy, len  );
    memcpy( copy, str, len );
    return( copy );
}

char *ChkToString( void *mem, unsigned len )
/******************************************/
{
    char        *str;

    _ChkAlloc( str, len + 1 );
    memcpy( str, mem, len );
    str[ len ] = '\0';
    return( str );
}

static void WalkList( node *list, void (*fn)( void * ) )
/******************************************************/
{
    for( ; list != NULL; list = list->next ) {
        fn( list );
    }
}

static void WalkModList( section *sect, void *rtn )
/*************************************************/
{
    CurrSect = sect;
    WalkList( (node *) sect->mods, rtn );
}

void WalkMods( void (*rtn)( mod_entry * ) )
/************************************************/
{
    ParmWalkAllSects( WalkModList, rtn );
    CurrSect = Root;
    WalkList( (node *)LibModules, (void (*)(void *))rtn );
}

static void WalkClass( class_entry *class, void (*rtn)( seg_leader * ) )
/********************************************************************/
{
    RingWalk( class->segs, (void (*)(void *))rtn );
}

void SectWalkClass( section *sect, void *rtn )
/***************************************************/
{
    class_entry         *class;

    CurrSect = sect;
    for( class = sect->classlist; class != NULL; class = class->next_class ) {
        WalkClass( class, rtn );
    }
}

void WalkLeaders( void (*rtn)( seg_leader * ) )
/****************************************************/
{
    ParmWalkAllSects( SectWalkClass, rtn );
}

seg_leader *FindSegment( section *sect, char *name )
/***************************************************/
/* NOTE: this doesn't work for overlays!
 *
 * sect != NULL then it works as FindFirstSegment
 * sect == NULL then it works as FindNextSegment
 *
 */
{
static seg_leader   *seg = NULL;
static class_entry  *class = NULL;

    if( sect != NULL ) {
        class = sect->classlist;
        seg = NULL;
    }
    for( ; class != NULL; class = class->next_class ) {
        while( (seg = RingStep( class->segs, seg )) != NULL ) {
            if( stricmp( seg->segname, name ) == 0 ) {
                return( seg );
            }
        }
    }
    return( seg );
}

void LinkList( void *in_head, void *newnode )
/*******************************************/
/* Link a new node into a linked list (new node goes at the end of the list) */
{
    node    **owner;

    owner = in_head;
    ((node *)newnode)->next = NULL;
    while( *owner != NULL ) {
        owner = (node **)&(*owner)->next;
    }
    *owner = newnode;
}

void FreeList( void *_curr )
/*********************************/
/* Free a list of nodes. */
{
    node        *curr = _curr;
    node        *next_node;

    while( curr ) {
        next_node = curr->next;
        _LnkFree( curr );
        curr = next_node;
    }
}

name_list *AddNameTable( char *name, unsigned len, bool is_mod, name_list **owner )
/*********************************************************************************/
{
    name_list   *imp;
    unsigned_32 off;
    unsigned_16 index;

    index = 1;
    off = 1;
    for( imp = *owner; imp != NULL; imp = imp->next ) {
        if( len == imp->len && memcmp( imp->name, name, len ) == 0 )
            break;
        off += imp->len + 1;
        ++index;
        owner = &imp->next;
    }
    if( imp == NULL ) {
        _PermAlloc( imp, sizeof( name_list ) );
        imp->next = NULL;
        imp->len = len;
        imp->name = AddSymbolStringTable( &PermStrings, name, len );
        imp->num = is_mod ? index : off;
        *owner = imp;
    }
    return( imp );
}

unsigned_16 binary_log( unsigned_16 value )
/************************************************/
// This calculates the binary log of value, truncating decimals.
{
    unsigned_16 log;

    if( value == 0 ) {
        return( 0 );
    }
    log = 15;
    for( ; ; ) {
        if( value & 0x8000 ) {  // done if high bit on
            break;
        }
        value <<= 1;            // shift left and decrease possible log.
        log--;
    }
    return( log );
}

unsigned_16 blog_32( unsigned_32 value )
/*********************************************/
// This calculates the binary log of a 32-bit value, truncating decimals.
{
    unsigned_16 log;

    if( value == 0 ) {
        return( 0 );
    }
    log = 31;
    for( ; ; ) {
        if( value & 0x80000000 ) {  // done if high bit on
            break;
        }
        value <<= 1;            // shift left and decrease possible log.
        log--;
    }
    return( log );
}

char *RemovePath( char *namestart, unsigned *len )
/**********************************************/
/* parse name as a filename, "removing" the path and the extension */
/* returns a pointer to the "base" of the filename, and a length without
 * the extension */
{
    char    *dotpoint;
    char    *string;
    char    ch;

    dotpoint = NULL;
    // ignore path & extension in module name.
    for( string = namestart; (ch = *string) != '\0'; string++ ) {
        if( ch == '.' ) {
            dotpoint = string;
            continue;
        }
        if( IS_PATH_SEP( ch ) ) {
            namestart = string + 1;
            dotpoint = NULL;
        }
    }
    if( dotpoint != NULL ) {
        *len = dotpoint - namestart;
    } else {
        *len = string - namestart;
    }
    return( namestart );
}

#define MAXDEPTH        ( sizeof( unsigned ) * 8 )

void VMemQSort( virt_mem base, unsigned n, unsigned width,
                        void (*swapfn)( virt_mem, virt_mem ),
                        int (*cmpfn)( virt_mem, virt_mem ) )
/***************************************************************/
// qsort stolen from clib, and suitably modified since we need to be able
// to swap parallel arrays.
{
    virt_mem    p1;
    virt_mem    p2;
    virt_mem    mid;
    int         comparison;
    int         last_non_equal_count;
    unsigned    i;
    unsigned    count;
    unsigned    sp;
    auto virt_mem base_stack[MAXDEPTH];
    auto unsigned n_stack[MAXDEPTH];

    sp = 0;
    for( ; ; ) {
        while( n > 1 ) {
            p1 = base + width;
            if( n == 2 ) {
                if( cmpfn( base, p1 ) > 0 ) {
                    swapfn( base, p1 );
                }
                break;
            } else {
                /* store mid element at base for pivot */
                /* this will speed up sorting of a sorted list */
                mid = base + ( n >> 1 ) * width;
                swapfn( base, mid );
                p2 = base;
                count = 0;
                last_non_equal_count = 0;
                for( i = 1; i < n; ++i ) {
                    comparison = cmpfn( p1, base );
                    if( comparison <= 0 ) {
                        p2 += width;
                        count++;
                        if( i != count ) {              /* p1 != p2 */
                            swapfn( p1, p2 );
                        }
                    }
                    if( comparison != 0 )
                        last_non_equal_count = count;
                    p1 += width;
                }
                /* special check to see if all values compared are equal */
                if( ( count == n-1 ) && ( last_non_equal_count == 0 ) )
                    break;
                if( count != 0 ) {  /* store pivot in right spot */
                    swapfn( base, p2 );
                }
#if 0
                qsort( base, count, size, cmp );
                qsort( p2 + size, n - count - 1, size, cmp );
#endif
                n = n - count - 1;          /* calc. size of right part */

                /* The pivot is at p2. It is in its final position.
                   There are count items to the left of the pivot.
                   There are n items to the right of the pivot.
                */

                if( count != last_non_equal_count ) {   /* 18-jul-90 */
                    /*
                       There are last_non_equal_count+1 items to the left
                       of the pivot that still need to be checked.
                       There are (count - (last_non_equal_count+1)) items
                       immediately to the left of the pivot that are
                       equal to the pivot. They are in their final position.
                    */
                    count = last_non_equal_count + 1;
                }
                if( count < n ) {           /* if left part is shorter */
                    base_stack[sp] = p2 + width;  /* - stack right part */
                    n_stack[sp] = n;
                    n = count;
                } else {                    /* right part is shorter */
                    base_stack[sp] = base;  /* - stack left part */
                    n_stack[sp] = count;
                    base = p2 + width;
                }
                ++sp;
            }
        }
        if( sp == 0 )
            break;
        --sp;
        base = base_stack[sp];
        n    = n_stack[sp];
    }
}

static void *SpawnStack;

int Spawn( void (*fn)( void ) )
/******************************/
{
    void    *save_env;
    jmp_buf env;
    int     status;

    save_env = SpawnStack;
    SpawnStack = env;
    status = setjmp( env );
    if( status == 0 ) {
        (*fn)();
    }
    SpawnStack = save_env;  /* unwind */
    return( status );
}

void Suicide( void )
/******************/
{
    if( SpawnStack != NULL ) {
        longjmp( SpawnStack, 1 );
    }
}

char *GetPathElement( char *path_list, char *end, char **path )
/*************************************************************/
{
    bool        is_blank;
    char        c;

    is_blank = TRUE;
    while( path_list != end ) {
        c = *path_list++;
        if( IS_INCL_SEP( c ) ) {
            if( !is_blank ) {
                break;
            }
        } else if( !is_blank ) {
            *(*path)++ = c;
        } else if( c != ' ' ) {
            is_blank = FALSE;
            *(*path)++ = c;
        }
    }
    return( path_list );
}

void InitEnvVars( void )
/**********************/
{
    char        *path_list;
    size_t      len;
    char        *p;
    char        *end;

    if( ExePath == NULL ) {
#if defined( __QNX__ )
        path_list = "/usr/watcom";
#else
        path_list = GetEnvString( "PATH" );
#endif
        if( path_list != NULL && *path_list != '\0' ) {
            len = strlen( path_list );
            _ChkAlloc( ExePath, len + 1 );
            p = ExePath;
            end = path_list + len;
            do {
                if( p != ExePath )
                    *p++ = PATH_LIST_SEP;
                path_list = GetPathElement( path_list, end, &p );
            } while( path_list != end );
            *p = '\0';
        } else {
            _ChkAlloc( ExePath, 1 );
            *ExePath = '\0';
        }
    }
    if( LibPath == NULL ) {
        path_list = GetEnvString("LIB");
        if( path_list != NULL && *path_list != '\0' ) {
            len = strlen( path_list );
            _ChkAlloc( LibPath, len + 1 );
            p = LibPath;
            end = path_list + len;
            do {
                if( p != LibPath )
                    *p++ = PATH_LIST_SEP;
                path_list = GetPathElement( path_list, end, &p );
            } while( path_list != end );
            *p = '\0';
        } else {
            _ChkAlloc( LibPath, 1 );
            *LibPath = '\0';
        }
    }
}

void FiniEnvVars( void )
/**********************/
{
    if( ExePath != NULL ) {
        _LnkFree( ExePath );
        ExePath = NULL;
    }
    if( LibPath != NULL ) {
        _LnkFree( LibPath );
        LibPath = NULL;
    }
}

f_handle FindPath( char *name )
/*****************************/
{
    char        *path_list;
    f_handle    file;
    char        fullpath[PATH_MAX];

    file = QObjOpen( name );
    if( file == NIL_FHANDLE && ExePath != NULL ) {
        path_list = ExePath;
        while( *path_list != '\0' ) {
            strcpy( MakePath( fullpath, &path_list ), name );
            file = QObjOpen( fullpath );
            if( file != NIL_FHANDLE ) {
                return( file );
            }
        }
    }
    return( file );
}

group_entry *FindGroup( segment seg )
/******************************************/
{
    group_entry *group;

    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->grp_addr.seg == seg ) {
            break;
        }
    }
    return( group );
}

offset FindLinearAddr( targ_addr *addr )
/*********************************************/
{
    group_entry *group;

    group = FindGroup( addr->seg );
    if( group != NULL ) {
        return( addr->off + ( group->linear - group->grp_addr.off ) );
    }
    return( addr->off );
}

offset FindLinearAddr2( targ_addr *addr )
/*********************************************/
{
    group_entry *group;

    group = FindGroup( addr->seg );
    if( group != NULL ) {
        return( addr->off + group->linear + FmtData.base );
    }
    return( addr->off );
}
