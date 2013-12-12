.func matherr
.synop begin
#include <math.h>
int matherr( struct _exception *err_info );
.ixfunc2 '&Errs' &func
.ixfunc2 '&Math' &func
.synop end
.desc begin
The
.id &func.
function is invoked each time an error is detected by
functions in the math library.
The default
.id &func.
function supplied in the library returns zero which
causes an error message to be displayed upon
.kw stderr
and
.kw errno
to be set with an appropriate error value.
An alternative version of this function can be provided, instead of
the library version, in order that the error handling for mathematical
errors can be handled by an application.
.np
A program may contain a user-written version of
.id &func.
to take any
appropriate action when an error is detected.
When zero is returned, an error message will be printed upon
.kw stderr
and
.kw errno
will be set as was the case with the default function.
When a non-zero value is returned, no message is printed and
.kw errno
is not changed.
The value
.id err_info->retval
is used as the return value for the function in which the error was
detected.
.np
The
.id &func.
function is passed a pointer to a structure of type
.id struct _exception
which contains information about the error that has been detected:
.millust begin
struct _exception
{ int type;      /* TYPE OF ERROR                */
  char *name;    /* NAME OF FUNCTION             */
  double arg1;   /* FIRST ARGUMENT TO FUNCTION   */
  double arg2;   /* SECOND ARGUMENT TO FUNCTION  */
  double retval; /* DEFAULT RETURN VALUE         */
};
.millust end
.np
The
.id type
field will contain one of the following values:
.begterm 12
.termhd1 Value
.termhd2 Meaning
.term .mono DOMAIN
A domain error has occurred, such as
.mono sqrt(-1e0).
.term .mono SING
A singularity will result, such as
.mono pow(0e0,-2).
.term .mono OVERFLOW
An overflow will result, such as
.mono pow(10e0,100).
.term .mono UNDERFLOW
An underflow will result, such as
.mono pow(10e0,-100).
.term .mono TLOSS
Total loss of significance will result, such as
.mono exp(1000).
.term .mono PLOSS
Partial loss of significance will result, such as
.mono sin(10e70).
.endterm
.np
The
.id name
field points to a string containing the name of the function which
detected the error.
The fields
.id arg1
and
.id arg2
(if required) give the values which caused the error.
The field
.id retval
contains the value which will be returned by the function.
This value may be changed by a user-supplied version of the &func
function.
.desc end
.return begin
The
.id &func.
function returns zero when an error message is to be printed
and a non-zero value otherwise.
.return end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Demonstrate error routine in which negative */
/* arguments to "sqrt" are treated as positive */
.exmp break
void main()
  {
    printf( "%e\n", sqrt( -5e0 ) );
    exit( 0 );
  }
.exmp break
int matherr( struct _exception *err )
  {
    if( strcmp( err->name, "sqrt" ) == 0 ) {
      if( err->type == DOMAIN ) {
        err->retval = sqrt( -(err->arg1) );
        return( 1 );
      } else
        return( 0 );
    } else
      return( 0 );
  }
.exmp end
.class WATCOM
.system
