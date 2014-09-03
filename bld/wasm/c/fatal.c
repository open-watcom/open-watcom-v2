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
* Description:  Fatal errors processing
*
****************************************************************************/


#include "asmglob.h"
#include <stdarg.h>

#include "objprs.h"
#include "asmalloc.h"
#include "womputil.h"
#include "fatal.h"

typedef void (*err_act)( void );

typedef struct {
    int       num;            // index
    int       message;        // message displayed
    err_act   action;         // function to call, if any
    int       ret;            // exit code
} Msg_Struct;

static const Msg_Struct Fatal_Msg[] = {
    #define pick( cmd, number, msg, act, ret )    { number, msg, act, ret }
    #include "fatald.h"
    #undef pick
};

extern void             ObjRecFini( void );
extern void             MsgPrintf( int resourceid );

extern bool             write_to_file;  // write if there is no error

void AsmShutDown( void )
/**********************/
{
    /* close ASM file */
    if( AsmFiles.file[ASM] != NULL ) {
        if( fclose( AsmFiles.file[ASM] ) != 0 ) {
            Fatal( MSG_CANNOT_CLOSE_FILE, AsmFiles.fname[ASM] );
        }
    }

    /* close OBJ file */
    ObjWriteClose( pobjState.file_out );

    ObjRecFini();
    if( !write_to_file || Options.error_count > 0 ) {
        remove( AsmFiles.fname[OBJ] );
    }
    AsmFree( AsmFiles.fname[ASM] );
    AsmFree( AsmFiles.fname[ERR] );
    AsmFree( AsmFiles.fname[LST] );
    AsmFree( AsmFiles.fname[OBJ] );
    MemFini();
}

void Fatal( unsigned msg, ... )
/******************************/
{
    va_list     arg;
    int         i;

    MsgPrintf( MSG_ERROR );
    MsgPrintf( Fatal_Msg[msg].message );
    if( Fatal_Msg[msg].num > 0 ) {
        va_start( arg, msg );
        for( i=Fatal_Msg[msg].num; i > 0; i-- ) {
            printf( "%s", va_arg( arg, char * ) );
        }
        va_end( arg );
    }
    printf("\n");
    if( Fatal_Msg[msg].action != NULL ) {
        Fatal_Msg[msg].action();
    }
    exit( Fatal_Msg[msg].ret );
}
