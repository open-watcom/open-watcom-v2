/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "_preproc.h"


static void MkMkDependency( const char *filename, size_t len, const char *fullname, incl_type incltype )
/******************************************************************************************************/
{
    const char  *fmt;

    /* unused parameters */ (void)len;

    if( incltype == PPINCLUDE_SYS ) {
        fmt = "#include <%s>  ==> <%s>\n";
    } else {
        fmt = "#include \"%s\"  ==> \"%s\"\n";
    }
    printf( fmt, filename, fullname );
}

int PP_MBCharLen( const char *p )
/*******************************/
{
    /* unused parameters */ (void)p;

    return( 1 );
}

int main( int argc, char *argv[] )
/********************************/
{
    if( argc < 2 ) {
        printf( "Usage: mkmk filename\n" );
        exit( 1 );
    }
    PP_Init( ( argv[2] != NULL ) ? argv[2][0] : '#' );
    if( PP_FileInit( argv[1], PPFLAG_DEPENDENCIES | PPFLAG_TRUNCATE_FILE_NAME, NULL ) != 0 ) {
        PP_Fini();
        printf( "Unable to open '%s'\n", argv[1] );
        exit( 1 );
    }
    // call PP_Define here to predefine any desired macros
    PP_Dependency_List( MkMkDependency );
    PP_FileFini();
    return( ( PP_Fini() ) ? EXIT_FAILURE : EXIT_SUCCESS );
}
