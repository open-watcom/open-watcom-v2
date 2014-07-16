#include <stdio.h>
#include <stdlib.h>

struct tri_graph {
    char	tri;
    char	rep;
};

static struct tri_graph TriGraphs[] = {
    { '=', '#' },
    { '(', '[' },
    { '/', '\\'},
    { ')', ']' },
    { '\'','^' },
    { '<', '{' },
    { '!', '|' },
    { '>', '}' },
    { '-', '~' },
    { '\0','\0'}
};

char makeFromTrigraph( char c )
{
    struct tri_graph *p;

    for( p = TriGraphs; p->rep != '\0'; ++p ) {
	if( p->rep == c ) {
	    return( p->tri );
	}
    }
    return( '\0' );
}

char isATrigraph( char c )
{
    struct tri_graph *p;

    for( p = TriGraphs; p->tri != '\0'; ++p ) {
	if( p->tri == c ) {
	    return( p->rep );
	}
    }
    return( '\0' );
}

int fail( char *p )
{
    fprintf( stderr, "%s\n", p );
    return( EXIT_FAILURE );
}

void addComment( FILE *i, FILE *o )
{
    int c;

    fprintf( o, "/*\n" );
    for(;;) {
	c = fgetc( i );
	if( c == EOF ) break;
	fputc( c, o );
    }
    fprintf( o, "*/\n" );
    rewind( i );
}

int main( int argc, char **argv )
{
    int flip;
    int c;
    char t;
    char p;
    char pp;
    FILE *fi;
    FILE *fo;

    if( argc != 3 ) {
	return( fail( "munge <input-file> <output-file>" ) );
    }
    flip = 0;
    fi = fopen( argv[1], "r" );
    if( fi == NULL ) {
	return( fail( "cannot open input file" ));
    }
    fo = fopen( argv[2], "w" );
    if( fo == NULL ) {
	return( fail( "cannot open output file" ) );
    }
    addComment( fi, fo );
    pp = '\0';
    p = '\0';
    for(;;) {
	c = fgetc( fi );
	if( c == EOF ) break;
	if( c == '\n' && p == '\\' ) {
	    return( fail( "cannot handle backslash newline" ) );
	}
	if( pp == '?' && p == '?' && isATrigraph( c ) ) {
	    return( fail( "cannot handle trigraphs" ) );
	}
	t = makeFromTrigraph( c );
	if( flip ) {
	    if( t != '\0' ) {
		fprintf( fo, "??" "%c\\  \t\t     \n", t );
	    } else {
		fprintf( fo, "%c\\\n", c );
	    }
	} else {
	    if( t != '\0' ) {
		fprintf( fo, "??" "%c" "??" "/" "\n", t );
	    } else {
		fprintf( fo, "%c" "?" "?" "/" " \t \n", c );
	    }
	}
	flip = !flip;
	pp = p;
	p = c;
    }
    fclose( fi );
    fclose( fo );
    return( EXIT_SUCCESS );
}
