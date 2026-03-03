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
* Description:  Fatal errors processing
*
****************************************************************************/


#include "asmglob.h"
#include <stdarg.h>
#include "asmalloc.h"
#include "fatal.h"
#include "omfgenio.h"
#include "omfobjre.h"


typedef void (*err_act)( void );

extern bool             write_to_file;  // write if there is no error

void CloseAsmFile( int i )
{
    if( AsmFiles.file[i] != NULL ) {
        if( fclose( AsmFiles.file[i] ) != 0 ) {
            Fatal( CANNOT_CLOSE_FILE, AsmFiles.fname[i] );
        }
        AsmFiles.file[i] = NULL;
    }
}

void AsmCloseFiles( void )
/************************/
{
    /* close ASM file */
    CloseAsmFile( ASM );

    /* close OBJ file */
    ObjWriteClose( !write_to_file || Options.error_count > 0 );
    ObjRecFini();
    CloseAsmFile( LST );
    CloseAsmFile( ERR );
    MemFree( AsmFiles.fname[ASM] );
    MemFree( AsmFiles.fname[ERR] );
    MemFree( AsmFiles.fname[LST] );
    MemFree( AsmFiles.fname[OBJ] );
}

void Fatal( unsigned msg, ... )
/*****************************/
{
    va_list     arg;

    MsgPrintf( MSG_ERROR );
    MsgPrintf( msg );
    switch( msg ) {
    case CANNOT_OPEN_FILE:
    case CANNOT_CLOSE_FILE:
    case OBJECT_FILE_ERROR:
        va_start( arg, msg );
        printf( "%s", va_arg( arg, char * ) );
        va_end( arg );
        break;
    }
    printf("\n");
    longjmp( errjmp, 3 );
}
