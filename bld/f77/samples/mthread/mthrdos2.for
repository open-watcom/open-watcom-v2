* MTHREAD.FOR
c$define INCL_DOS
c$include os2.fap

      integer NumThreads
      logical HoldThreads
      common NumThreads, HoldThreads

      integer STACK_SIZE
      parameter (STACK_SIZE=32768)
      integer NUM_THREADS
      parameter (NUM_THREADS=5)

      integer i, threadid, beginthread
      external a_thread

      print '(''main thread id = '', i4)', threadid()
      NumThreads = 0
      HoldThreads = .true.
      ! main thread counts as 1
      do i = 2, NUM_THREADS
          if( beginthread( a_thread, STACK_SIZE ) .eq. -1 )then
              print '(''creation of thread'', i4, ''failed'')', i
          else
              NumThreads = NumThreads + 1
          end if
      end do
      HoldThreads = .false.
      while( NumThreads .ne. 0 )do
          call DosSleep( 1 )
      end while
      end

      subroutine a_thread()
      integer NumThreads
      logical HoldThreads
      common NumThreads, HoldThreads
      integer threadid
      while( HoldThreads )do
          call DosSleep( 1 )
      end while
      call DosEnterCritSec()
      print '(''Hi from thread '', i4)', threadid()
      NumThreads = NumThreads - 1
      call DosExitCritSec()
      call endthread()
      end
