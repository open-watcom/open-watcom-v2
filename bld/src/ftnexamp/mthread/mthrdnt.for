* MTHREAD.FOR

*$pragma aux (__stdcall) Sleep parm( value )
*$pragma aux (__stdcall) InitializeCriticalSection parm( reference )
*$pragma aux (__stdcall) DeleteCriticalSection parm( reference )
*$pragma aux (__stdcall) EnterCriticalSection parm( reference )
*$pragma aux (__stdcall) LeaveCriticalSection parm( reference )

        structure /RTL_CRITICAL_SECTION/
            integer*4 DebugInfo
            integer*4 LockCount
            integer*4 RecursionCount
            integer*4 OwningThread
            integer*4 LockSemaphore
            integer*4 Reserved
        end structure

        integer NumThreads
        logical HoldThreads
        volatile HoldThreads, NumThreads
        record /RTL_CRITICAL_SECTION/ CriticalSection
        common NumThreads, HoldThreads, CriticalSection

        integer STACK_SIZE
        parameter (STACK_SIZE=8192)
        integer NUM_THREADS
        parameter (NUM_THREADS=5)

        integer i, threadid, beginthread
        external a_thread

        print '(''main thread id = '',i4)', threadid()
        NumThreads = 0
        HoldThreads = .true.
        ! main thread counts as 1
        call InitializeCriticalSection( CriticalSection )
        do i = 2, NUM_THREADS
            if( beginthread( a_thread, STACK_SIZE ) .eq. -1 )then
                print '(''creation of thread'',i4,''failed'')', i
            else
                NumThreads = NumThreads + 1
            end if
        end do
        HoldThreads = .false.
        while( NumThreads .ne. 0 ) do
            call Sleep( 1 )
        end while
        call DeleteCriticalSection( CriticalSection )
        end

        subroutine a_thread()

        structure /RTL_CRITICAL_SECTION/
            integer*4 DebugInfo
            integer*4 LockCount
            integer*4 RecursionCount
            integer*4 OwningThread
            integer*4 LockSemaphore
            integer*4 Reserved
        end structure

        integer NumThreads
        logical HoldThreads
        volatile HoldThreads
        record /RTL_CRITICAL_SECTION/ CriticalSection
        common NumThreads, HoldThreads, CriticalSection

        integer threadid

        while( HoldThreads ) do
            call Sleep( 1 )
        end while
        print '(''Hi from thread '', i4)', threadid()
        call EnterCriticalSection( CriticalSection )
        NumThreads = NumThreads - 1
        call LeaveCriticalSection( CriticalSection )
        call endthread()
        end
