#include <stdio.h>
#include <stdarg.h>

void test_fn( const char *msg,
              const char *types,
              ... );

void main()
  {
    printf( "VA...TEST\n" );
    test_fn( "PARAMETERS: 1, \"abc\", 546",
             "isi", 1, "abc", 546 );
    test_fn( "PARAMETERS: \"def\", 789",
             "si", "def", 789 );
  }

static void test_fn(
  const char *msg,   /* message to be printed    */
  const char *types, /* parameter types (i,s)    */
  ... )              /* variable arguments       */
  {
    va_list argument;
    int   arg_int;
    char *arg_string;
    const char *types_ptr;

    types_ptr = types;
    printf( "\n%s -- %s\n", msg, types );
    va_start( argument, types );
    while( *types_ptr != '\0' ) {
      if (*types_ptr == 'i') {
        arg_int = va_arg( argument, int );
        printf( "integer: %d\n", arg_int );
      } else if (*types_ptr == 's') {
        arg_string = va_arg( argument, char * );
        printf( "string:  %s\n", arg_string );
      }
      ++types_ptr;
    }
    va_end( argument );
  }
