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


#include <stdlib.h>
#include <string.h>
#include "asmglob.h"
#include "asmdefs.h"
#include "asmalloc.h"
#include "asmerr.h"
#include "asmops1.h"
#include "asmops2.h"
#include "asmins1.h"
#include "namemgr.h"
#include "asmsym.h"
#include "directiv.h"

extern void             PushLineQueue(void);
extern void             AddTokens( struct asm_tok **, int, int );
extern void             InputQueueLine( char * );
extern void             PushMacro( char *, bool );

const char macroname[] = "__STATIC_IRP_MACRO_";

int ForDirective( int i, bool is_char )
/*************************************/
{
    int start = i - 1; /* location of "directive name .. after any labels" */
    int arg_loc;
    char *parmstring;
    char *ptr;
    char *next_parm;
    char *end_of_parms;
    char buffer[MAX_LINE_LEN];
    int len;

    /* save the parm list, make a temporary macro, then call it with each parm */
    if( AsmBuffer[i]->token != T_ID ) {
        AsmError( OPERAND_EXPECTED );
        return( ERROR );
    }
    arg_loc = i;
    for( ;AsmBuffer[i]->token != T_COMMA; i++ ) {
        if( AsmBuffer[i]->token == T_FINAL ) {
            AsmError( EXPECTING_COMMA );
            return( ERROR );
        }
    }
    i++;
    if( AsmBuffer[i]->token != T_STRING ) {
        AsmError( PARM_REQUIRED );
        return( ERROR );
    }
    parmstring = AsmTmpAlloc( strlen( AsmBuffer[i]->string_ptr ) + 1 );
    strcpy( parmstring, AsmBuffer[i]->string_ptr );
    AsmBuffer[i]->token = T_FINAL;

    /* now make a macro */
    AddTokens( AsmBuffer, arg_loc, 1 );
    i = start;
    sprintf( buffer, "%s%d", macroname, Globals.for_counter );
    AsmBuffer[i]->string_ptr = buffer;
    AsmBuffer[i]->token = T_ID;
    i++;
    AsmBuffer[i]->token = T_DIRECTIVE;
    AsmBuffer[i]->value = T_MACRO;

    if( MacroDef( start, TRUE ) == ERROR ) return( ERROR );

    /* now call the above macro with each of the given parms */

    PushLineQueue();
    end_of_parms = parmstring + strlen( parmstring );
    for( ptr = parmstring; ptr <= end_of_parms; ) {
        sprintf( buffer, "%s%d", macroname, Globals.for_counter );
        strcat( buffer, " " );

        if( is_char ) {
            len = strlen( buffer );
            buffer[len] = *ptr;
            buffer[len+1] = NULLC;
            ptr++;
        } else {
            next_parm = ptr + strcspn( ptr, ",\0" );
            *next_parm = NULLC;
            next_parm++;
            strcat( buffer, ptr );
            ptr = next_parm;
        }
        InputQueueLine( buffer );
    }
    sprintf( buffer, "%s%d", macroname, Globals.for_counter );
    Globals.for_counter++;
    PushMacro( buffer, TRUE );
    return( NOT_ERROR );
}

