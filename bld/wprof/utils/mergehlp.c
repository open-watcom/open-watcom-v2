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


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <process.h>

#define MAX_HELP_WIDTH  72
#define MAX_LINE_SIZE   1000

FILE *cfp;
FILE *hfp;
char *ok;

char cbuff[MAX_LINE_SIZE];
char hbuff[MAX_LINE_SIZE];

void RemoveNewline( char *p )
{
    for( ; *p; ++p ) {
        if( *p == '\n' ) {
            *p = '\0';
            return;
        }
    }
}

void ReadCNames( void )
{
    ok = fgets( cbuff, MAX_LINE_SIZE, cfp );
    if( !ok ) {
        cbuff[0] = '\0';
        return;
    }
    RemoveNewline( cbuff );
}

int IsHelpName( char *p )
{
    if( !p ) {
        hbuff[0] = '\0';
        return( 1 );
    }
    if( p[0] != ':' ) {
        return( 0 );
    }
    if( p[1] != ':' ) {
        return( 0 );
    }
    if( p[2] != ':' ) {
        return( 0 );
    }
    if( p[3] != ':' ) {
        return( 0 );
    }
    return( 1 );
}

int IsIndirect( char *p )
{
    if( !p ) {
        hbuff[0] = '\0';
        return( 0 );
    }
    if( p[0] != ' ' ) {
        return( 0 );
    }
    if( p[1] != '-' ) {
        return( 0 );
    }
    if( p[2] != '>' ) {
        return( 0 );
    }
    return( 1 );
}

void ReadHNames( void )
{
    if( !IsHelpName( ok ) ) {
        puts( "out of sync" );
        return;
    }
    if( !ok ) {
        return;
    }
    memmove( hbuff, hbuff + 4, strlen( hbuff ) - 3 );
    RemoveNewline( hbuff );
}

void CenterTitle( char *p )
{
    int state;
    char *start;
    char *q;
    size_t len;
    size_t column;

    state = 0;
    start = p;
    len = 0;
    for( q = p; *q; ++q ) {
        if( ! isspace( *q ) ) {
            state = 1;
        }
        if( state ) {
            *p = *q;
            ++p;
            ++len;
        }
    }
    if( ! state ) {
        puts( "no title to center" );
        return;
    }
    *p = '\0';
    for( column = ( MAX_HELP_WIDTH - len ) / 2; column != 0; --column ) {
        putchar( ' ' );
    }
}

void UnderLine( char *p )
{
    int state;

    state = 0;
    for( ; *p; ++p ) {
        if( ! isspace( *p ) ) {
            state = 1;
        }
        if( state ) {
            *p = 'Í';
        }
    }
}

void FlushText( void )
{
    ok = fgets( hbuff, MAX_LINE_SIZE, hfp );
    if( !ok || IsHelpName( ok ) ) {
        return;
    }
    RemoveNewline( hbuff );
    if( ! IsIndirect( hbuff ) ) {
        CenterTitle( hbuff );
        puts( hbuff );
        UnderLine( hbuff );
        CenterTitle( hbuff );
    } else {
        puts( hbuff );
        ok = fgets( hbuff, MAX_LINE_SIZE, hfp );
    }
    while( ! IsHelpName( ok ) ) {
        RemoveNewline( hbuff );
        puts( hbuff );
        ok = fgets( hbuff, MAX_LINE_SIZE, hfp );
    }
}

int CompareNames( void )
{
    if( cbuff[0] == '\0' ) {
        if( hbuff[0] == '\0' ) {
            return( 0 );
        }
        return( 1 );
    } else if( hbuff[0] == '\0' ) {
        return( -1 );
    }
    return( stricmp( cbuff, hbuff ) );
}

void Merge( void )
{
    int comp;

    ReadCNames();
    FlushText();
    ReadHNames();
    for(;;) {
        if( cbuff[0] == '\0' && hbuff[0] == '\0' ) {
            break;
        }
        comp = CompareNames();
        if( comp == 0 ) {
            printf( "::::%s\n", hbuff );
            FlushText();
            ReadHNames();
            ReadCNames();
        } else if( comp > 0 ) {
            fprintf( stderr, "Extra help text: %s\n", hbuff );
            printf( "::::%s\n", hbuff );
            FlushText();
            ReadHNames();
        } else {
            fprintf( stderr, "Missing help text added: %s\n", cbuff );
            printf( "::::%s\n", cbuff );
            puts( "No help text is available." );
            ReadCNames();
        }
    }
}

int main( int argc, char **argv )
{
    if( argc != 3 ) {
        puts( "usage: mergehlp <c_names_file> <help_text>" );
        exit( 1 );
    }
    cfp = fopen( argv[1], "r" );
    if( cfp == NULL ) {
        puts( "cannot open file" );
    }
    hfp = fopen( argv[2], "r" );
    if( cfp == NULL ) {
        puts( "cannot open file" );
    }
    Merge();
    fclose( cfp );
    fclose( hfp );
    return( 0 );
}
