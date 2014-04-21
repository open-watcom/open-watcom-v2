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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "cmdline.h"
#include "error.h"
#include "memory.h"

#define GRANULARITY     30
#define BUFFER_SIZE     1024


/*
 * Initialize the command line sections.  Use AppendCmdLine to insert
 * an item into any of these sections, and MergeCmdLine to merge them all
 * together.  Valid sections will be in the range [0,sections-1].
 */
CmdLine *InitCmdLine( int sections )
/**********************************/
{
    int                 count;
    CmdLine *           cmdLine;

    /*** Allocate the sections ***/
    if( sections < 1 )  Zoinks();
    cmdLine = AllocMem( sections * sizeof(struct CmdLine) );
    cmdLine->numSections = sections;
    cmdLine->merged = NULL;

    /*** Initialize each section ***/
    for( count=0; count<sections; count++ ) {
        cmdLine[count].maxItems = GRANULARITY;  /* some space to start with */
        cmdLine[count].args = AllocMem( cmdLine[count].maxItems * sizeof(char*) );
        cmdLine[count].curItems = 0;            /* no parameters stored yet */
    }

    return( cmdLine );
}


/*
 * Make a completely separate copy of a CmdLine.
 */
CmdLine *CloneCmdLine( const CmdLine *oldCmdLine )
/************************************************/
{
    CmdLine *           newCmdLine;
    int                 countSection;
    int                 countArg;

    newCmdLine = InitCmdLine( oldCmdLine->numSections );
    for( countSection=0; countSection<oldCmdLine->numSections; countSection++ ) {
        for( countArg=0; countArg<oldCmdLine[countSection].curItems; countArg++ ) {
            AppendCmdLine( newCmdLine, countSection, oldCmdLine[countSection].args[countArg] );
        }
    }

    return( newCmdLine );
}


/*
 * Free all memory occupied by a CmdLine, invalidating it for future use.
 */
void DestroyCmdLine( CmdLine *cmdLine )
/*************************************/
{
    int                 countSection;
    int                 countArg;

    for( countSection=0; countSection<cmdLine->numSections; countSection++ ) {
        for( countArg=0; countArg<cmdLine[countSection].curItems; countArg++ ) {
            FreeMem( cmdLine[countSection].args[countArg] );
        }
        FreeMem( cmdLine[countSection].args );
    }
    FreeMem( cmdLine );
}


/*
 * Add one more parameter to the section specified by block.  Reallocates
 * memory if necessary, and does all necessary bookkeeping.
 */
void AppendCmdLine( CmdLine *cmdLine, int section, const char *parm )
/*******************************************************************/
{
    /*** Allocate GRANULARITY more elements if we're out of room ***/
    if( section < 0  ||  section >= cmdLine->numSections )  Zoinks();
    if( cmdLine[section].curItems+2 > cmdLine[section].maxItems ) {
        cmdLine[section].args = ReallocMem( cmdLine[section].args, (cmdLine[section].maxItems+GRANULARITY) * sizeof(char*) );
        cmdLine[section].maxItems += GRANULARITY;
    }

    /*** Store the specified pointer and update curItems ***/
    if( parm != NULL )  parm = DupStrMem( parm );               /* copy it */
    cmdLine[section].args[ cmdLine[section].curItems ] = (char*)parm;
    cmdLine[section].args[ cmdLine[section].curItems+1 ] = NULL;/* last one */
    cmdLine[section].curItems++;
}


/*
 * Add one more parameter to the section specified by block.  Reallocates
 * memory if necessary, and does all necessary bookkeeping.  The stored
 * string is generated using the printf-style format string and any
 * additional parmameters.
 */
void AppendFmtCmdLine( CmdLine *cmdLine, int section, const char *format, ... )
/*****************************************************************************/
{
    static char         buf[BUFFER_SIZE];
    va_list             args;

    /*** Format it, then pass it though to AppendCmdLine ***/
    if( section < 0  ||  section >= cmdLine->numSections )  Zoinks();
    va_start( args, format );
    vsnprintf( buf, BUFFER_SIZE, format, args );
    va_end( args );
    AppendCmdLine( cmdLine, section, buf );
}


/*
 * Merge all the sections together.  Returns a pointer to the resulting
 * argument vector; the caller should not modify its contents.  Any call
 * to this function invalidates any pointer it returned previously for
 * the same CmdLine.  If section==INVALID_MERGE_CMDLINE, then all sections
 * are merged in order.  Otherwise, section is the first item in a
 * INVALID_MERGE_CMDLINE-terminated list of section numbers; the merged
 * command line will consist of items from the specified sections, in order.
 */
char **MergeCmdLine( CmdLine *cmdLine, int section, ... )
/*******************************************************/
{
    int                 countSection;
    int                 countArg;
    int                 total = 0;
    va_list             args;
    bool                done = FALSE;

    /*** Count the total number of items ***/
    if( section == INVALID_MERGE_CMDLINE ) {
        for( countSection=0; countSection<cmdLine->numSections; countSection++ ) {
            for( countArg=0; countArg<cmdLine[countSection].curItems; countArg++ ) {
                total++;
            }
        }
    } else {
        for( countArg=0; countArg<cmdLine[section].curItems; countArg++ ) {
            total++;
        }
        va_start( args, section );
        while( !done ) {
            countSection = va_arg( args, int );
            if( countSection != INVALID_MERGE_CMDLINE ) {
                for( countArg=0; countArg<cmdLine[countSection].curItems; countArg++ ) {
                    total++;
                }
            } else {
                done = TRUE;
            }
        }
        va_end( args );
    }

    /*** Allocate enough room for all the needed pointers ***/
    if( cmdLine->merged != NULL )  FreeMem( cmdLine->merged ); /* free old */
    cmdLine->merged = AllocMem( (total+1) * sizeof(char*) );

    /*** Add the items to the cmdLine->merged array ***/
    total = 0;
    done = FALSE;
    if( section == INVALID_MERGE_CMDLINE ) {
        for( countSection=0; countSection<cmdLine->numSections; countSection++ ) {
            for( countArg=0; countArg<cmdLine[countSection].curItems; countArg++ ) {
                cmdLine->merged[total++] = cmdLine[countSection].args[countArg];
            }
        }
    } else {
        for( countArg=0; countArg<cmdLine[section].curItems; countArg++ ) {
            cmdLine->merged[total++] = cmdLine[section].args[countArg];
        }
        va_start( args, section );
        while( !done ) {
            countSection = va_arg( args, int );
            if( countSection != INVALID_MERGE_CMDLINE ) {
                for( countArg=0; countArg<cmdLine[countSection].curItems; countArg++ ) {
                    cmdLine->merged[total++] = cmdLine[countSection].args[countArg];
                }
            } else {
                done = TRUE;
            }
        }
        va_end( args );
    }
    cmdLine->merged[total] = NULL;              /* terminate the list */

    return( cmdLine->merged );
}
