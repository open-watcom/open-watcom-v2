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


static int      doOpen( FILE **fps, const char *name, int *size )
/***************************************************************/
{
    int         x;
    int         s;
    char        buffer[_MAX_PATH];

    assert( fps );
    assert( name );
    assert( size );
    assert( *size > 0 );

    s = *size;

    for( x = 0; x < s; x++ ) {
        sprintf( buffer, "%s.%d", name, x );
        fps[x] = fopen( buffer, "rt" );
        if( !fps[x] ) {
            *size = x;
            return( 0 );
        }
    }
    return( 1 );
}


static void     doClose( FILE **fps, int size )
/*********************************************/
{
    assert( fps );

    size--;
    while( size >= 0 ) {
        assert( fps[size] );
        fclose( fps[size] );
        size--;
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
    double      mean = 0.0;
    double      stddev = 0.0;
    double      tmp;

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
        mean += times[x];

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

    mean /= (double)size;

    for( x = 0; x < size; x++ ) {
        tmp = times[x] - mean;
        stddev += ( tmp * tmp );
    }

    stddev = sqrt( stddev / (double)size );

    sprintf( buffer, "%-20.20s:  m = %10.3e, s = %10.3e, %%s = %.3f", name,
                mean, stddev, ( stddev / mean ) * 100.0 );
    displayOutput( buffer );

    return( 1 );
}


static void     displayUsage( void )
/**********************************/
{
    displayOutput( "Usage:" );
    displayOutput( "  banal <name> <num>" );
    displayOutput( "where:" );
    displayOutput( "  name - name of the body of the data files, i.e. watcom" );
    displayOutput( "         where the file names are watcom.[0-999]        " );
    displayOutput( "  num  - number of files in set number 0 ... num - 1    " );
    displayOutput( "results:" );
    displayOutput( "  values are elapsed times in seconds; lower is better  " );
    displayOutput( "  m    - mean value             " );
    displayOutput( "  s    - standard deviation     " );
    displayOutput( "  %s   - s / m * 100            " );
}


void main( int argc, char **argv )
/********************************/
{
    FILE        **fps;
    int         size;
    double      *times;

    if( argc < 3 ) {
        displayUsage();
        return;
    }

    size = strtol( argv[2], NULL, 10 );
    if( size < 1 ) {
        displayUsage();
        return;
    }

    fps = malloc( sizeof( FILE * ) * size );
    times = malloc( sizeof( double ) * size );
    if( !fps || !times ) {
        displayError( "Out of Memory" );
        if( fps ) {
            free( fps );
        } else if( times ) {
            free( times );
        }
        return;
    }

    memset( fps, 0, sizeof( FILE * ) * size );

    if( doOpen( fps, argv[1], &size ) ) {
        while( doProcessTime( fps, times, size ) );
    } else {
        displayError( "Opening files" );
    }

    doClose( fps, size );
    free( fps );
    free( times );
}
