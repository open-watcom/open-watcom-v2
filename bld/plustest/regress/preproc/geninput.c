#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct gen_item GEN_ITEM;
struct gen_item {
    char	*fname;
    char	*contents;
};

GEN_ITEM items[] = {
    { "pp17a.gh",
	"/**** pp17a.gh ****/\n"
	"int pp17a_line2;"
#if __WATCOM_REVISION__ < 8
	    "\n"
#endif
    },
    { "pp17b.gh",
	"/**** pp17b.gh ****/\n"
	"int pp17b_line2;\n"
    },
    { "pp17c.gh",
	"/**** pp17c.gh ****/\n"
	"int pp17c_line2;"
#if __WATCOM_REVISION__ < 8
	    "\n"
#endif
    },
    { NULL, NULL },
};

void die( unsigned line ) {
    fprintf( stderr, "error on line %u\n", line );
    exit( EXIT_FAILURE );
}

int main() {
    size_t len;
    FILE *fp;
    GEN_ITEM *curr;
    char *gen;

    for( curr = items; curr->fname != NULL; ++curr ) {
	fp = fopen( curr->fname, "w" );
	if( fp == NULL ) die( __LINE__ );
	gen = curr->contents;
	len = strlen( gen );
	if( fwrite( gen, sizeof( char ), len, fp ) != len*sizeof(char) ) die(__LINE__);
	fclose( fp );
    }
    return( EXIT_SUCCESS );
}
