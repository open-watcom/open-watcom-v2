/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#define IRP_MACRONAME   "__STATIC_IRP_MACRO_"

bool ForDirective( token_buffer *tokbuf, token_idx i, irp_type type )
/*******************************************************************/
{
    token_idx   start;
    char        *parmstring;
    char        *ptr;
    char        buffer[MAX_LINE_LEN];
    size_t      len;
    size_t      lenx;
    token_idx   j;

    start = i - 1; /* location of "directive name .. after any labels" */
    parmstring = NULL;
    len = 0;
    if( type == IRP_REPEAT ) {
        ExpandTheWorld( tokbuf, i, false, true );
        /*
         * make a temporary macro, then call it
         */
        if( tokbuf->tokens[i].class != TC_NUM ) {
            AsmError( OPERAND_EXPECTED );
            return( RC_ERROR );
        }
        len = tokbuf->tokens[i].u.value;
        i++;
        if( tokbuf->tokens[i].class != TC_FINAL ) {
            AsmError( OPERAND_EXPECTED );
            return( RC_ERROR );
        }
    } else {
        /*
         * save the parm list, make a temporary macro, then call it with each parm
         */
        if( tokbuf->tokens[i].class != TC_ID ) {
            AsmError( OPERAND_EXPECTED );
            return( RC_ERROR );
        }
        i++;
        for( j = i; j < tokbuf->count; j++ ) {
            if( tokbuf->tokens[j].class == TC_COMMA ) {
                break;
            }
        }
        if( j >= tokbuf->count ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }
        j++;
        if( tokbuf->tokens[j].class != TC_STRING ) {
            AsmError( PARM_REQUIRED );
            return( RC_ERROR );
        }
        parmstring = AsmTmpAlloc( strlen( tokbuf->tokens[j].string_ptr ) + 1 );
        strcpy( parmstring, tokbuf->tokens[j].string_ptr );
        /*
         * copy parameter name token to correct location (one token up)
         */
        tokbuf->tokens[i] = tokbuf->tokens[i - 1];
        /*
         * correct tokens count and set TC_FINAL token after parameter name
         */
        SetFinalToken( tokbuf, i + 1 );
    }
    /*
     * now make a macro definition
     */
    i = start;
    lenx = sprintf( buffer, IRP_MACRONAME "%d", Globals.for_counter );
    if( Options.mode & MODE_IDEAL ) {
        tokbuf->tokens[i + 1].string_ptr = buffer;
        tokbuf->tokens[i + 1].class = TC_ID;
    } else {
        tokbuf->tokens[i].string_ptr = buffer;
        tokbuf->tokens[i].class = TC_ID;
        i++;
    }
    tokbuf->tokens[i].class = TC_DIRECTIVE;
    tokbuf->tokens[i].u.token = T_MACRO;

    if( MacroDef( tokbuf, i, true ) )
        return( RC_ERROR );

    PushLineQueue();
    if( type == IRP_REPEAT ) {
        /*
         * now call the macro <number> times
         */
        while( len-- > 0 ) {
            InputQueueLine( buffer );
        }
    } else {
        /*
         * now call the above macro with each of the given parms
         */
        ptr = parmstring;
        while( *ptr != NULLC ) {
            len = lenx;
            buffer[len++] = ' ';
            if( type == IRP_CHAR ) {
                buffer[len++] = *ptr++;
            } else {    // IRP_WORD
                size_t len1;

                len1 = strcspn( ptr, "," );
                strncpy( buffer + len, ptr, len1 );
                len += len1;
                ptr += len1;
                if( *ptr == ',' ) {
                    ++ptr;
                }
            }
            buffer[len] = NULLC;
            InputQueueLine( buffer );
        }
        /*
         * reset buffer to macro name only
         */
        buffer[lenx] = '\0';
    }
    Globals.for_counter++;
    PushMacro( buffer, true );
    return( RC_OK );
}
