!-----------------------------------------------------------------------
! MAIN - the main routine

*$include adsapi.fi

        program main

        include 'adslib.fi'

        integer*2 scode/RSRSLT/ ! Normal result code (default)
        integer stat

        external funcload, dofun, funcinit
        integer funcload, dofun, funcinit

        stat = funcinit()

        call ads_init( 0, 0 )        ! Open communication with AutoLISP

        loop        ! Request/Result loop

            stat = ads_link( scode )
            if( stat .lt. 0 )then
                print *, 'FACT: bad status from ads_link() =', stat
                return
            end if

            scode = RSRSLT      ! Reset result code

            select( stat )

            case( RQXLOAD )     ! Load & define functions
                if( funcload() .eq. RTNORM )then
                    scode = RSRSLT
                else
                    scode = RSERR
                end if

            case( RQSUBR )      ! Handle external function requests
                if( dofun() .eq. RTNORM )then
                    scode = RSRSLT
                else
                    scode = RSERR
                end if

            end select
        end loop

        end

!-----------------------------------------------------------------------
! FUNCINIT  --  Initialize function definition structure

        function funcinit()

        include 'adslib.fi'

        integer funcinit, funcload, dofun

        integer  fact, squareroot
        external fact, squareroot

        integer*4 args
        record /resbuf/ rb(:)

        integer val, i

        structure /func_entry/
            character*32        func_name
            integer             func
        end structure

        ! To add another function, change the value of "NUM_FUNS" and
        ! add the new function(s) to the function table ("func_table").

        integer NUM_FUNS
        parameter ( NUM_FUNS = 2 )
        record /func_entry/ func_table(NUM_FUNS)

        func_table(1).func_name = 'fact'//char(0)
        func_table(1).func = loc( fact )
        func_table(2).func_name = 'sqr'//char(0)
        func_table(2).func = loc( squareroot )

        funcinit = 0

        return

!-----------------------------------------------------------------------
! FUNCLOAD  --  Define this application's external functions.  Return
!               RTERROR on error, else RTNORM.

        entry funcload()

        do i = 1, NUM_FUNS
            if( ads_defun( func_table(i).func_name, i - 1 ) .eq. 0 )then
                funcload = RTERROR
                return
            endif
        end do
        funcload = RTNORM

        return

!-----------------------------------------------------------------------
! DOFUN -- Execute external function (called upon an RQSUBR request).
!          Return value from the function executed, RTNORM or RTERROR.

        entry dofun()

        ! Get the function code and check that it's within range.
        ! (It can't fail to be, but paranoia doesn't hurt.)
        val = ads_getfuncode()
        if( val .lt. 0 .or. val .gt. NUM_FUNS )then
            call ads_fail(
     &          'Received nonexistent function code.'//char(0) )
            dofun = RTERROR
            return
        end if

! Code that handles a single argument.

        ! Fetch the arguments, if any.
        args = ads_getargs()

        ! Point allocatable array to a result buffer.
        allocate( rb(1), location=args )

        ! Call the handler and return its success-failure status.
        dofun = call_handler( func_table(val+1).func, rb )

        ! Disassociate buffer from allocatable array
        deallocate( rb )

! Code that handles multiple arguments.

*       ! Fetch the arguments, if any.
*       args = ads_getargs()
*
*       while( args .ne. NULL )do
*
*           ! Point allocatable array to a result buffer.
*           allocate( rb(1), location=args )
*
*           ! Call the handler and return its success-failure status.
*           dofun = call_handler( func_table(val+1).func, rb )
*           if( dofun .ne. RTNORM ) quit
*
*           ! Get next argument in list.
*           args = rb(1).rbnext
*
*           ! Disassociate buffer from allocatable array so we don't
*           ! get an error on next allocate
*           deallocate( rb )
*
*       end while

        end

!-----------------------------------------------------------------------
! FACT -- First set up the argument, then call the factorial function

        integer function fact( rb )
        record /resbuf/ rb

        include 'adslib.fi'

        integer x

        double precision rfact
        external rfact

        if( loc( rb ) .eq. NULL )then
            fact = RTERROR
            return
        end if

        if( rb.restype .eq. RTSHORT )then
            x = rb.resval.rint  ! Save in local variable
        else
            call ads_fail( 'Argument should be an integer.'//char(0) )
            fact = RTERROR
            return
        end if

        if( x .lt. 0 )then      ! Check argument range
            call ads_fail( 'Argument should be positive.'//char(0) )
            fact = RTERROR
            return
        else if( x .gt. 170 )then       ! Avoid floating-point overflow
            call ads_fail( 'Argument should be 170 or less.'//char(0) )
            fact = RTERROR
            return
        endif

        call ads_retreal( rfact( x ) )  ! Call the function itself, and
                                        ! return the value to AutoLISP
        fact = RTNORM
        end

!-----------------------------------------------------------------------
! This is the implementation of the actual external factorial function

        double precision function rfact( n ) ! static
        integer n

        integer i

        rfact = 1.0
        do i = n, 1, -1
            rfact = rfact * i
        end do

        end

!-----------------------------------------------------------------------
! SQUAREROOT -- First set up the argument, then call the root function

        integer function squareroot( rb )
        record /resbuf/ rb

        include 'adslib.fi'

        double precision x

        double precision rsqr
        external rsqr

        if( loc( rb ) .eq. NULL )then
            squareroot = RTERROR        ! A proper error msg would be better
            return
        endif

        if( rb.restype .eq. RTSHORT )then       ! Save in local variable
            x = rb.resval.rint
        else if( rb.restype .eq. RTREAL )then
            x = rb.resval.rreal                 ! can accept either real
        else                                    ! or integer
            call ads_fail(
     &          'Argument should be a real or integer value.'//char(0) )
            squareroot = RTERROR
        end if

        if( x .lt. 0.0 )then    ! Check argument range
            call ads_fail( 'Argument should be positive.'//char(0) )
            squareroot = RTERROR
        end if

        call ads_retreal( rsqr( x ) )   ! Call the function itself, and
                                        ! return the value to AutoLISP
        squareroot = RTNORM
        end

!-----------------------------------------------------------------------
! This is the implementation of the actual external function

        double precision function rsqr( x ) ! static
        double precision x

        ! Square root by Newton's method

        integer n/50/
        double precision c, cl

        if( x .eq. 0.0 )then
            rsqr = 0.0
            return
        end if

        rsqr = (x * 2 + .1) / (x + 1.0)
        c = (rsqr - x / rsqr) / 2
        cl= 0.0

        while( (c .ne. cl) .and. (n .gt. 0) )do
            rsqr = rsqr - c
            cl = c
            c  = (rsqr - x / rsqr) / 2
            n = n - 1
        end while

        end
