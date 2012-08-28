c$ifdef nt
c$pragma aux (__stdcall) Sleep parm( value*4 )
c$pragma aux (__stdcall) CreateMutexA parm( reference, value*4, reference )
c$pragma aux (__stdcall) CloseHandle parm( value*4 )
c$pragma aux (__stdcall) WaitForSingleObject parm( value*4, value*4 )
c$pragma aux (__stdcall) ReleaseMutex parm( value*4 )
c$else
c$pragma aux (__syscall) DosSleep parm( value*4 )
c$pragma aux (__syscall) DosEnterCritSec
c$pragma aux (__syscall) DosExitCritSec
c$pragma aux (__syscall) DosEnterExitSec
c$pragma aux (__syscall) DosKillThread parm( value*4 )
c$endif

        integer NumThreads, MaxThreads
        logical HoldThreads
        common NumThreads, HoldThreads
        volatile NumThreads, HoldThreads
c$ifdef nt
        integer ThreadSem
        common ThreadSem
c$endif

        integer STACK_SIZE
        parameter (STACK_SIZE=16*1024)

        integer i, threadid, beginthread, __getmaxthreads
        external a_thread, dll_thread
c$ifdef nt
        integer CreateMutexA
c$endif

        MaxThreads = __getmaxthreads()
        print '(''main thread id = '', i4 )', threadid()
        NumThreads = 0
        HoldThreads = .true.
c$ifdef nt
        ThreadSem = CreateMutexA( 0, 0, 0 )
c$endif
        ! main thread counts as 1

        call dll_entry_1()
        do i = 2, MaxThreads
            if( beginthread( a_thread, STACK_SIZE ) .eq. -1 )then
                print '(''creation of thread'', i4, ''failed'')', i
            else
                NumThreads = NumThreads + 1
            end if
        end do
        call dll_entry_2()
        call dll_begin_thread( dll_thread )
        HoldThreads = .false.
        while( NumThreads .ne. 0 )do
c$ifdef nt
            call Sleep( 0 )
c$else
            call DosSleep( 0 )
c$endif
        end while
        i = beginthread( a_thread, STACK_SIZE )
        if( i .eq. -1 )then
            print *, 'creation of last thread failed'
        else
            print '(''killing thread'', i4)', i
c$ifdef nt
c$else
            call DosKillThread( i )
c$endif
        endif
c$ifdef nt
        call CloseHandle( ThreadSem )
c$endif
        end


        subroutine a_thread()
        integer NumThreads, threadid
        logical HoldThreads
        common NumThreads, HoldThreads
        volatile NumThreads, HoldThreads
c$ifdef nt
        integer ThreadSem
        common ThreadSem
c$endif
        integer counter
        while( HoldThreads )do
c$ifdef nt
            call Sleep( 0 )
c$else
            call DosSleep( 0 )
c$endif
        end while
        do counter = 1, 10
c$ifdef nt
            call Sleep( 0 )
c$else
            call DosSleep( 0 )
c$endif
            call dll_print( counter )
        end do
c$ifdef nt
        call WaitForSingleObject( ThreadSem, -1 )
c$else
        call DosEnterCritSec()
c$endif
        NumThreads = NumThreads - 1
c$ifdef nt
        call ReleaseMutex( ThreadSem )
c$else
        call DosExitCritSec()
c$endif
        print *, 'Hi from thread', threadid()
*       print *, NumThreads / 0

        call endthread()
        end

        subroutine dll_thread()
        integer threadid
        print '(''Hi from user code called by DLL thread = '', i4 )', threadid()
        end

        integer function __getmaxthreads()
        __getmaxthreads = 32
        end
