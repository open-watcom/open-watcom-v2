      subroutine cw87

* CW87.FOR
* This subroutine uses the C Library routine "_control87"
* to modify the math coprocessor exception mask.

* Compile: wfc[386] cw87

      include 'fsignal.fi'

      character*9 status(0:1)/' disabled',' enabled'/
      integer fp_cw, fp_mask, bits, i

* Enable floating-point underflow since default is disabled.
* The mask defines which bits we want to change (1 means change,
* 0 means do not change).  The corresponding bit in the control
* word (fp_cw) is set to 0 to enable the exception or 1 to disable
* the exception.  In this example, we change only the underflow
* bit and leave the others unchanged.

      fp_mask = EM_UNDERFLOW	! mask for the bits to set/reset
      fp_cw = '0000'x		! new bit settings (0=enable/1=disable)
      fp_cw = _control87( fp_cw, fp_mask )

* Now get up-to-date setting

      fp_cw = _control87( 0, 0 )

      bits = IAND( fp_cw, MCW_EM )
      print '(a,1x,z4)', 'Interrupt Exception Mask', bits
      i = 0
      if( IAND(fp_cw, EM_INVALID) .eq. 0 ) i = 1
      print *, '  Invalid Operation exception', status(i)
      i = 0
      if( IAND(fp_cw, EM_DENORMAL) .eq. 0 ) i = 1
      print *, '  Denormalized exception', status(i)
      i = 0
      if( IAND(fp_cw, EM_ZERODIVIDE) .eq. 0 ) i = 1
      print *, '  Divide-By-Zero exception', status(i)
      i = 0
      if( IAND(fp_cw, EM_OVERFLOW) .eq. 0 ) i = 1
      print *, '  Overflow exception', status(i)
      i = 0
      if( IAND(fp_cw, EM_UNDERFLOW) .eq. 0 ) i = 1
      print *, '  Underflow exception', status(i)
      i = 0
      if( IAND(fp_cw, EM_PRECISION) .eq. 0 ) i = 1
      print *, '  Precision exception', status(i)
      end
