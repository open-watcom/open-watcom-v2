.func _set_matherr
#include <math.h>
void _set_matherr( int (*rtn)( struct _exception *err_info ) )
.ixfunc2 '&Errs' &func
.ixfunc2 '&Math' &func
.funcend
.desc begin
The default
.kw matherr
function supplied in the library can be replaced so that the
application can handle mathematical errors.
To do this, the &func function must be called with the address of the
new mathematical error handling routine.
.pp
.bi Note:
Under some systems, the default math error handler can be replaced by
providing a user-written function of the same name,
.kw matherr
.ct , and using linking strategies to replace the default handler.
Under PenPoint, the default handler is bound into a dynamic link
library and can only be replaced by notifying the C library with a
call to the &func function.
.pp
A program may contain a user-written version of
.kw matherr
to take any appropriate action when an error is detected.
When zero is returned by the user-written routine, an error message
will be printed upon
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
.pp
When called, the user-written math error handler is passed a pointer
to a structure of type
.id struct _exception
which contains information about the error that has been detected:
.blkcode begin
struct _exception
{ int type;      /* TYPE OF ERROR                */
  char *name;    /* NAME OF FUNCTION             */
  double arg1;   /* FIRST ARGUMENT TO FUNCTION   */
  double arg2;   /* SECOND ARGUMENT TO FUNCTION  */
  double retval; /* DEFAULT RETURN VALUE         */
};
.blkcode end
.blktext begin
The
.id type
field will contain one of the following values:
.begterm 15
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
.pp
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
The &func function returns no value.
.blktext end
.return end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Demonstrate error routine in which negative */
/* arguments to "sqrt" are treated as positive */

.exmp break
int my_matherr( struct _exception *err )
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
.exmp break
void main( void )
{
    _set_matherr( &my_matherr );
    printf( "%e\n", sqrt( -5e0 ) );
    exit( 0 );
}
.exmp end
.class WATCOM
.system
