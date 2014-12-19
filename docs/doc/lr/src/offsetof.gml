.func offsetof
.synop begin
#include <stddef.h>
size_t offsetof( composite, name );
.synop end
.desc begin
The
.id &funcb.
macro returns the offset of the element
.arg name
within the
.kw struct
or
.kw union
.arg composite
.ct .li .
This provides a portable method to determine the offset.
.desc end
.return begin
The
.id &funcb.
function returns the offset of
.arg name
.ct .li .
.return end
.exmp begin
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
.exmp output
.blktext begin
In a small data model, the following would result:
.blktext end
.blkcode begin
first:0 second:2 third:12
.blkcode end
.blktext begin
In a large data model, the following would result:
.blktext end
.blkcode begin
first:0 second:4 third:14
.blkcode end
.exmp end
.class ISO C
.system
