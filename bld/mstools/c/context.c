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


#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "context.h"
#include "error.h"
#include "memory.h"

#define MAX_INDIRECTIONS        8


/*
 * Define the context data structure
 */
struct Context {
    int                 type;           /* context type */
    FILE *              fp;             /* for parsing data on disk */
    char *              data;           /* for parsing data in memory */
    char *              dataStart;      /* for parsing data in memory */
    size_t              dataLen;        /* for parsing data in memory */
    long                markPos;        /* position of the context's mark */
};


static struct Context   curContext;
static int              curContextInitialized = 0;

static struct Context   stack[MAX_INDIRECTIONS];
static int              stackItems = 0;



/*
 * Clear the specified context to an uninitialized state.
 */
static void clear_context( struct Context *context )
/**************************************************/
{
    memset( context, 0, sizeof(struct Context) );
    context->markPos = -1L;
}


/*
 * Open a new command line context.
 */
int OpenCmdLineContext( void )
/****************************/
{
    int                 len;

    if( curContextInitialized )  Zoinks();
    clear_context( &curContext );

    /*** Make a copy of the command line ***/
    len = _bgetcmd( NULL, 0 ) + 1;
    curContext.dataStart = AllocMem( len );
    getcmd( curContext.dataStart );

    curContext.data = curContext.dataStart;
    curContext.dataLen = strlen( curContext.dataStart );
    curContextInitialized = 1;
    curContext.type = COMMAND_LINE_CONTEXT;
    return( 0 );
}


/*
 * Open a new environment variable context.
 */
int OpenEnvironContext( const char *envVar )
/******************************************/
{
    if( curContextInitialized )  Zoinks();
    clear_context( &curContext );

    curContext.data = getenv( envVar );
    curContext.dataStart = curContext.data;
    if( curContext.data == NULL )  return( 1 );
    curContext.dataLen = strlen( curContext.dataStart );

    curContextInitialized = 1;
    curContext.type = ENVIRON_VAR_CONTEXT;
    return( 0 );
}


/*
 * Open a new file context.
 */
int OpenFileContext( const char *filename )
/*****************************************/
{
    if( curContextInitialized )  Zoinks();
    clear_context( &curContext );

    curContext.fp = fopen( filename, "rt" );
    if( curContext.fp == NULL )  return( 1 );

    curContextInitialized = 1;
    curContext.type = COMMAND_FILE_CONTEXT;
    return( 0 );
}


/*
 * Close the current context.
 */
void CloseContext( void )
/***********************/
{
    if( curContextInitialized ) {
        switch( curContext.type ) {
          case COMMAND_LINE_CONTEXT:
            free( curContext.dataStart );
          case ENVIRON_VAR_CONTEXT:
            break;
          case COMMAND_FILE_CONTEXT:
            fclose( curContext.fp );
            break;
          default:
            Zoinks();
        }
    }

    curContextInitialized = 0;
    clear_context( &curContext );
}


/*
 * Push the current context onto the context stack; the current context
 * is disabled, but not closed.
 */
void PushContext( void )
/**********************/
{
    if( stackItems == MAX_INDIRECTIONS ) {
        FatalError( "Too many levels of context indirection." );
    }
    memcpy( &stack[stackItems], &curContext, sizeof(struct Context) );
    stackItems++;
    clear_context( &curContext );
    curContextInitialized = 0;
}


/*
 * Pop the top context off the context stack, and make it the current
 * context.  The current context must be closed before calling PopContext.
 */
void PopContext( void )
/*********************/
{
    CloseContext();
    if( stackItems == 0 )  Zoinks();
    memcpy( &curContext, &stack[stackItems-1], sizeof(struct Context) );
    stackItems--;
    curContextInitialized = 1;
}


/*
 * Get a character from the current context.  Returns a null byte if the
 * end of the context has been reached.
 */
int GetCharContext( void )
/************************/
{
    int     ch;

    ch = '\0';
    if( !curContextInitialized )
        Zoinks();
    switch( curContext.type ) {
    case COMMAND_LINE_CONTEXT:
        /* fall through */
    case ENVIRON_VAR_CONTEXT:
        if( (size_t)( curContext.data - curContext.dataStart ) < curContext.dataLen ) {
            ch = *curContext.data;
        }
        curContext.data++;
        break;
    case COMMAND_FILE_CONTEXT:
        ch = fgetc( curContext.fp );
        if( ch == EOF )
            ch = '\0';
        break;
    default:
        Zoinks();
    }
    return( ch );
}


/*
 * Unget a character back into the current context.
 */
void UngetCharContext( void )
/***************************/
{
    if( !curContextInitialized )  Zoinks();
    SetPosContext( GetPosContext() - 1 );
}


/*
 * Mark the current position in the current context.  Only one mark at a
 * time is stored; use GoToMarkContext to return to it.
 */
void MarkPosContext( void )
/*************************/
{
    if( !curContextInitialized )  Zoinks();
    curContext.markPos = GetPosContext();
}


/*
 * Back up the current position in the current context to the last place
 * marked by MarkPosContext.
 */
void GoToMarkContext( void )
/**************************/
{
    if( !curContextInitialized )  Zoinks();
    if( curContext.markPos == -1L )  Zoinks();
    SetPosContext( curContext.markPos );
}


/*
 * Get the current position within the current context.
 */
long GetPosContext( void )
/************************/
{
    if( !curContextInitialized )  Zoinks();
    switch( curContext.type ) {
      case COMMAND_LINE_CONTEXT:
        /* fall through */
      case ENVIRON_VAR_CONTEXT:
        return( (long)( curContext.data - curContext.dataStart ) );
        break;
      case COMMAND_FILE_CONTEXT:
        return( ftell( curContext.fp ) );
        break;
      default:
        Zoinks();
    }
    return( EOF );              /* dummy value so compiler won't complain */
}


/*
 * Set the current position within the current context.
 */
void SetPosContext( long pos )
/****************************/
{
    if( !curContextInitialized )  Zoinks();
    if( pos < 0 )  Zoinks();
    switch( curContext.type ) {
      case COMMAND_LINE_CONTEXT:
        /* fall through */
      case ENVIRON_VAR_CONTEXT:
        curContext.data = curContext.dataStart + pos;
        break;
      case COMMAND_FILE_CONTEXT:
        fseek( curContext.fp, pos, SEEK_SET );
        break;
      default:
        Zoinks();
    }
}
