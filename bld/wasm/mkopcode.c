#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char Chars[3000];
struct words {
        char    *word;
        int     index;
};

int len_compare( const struct words *p1, const struct words *p2 )
{
    int         len1;
    int         len2;

    len1 = strlen( p1->word );
    len2 = strlen( p2->word );
    if( len1 < len2 )  return( 1 );
    if( len1 > len2 )  return( -1 );
    return( strcmp( p1->word, p2->word ) );
}

int str_compare( const struct words *p1, const struct words *p2 )
{
    return( strcmp( p1->word, p2->word ) );
}

main( int argc, char *argv[] )
{
    FILE        *in;
    FILE        *out;
    char        *out_name;
    unsigned int i;
    unsigned int index;
    unsigned int j;
    unsigned int count;
    unsigned int len;
    unsigned int idx;
    struct words *Words;
    char        *word;
    char        buf[80];
    char        *suffix;
    char        *ptr;

    out_name = argv[argc-1];
    --argc;
    count = 0;
    for( idx = 1; idx < argc; ++idx ) {
        in = fopen( argv[idx], "r" );
        if( in == NULL ) {
            printf( "Unable to open '%s'\n", argv[idx] );
            exit( 1 );
        }
        for(;;) {
            if( fgets( buf, 80, in ) == NULL ) break;
            count++;
        }
        fclose( in );
    }
    Words = malloc( (count+1) * sizeof( struct words ) );
    if( Words == NULL ) {
        printf( "Unable to allocate Words array\n" );
        exit( 1 );
    }
    Words[count].word = NULL;
    index = 0;
    for( idx = 1; idx < argc; ++idx ) {
        in = fopen( argv[idx], "r" );
        if( in == NULL ) {
            printf( "Unable to open '%s'\n", argv[idx] );
            exit( 1 );
        }
        for( ;; ) {
            if( fgets( buf, 80, in ) == NULL ) break;
            for( i = 0; buf[i]; i++ ) {
                if( buf[i] == '\n' || buf[i] == ' ' || buf[i] == '\t' ) break;
            }
            buf[i] = '\0';
            Words[index].word = strdup( buf );
            if( Words[index].word == NULL ) {
                printf( "Out of memory\n" );
                exit( 1 );
            }
            ++index;
        }
        fclose( in );
    }
    qsort( Words, count, sizeof( struct words ), len_compare );
    index = 0;
    Chars[0] = '\0';
    for( i = 0; i < count; i++ ) {
        word = strstr( Chars, Words[i].word );
        if( word == NULL ) {
            word = &Chars[index];
            len = strlen( Words[i].word ) - 1;
            if( index < len )  len = index;
            for(;;) {
                if( len == 0 ) break;
                if( memcmp( word - len, Words[i].word, len ) == 0 ) {
                    word -= len;
                    index -= len;
                    break;
                }
                len--;
            }
            strcpy( word, Words[i].word );
            index += strlen( word );
        }
        Words[i].index = word - Chars;
    }
    qsort( Words, count, sizeof( struct words ), str_compare );

    out = fopen( out_name, "w" );
    if( out == NULL ) {
        printf( "Unable to open '%s'\n", out_name );
        exit( 1 );
    }
    fprintf( out, "\n#ifndef asm_op\n" );
    fprintf( out, "  #define asm_op(token,len,index) token\n" );
    fprintf( out, "  enum asm_token {\n" );
    fprintf( out, "#else\n" );
    fprintf( out, "  char AsmChars[] = {\n" );
    for( i = 0; i < index; i++ ) {
        if( i % 10 == 0 )  fprintf( out, "/*%4d*/ ", i );
        fprintf( out, "'%c',", Chars[i] );
        if( i % 10 == 9 )  fprintf( out, "\n" );
    }
    fprintf( out, "'\\0'\n};\n\n" );
    fprintf( out, "  #undef asm_op\n" );
    fprintf( out, "  #define asm_op(token,len,index) {0,len,index}\n" );
    fprintf( out, "  extern struct AsmCodeName AsmOpcode[] = {\n" );
    fprintf( out, "#endif\n\n" );
    for( i = 0; i < count; i++ ) {
        strcpy( buf, Words[i].word );
        strupr( buf );

        j = strlen( buf );
        /*** Some special cases for simplified segment ***/
        if( stricmp( buf, ".DATA?" ) == 0 ) {
            strcpy( buf, ".DATA_UN" );
        } else if( stricmp( buf, ".FARDATA?" ) == 0 ) {
            strcpy( buf, ".FARDATA_UN" );
        }

        /*** Some special cases because of same name in pcobj.h ***/
        if( stricmp( buf, "ABS" ) == 0 ||
            stricmp( buf, "INS" ) == 0 ||
            stricmp( buf, "SEG" ) == 0 ) {
            suffix = "2";
        } else {
            suffix = "";
        }
        ptr = buf;
        if( *ptr == '.' ) ptr++;
        fprintf( out, "asm_op( T_%s%s,\t", ptr, suffix );
        if( j < 5 ) fprintf( out, "\t" );
        fprintf( out, "%d, %d\t),\n", j, Words[i].index );
    }
    fprintf( out, "asm_op( T_NULL,\t\t0, 0\t)\n" );
    fprintf( out, "};\n" );
    fclose( out );
    exit( 0 );
}
