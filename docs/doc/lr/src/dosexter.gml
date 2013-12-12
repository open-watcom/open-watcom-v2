.func dosexterr
.synop begin
#include <&doshdr>
int dosexterr( struct DOSERROR *err_info );

struct _DOSERROR {
        int exterror;   /* contents of AX register */
        char errclass;  /* contents of BH register */
        char action;    /* contents of BL register */
        char locus;     /* contents of CH register */
};
.ixfunc2 '&Errs' &func
.synop end
.desc begin
The &func function extracts extended error information following
a failed DOS function.
This information is placed in the structure located by
.arg err_info
.ct .li .
This function is only useful with DOS version 3.0 or later.
.np
You should consult the technical documentation for the DOS system on
your computer for an interpretation of the error information.
.desc end
.return begin
The &func function returns an unpredictable result when the preceding
DOS call did not result in an error.
Otherwise, &func returns the number of the extended error.
.return end
.see begin
.seelist dosexterr perror
.see end
.exmp begin
#include <stdio.h>
#include <dos.h>
#include <fcntl.h>

struct _DOSERROR dos_err;
.exmp break
void main()
  {
    int &fd;
.exmp break
    /* Try to open "stdio.h" and then close it */
    if( _dos_open( "stdio.h", O_RDONLY, &amp.&fd ) != 0 ){
      dosexterr( &dos_err );
      printf( "Unable to open file\n" );
      printf( "exterror (AX) = %d\n", dos_err.exterror );
      printf( "errclass (BH) = %d\n", dos_err.errclass );
      printf( "action   (BL) = %d\n", dos_err.action );
      printf( "locus    (CH) = %d\n", dos_err.locus );
    } else {
      printf( "Open succeeded\n" );
      if( _dos_close( &fd ) != 0 ) {
        printf( "Close failed\n" );
      } else {
        printf( "Close succeeded\n" );
      }
    }
  }
.exmp output
Unable to open file
exterror (AX) = 2
errclass (BH) = 8
action   (BL) = 3
locus    (CH) = 2
.exmp end
.class DOS
.system
