! funcload.for -- Defines the two functions 'EZLoadFuncs' and 'EZDropFuncs',
!                 which load/drop all the functions defined in this DLL.

c$define INCL_REXXSAA
c$include rexxsaa.fap

! Declare our functions.  Make sure to export these in the .LNK file!

c$pragma aux (RexxFunctionHandler) EZLoadFuncs "EZLoadFuncs"
c$pragma aux (RexxFunctionHandler) EZDropFuncs "EZDropFuncs"

! Define the table that lists REXX function names and the corresponding
! DLL entry point.  You must change this table whenever you add/remove
! a function or entry point.

        blockdata
        include 'funcload.fi'
        data RxFncTable/
                ! function      entry point
     &          'EZLoadFuncs'c,  'EZLoadFuncs'c,
     &          'EZDropFuncs'c,  'EZDropFuncs'c,
     &          'EZFunc1'c,      'EZFunc1'c,
     &          'EZFunc2'c,      'EZFunc2'c,
     &          'EZFunc3'c,      'EZFunc2'c/ ! yes, EZFunc2 is intentional!
        end

! EZLoadFuncs -- Register all the functions with REXX.

c$noreference
        integer function EZLoadFuncs( name, numargs, args,
     &                                queuename, retstr )
c$reference
        integer numargs, name, queuename
        record /RXSTRING/ args(numargs), retstr

        include 'rxsutils.fi'
        include 'funcload.fi'

        integer j
        character*9 DLLNAME ! Change this if you're changing the DLL name...
        parameter (DLLNAME = 'EZRXFUNC'c)

        retstr.strlength = 0

        if( numargs .gt. 0 )then
            EZLoadFuncs = INVALID_ROUTINE
            return
        endif

        do j = 1, ENTRIES
            call RexxRegisterFunctionDll( RxFncTable(j).rxName, DLLNAME,
     &                                    RxFncTable(j).cName )
        enddo

        EZLoadFuncs = VALID_ROUTINE

        end

* EZDropFuncs -- Deregister all the functions with REXX.

c$noreference
        integer function EZDropFuncs( name, numargs, args,
     &                                queuename, retstr )
c$reference
        integer numargs, name, queuename
        record /RXSTRING/ args(numargs), retstr

        include 'rxsutils.fi'
        include 'funcload.fi'

        integer j

        retstr.strlength = 0

        if( numargs .gt. 0 )then
            EZDropFuncs = INVALID_ROUTINE
            return
        endif

        do j = 1, ENTRIES
            call RexxDeregisterFunction( RxFncTable(j).rxName )
        enddo

        EZDropFuncs = VALID_ROUTINE

        end
