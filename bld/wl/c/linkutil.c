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


/*
   LINKUTILS : utility routines for wlink

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "linkstd.h"
#include "pcobj.h"
#include "newmem.h"
#include "alloc.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "linkutil.h"
#include "fileio.h"
#include "ring.h"
#include "overlays.h"
#include "strtab.h"
#include "loadfile.h"
#include "permdata.h"
#include "mapio.h"
#include "wressetr.h"   // from wres project

#if _LINKER != _WATFOR77
static int ResWrite( int dummy, const void *buff, int size )
/**********************************************************/
/* redirect wres write to writeload */
{
    dummy = dummy;
    DbgAssert(dummy == Root->outfile->handle);
    WriteLoad( (void *) buff, size);
    return size;
}

extern int WLinkItself;
static long ResSeek( int handle, long position, int where )
/*********************************************************/
/* Workaround wres bug */
{
    if( where == SEEK_SET && handle == WLinkItself) {
        return(QLSeek( handle, position + FileShift, where, NULL ) - FileShift);
    } else {
        return( QLSeek( handle, position, where, NULL ) );
    }
}

static int ResClose( int handle )
/*******************************/
{
    return close( handle );
}

static int ResRead( int handle, char * buffer, int len )
/******************************************************/
{
    return( QRead( handle, buffer, len, NULL ) );
}

static long ResPos( int handle )
/******************************/
{
    return QPos( handle );
}

WResSetRtns(ResOpen,ResClose,ResRead,ResWrite,ResSeek,ResPos,ChkLAlloc,LFree);
#endif

#if _LINKER != _DLLHOST
extern void WriteStdOut( char *str )
/**********************************/
{
    QWrite( STDOUT_HANDLE, str, strlen( str ), NULL );
}

extern void WriteNLStdOut( void )
/*******************************/
{
    QWriteNL( STDOUT_HANDLE, NULL );
}

extern void WriteInfoStdOut( char *str, unsigned level, char *sym )
/*****************************************************************/
{
    level = level;
    sym = sym;
    WriteStdOut( str );
    WriteNLStdOut();
}

extern char * GetEnvString( char *envname )
/*****************************************/
{
    return getenv( envname );
}

extern bool GetAddtlCommand( unsigned cmd, char *buf )
/****************************************************/
{
    cmd = cmd;
    buf = buf;
    return FALSE;
}

extern bool IsStdOutConsole( void )
/*********************************/
{
    return QIsDevice( STDOUT_HANDLE );
}
#endif

