
#include <stdio.h>

int main( int argc, char *argv[] )
  {
    int i;
    for( i = 0; i < argc; ++i ) {
      printf( "argv[%d] = %s\n", i, argv[i] );
    }
    return( 0 );
  }
#ifdef _WIDE_
int wmain( int wargc, wchar_t *wargv[] )
  {
    int i;
    for( i = 0; i < wargc; ++i ) {
      wprintf( L"wargv[%d] = %s\n", i, wargv[i] );
    }
    return( 0 );
  }
#endif
