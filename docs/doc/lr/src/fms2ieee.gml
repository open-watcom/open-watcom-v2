.func _fmsbintoieee
#include <math.h>
extern int _fmsbintoieee( float *src, float *dest );
.ixfunc2 '&Math' &func
.synop end
.desc begin
The &func function loads the float pointed to by
.arg src
in Microsoft binary format and converts it to IEEE format, storing the
result &into the float pointed to by
.arg dest
.ct .li .
.np
The range of Microsoft binary format floats is 2.938736e-39 to
1.701412e+38.
The range of Microsoft binary format doubles is 2.938735877056e-39 to
1.701411834605e+38.
.np
Microsoft Binary Format was used by early versions of Microsoft
QuickBASIC before coprocessors became standard.
.desc end
.return begin
The &func function returns 0 if the conversion was successful.
Otherwise, it returns 1 if conversion would cause an overflow.
.return end
.see begin
.seelist &function. _dieeetomsbin _dmsbintoieee _fieeetomsbin _fmsbintoieee
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    float fieee, fmsb;
    double dieee, dmsb;

    fieee = 0.5;
    dieee = -2.0;

    /* Convert IEEE format to Microsoft binary format */
    _fieeetomsbin( &fieee, &fmsb );
    _dieeetomsbin( &dieee, &dmsb );

    /* Convert Microsoft binary format back to IEEE format */
    _fmsbintoieee( &fmsb, &fieee );
    _dmsbintoieee( &dmsb, &dieee );

    /* Display results */
    printf( "fieee = %f, dieee = %f\n", fieee, dieee );
  }
.exmp output
fieee = 0.500000, dieee = -2.000000
.exmp end
.class WATCOM
.system
