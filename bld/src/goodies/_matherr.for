*
* _MATHERR.FOR	: math error handler
*
* Compile: wfc[386] _matherr

*$pragma aux __imath2err "*_" parm( value, reference, reference )
*$pragma aux __amath1err "*_" parm( value, reference )
*$pragma aux __amath2err "*_" parm( value, reference, reference )
*$pragma aux __math1err "*_" parm( value, reference )
*$pragma aux __math2err "*_" parm( value, reference, reference )
*$pragma aux __zmath2err "*_" parm( value, reference, reference )
*$pragma aux __qmath2err "*_" parm( value, reference, reference )


      integer function __imath2err( err_info, arg1, arg2 )
      integer err_info
      integer arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
	  select( err_info .and. FUNC_MASK )
	  case( FUNC_POW )
	      print *, 'arg2 cannot be <= 0'
	  case( FUNC_MOD )
	      print *, 'arg2 cannot be 0'
	  end select
      end if
      __imath2err = 0
      end


      real function __amath1err( err_info, arg1 )
      integer err_info
      real arg1
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
	  select( err_info .and. FUNC_MASK )
	  case( FUNC_COTAN )
	      print *, 'overflow'
	  end select
      end if
      __amath1err = 0.0
      end


      real function __amath2err( err_info, arg1, arg2 )
      integer err_info
      real arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
	  select( err_info .and. FUNC_MASK )
	  case( FUNC_MOD )
	      print *, 'arg2 cannot be 0'
	  end select
      end if
      __amath2err = 0.0
      end


      double precision function __math1err( err_info, arg1 )
      integer err_info
      double precision arg1, __math2err
      __math1err = __math2err( err_info, arg1, arg1 )
      end


      double precision function __math2err( err_info, arg1, arg2 )
      integer err_info
      double precision arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
	  select( err_info .and. FUNC_MASK )
	  case( FUNC_SQRT )
	      print *, 'argument cannot be negative'
	  case( FUNC_ASIN, FUNC_ACOS )
	      print *, 'argument must be less than or equal to one'
	  case( FUNC_ATAN2 )
	      print *, 'both arguments must not be zero'
	  case( FUNC_POW )
	      if( arg1 .eq. 0.0 )then
		  print *, 'a zero base cannot be raised to a ',
     &			  'negative power'
	      else ! base < 0 and non-integer power
		  print *, 'a negative base cannot be raised to a ',
     &			  'non-integral power'
	      endif
	  case( FUNC_LOG, FUNC_LOG10 )
	      print *, 'argument must not be negative'
	  end select
      else if( ( err_info .and. M_SING ) .ne. 0 )then
	  if( ( ( err_info .and. FUNC_MASK ) .eq. FUNC_LOG ) .or.
     &	      ( ( err_info .and. FUNC_MASK ) .eq. FUNC_LOG10 ) )then
	      print *, 'argument must not be zero'
	  endif
      else if( ( err_info .and. M_OVERFLOW ) .ne. 0 )then
	  print *, 'value of argument will cause overflow condition'
      else if( ( err_info .and. M_UNDERFLOW ) .ne. 0 )then
	  print *, 'value of argument will cause underflow ',
     &		    'condition - return zero'
      end if
      __math2err = 0
      end


      complex function __zmath2err( err_info, arg1, arg2 )
      integer err_info
      complex arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
	  select( err_info .and. FUNC_MASK )
	  case( FUNC_POW )
	      ! arg1 is (0,0)
	      if( imag( arg2 ) .ne. 0 )then
		  print *, 'a zero base cannot be raised to a',
     &		  ' complex power with non-zero imaginary part'
	      else
		  print *, 'a zero base cannot be raised to a',
     &		  ' complex power with non-positive real part'
	      endif
	  end select
      end if
      __zmath2err = (0,0)
      end


      double complex function __qmath2err( err_info, arg1, arg2 )
      integer err_info
      double complex arg1, arg2
      include 'mathcode.fi'
      arg1 = arg1     ! to avoid unreferenced warning message
      arg2 = arg2     ! to avoid unreferenced warning message
      if( ( err_info .and. M_DOMAIN ) .ne. 0 )then
	  select( err_info .and. FUNC_MASK )
	  case( FUNC_POW )
	      ! arg1 is (0,0)
	      if( imag( arg2 ) .ne. 0 )then
		  print *, 'a zero base cannot be raised to a',
     &		   ' complex power with non-zero imaginary part'
	      else
		  print *, 'a zero base cannot be raised to a',
     &		    ' complex power with non-positive real part'
	      endif
	  end select
      end if
      __qmath2err = (0,0)
      end
