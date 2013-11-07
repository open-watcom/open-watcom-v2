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


#include "linkstd.h"
#include "loados2.h"
#include "alloc.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "command.h"
#include <string.h>
#include <stdarg.h>
#include "impexp.h"     // For entry_export, etc.
#include "fileio.h"     // For NIL_HANDLE, etc.

extern void             LnkMemInit( void );
extern void             LnkMemFini( void );
extern void             InitCmdFile( void );
extern f_handle         FindPath( char * );
extern int              Spawn(void (*)() );
extern void             BurnSystemList( void );
extern void             Burn( void );
extern void             FreeFormatStuff( void );
extern section          *NewSection( void );
extern int              Msg_Fini();
extern int              Msg_Init( char * );

#pragma off( unreferenced );
bool        CleanCachedHandles( void ) {

    return( FALSE );
}

unsigned_16     AddImpNameTab( char *a, unsigned b, bool c ) {

    return( 0 );
}

void            AddLibPaths( char *a, int b, bool c ) {}
void            AddToExportList( entry_export *a ) {}
void            BufWrite(char *a, int b ) {}
void            CheckLibTrace( file_list * lib ) {}
void            CheckTraces( void ) {}
void            DBIInit( section *a, bool b ) {}
void            AddEnvPaths( char *a ) {}

symbol *        DefImpSymbol( char *a, int b ) {

    return( NULL );
}

void            FillOutFilePtrs( void ) {}
void            FreeImpNameTab(void) {}
void            FreeExportList(void) {}

extern void FreeList( void *parm )
/*******************************/
/* Free a list of nodes. */
{
    node *      curr;
    node *      next;

    for( curr = parm; curr != NULL; curr = next ) {
        next = curr->next;
        _LnkFree( curr );
    }
}

extern void FreeSegFlags( seg_flags * curr )
/******************************************/
{
    seg_flags * next;

    for( ; curr != NULL; curr = next ) {
        next = curr->next;
        _LnkFree( curr->name );
        _LnkFree( curr );
    }
}

bool            IsCodeClass( char *a, unsigned b ) {

    return( TRUE );
}

symbol *        LenFindISymbol( char *a, int b ) {

    return( NULL );
}

extern void LinkList( void **in_head, void *newnode )
/***************************************************/
/* Link a new node into a linked list (new node goes at the end of the list) */
{
    node                **owner;

    owner = in_head;
    ((node *)newnode)->next = NULL;
    while( *owner != NULL ) {
        owner = &(*owner)->next;
    }
    *owner = newnode;
}

void            MakeSymAlias( char *a, int b, char *c, int d ) {}
void            NumberSections() {}

static bool AddSymTrace( void )
/*****************************/
{
    return( TRUE );
}

extern bool ProcSymTrace( void )
/******************************/
{
    bool ret;

    LinkFlags |= TRACE_FLAG;
    ret = ProcArgList( &AddSymTrace, TOK_INCLUDE_DOT );
    return( ret );
}

static bool AddModTrace( void )
/*****************************/
{
    char *          membname;

    GetFileName( &membname, FALSE );
    return( TRUE );
}

extern bool ProcModTrace( void )
/******************************/
{
    bool            ret;

    LinkFlags |= TRACE_FLAG;
    ret = ProcArgList( &AddModTrace, TOK_INCLUDE_DOT );
    return( ret );
}

symbol *        RefXSymbol(char *a,int b) {

    static symbol tmp;

    return( &tmp );
}

void    DoWriteMap( char *format, va_list arglist ) {}
void    WriteLibsUsed( void ) {}
void    SetRelocSize( void ) {}
void    SetSymCase( void ) {}
void    Vectorize( symbol *a ) {}
void            WriteMapNL( unsigned a ) {}

bool    ObjBuffering;

void    *PermAlloc( unsigned size ) {

    return( ChkLAlloc( size ) );
}

bool    PermShrink( void ) {

    return( FALSE );
}

bool SwapOutVirt( void ) {

    return( FALSE );
}

bool SwapOutRelocs( void ) {

    return( FALSE );
}

bool DumpObjCache( void ) {

    return( FALSE );
}

bool DiscardDicts( void ) {

    return( FALSE );
}

/* this is adapted from the linker ...*/

static void FreeFiles( file_list *list )
/**************************************/
{
    FILE_LIST           *next;

    for( ; list != NULL; list = next ) {
        next = list->next_file;
        if( list->status & STAT_HAS_MEMBER && list->u.member != NULL ) {
            FreeList( list->u.member );
        }
        _LnkFree( list );
    }
}

extern char *           SymFileName;

extern void FreeLinkStruct( void )
/********************************/
/* free all linker specific structures */
{
    FreeFiles( ObjLibFiles );
    if( Root != NULL ) {
        FreeFiles( Root->files );
        Root->files = NULL;
    }
    ObjLibFiles = NULL;
    if( SymFileName != NULL ) {
        _LnkFree( SymFileName );
    }
    if( FmtData.osname != NULL ) {
        _LnkFree( FmtData.osname );
    }
    FreeFormatStuff();
}

/*
 * and now, the code which actually drives the parser
*/

#define INIT_FILE_NAME "wlink.lnk"

extern sysblock *       SysBlocks;
extern parse_entry      Directives[];
extern tok              Token;

void ParseDirectives( void )
{
    while( GetToken( SEP_END, TOK_INCLUDE_DOT ) == FALSE ) {
        if( ProcOne( Directives, SEP_NO, FALSE ) == FALSE ) {
            /* a directive error happened. deal with it? */
            break;
        }
        RestoreParser();
    }
}

static void SetUpCommands()
/*************************/
/* Get the command processor set up to start working again. */
{
    InitCmdFile();
    LinkState = 0;      /* make sure these are reset ! */
    LinkFlags = 0;
    NewCommandSource( NULL, NULL, ENVIRONMENT ); // the "command line"
    Token.where = ENDOFCMD;     // nothing on this command line
}

static void ProcessInfo( void )
/*****************************/
/* the "mainline" routine, protected from suiciding things */
{
    char *      fname;
    f_handle    file;
    sysblock *  sys;

    Root = NewSection();
    SetUpCommands();
    file = FindPath( INIT_FILE_NAME );
    if( file == NIL_HANDLE ) return;   /* NO WLINK.LNK */
    _ChkAlloc( fname, sizeof(INIT_FILE_NAME));
    memcpy( fname, INIT_FILE_NAME, sizeof(INIT_FILE_NAME) );
    SetCommandFile( file, fname );
    ParseDirectives();
    Burn();   /* clean up everything but the system list */
    FreeLinkStruct();
    for( sys = SysBlocks; sys != NULL; sys = sys->next ) {
        LnkMsg( WRN+MSG_INTERNAL, "s", sys->name );
        SetUpCommands();
        NewCommandSource( sys->name, sys->commands, SYSTEM ); // input file
        sys->name = NULL;   /* see note 1 at end of function */
        ParseDirectives();
        Burn();
        FreeLinkStruct();
    }
    BurnSystemList();
    _LnkFree( Root );
}
/* NOTE for above function. This needs to be done, since the name will
 * automatically be freed by the linker's command parser. To save the name,
 * pass an allocated copy of it to NewCommandSource */

void main( int argc, char * argv[] )
/**********************************/
{
    LnkMemInit();
    Msg_Init( argv[0] );
    Spawn( &ProcessInfo );
    Msg_Fini();
    LnkMemFini();
}
