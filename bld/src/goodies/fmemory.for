* FMEMORY.FOR
* This example shows how to get information about free
* memory using DPMI call 0500h under DOS/4GW using WATCOM
* FORTRAN 77/386.  Note that only the first field of the
* structure is guaranteed to contain a valid value; any
* field not returned by DOS/4GW is set to -1 (0FFFFFFFFh).

* Compile & Link:   set finclude=\watcom\src\fortran\dos
*		    wfl386 /l=dos4g fmemory

* Pragma to get the default data segment

*$pragma aux GetDS = "mov ax,ds" value [ax] modify exact [ax]

      program memory
      implicit none
      include 'dos.fi'

      structure /meminfo/
	  integer*4 LargestBlockAvail
	  integer*4 MaxUnlockedPage
	  integer*4 LargestLockablePage
	  integer*4 LinAddrSpace
	  integer*4 NumFreePagesAvail
	  integer*4 NumPhysicalPagesFree
	  integer*4 TotalPhysicalPages
	  integer*4 FreeLinAddrSpace
	  integer*4 SizeOfPageFile
	  integer*4 Reserved1
	  integer*4 Reserved2
      end structure

* Set up the register information for the interrupt call

      record /meminfo/ MemInfo
      integer interrupt_no
      integer*2 GetDS

      parameter( interrupt_no='31'x)
      ds = es = fs = gs = 0
      EAX = '00000500'x
      ES = GetDS()
      EDI = loc(MemInfo)

      call fintr( interrupt_no, regs)

* Report the information returned by the DPMI host

      print *,'------------------------------------------'
      print *,'Largest available block (in bytes): ',
     &	      Meminfo.LargestBlockAvail
      print *,'Maximum unlocked page allocation: ',
     &	      MemInfo.MaxUnlockedPage
      print *,'Pages that can be allocated and locked: ',
     &	      MemInfo.LargestLockablePage
      print *,'Total linear address space including' //
     &	      ' allocated pages:', MemInfo.LinAddrSpace
      print *,'Number of free pages available: ',
     &	      MemInfo.NumFreePagesAvail
      print *,'Number of physical pages not in use: ',
     &	      MemInfo.NumPhysicalPagesFree
      print *,'Total physical pages managed by host: ',
     &	      MemInfo.TotalPhysicalPages
      print *,'Free linear address space (pages): ',
     &	      MemInfo.FreeLinAddrSpace
      print *,'Size of paging/file partition (pages): ',
     &	      MemInfo.SizeOfPageFile

      end
