#include <stdio.h>
#include <wchar.h>

char *types[11] = {
    "alnum",
    "alpha",
    "cntrl",
    "digit",
    "graph",
    "lower",
    "print",
    "punct",
    "space",
    "upper",
    "xdigit"
};

void main()
  {
    int     i;
    wint_t  wc = 'A';

    for( i = 0; i < 11; i++ )
      if( iswctype( wc, wctype( types[i] ) ) )
        printf( "%s\n", types[ i ] );
  }
