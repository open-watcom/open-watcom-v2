! func1.for -- Example of a simple REXX function.

c$define INCL_DOS
c$include os2.fap
c$define INCL_REXXSAA
c$include rexxsaa.fap

! Declare our exported function.  Don't forget to export it!

c$pragma aux (RexxFunctionHandler) EZFunc1 "EZFunc1"

!
! EZFunc1 -- Bumps a counter and returns a string.  Arguments are as
!            follows:
!
!              name -- Name of function being called.
!              numargs -- Number of argument strings passed from REXX.
!              args -- Array of argument strings passed from REXX.
!              queuename -- Name of currently active queue.
!              retstr -- Pointer to buffer for result string.
!
!            The only one you may modify is the retstr.
!

c$noreference
        integer function EZFunc1( name, numargs, args, queuename, retstr )
c$reference
        integer numargs
        record /RXSTRING/ retstr, args(numargs)
        character name, queuename

        include 'rxsutils.fi'

        character*100 buf

        save count
        integer count/0/

        ! Check # of arguments.  We don't want any.  Exit with error
        ! (which halts the REXX program) if some are given.

        if( numargs .gt. 0 )then
            EZFunc1 = INVALID_ROUTINE
            return
        endif

        ! Enter a critical section.  Because the counter is static, it
        ! will be shared across threads, so we need to update it atomically.

        call DosEnterCritSec()

        count = count + 1
        write( buf, '(''You called EZFunc1 '', i3, '' times.'' )' ) count

        call DosExitCritSec()

        ! Set the retstr to the new string.

        call CopyResult( buf, lentrim( buf ), retstr )

        ! Return 0 if no error occurred
        EZFunc1 = VALID_ROUTINE

        end
