#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>


static void     displayOutput( const char *str )
/**********************************************/
{
    assert( str );

    puts( str );
}


static void     displayError( const char *err )
/*********************************************/
{
    char        buffer[100];

    assert( err );

    sprintf( buffer, "Error: %s", err );
    displayOutput( buffer );
}


static int      doOpen( FILE **fps, const char **names )
/******************************************************/
{
    int         x;

    assert( fps );
    assert( names );
    assert( names[0] );
    assert( names[1] );

    for( x = 0; x < 2; ++x ) {
        fps[x] = fopen( names[x], "r" );
        if( !fps[x] ) {
            return( 0 );
        }
    }
    return( 1 );
}


static void     doClose( FILE **fps )
/***********************************/
{
    int         x;

    assert( fps );

    for( x = 0; x < 2; ++x ) {
        assert( fps[x] );
        fclose( fps[x] );
    }
}


static double   getTime( char *buffer )
/*************************************/
{
    int         len;

    assert( buffer );

    len = strlen( buffer );
    if( len <= 0 ) return( -1.0 );
    len--;

    while( len && isspace( buffer[len] ) ) {
        len--;
    }

    while( len && !isspace( buffer[len] ) ) {
        len--;
    }

    if( isspace( buffer[len] ) ) {
        buffer[len] = 0;
        len++;
    }

    return( strtod( &buffer[len], NULL ) );
}


static int      doProcessTime( FILE **fps, double *times, int size )
/******************************************************************/
{
    int         x;
    char        buffer[100];
    char        name[100];
    char        ind;
    double      pct;

    assert( fps );
    assert( times );
    assert( size > 0 );

    for( x = 0; x < size; x++ ) {
        assert( fps[x] );
        if( !fgets( buffer, 100, fps[x] ) ) {
            if( !feof( fps[x] ) || x ) {
                displayError( "Reading file" );
            }
            return( 0 );
        }

        times[x] = getTime( buffer );

        if( times[x] <= 0.0 ) {
            displayError( "Bad file format" );
            return( 0 );
        } else if( !x ) {
            strcpy( name, buffer );
        } else if( strcmp( name, buffer ) ) {
            displayError( "Benchmark mismatch" );
            return( 0 );
        }
    }

    pct = (times[1] / times[0]) * 100.0;

    ind = ' ';
    if( pct > 105.0 )
        ind = '-';
    else if( pct < 95.0 )
        ind = '+';

    sprintf( buffer, "%-20.20s:  t1 = %10.3e, t2 = %10.3e, %% = %8.3f %c", name,
                times[0], times[1], pct, ind );
    displayOutput( buffer );

    return( 1 );
}


static void     displayUsage( void )
/**********************************/
{
    displayOutput( "Usage:" );
    displayOutput( "  bcomp <file1> <file2>" );
    displayOutput( "where:" );
    displayOutput( "  file1" );
    displayOutput( "  file2 - names of the data files to compare" );
    displayOutput( "results:" );
    displayOutput( "  data files are compared and results printed;" );
    displayOutput( "  file1 is the baseline (ie. 100%); values are" );
    displayOutput( "  run-times in seconds, lower is better" );
}


void main( int argc, char **argv )
/********************************/
{
    FILE        *fps[2];
    double      times[2];

    if( argc != 3 ) {
        displayUsage();
        return;
    }

    if( doOpen( fps, (const char **)argv + 1 ) ) {
        while( doProcessTime( fps, times, 2 ) );
    } else {
        displayError( "Opening files" );
    }

    doClose( fps );
}
