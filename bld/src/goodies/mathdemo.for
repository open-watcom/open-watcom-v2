      program mathdemo

* MATHDEMO.FOR - This program forms part of a collection of FORTRAN
*		 code that demonstrates how to take over control of
*		 math error handling from the run-time system.

* Compile: wfl[386] mathdemo cw87 _matherr

* Notes:
* (1) We call "cw87" to enable underflow exceptions which are
*     masked (ignored) by default.
* (2) The signal handler must be re-installed after each signal
*     (it can also be re-installed even when there is no signal).
* (3) To prevent compile-time constant folding in expressions,
*     we add log(1.0) which is 0.  We do this for the sake of
*     demonstrating exception handling.

      implicit none

      double precision x, y, z

      call cw87 	! init 80x87 control word

      call resetFPE	! install signal handler
      print *, ' '
      print *, 'Divide by zero will be attempted'
      x = 1.0d0 + DLOG( 1.0d0 )
      y = 0.0d0
      z = x / y
      call chkFPE	! check for exception
      print *, z

      call resetFPE	! install signal handler
      print *, ' '
      print *, 'Overflow will be attempted'
      x = 1.2d300 + DLOG( 1.0d0 )
      y = 1.2d300
      z = x * y
      call chkFPE	! check for exception
      print *, z

      call resetFPE	! install signal handler
      print *, ' '
      print *, 'Underflow will be attempted'
      x = 1.14d-300 + DLOG( 1.0d0 )
      y = 2.24d-308
      z = x * y
      call chkFPE	! check for exception
      print *, z

      call resetFPE	! install signal handler
      print *, ' '
      print *, 'Math error will be attempted'
      x = -12.0
      ! an exception will not be generated since the intrinsic function
      ! will validate the argument - if you compile with /om, the "fsqrt"
      ! 80x87 instruction will be generated in-line and an exception
      ! will occur
      y = SQRT( x )
      call chkFPE	! check for exception
      print *, x, y
      end

      subroutine resetFPE
      include 'fsignal.fi'
      external fpe_handler
      logical fpe_flag
      integer fpe_sig, fpe_fpe
      common fpe_flag, fpe_sig, fpe_fpe
      fpe_flag = .false.
      call fsignal( SIGFPE, fpe_handler )
      end

*$pragma aux fpe_handler parm( value )

      subroutine fpe_handler( sig, fpe )
      integer*2 sig, fpe
      logical fpe_flag
      integer fpe_sig, fpe_fpe
      common fpe_flag, fpe_sig, fpe_fpe
      fpe_flag = .true.
      fpe_sig = sig
      fpe_fpe = fpe
      end

*$pragma aux fwait = "fwait"

      subroutine chkFPE
      include 'fsignal.fi'
      logical fpe_flag
      integer fpe_sig, fpe_fpe
      common fpe_flag, fpe_sig, fpe_fpe
* Notes:
* (1) An fwait is required to make sure that the last
*     floating-point instruction has completed.
* (2) "volatile" is not needed here but would be
*     needed in main program if it references "fpe_flag"
      call fwait()
      if( volatile( fpe_flag ) ) then
	print *, '*ERROR* exception occurred',
     &		 fpe_sig, fpe_fpe
	if( fpe_fpe .eq. FPE_INVALID )then
	  print *, 'Invalid'
	else if( fpe_fpe .eq. FPE_DENORMAL )then
	  print *, 'Denormalized operand error'
	else if( fpe_fpe .eq. FPE_ZERODIVIDE )then
	  print *, 'Divide by zero error'
	else if( fpe_fpe .eq. FPE_OVERFLOW )then
	  print *, 'Overflow error'
	else if( fpe_fpe .eq. FPE_UNDERFLOW )then
	  print *, 'Underflow error'
	else if( fpe_fpe .eq. FPE_INEXACT )then
	  print *, 'Inexact result (precision)then error'
	else if( fpe_fpe .eq. FPE_UNEMULATED )then
	  print *, 'Unemulated instruction error'
	else if( fpe_fpe .eq. FPE_SQRTNEG )then
	  print *, 'Square root of a negative number error'
	else if( fpe_fpe .eq. FPE_STACKOVERFLOW )then
	  print *, 'NDP stack overflow error'
	else if( fpe_fpe .eq. FPE_STACKUNDERFLOW )then
	  print *, 'NDP stack underflow error'
	else if( fpe_fpe .eq. FPE_EXPLICITGEN )then
	  print *, 'SIGFPE signal raised (software)'
	else if( fpe_fpe .eq. FPE_IOVERFLOW )then
	  print *, 'Integer overflow error'
	endif
      else
	print *, '*OK* no exception occurred'
      endif
      end
