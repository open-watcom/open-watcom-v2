#include <stdio.h>
#include <stdlib.h>

void main()
  {
    void func1(void), func2(void), func3(void);

    atexit( func1 );
    atexit( func2 );
    atexit( func3 );
    printf( "Do this first.\n" );
  }

void func1(void) { printf( "last.\n" ); }

void func2(void) { printf( "this " ); }

void func3(void) { printf( "Do " ); }
