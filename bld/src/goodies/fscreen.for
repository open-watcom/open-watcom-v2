* FSCREEN.FOR
* The following program shows how to access screen memory
* from a FORTRAN program under the DOS/4GW DOS extender.

* Compile & Link: wfl386 /l=dos4g fscreen

      program fscreen

* Allocatable arrays must be declared by specifying their
* dimensions using colons only (see WATCOM FORTRAN 77
* Language Reference on the ALLOCATE statement for details).

      character*1 screen(:,:)
      integer SCRSIZE, i

      parameter ( SCRSIZE = 80*25 )

* Under DOS/4GW, the first megabyte of physical memory - the
* real memory - is mapped as a shared linear address space.
* This allows your application to access video RAM using its
* linear address.  The DOS segment:offset of B800:0000
* corresponds to a linear address of B8000.

      allocate( screen(0:1,0:SCRSIZE-1), location='B8000'x )

      do i = 0, SCRSIZE - 1
	  screen(0,i) = '*'
      enddo

      end