extern void WriteNulls( f_handle file, unsigned_32 len, char * name )
/*******************************************************************/
/* copy nulls for uninitialized data */
{
    static unsigned NullArray[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for(; len > sizeof( NullArray ); len -= sizeof( NullArray ) ) {
        QWrite( file, NullArray, sizeof( NullArray ), name );
    }
    if( len > 0 ) {
        QWrite( file, NullArray, len, name );
    }
}

extern void CheckErr( void )
/**************************/
{
    if( LinkState & (LINK_ERROR|STOP_WORKING) ) {
        WriteLibsUsed();
        Suicide();
    }
}

extern void CheckStop( void )
/***************************/
{
    if( LinkState & STOP_WORKING ) {
        Suicide();
    }
}

extern void LnkFatal( char * msg )
/********************************/
{
    LnkMsg( FTL+MSG_INTERNAL, "s", msg );
}

extern bool TestBit( byte * array, unsigned num )
/***********************************************/
/* return TRUE if the specified bit is on */
{
    byte        mask;

    mask = 1 << (num % 8);
    num /= 8;
    return( *(array + num) & mask );
}

extern void ClearBit( byte * array, unsigned num )
/************************************************/
/* make sure a bit is turned off */
{
    byte        mask;

    mask = 1 << (num % 8);
    num /= 8;
    array += num;
    *array &= ~mask;
}

extern char * ChkStrDup( char * str )
/***********************************/
{
    size_t      len;
    char *      copy;

    len = strlen( str ) + 1;
    _ChkAlloc( copy, len  );
    memcpy( copy, str, len );
    return copy;
}

extern void * ChkMemDup( void * mem, unsigned len  )
/**************************************************/
{
    char *      copy;

    _ChkAlloc( copy, len  );
    memcpy( copy, mem, len );
    return copy;
}

static void WalkModList( section *sect, void *rtn )
/*************************************************/
{
    CurrSect = sect;
    WalkList( (node *) sect->mods, rtn );
}

extern void WalkMods( void (*rtn)( mod_entry * ) )
/************************************************/
{
    CurrSect = Root;
    WalkList( (node *) Root->mods, rtn );
    ParmWalkOvl( WalkModList, rtn );
    CurrSect = Root;
    WalkList( (node *) LibModules, rtn );
}

static void WalkClass( class_entry *class, void (*rtn)(seg_leader *) )
/********************************************************************/
{
    RingWalk( class->segs, (void (*)(void *))rtn );
}

extern void SectWalkClass( section *sect, void (*rtn)(seg_leader *) )
/*******************************************************************/
{
    class_entry *       class;

    CurrSect = sect;
    for( class = sect->classlist; class != NULL; class = class->next_class ) {
        WalkClass( class, rtn );
    }
}

extern void WalkLeaders( void (*rtn)( seg_leader * ) )
/****************************************************/
{
    SectWalkClass( Root, rtn );
    ParmWalkOvl( SectWalkClass, rtn );
}

static void WalkList( node *list, void (*fn)( void * ) )
/******************************************************/
{
    while( list != NULL ) {
        fn( list );
        list = list->next;
    }
}

static bool CmpSegName( seg_leader *leader, char *name )
/******************************************************/
{
    return stricmp( leader->segname, name ) == 0;
}

extern seg_leader * FindSegment( char *name )
/*******************************************/
/* NOTE: this doesn't work for overlays! */
{
    class_entry *class;
    seg_leader * seg;

    seg = NULL;
    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        seg = RingLookup( class->segs, CmpSegName, name );
        if( seg != NULL ) break;
    }
    return seg;
}

extern void LinkList( void **in_head, void *newnode )
/***************************************************/
/* Link a new node into a linked list (new node goes at the end of the list) */
{
    node **     owner;

    owner = in_head;
    ((node *)newnode)->next = NULL;
    while( *owner != NULL ) {
        owner = &(*owner)->next;
    }
    *owner = newnode;
}

extern void FreeList( node *curr )
/*******************************/
/* Free a list of nodes. */
{
    node                *next_node;

    while( curr ) {
        next_node = curr->next;
        _LnkFree( curr );
        curr = next_node;
    }
}

extern name_list * AddNameTable( char *name, unsigned len, bool is_mod,
                                                        name_list **owner )
/*************************************************************************/
{
    name_list * imp;
    unsigned_16 off;
    unsigned_16 index;

    index = 1;
    off = 1;
    for( ;; ) {
        imp = *owner;
        if( imp == NULL ) {
            _PermAlloc( imp, sizeof(name_list) );
            imp->next = NULL;
            imp->len = len;
            ReserveStringTable( &PermStrings, len + 1 );
            imp->name = AddStringTable( &PermStrings, name, len );
            CharStringTable( &PermStrings, '\0' );
            imp->num = is_mod ? index : off;
            *owner = imp;
            break;
        }
        if( len == imp->len && memcmp(imp->name,name,len) == 0 ) break;
        off += imp->len + 1;
        owner = &imp->next;
        ++index;
    }
    return imp;
}

extern unsigned_16 binary_log( unsigned_16 value )
/************************************************/
// This calculates the binary log of value, truncating decimals.
{
    unsigned_16 log;

    if( value == 0 ) {
        return( 0 );
    }
    log = 15;
    for(;;) {
        if( value & 0x8000 ) {  // done if high bit on
            break;
        }
        value <<= 1;            // shift left and decrease possible log.
        log--;
    }
    return( log );
}

