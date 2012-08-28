! func2.c -- Another example of a simple REXX function.

c$define INCL_REXXSAA
c$include rexxsaa.fap

! Declare our exported function.  Export it!

c$pragma aux (RexxFunctionHandler) EZFunc2 "EZFunc2"

! EZFunc2 -- Returns the name of the function that you called.  This
!            function will be registered as both 'EZFunc2' and 'EZFunc3'.
!

c$noreference
        integer function EZFunc2( name, numargs, args,
     &                            queuename, retstr )
c$reference
        character*(*) name, queuename
        integer numargs
        record /RXSTRING/ retstr, args(numargs)

        include 'rxsutils.fi'

        integer memstat, len
        character*(*) tmp, arg1

        ! Make sure we have only one argument, and that it's not a
        ! null string...

        if( numargs .ne. 1 )then
            EZFunc2 = INVALID_ROUTINE
            return
        endif
        if( args(1).strptr .eq. 0 .or. args(1).strlength .eq. 0 )then
            EZFunc2 = INVALID_ROUTINE
            return
        endif

        allocate( arg1*args(1).strlength, location=args(1).strptr )

        ! Get some buffer space....

        len = strlen( name ) + args(1).strlength + 100
        allocate( tmp*len, stat=memstat )
        if( memstat .ne. 0 )then
            EZFunc2 = INVALID_ROUTINE
            return
        endif

        ! Return the name of the func and the argument
        write( tmp, '(''EZFunc2 (called as '''''', a, '''''') called with argument '''''', a, '''''''')' )
     &         name(1:strlen(name)), arg1

        call CopyResult( tmp, lentrim( tmp ), retstr )
        deallocate( tmp )

        EZFunc2 = VALID_ROUTINE

        end
