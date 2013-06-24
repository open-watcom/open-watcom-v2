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
* Description:  Helper program used in wmake regression testing.
*
****************************************************************************/



#include <stdio.h>
#include <stdlib.h>

FILE *safeopen( char *name, char *mode )
{
    FILE *fp;

    fp = fopen( name, mode );
    if( !fp ) {
        fprintf( stderr, "Error opening %s\n", name );
        exit(20);
    }
    return( fp );
}

int main( int argc, char **argv )
{
    int i, max;
    FILE *fp;
    char buf[30];

    if( argc != 2 ) {
        printf( "Usage: create n\n" );
        return( 1 );
    }
    max = atoi( argv[1] );

    for( i = 1; i <= max; i++ ) {
        sprintf(buf, "foo%04d.c", i );
        fp = safeopen( buf, "w" );
        fprintf( fp, "extern int printf(char *,...);"
                     "void foo%04d(void){printf(\"%04d\\t\");}\n", i, i );
        fclose( fp );
    }

    fp = safeopen( "main.c", "w" );
    fprintf( fp, "#include <stdio.h>\n" );
    for( i=1; i <= max; i++ ) {
        fprintf(fp, "extern void foo%04d(void);\n", i );
    }
    fprintf(fp, "\nint main(void)\n{\n" );
    for( i=1; i <= max; i++ ) {
        fprintf(fp, "    foo%04d();\n", i );
    }
    fprintf(fp, "    printf( \"\\n\" );\n    return( 0 );\n}\n" );
    fclose(fp);

    fp = safeopen( "maketst1", "w" );
    fputs("# big makefile!\n"
          ".c.obj:\n"
          "    wcc386 $[* -zq\n"
          "\n"
          "main.exe : main.obj &\n    ", fp);
    for( i=1; i<= max; i++ ) {
        fprintf( fp, "foo%04d.obj ", i );
        if( i%6 == 0 ) fputs( "&\n    ", fp );
    }
    fputs("\n    wlink @main.lnk\n", fp);
    fclose(fp);

    fp = safeopen( "main.lnk", "w" );
    fputs( "NAME main\nFILE MAIN\nOPTION quiet\n", fp);
    for( i=1; i<=max; i++ ) {
        fprintf( fp, "FILE foo%04d\n", i);
    }
    fclose(fp);
    return( 0 );
}
