#include <stdio.h>
#include <stdlib.h>

static char buffer[ 130 ];
static char output_name[ 13 ];
static char func_name[ 20 ];

void main( int argc, char *argv[] )
{
    FILE    *input;
    FILE    *output;
    int     i, j;

    if( argc < 2 ) {
	printf( "Usage: mkit cmd_filename\n" );
	exit( EXIT_FAILURE );
    }
    input = fopen( argv[1], "r" );
    if( input == NULL ) {
	printf( "Can't open %s\n", argv[1] );
	exit( EXIT_FAILURE );
    }

    for(;;) {
	if( fgets( buffer, 130-1, input ) == NULL )break;
	strncpy( output_name, buffer, 8 );
	for( i = 7; i != 0; i-- ) {
	    if( output_name[i] != ' ' )break;
	}
	output_name[++i] = '\0';
	strcat( output_name, ".gml" );
	printf( "Output file name is %s\n", output_name );
	for( i = 9; i < 130; i++ ) {
	    if( buffer[i] == ' ' )break;
	}
	for( ; i < 130; i++ ) {
	    if( buffer[i] != ' ' )break;
	}
	j = 0;
	for( ; i < 130; i++ ) {
	    if( (func_name[j] = buffer[i]) == '(' )break;
	    j++;
	}
	func_name[j] = '\0';
	printf( "Function name is %s\n", func_name );

	output = fopen( output_name, "w" );
	if( output != NULL ) {
	    fprintf( output, ".func %s\n", func_name );
	    fprintf( output, "#include <mmintrin.h>\n" );
	    fprintf( output, &buffer[9] );
	    fprintf( output, ".funcend\n" );
	    fprintf( output, ".desc begin\n" );
	    fprintf( output, ".desc end\n" );
	    fprintf( output, ".return begin\n" );
	    fprintf( output, ".return end\n" );
	    fprintf( output, ".see begin\n" );
	    fprintf( output, ".im seemmfun &function.\n" );
	    fprintf( output, ".see end\n" );
	    fprintf( output, ".seexmp _m_pcmpeqb\n" );
	    fprintf( output, ".class Intel\n" );
	    fprintf( output, ".system\n" );
	    fclose( output );
	}


    }
}
