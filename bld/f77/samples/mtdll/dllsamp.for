c$ifdef nt
c$pragma aux (__stdcall) Sleep parm( value*4 )
c$else
c$pragma aux (__syscall) DosSleep parm( value*4 )
c$endif

        subroutine dll_entry_1()
        print *, 'hi from dll entry #1'
        end

        subroutine dll_entry_2()
        print *, 'hi from dll entry #2'
        end

        subroutine dll_print( counter )
        integer counter, threadid
        print '(''Hi from thread '', i4, '' counter = '', i4 )',
     &        threadid(), counter
        end


c$pragma aux call_thread parm( value )
        subroutine call_thread( rtn )
        integer rtn
        call rtn()
        end


        subroutine dll_thread()
        logical WaitForThread
        volatile WaitForThread
        integer my_exe_thread, threadid
        common WaitForThread, my_exe_thread
        print '(''Hi from thread '', i4, '' started in DLL'')',
     &        threadid()
        call call_thread( my_exe_thread )
        WaitForThread = .false.
        end


        subroutine dll_begin_thread( exe_thread )
        external exe_thread

        logical WaitForThread
        volatile WaitForThread
        common WaitForThread, my_exe_thread
        integer threadid, beginthread, my_exe_thread
        external dll_thread

        integer STACK_SIZE
        parameter (STACK_SIZE=16*1024)

        print '(''Hi from thread '', i4, '' starting new thread in DLL'')',
     &        threadid()
        WaitForThread = .true.
        my_exe_thread = loc( exe_thread )
        if( beginthread( dll_thread, STACK_SIZE ) .eq. -1 )then
            print *, 'creation of DLL thread failed'
            WaitForThread = .false.
        end if
        while( WaitForThread )do
c$ifdef nt
            call Sleep( 0 )
c$else
            call DosSleep( 0 )
c$endif
        end while
        print *, 'DLL thread terminated'
        end


        integer function __fdll_initialize_()
        print *, 'DLL initialized'
        __fdll_initialize_ = 1
        end


        integer function __fdll_terminate_()
        print *, 'DLL terminated'
        __fdll_terminate_ = 1
        end