extern unsigned_16 blog_32( unsigned_32 value )
/*********************************************/
// This calculates the binary log of a 32-bit value, truncating decimals.
{
    unsigned_16 log;

    if( value == 0 ) {
        return( 0 );
    }
    log = 31;
    for(;;) {
        if( value & 0x80000000 ) {  // done if high bit on
            break;
        }
        value <<= 1;            // shift left and decrease possible log.
        log--;
    }
    return( log );
}

extern char * RemovePath( char *name, int *len )
/**********************************************/
/* parse name as a filename, "removing" the path and the extension */
/* returns a pointer to the "base" of the filename, and a length without
 * the extension */
{
    char *  dotpoint;
    char *  namestart;
    char *  string;
    char    ch;

    dotpoint = NULL;
    string = namestart = name;
    while( *string != '\0' ) {    // ignore path & extension in module name.
        ch = *string;
        if( ch == '.' ) {
            dotpoint = string;
        }
        if( IS_PATH_SEP( ch ) ) {
            namestart = string + 1;
            dotpoint = NULL;
        }
        string++;
    }
    if( dotpoint != NULL ) {
        *len = dotpoint - namestart;
    } else {
        *len = string - namestart;
    }
    return( namestart );
}

#define MAXDEPTH        (sizeof(unsigned)*8)

extern void VMemQSort( virt_mem base, unsigned n, unsigned width,
                        void (*swapfn)(virt_mem, virt_mem),
                        int (*cmpfn)(virt_mem, virt_mem) )
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
    for(;;) {
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
                mid = base + (n >> 1) * width;
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
                    if( comparison != 0 ) last_non_equal_count = count;
                    p1 += width;
                }
                /* special check to see if all values compared are equal */
                if( count == n-1  &&  last_non_equal_count == 0 ) break;
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
        if( sp == 0 ) break;
        --sp;
        base = base_stack[sp];
        n    = n_stack[sp];
    }
}

#if _LINKER == _WATFOR77
extern  void    Suicide(void);
#else
static void *SpawnStack;

extern int Spawn( void (*fn)() )
/******************************/
{
    void *  save_env;
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

extern void Suicide( void )
/*************************/
{
    if( SpawnStack != NULL ) {
        longjmp( SpawnStack, 1 );
    }
}
#endif

extern f_handle SearchPath( char *name )
/**************************************/
{
    char *      path;
    f_handle    file;
    char        fullpath[PATH_MAX];

    file = QObjOpen( name );
    if( file != NIL_HANDLE ) {
        return( file );
    }
#if _OS == _QNX
    path = "/usr/watcom";
#else
    path = GetEnvString( "PATH" );
#endif
    if( path != NULL ) {
        while( QMakeFileName( &path, name, fullpath ) ) {
            file = QObjOpen( fullpath );
            if( file != NIL_HANDLE ) {
                return( file );
            }
        }
    }
    return( NIL_HANDLE );
}

extern group_entry *FindGroup( segment seg )
/******************************************/
{
    group_entry *group;

    group = Groups;
    for( ;; ) {
        if( group == NULL ) break;
        if( group->grp_addr.seg == seg ) break;
        group = group->next_group;
    }
    return group;
}

extern offset FindLinearAddr( targ_addr *addr )
/*********************************************/
{
    group_entry *group;

    group = FindGroup( addr->seg );
    if( group != NULL ) {
        return addr->off + (group->linear - group->grp_addr.off);
    }
    return addr->off;
}

extern offset FindLinearAddr2( targ_addr *addr )
/*********************************************/
{
    group_entry *group;

    group = FindGroup( addr->seg );
    if( group != NULL ) {
        return addr->off + group->linear + FmtData.base;
    }
    return addr->off;
}

