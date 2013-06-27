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


#include "asmglob.h"

#include "asmalloc.h"
#include "directiv.h"
#include "asmexpnd.h"
#include "asminput.h"

extern void             AddTokens( asm_tok **, int, int );

static const char macroname[] = "__STATIC_IRP_MACRO_";

int ForDirective( int i, enum irp_type type )
/*******************************************/
{
    int start = i - 1; /* location of "directive name .. after any labels" */
    int arg_loc;
    char *parmstring = NULL;
    char *ptr;
    char *next_parm;
    char *end_of_parms;
    char buffer[MAX_LINE_LEN];
    int len = 0;

    if( type == IRP_REPEAT ) {
        ExpandTheWorld( i, FALSE, TRUE );
        /* make a temporary macro, then call it */
        if( AsmBuffer[i]->class != TC_NUM ) {
            AsmError( OPERAND_EXPECTED );
            return( ERROR );
        }
        arg_loc = i;
        len = AsmBuffer[i]->u.value;
        i++;
        if( AsmBuffer[i]->class != TC_FINAL ) {
            AsmError( OPERAND_EXPECTED );
            return( ERROR );
        }
    } else {
        /* save the parm list, make a temporary macro, then call it with each parm */
        if( AsmBuffer[i]->class != TC_ID ) {
            AsmError( OPERAND_EXPECTED );
            return( ERROR );
        }
        arg_loc = i;
        for( ;AsmBuffer[i]->class != TC_COMMA; i++ ) {
            if( AsmBuffer[i]->class == TC_FINAL ) {
                AsmError( EXPECTING_COMMA );
                return( ERROR );
            }
        }
        i++;
        if( AsmBuffer[i]->class != TC_STRING ) {
            AsmError( PARM_REQUIRED );
            return( ERROR );
        }
        parmstring = AsmTmpAlloc( strlen( AsmBuffer[i]->string_ptr ) + 1 );
        strcpy( parmstring, AsmBuffer[i]->string_ptr );
        AsmBuffer[i]->class = TC_FINAL;

        AddTokens( AsmBuffer, arg_loc, 1 );
    }
    /* now make a macro */
    i = start;
    sprintf( buffer, "%s%d", macroname, Globals.for_counter );
    if( Options.mode & MODE_IDEAL ) {
        AsmBuffer[i+1]->string_ptr = buffer;
        AsmBuffer[i+1]->class = TC_ID;
    } else {
        AsmBuffer[i]->string_ptr = buffer;
        AsmBuffer[i++]->class = TC_ID;
    }
    AsmBuffer[i]->class = TC_DIRECTIVE;
    AsmBuffer[i]->u.token = T_MACRO;

    if( MacroDef( i, TRUE ) == ERROR ) return( ERROR );

    /* now call the above macro with each of the given parms */

    PushLineQueue();
    if( type == IRP_REPEAT ) {
        sprintf( buffer, "%s%d", macroname, Globals.for_counter );
        while( len-- > 0 ) {
            InputQueueLine( buffer );
        }
    } else {
        end_of_parms = parmstring + strlen( parmstring );
        for( ptr = parmstring; ptr < end_of_parms; ) {
            sprintf( buffer, "%s%d", macroname, Globals.for_counter );
            strcat( buffer, " " );

            if( type == IRP_CHAR ) {
                len = strlen( buffer );
                buffer[len] = *ptr;
                buffer[len+1] = NULLC;
                ptr++;
            } else if( type == IRP_WORD ) {
                next_parm = ptr + strcspn( ptr, ",\0" );
                *next_parm = NULLC;
                next_parm++;
                strcat( buffer, ptr );
                ptr = next_parm;
            }
            InputQueueLine( buffer );
        }
        sprintf( buffer, "%s%d", macroname, Globals.for_counter );
    }
    Globals.for_counter++;
    PushMacro( buffer, TRUE );
    return( NOT_ERROR );
}
