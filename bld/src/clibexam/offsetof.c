#include <stdio.h>
#include <stddef.h>

struct new_def
{  char *first;
   char second[10];
   int third;
};

void main()
  {
    printf( "first:%d second:%d third:%d\n",
        offsetof( struct new_def, first ),
        offsetof( struct new_def, second ),
        offsetof( struct new_def, third ) );
  }
