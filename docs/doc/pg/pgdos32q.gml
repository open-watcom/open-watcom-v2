.chap 32-bit Extended DOS Application Development
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'DOS extenders' 'common problems'
The purpose of this chapter is to anticipate common programming
questions for 32-bit extended DOS application development. Note that
these programming solutions may be DOS-extender specific and therefore
may not work for other DOS extenders.
.np
The following topics are discussed in this chapter:
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.begbull
.bull
How can I write directly to video memory using a DOS extender?
.bull
How do I get information about free memory in the 32-bit environment?
.bull
How do I access the first megabyte in the extended DOS environment?
.bull
How do I spawn a protected-mode application?
.bull
How can I use the mouse interrupt (0x33) with DOS/4GW?
.bull
How do I simulate a real-mode interrupt with DOS/4GW?
.bull
How do you install a bi-modal interrupt handler with DOS/4GW?
.endbull
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.begbull
.bull
How can I write directly to video memory using DOS/4GW?
.bull
How do I issue interrupts in a DOS/4GW application?
.bull
How do I get information about free memory with DOS/4GW?
.endbull
.do end
.np
.ix 'DPMI specification'
Please refer to the
.book DOS Protected-Mode Interface (DPMI) Specification
for information on DPMI services.
In the past, the DPMI specification could be obtained free of charge
by contacting Intel Literature JP26 at 800-548-4725 or by writing
to the address below.
We have been advised that the DPMI specification is no longer
available in printed form.
.illust begin
Intel Literature JP26
3065 Bowers Avenue
P.O. Box 58065
Santa Clara, California
U.S.A. 95051-8065
.illust end
.np
However, the DPMI 1.0 specification can be obtained from the Intel ftp
site.
Here is the URL.
.code begin
ftp://ftp.intel.com/pub/IAL/software_specs/dpmiv1.zip
.code end
.pc
This ZIP file contains a Postscript version of the DPMI 1.0 specification.
.*
.section How can I write directly to video memory using a DOS extender?
.*
.np
.ix 'video memory'
Many programmers require access to video RAM in order to directly
manipulate data on the screen.
Under DOS, it was standard practice to use a far pointer, with the
segment part of the far pointer set to the screen segment.
Under DOS extenders, this practice is not so standard.
Each DOS extender provides its own method for accessing video memory.
.if '&lang' eq 'FORTRAN 77' .do begin
The following program demonstrates the method used with DOS/4GW.
.code begin
* FSCREEN.FOR
* The following program shows how to access screen memory
* from a FORTRAN program under the DOS/4GW DOS extender.

* Compile & Link: wfl386 -l=dos4g fscreen

      program screen

* Allocatable arrays must be declared by specifying their
* dimensions using colons only (see &company FORTRAN 77
* Language Reference on the ALLOCATE statement for details).

      character*1 screen(:,:)
      integer SCRSIZE, i

      parameter ( SCRSIZE = 80*25 )
.code break

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
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.beglevel
.section Writing to Video Memory under Tenberry Software DOS/4GW
.*
.np
.ix 'video memory' 'using DOS/4GW'
Under DOS/4GW, the first megabyte of physical memory is mapped as a
shared linear address space.
This allows your application to access video RAM using a near pointer
set to the screen's linear address.
The following program demonstrates this method.
.millust begin
/*
    SCREEN.C - This example shows how to write directly
    to screen memory under the DOS/4GW dos-extender.

    Compile & Link: wcl386 -l=dos4g SCREEN
 */
#include <stdio.h>
#include <dos.h>

/*
  Under DOS/4GW, the first megabyte of physical memory
  (real-mode memory) is mapped as a shared linear address
  space. This allows your application to access video RAM
  using its linear address.  The DOS segment:offset of
  B800:0000 corresponds to a linear address of B8000.
 */
#define SCREEN_AREA 0xb800
#define SCREEN_LIN_ADDR ((SCREEN_AREA) << 4)
#define SCREEN_SIZE 80*25
.millust break

void main()
{
    char       *ptr;
    int         i;

    /* Set the pointer to the screen's linear address */
    ptr = (char *)SCREEN_LIN_ADDR;
    for( i = 0; i < SCREEN_SIZE - 1; i++ ) {
        *ptr = '*';
        ptr += 2 * sizeof( char );
    }
}
.millust end
.np
Please refer to the chapter entitled :HDREF refid='linexe'. for more
information on how DOS/4GW maps the first megabyte.
.*
.section Writing to Video Memory under the Phar Lap 386|DOS-Extender
.*
.np
.ix 'video memory' 'using Phar Lap'
The Phar Lap DOS extender provides screen access through the special
segment selector 0x1C.
This allows far pointer access to video RAM from a 32-bit program.
The following example illustrates this technique.
.millust begin
/*
    SCREENPL.C - This example shows how to write directly
    to screen memory under the Phar Lap DOS extender.

    Compile & Link: wcl386 -l=pharlap SCREENPL
 */
#include <stdio.h>
#include <dos.h>

/*
    Phar Lap allows access to screen memory through a
    special selector.  Refer to "Hardware Access" in
    Phar Lap's documentation for details.
 */
#define PL_SCREEN_SELECTOR 0x1c
#define SCREEN_SIZE 80*25
.millust break

void main()
{
    /* Need a far pointer to use the screen selector */
    char far   *ptr;
    int         i;

    /* Make a far pointer to screen memory */
    ptr = MK_FP( PL_SCREEN_SELECTOR, 0 );
    for( i = 0; i < SCREEN_SIZE - 1; i++ ) {
        *ptr = '*';
        ptr += 2 * sizeof( char );
    }
}
.millust end
.np
It is also possible to map screen memory into your near memory using
Phar Lap system calls.
Please refer to the chapter entitled "386|DOS-Extender System Calls"
in Phar Lap's
.book 386|DOS-Extender Reference Manual
for details.
.*
.endlevel
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section  How do I issue interrupts in a DOS/4GW application?
.*
.np
.ix 'interrupts' 'using DOS/4GW'
The &cmpname library files contain the FINTR subroutine which allows
the user to perform interrupt calls within a &lang program.
This subroutine is described in the Subprogram Library section of the
.book &product User's Guide.
.np
The following sample program illustrates the use of the FINTR subroutine
to set up the register information required for Interrupt 21h.
The register information is loaded into the regs structure.  This structure
is defined in the DOS.FI file located in the \WATCOM\SRC\FORTRAN\DOS
directory.  Assign values to the register elements according to the
interrupt call requirements.  For example, Interrupt 21h, function 4Eh
needs valid values for the AH, ECX, DS and EDX to set up the registers
for the Interrupt 21h call.  This procedure can be used to perform
any interrupt calls that are supported in protected mode by DOS/4GW.
.*
.code begin
* DTA.FOR
* This program demonstrates the use of the FINTR
* function to list the files of the current directory.
* Interrupt 21 Functions for FIND FIRST, FIND NEXT,
* and GET DTA are used.

* Compile & Link: set finclude=\watcom\src\fortran\dos
*                 wfl386 -l=dos4g dta

*$pragma aux GetDS = "mov ax,ds" value [ax]

        program dta
        implicit integer*2 (i-n)
        integer*2 res
        integer*2 GetDS
        integer*4 dir,addr
        integer*1 dta(:)
        character fname*1(12), fname2*12
        equivalence (fname, fname2)
.code break

* DTA is declared as a FAR array.  When referencing an array
* element, the pointer to the array is a FAR pointer.  With a
* character variable, the result is a pointer to a string
* control block (SCB).  The run-time library expects the SCB
* to contain a near pointer.  To get around the problem, we
* define the DTA as a byte array, then use the CHAR function
* to get the character equivalent for printing a filename.

*$pragma array dta far
.code break

        include 'dos.fi'
*
* Listing of current directory
*
        call fsystem( 'dir/w *.*'//char(0) )
        dir = loc( '*.*'//char(0) )

        i = 0
10      i = i + 1
.code break
        if( i .eq. 1 )then
*
* Find first file
*
            AH = '4E'x
            ECX = 0
            DS = GetDS()
            EDX = dir
        else
*
* Find next file
*
            AH = '4F'x
        endif
        call fintr( '21'x, regs )
        res = AX
.code break

        if( res .eq. 0 )then
*
* Extract filename from DTA
*
            AH = '2F'x
            call fintr( '21'x, regs )

            addr = ISHL( IAND( INT( ES ), '0000FFFF'x ), 16 )
            addr = IOR( addr, IAND( INT( BX ), '0000FFFF'x ) )
            allocate( dta(0:42), location=addr )
            fname2 = ' '
            do j = 30, 41
                if( dta(j) .eq. 0 ) goto 20
                fname(j - 29) = char( dta(j) )
            enddo
20          print *, fname2
            deallocate( dta )
            goto 10
        endif

        end
.code end
.do end
.*
.section How do I get information about free memory in the 32-bit environment?
.*
.np
.ix 'free memory'
Under a virtual memory system, programmers are often interested
in the amount of physical memory they can allocate.
Information about the amount of free memory that is available is always
provided under a DPMI host, however,
the manner in which this information is provided may differ
under various environments.
Keep in mind that in a multi-tasking environment, the information
returned to your task from the DPMI host can easily become obsolete
if other tasks allocate memory independently of your task.
.if '&lang' eq 'FORTRAN 77' .do begin
.np
DOS/4GW provides a DPMI interface through interrupt 0x31.
This allows you to use DPMI service 0x0500 to get free memory
information.
The following program illustrates this procedure.
.code begin
* FMEMORY.FOR
* This example shows how to get information about free
* memory using DPMI call 0500h under DOS/4GW using &company
* FORTRAN 77.  Note that only the first field of the
* structure is guaranteed to contain a valid value; any
* field not returned by DOS/4GW is set to -1 (0FFFFFFFFh).

* Compile & Link:   set finclude=\watcom\src\fortran\dos
*                   wfl386 -l=dos4g fmemory

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
.code break

* Set up the register information for the interrupt call

      record /meminfo/ MemInfo
      integer interrupt_no
      integer*2 GetDS

      parameter( interrupt_no='31'x)
      DS = FS = GS = 0
      EAX = '00000500'x
      ES = GetDS()
      EDI = loc(MemInfo)

      call fintr( interrupt_no, regs)
.code break

* Report the information returned by the DPMI host

      print *,'------------------------------------------'
      print *,'Largest available block (in bytes): ',
     &        Meminfo.LargestBlockAvail
      print *,'Maximum unlocked page allocation: ',
     &        MemInfo.MaxUnlockedPage
      print *,'Pages that can be allocated and locked: ',
     &        MemInfo.LargestLockablePage
      print *,'Total linear address space including' //
     &        ' allocated pages:', MemInfo.LinAddrSpace
      print *,'Number of free pages available: ',
     &        MemInfo.NumFreePagesAvail
      print *,'Number of physical pages not in use: ',
     &        MemInfo.NumPhysicalPagesFree
      print *,'Total physical pages managed by host: ',
     &        MemInfo.TotalPhysicalPages
      print *,'Free linear address space (pages): ',
     &        MemInfo.FreeLinAddrSpace
      print *,'Size of paging/file partition (pages): ',
     &        MemInfo.SizeOfPageFile

      end
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.beglevel
.section Getting Free Memory Information under DOS/4GW
.*
.np
.ix 'free memory' 'using DOS/4GW'
DOS/4GW provides a DPMI interface through interrupt 0x31.
This allows you to use DPMI service 0x0500 to get free memory
information.
The following program illustrates this procedure.
.millust begin
/*
    MEMORY.C - This example shows how to get information
    about free memory using DPMI call 0500h under DOS/4GW.
    Note that only the first field of the structure is
    guaranteed to contain a valid value; any field that
    is not returned by DOS/4GW is set to -1 (0FFFFFFFFh).

    Compile & Link: wcl386 -l=dos4g memory
 */
#include <i86.h>
#include <dos.h>
#include <stdio.h>

#define DPMI_INT        0x31

.millust break
struct meminfo {
    unsigned LargestBlockAvail;
    unsigned MaxUnlockedPage;
    unsigned LargestLockablePage;
    unsigned LinAddrSpace;
    unsigned NumFreePagesAvail;
    unsigned NumPhysicalPagesFree;
    unsigned TotalPhysicalPages;
    unsigned FreeLinAddrSpace;
    unsigned SizeOfPageFile;
    unsigned Reserved[3];
} MemInfo;

.millust break
void main()
{
    union REGS regs;
    struct SREGS sregs;

    regs.x.eax = 0x00000500;
    memset( &sregs, 0, sizeof(sregs) );
    sregs.es = FP_SEG( &MemInfo );
    regs.x.edi = FP_OFF( &MemInfo );

.millust break
    int386x( DPMI_INT, &regs, &regs, &sregs );
    printf( "Largest available block (in bytes): %lu\n",
            MemInfo.LargestBlockAvail );
    printf( "Maximum unlocked page allocation: %lu\n",
            MemInfo.MaxUnlockedPage );
    printf( "Pages that can be allocated and locked: "
            "%lu\n", MemInfo.LargestLockablePage );
    printf( "Total linear address space including "
            "allocated pages: %lu\n",
            MemInfo.LinAddrSpace );
    printf( "Number of free pages available: %lu\n",
             MemInfo.NumFreePagesAvail );
    printf( "Number of physical pages not in use: %lu\n",
             MemInfo.NumPhysicalPagesFree );
    printf( "Total physical pages managed by host: %lu\n",
             MemInfo.TotalPhysicalPages );
    printf( "Free linear address space (pages): %lu\n",
             MemInfo.FreeLinAddrSpace );
    printf( "Size of paging/file partition (pages): %lu\n",
             MemInfo.SizeOfPageFile );
}
.millust end
.np
.ix 'INT 31H'
Please refer to the chapter entitled :HDREF refid='int31'. for more
information on DPMI services.
.*
.section Getting Free Memory Information under the Phar Lap 386|DOS-Extender
.*
.np
.ix 'free memory' 'using Phar Lap'
Phar Lap provides memory statistics through 386|DOS-Extender System
Call 0x2520.
The following example illustrates how to use this system call from a
32-bit program.
.millust begin
/*
    MEMPLS40.C - This is an example of how to get the
    amount of physical memory present under Phar Lap
    386|DOS-Extender v4.0.

    Compile & Link: wcl386 -l=pharlap MEMPLS40
 */
#include <dos.h>
#include <stdio.h>

typedef struct {
    unsigned data[25];
} pharlap_mem_status;

.millust break
/* Names suggested in Phar Lap documentation */
#define APHYSPG     5
#define SYSPHYSPG   7
#define NFREEPG     21

.millust break
unsigned long memavail( void )
{
    pharlap_mem_status status;
    union REGS regs;
    unsigned long amount;

.millust break
    regs.h.ah = 0x25;
    regs.h.al = 0x20;
    regs.h.bl = 0;
    regs.x.edx = (unsigned int) &status;
    intdos( &regs, &regs );
    /* equation is given in description for nfreepg */
    amount = status.data[ APHYSPG ];
    amount += status.data[ SYSPHYSPG ];
    amount += status.data[ NFREEPG ];
    return( amount * 4096 );
}

.millust break
void main()
{
    printf( "%lu bytes of memory available\n",
            memavail() );
}
.millust end
.np
Please refer to the chapter entitled "386|DOS-Extender System Calls"
in Phar Lap's
.book 386|DOS-Extender Reference Manual
for more information on 386|DOS-Extender System Calls.
.*
.section Getting Free Memory Information in the 32-bit Environment under Windows 3.x
.*
.np
.ix 'free memory' 'using Windows 3.x'
Windows 3.x provides a DPMI host that you can access from a 32-bit
program.
The interface to this host is a 16-bit interface, hence there are some
considerations involved when calling Windows 3.x DPMI services from
32-bit code.
If a pointer to a data buffer is required to be passed in ES:DI, for
example, an AllocAlias16() may be used to get a 16-bit far pointer
that can be passed to Windows 3.x through these registers.
Also, an int86() call should be issued rather than an int386() call.
The following program demonstrates the techniques mentioned above.
.millust begin
/*
    MEMWIN.C - This example shows how to get information
    about free memory with DPMI call 0x0500 using Windows
    as a DPMI host.  Note that only the first field of the
    structure is guaranteed to contain a valid value; any
    field that is not returned by the DPMI implementation
    is set to -1 (0FFFFFFFFh).

    Compile & Link: wcl386 -l=win386 -zw memwin
    Bind: wbind -n memwin
 */
#include <windows.h>
#include <i86.h>
#include <dos.h>
#include <stdio.h>

.millust break
struct meminfo {
    unsigned LargestBlockAvail;
    unsigned MaxUnlockedPage;
    unsigned LargestLockablePage;
    unsigned LinAddrSpace;
    unsigned NumFreePagesAvail;
    unsigned NumPhysicalPagesFree;
    unsigned TotalPhysicalPages;
    unsigned FreeLinAddrSpace;
    unsigned SizeOfPageFile;
    unsigned Reserved[3];
} MemInfo;

#define DPMI_INT        0x31
.millust break

void main()
{
    union REGS regs;
    struct SREGS sregs;
    DWORD mi_16;

    regs.w.ax = 0x0500;
    mi_16 = AllocAlias16( &MemInfo );
    sregs.es = HIWORD( mi_16 );
    regs.x.di = LOWORD( mi_16 );

.millust break
    int86x( DPMI_INT, &regs, &regs, &sregs );
    printf( "Largest available block (in bytes): %lu\n",
            MemInfo.LargestBlockAvail );
    printf( "Maximum unlocked page allocation: %lu\n",
            MemInfo.MaxUnlockedPage );
    printf( "Pages that can be allocated and locked: "
            "%lu\n", MemInfo.LargestLockablePage );
    printf( "Total linear address space including "
            "allocated pages: %lu\n",
            MemInfo.LinAddrSpace );
    printf( "Number of free pages available: %lu\n",
             MemInfo.NumFreePagesAvail );
    printf( "Number of physical pages not in use: %lu\n",
             MemInfo.NumPhysicalPagesFree );
    printf( "Total physical pages managed by host: %lu\n",
             MemInfo.TotalPhysicalPages );
    printf( "Free linear address space (pages): %lu\n",
             MemInfo.FreeLinAddrSpace );
    printf( "Size of paging/file partition (pages): %lu\n",
             MemInfo.SizeOfPageFile );
    FreeAlias16( mi_16 );
}
.millust end
.np
.ix 'DPMI specification'
Please refer to the
.book DOS Protected-Mode Interface (DPMI) Specification
for information on DPMI services.
In the past, the DPMI specification could be obtained free of charge
by contacting Intel Literature JP26 at 800-548-4725 or by writing
to the address below.
We have been advised that the DPMI specification is no longer
available in printed form.
.illust begin
Intel Literature JP26
3065 Bowers Avenue
P.O. Box 58065
Santa Clara, California
U.S.A. 95051-8065
.illust end
.np
However, the DPMI 1.0 specification can be obtained from the Intel ftp
site.
Here is the URL.
.code begin
ftp://ftp.intel.com/pub/IAL/software_specs/dpmiv1.zip
.code end
.pc
This ZIP file contains a Postscript version of the DPMI 1.0 specification.
.*
.endlevel
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section How do I access the first megabyte in the extended DOS environment?
.*
.np
.ix 'DOS memory'
.ix 'real-mode memory'
Many programmers require access to the first megabyte of memory in
order to look at key low memory addresses.
Under DOS, it was standard practice to use a far pointer, with the far
pointer set to the segmented address of the memory area that was being
inspected.
Under DOS extenders, this practice is not so standard.
Each DOS extender provides its own method for accessing the first
megabyte of memory.
.*
.beglevel
.*
.section Accessing the First Megabyte under Tenberry Software DOS/4GW
.*
.np
.ix 'DOS memory' 'using DOS/4GW'
.ix 'real-mode memory' 'using DOS/4GW'
Under DOS/4GW, the first megabyte of physical memory - the real memory
- is mapped as a shared linear address space.
This allows your application to access the first megabyte of memory
using a near pointer set to the linear address.
The following program demonstrates this method.
This example is similar to the screen memory access example.
.millust begin
/*
    KEYSTAT.C - This example shows how to get the keyboard
    status under DOS/4GW by looking at the ROM BIOS
    keyboard status byte in low memory.

    Compile & Link: wcl386 -l=dos4g keystat
 */
#include <stdio.h>
#include <dos.h>

.millust break
/*
    Under DOS, the keyboard status byte has a segmented
    address of 0x0040:0x0017.  This corresponds to a
    linear address of 0x417.
 */
#define LOW_AREA 0x417

.millust break
void main()
{
    /* Only need a near pointer in the flat model */
    char *ptr;

    /* Set pointer to linear address of the first
       status byte */
    ptr = (char *)LOW_AREA;

.millust break
    /* Caps lock state is in bit 6 */
    if( *ptr & 0x40 ) {
        puts( "Caps Lock on" );
    }
.millust break
    /* Num lock state is in bit 5 */
    if( *ptr & 0x20 ) {
        puts( "Num Lock on" );
    }
.millust break
    /* Scroll lock state is in bit 4 */
    if( *ptr & 0x10 ) {
        puts( "Scroll Lock on" );
    }
}
.millust end
.np
Please refer to the chapter entitled :HDREF refid='linexe'. for more
information on how DOS/4GW maps the first megabyte.
.*
.section Accessing the First Megabyte under the Phar Lap 386|DOS-Extender
.*
.np
.ix 'DOS memory' 'using Phar Lap'
.ix 'real-mode memory' 'using Phar Lap'
The Phar Lap DOS extender provides access to real memory through the
special segment selector 0x34.
This allows far pointer access to the first megabyte from a 32-bit
program.
The following example illustrates this technique.
.millust begin
/*
    KEYSTAPL.C - This example shows how to get the keyboard
    status under 386|DOS-Extender by looking at the ROM
    BIOS keyboard status byte in low memory.

    Compile & Link: wcl386 -l=pharlap keystapl
 */
#include <stdio.h>
#include <dos.h>

.millust break
/*
    Under DOS, the keyboard status byte has a segmented
    address of 0x0040:0x0017.  This corresponds to a
    linear address of 0x417.
 */

.millust break
void main()
{
    /* We require a far pointer to use selector
       for 1st megabyte */
    char far *ptr;
.millust break

    /* Set pointer to segmented address of the first
       status byte */
    ptr = MK_FP( 0x34, 0x417 );
.millust break

    /* Caps lock state is in bit 6 */
    if( *ptr & 0x40 ) {
        puts( "Caps Lock on" );
    }
.millust break
    /* Num lock state is in bit 5 */
    if( *ptr & 0x20 ) {
        puts( "Num Lock on" );
    }
.millust break
    /* Scroll lock state is in bit 4 */
    if( *ptr & 0x10 ) {
        puts( "Scroll Lock on" );
    }
}
.millust end
.np
Please refer to the chapter entitled "Program Environment"
in Phar Lap's
.book 386|DOS-Extender Reference Manual
for more information on segment selectors available to your program.
.*
.endlevel
.*
.section How do I spawn a protected-mode application?
.*
.np
.ix 'spawn'
Sometimes applications need to spawn other programs as part of their
execution.
In the extended DOS environment, spawning tasks is much the same as
under DOS, however it should be noted that the only mode supported is
P_WAIT.
The P_OVERLAY mode is not supported since the DOS extender cannot be
removed from memory by the application (this is also the reason why
the exec() functions are unsupported).
The other modes are for concurrent operating systems only.
.np
Also, unless the application being spawned is bound or stubbed, the
DOS extender must be spawned with the application and its arguments
passed in the parameter list.
.*
.beglevel
.section Spawning Protected-Mode Applications Under Tenberry Software DOS/4GW
.*
.np
.ix 'spawn' 'using DOS/4GW'
In the case of DOS/4GW, some real-mode memory must be set aside at run
time for spawning the DOS extender, otherwise the spawning application
could potentially allocate all of system memory.
The real memory can be reserved from within your program by assigning
the global variable
.mono __minreal
the number of bytes to be set aside.
This variable is referenced in
.mono <stdlib.h>.
The following two programs demonstrate how to spawn a DOS/4GW
application.
.millust begin
/*
    SPWNRD4G.C - The following program demonstrates how to
    spawn another DOS/4GW application.

    Compile and link: wcl386 -l=dos4g spwnrd4g
 */
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

/* DOS/4GW var for WLINK MINREAL option */
unsigned __near __minreal = 100*1024;

.millust break
void main()
{
    int app2_exit_code;

    puts( "Spawning a protected-mode application..."
          "using spawnlp() with P_WAIT" );
    app2_exit_code = spawnlp( P_WAIT, "dos4gw",
                            "dos4gw", "spwndd4g", NULL );
    printf( "Application #2 returned with exit code %d\n",
                            app2_exit_code );
}
.millust end
.millust begin
/*
    SPWNDD4G.C - Will be spawned by the SPWNRD4G program.

    Compile & Link: wcl386 -l=dos4g spwndd4g
 */
#include <stdio.h>
#include <stdlib.h>

.millust break
void main()
{
    puts( "\nApplication #2 spawned\n" );
    /* Send back exit code 59 */
    exit( 59 );
}
.millust end
.*
.section Spawning Protected-Mode Applications Under Phar Lap 386|DOS-Extender
.*
.np
.ix 'spawn' 'using Phar Lap'
In the case of the Phar Lap 386|DOS-Extender, some real-mode memory
must be set aside at link time for spawning the DOS extender,
otherwise the spawning application will be assigned all the system
memory at startup.
This is done at link time by specifying the
.bd runtime minreal
and
.bd runtime maxreal
options, as demonstrated by the following programs.
.millust begin
/*
    SPWNRPLS.C - The following program demonstrates how to
    spawn a Phar Lap application.

    Compile & Link:
    wcl386 -l=pharlap -"runt minr=300K,maxr=400K" spwnrpls
 */
#include <process.h>
#include <stdio.h>

.millust break
void main()
{
    int app2_exit_code;

    puts( "Spawning a protect-mode application..."
          "using spawnlp() with P_WAIT" );
    puts( "Spawning application #2..." );
    app2_exit_code = spawnlp( P_WAIT, "run386",
                            "run386", "spwndpls", NULL );

    printf( "Application #2 returned with exit code %d",
                app2_exit_code );
}
.millust end
.millust begin
/*
    SPWNDPLS.C - Will be spawned by the SPWNRPLS program.

    Compile & Link: wcl386 -l=pharlap spwndpls
 */
#include <stdio.h>
#include <stdlib.h>

void main()
{
    puts( "\nApplication #2 spawned\n" );
    /* Exit with error code 59 */
    exit( 59 );
}
.millust end
.endlevel
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section How Can I Use the Mouse Interrupt (0x33) with DOS/4GW?
.*
.np
.ix 'INT 33H' 'using DOS/4GW'
.ix 'mouse interrupt' 'using DOS/4GW'
Several commonly used interrupts are automatically supported in
protected mode with DOS/4GW.
The DOS extender handles the switch from protected mode to real mode
and manages any intermediate real-mode data buffers that are required.
To use a supported interrupt, set up the register information as
required for the interrupt and use one of the int386() or int386x()
library functions to execute the interrupt.
For calls that are not supported by DOS/4GW, you can use the DPMI
function, Simulate a Real-Mode Interrupt (0x0300).
This process is described in the next section.
.np
Since the mouse interrupt (0x33) is quite commonly used, DOS/4GW
provides protected-mode support for the interrupt and any mouse data
buffer that is required.
The following example demonstrates how a programmer could use the
Microsoft standard mouse interrupt (0x33) from within a DOS/4GW
application.
.code begin
/*
        mouse.c - The following program demonstrates how
        to use the mouse interrupt (0x33) with DOS/4GW.

        Compile and link: wcl386 -l=dos4g mouse
*/
#include <stdio.h>
#include <dos.h>
#include <i86.h>

.code break
/* Data touched at mouse callback time -- they are
   in a structure to simplify calculating the size
   of the region to lock.
*/
struct callback_data {
    int             right_button;
    int             mouse_event;
    unsigned short  mouse_code;
    unsigned short  mouse_bx;
    unsigned short  mouse_cx;
    unsigned short  mouse_dx;
    signed short    mouse_si;
    signed short    mouse_di;
} cbd = { 0 };

.code break
/* Set up data buffer for mouse cursor bitmap */
unsigned short cursor[] = {
    /* 16 words of screen mask */
    0x3fff,  /*0011111111111111*/
    0x1fff,  /*0001111111111111*/
    0x0fff,  /*0000111111111111*/
    0x07ff,  /*0000011111111111*/
    0x03ff,  /*0000001111111111*/
    0x01ff,  /*0000000111111111*/
    0x00ff,  /*0000000011111111*/
    0x007f,  /*0000000001111111*/
    0x01ff,  /*0000000111111111*/
    0x10ff,  /*0001000011111111*/
    0xb0ff,  /*1011000011111111*/
    0xf87f,  /*1111100001111111*/
    0xf87f,  /*1111100001111111*/
    0xfc3f,  /*1111110000111111*/
    0xfc3f,  /*1111110000111111*/
    0xfe1f,  /*1111111000011111*/

.code break
    /* 16 words of cursor mask */
    0x0000,  /*0000000000000000*/ 
    0x4000,  /*0100000000000000*/
    0x6000,  /*0110000000000000*/
    0x7000,  /*0111000000000000*/
    0x7800,  /*0111100000000000*/
    0x7c00,  /*0111110000000000*/
    0x7e00,  /*0111111000000000*/
    0x7f00,  /*0111111100000000*/
    0x7c00,  /*0111110000000000*/
    0x4600,  /*0100011000000000*/
    0x0600,  /*0000011000000000*/
    0x0300,  /*0000001100000000*/
    0x0300,  /*0000001100000000*/
    0x0180,  /*0000000110000000*/
    0x0180,  /*0000000110000000*/
    0x00c0,  /*0000000011000000*/
};

.code break
int lock_region( void *address, unsigned length )
{
    union REGS      regs;
    unsigned        linear;

    /* Thanks to DOS/4GW's zero-based flat memory
       model, converting a pointer of any type to
       a linear address is trivial.
    */
    linear = (unsigned)address;

    /* DPMI Lock Linear Region */
    regs.w.ax = 0x600;
    /* Linear address in BX:CX */
    regs.w.bx = (unsigned short)(linear >> 16); 
    regs.w.cx = (unsigned short)(linear & 0xFFFF);
    /* Length in SI:DI */
    regs.w.si = (unsigned short)(length >> 16);
    regs.w.di = (unsigned short)(length & 0xFFFF);
    int386( 0x31, &regs, &regs );
    /* Return 0 if lock failed */
    return( !regs.w.cflag );
}

.code break
#pragma off( check_stack )
void _loadds far click_handler( int max, int mbx,
                                int mcx, int mdx,
                                int msi, int mdi )
{
#pragma aux click_handler parm [EAX] [EBX] [ECX] \
                               [EDX] [ESI] [EDI]
    cbd.mouse_event = 1;

    cbd.mouse_code = (unsigned short)max;
    cbd.mouse_bx   = (unsigned short)mbx;
    cbd.mouse_cx   = (unsigned short)mcx;
    cbd.mouse_dx   = (unsigned short)mdx;
    cbd.mouse_si   = (signed short)msi;
    cbd.mouse_di   = (signed short)mdi;
    if( cbd.mouse_code & 8 )
        cbd.right_button = 1;
}

.code break
/* Dummy function so we can calculate size of
  code to lock (cbc_end - click_handler).
*/
void cbc_end( void )    
{
}
#pragma on( check_stack )

.code break
void main (void)
{
    struct SREGS        sregs;
    union REGS          inregs, outregs;
    int                 installed = 0;
    unsigned char       orig_mode = 0;
    unsigned short far  *ptr;
    void (far *function_ptr)();

.code break
    segread( &sregs );

    /* get original video mode */

    inregs.w.ax = 0x0f00;
    int386( 0x10, &inregs, &outregs );
    orig_mode = outregs.h.al;

    /* goto graphics mode */

    inregs.h.ah = 0x00;
    inregs.h.al = 0x6;
    int386( 0x10, &inregs, &outregs );

    printf( "Previous Mode = %u\n", orig_mode );
    printf( "Current Mode = %u\n", inregs.h.al );

.code break
    /* check for mouse driver */

    inregs.w.ax = 0;
    int386( 0x33, &inregs, &outregs );
    if( installed = (outregs.w.ax == 0xffff) )
        printf( "Mouse installed...\n" );
    else
        printf( "Mouse NOT installed...\n" );

.code break
    if( installed ) {
        /* lock callback code and data (essential under VMM!)
           note that click_handler, although it does a far
           return and is installed using a full 48-bit pointer,
           is really linked into the flat model code segment
           -- so we can use a regular (near) pointer in the
           lock_region() call.
        */
        if( (! lock_region( &cbd, sizeof( cbd ) )) ||
            (! lock_region( (void near *)click_handler,
               (char *)cbc_end - (char near *)click_handler )) )
        {
            printf( "locks failed\n" );
        } else {
.code break
            /* show mouse cursor */

            inregs.w.ax = 0x1;
            int386( 0x33, &inregs, &outregs );

            /* set mouse cursor form */

            inregs.w.ax  = 0x9;
            inregs.w.bx  = 0x0;
            inregs.w.cx  = 0x0;
            ptr          = cursor;
            inregs.x.edx = FP_OFF( ptr );
            sregs.es     = FP_SEG( ptr );
            int386x( 0x33, &inregs, &outregs, &sregs );

.code break
            /* install click watcher */

            inregs.w.ax  = 0xC;
            inregs.w.cx  = 0x0002 + 0x0008;
            function_ptr = ( void (far *)( void ) )click_handler;
            inregs.x.edx = FP_OFF( function_ptr );
            sregs.es     = FP_SEG( function_ptr );
            int386x( 0x33, &inregs, &outregs, &sregs );

.code break
            while( !cbd.right_button ) {
                if( cbd.mouse_event ) {
                    printf( "Ev %04hxh  BX %hu  CX %hu  DX %hu  "
                            "SI %hd  DI %hd\n",
                            cbd.mouse_code, cbd.mouse_bx,
                            cbd.mouse_cx, cbd.mouse_dx,
                            cbd.mouse_si, cbd.mouse_di );
                    cbd.mouse_event = 0;
                }
            }
        }
    }

.code break
    /* check installation again (to clear watcher) */

    inregs.w.ax = 0;
    int386( 0x33, &inregs, &outregs );
    if( outregs.w.ax == 0xffff )
        printf( "DONE : Mouse still installed...\n" );
    else
        printf( "DONE : Mouse NOT installed...\n" );

.code break
    printf( "Press Enter key to return to original mode\n" );
    getc( stdin );
    inregs.h.ah = 0x00;
    inregs.h.al = orig_mode;
    int386( 0x10, &inregs, &outregs );
}
.code end
.*
.section How Do I Simulate a Real-Mode Interrupt with DOS/4GW?
.*
.np
.ix 'interrupts' 'real-mode simulation'
.ix 'simulating real-mode interrupts'
Some interrupts are not supported in protected mode with DOS/4GW but
they can still be called using the DPMI function, Simulate Real-Mode
Interrupt (0x0300).
Information that needs to be passed down to the real-mode interrupt is
transferred using an information data structure that is allocated in
the protected-mode application.
The address to this protected-mode structure is passed into DPMI
function 0x0300.
DOS/4GW will then use this information to set up the real-mode
registers, switch to real mode and then execute the interrupt in real
mode.
.np
If your protected-mode application needs to pass data down into the
real-mode interrupt, an intermediate real-mode buffer must be used.
This buffer can be created using DPMI function 0x0100 to allocate
real-mode memory.
You can then transfer data from the protected-mode memory to the
real-mode memory using a far pointer as illustrated in the
"SIMULATE.C" example.
.np
The following example illustrates how to allocate some real-mode
memory, transfer a string of characters from protected mode into the
real-mode buffer, then set up and call the Interrupt 0x0021 function
to create a directory.
The string of characters are used to provide the directory name.
This example can be adapted to handle most real-mode interrupt calls
that aren't supported in protected mode.
.code begin
/*
    SIMULATE.C - Shows how to issue a real-mode interrupt
    from protected mode using DPMI call 300h.  Any buffers
    to be passed to DOS must be allocated in DOS memory
    This can be done with DPMI call 100h.  This program
    will call DOS int 21, function 39h, "Create
    Directory".

    Compile & Link: wcl386 -l=dos4g simulate
*/
#include <i86.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>

.code break
static struct rminfo {
    long EDI;
    long ESI;
    long EBP;
    long reserved_by_system;
    long EBX;
    long EDX;
    long ECX;
    long EAX;
    short flags;
    short ES,DS,FS,GS,IP,CS,SP,SS;
} RMI;
.code break

void main()
{
    union REGS regs;
    struct SREGS sregs;
    int interrupt_no=0x31;
    short selector;
    short segment;
    char far *str;

    /* DPMI call 100h allocates DOS memory */
    memset(&sregs,0,sizeof(sregs));
    regs.w.ax=0x0100;
    regs.w.bx=0x0001;
    int386x( interrupt_no, &regs, &regs, &sregs);
    segment=regs.w.ax;
    selector=regs.w.dx;
.code break

    /* Move string to DOS real-mode memory */
    str=MK_FP(selector,0);
    _fstrcpy( str, "myjunk" );

    /* Set up real-mode call structure */
    memset(&RMI,0,sizeof(RMI));
    RMI.EAX=0x00003900; /* call service 39h ah=0x39  */
    RMI.DS=segment;     /* put DOS seg:off into DS:DX*/
    RMI.EDX=0;          /* DOS ignores EDX high word */
.code break

    /* Use DPMI call 300h to issue the DOS interrupt */
    regs.w.ax = 0x0300;
    regs.h.bl = 0x21;
    regs.h.bh = 0;
    regs.w.cx = 0;
    sregs.es = FP_SEG(&RMI);
    regs.x.edi = FP_OFF(&RMI);
    int386x( interrupt_no, &regs, &regs, &sregs );
}
.code end
.*
.section How do you install a bi-modal interrupt handler using DOS/4GW?
.*
.np
.ix 'DOS/4GW' 'bi-modal interrupt'
.ix 'bi-modal interrupt'
Due to the nature of the protected-mode/real-mode interface, it is often
difficult to handle high speed communications with hardware interrupt
handlers.
For example, if you install your communications interrupt handler in
protected mode, you may find that some data is lost when transmitting
data from a remote machine at the rate of 9600 baud.
This occurs because the data arrived at the communication port while the
machine was in the process of transferring the previous interrupt up to
protected mode.
Data will also be lost if you install the interrupt handler in real mode
since your program, running in protected mode, will have to switch down
into real mode to handle the interrupt.
The reason for this is that the data arrived at the communication port
while the DOS extender was switching between real mode and protected
mode, and the machine was not available to process the interrupt.
.np
To avoid the delay caused by switching between real-mode and protected
mode to handle hardware interrupts, install interrupt handlers in both
real-mode and protected-mode.
During the execution of a protected-mode program, the system often
switches down into real-mode for DOS system calls.
If a communications interrupt occurs while the machine is in real-mode,
then the real-mode interrupt handler will be used.
If the interrupt occurs when the machine is executing in protected-mode,
then the protected-mode interrupt handler will be used.
This enables the machine to process the hardware interrupts faster and
avoid the loss of data caused by context switching.
.np
Installing the interrupt handlers in both protected-mode and real-mode
is called bi-modal interrupt handling.
The following program is an example of how to install both handlers for
Interrupt 0x0C (also known as COM1 or IRQ4).
The program writes either a 'P' to absolute address 0xB8002 or an 'R' to
absolute address 0xB8000.
These locations are the first two character positions in screen memory
for a color display.
As the program runs, you can determine which interrupt is handling the
COM1 port by the letter that is displayed.
A mouse attached to COM1 makes a suitable demo.
Type on the keyboard as you move the mouse around.
The ESC key can be used to terminate the program.
Transmitted data from a remote machine at 9600 baud can also be used to
test the COM1 handling.
.code begin
/*
    BIMODAL.C - The following program demonstrates how
    to set up a bi-modal interrupt handler for DOS/4GW

    Compile and link: wcl386 -l=dos4g bimodal bimo.obj
*/

#include <stdio.h>
#include <conio.h>
#include <dos.h>

#define D32RealSeg(P)   ((((DWORD) (P)) >> 4) & 0xFFFF)
#define D32RealOff(P)   (((DWORD) (P)) & 0xF)

typedef unsigned int WORD;
typedef unsigned long DWORD;

extern void com1_init (void);
extern void __interrupt pmhandler (void);
extern void __interrupt __far rmhandler (void);
.code break

void *D32DosMemAlloc (DWORD size)
{
    union REGS r;

    r.x.eax = 0x0100;           /* DPMI allocate DOS memory */
    r.x.ebx = (size + 15) >> 4; /* Number of paragraphs requested */
    int386 (0x31, &r, &r);

    if( r.x.cflag )  /* Failed */
        return ((DWORD) 0);
    return (void *) ((r.x.eax & 0xFFFF) << 4);
}
.code break

void main (void)
{
    union REGS      r;
    struct SREGS    sr;
    void           *lowp;
    void far       *fh;
    WORD            orig_pm_sel;
    DWORD           orig_pm_off;
    WORD            orig_rm_seg;
    WORD            orig_rm_off;
    int             c;

/*  Save the starting protected-mode handler address */
    r.x.eax = 0x350C;   /* DOS get vector (INT 0Ch) */
    sr.ds = sr.es = 0;
    int386x (0x21, &r, &r, &sr);
    orig_pm_sel = (WORD) sr.es;
    orig_pm_off = r.x.ebx;
.code break

/*
    Save the starting real-mode handler address using DPMI
    (INT 31h).
*/
    r.x.eax = 0x0200;   /* DPMI get real mode vector */
    r.h.bl = 0x0C;
    int386 (0x31, &r, &r);
    orig_rm_seg = (WORD) r.x.ecx;
    orig_rm_off = (WORD) r.x.edx;
.code break

/*
    Allocate 128 bytes of DOS memory for the real-mode
    handler, which must of course be less than 128 bytes
    long.  Then copy the real-mode handler into that
    segment.
*/
    if(! ( lowp = D32DosMemAlloc(128) ) ) {
        printf ("Couldn't get low memory!\n");
        exit (1);
    }
    memcpy (lowp, (void *) rmhandler, 128);
.code break

/*
    Install the new protected-mode vector.  Because INT 0Ch
    is in the auto-passup range, its normal "passdown"
    behavior will change as soon as we install a
    protected-mode handler.  After this next call, when a
    real mode INT 0Ch is generated, it will be resignalled
    in protected mode and handled by pmhandler.
*/
    r.x.eax = 0x250C;   /* DOS set vector (INT 0Ch) */
    fh = (void far *) pmhandler;
    r.x.edx = FP_OFF (fh);
    /* DS:EDX == &handler */
    sr.ds = FP_SEG (fh);
    sr.es = 0;
    int386x (0x21, &r, &r, &sr);
.code break

/*
    Install the new real-mode vector.  We do this after
    installing the protected-mode vector in order to
    override the "passup" behavior.  After the next call,
    interrupts will be directed to the appropriate handler,
    regardless of which mode we are in when they are
    generated.
*/
    r.x.eax = 0x0201;
    r.h.bl = 0x0C;
    /* CX:DX == real mode &handler */
    r.x.ecx = D32RealSeg(lowp);
    r.x.edx = D32RealOff(lowp);
    int386 (0x31, &r, &r);
.code break

/*
    Initialize COM1.
*/
    com1_init ();

    puts( "Move mouse, transmit data; ESC to quit\n" );

    while( 1 ) {
        if( kbhit() ) {
            if( ( (c = getch ()) & 0xff ) == 27 )
                    break;
            putch (c);
        }
    delay( 1 );
    }
.code break

/*
    Clean up.
*/
    r.x.eax = 0x250C;   /* DOS set vector (INT 0Ch) */
    r.x.edx = orig_pm_off;
    sr.ds = orig_pm_sel;    /* DS:EDX == &handler */
    sr.es = 0;
    int386x (0x21, &r, &r, &sr);

    r.x.eax = 0x0201;   /* DPMI set real mode vector */
    r.h.bl = 0x0C;
    /* CX:DX == real mode &handler */
    r.x.ecx = (DWORD) orig_rm_seg;
    r.x.edx = (DWORD) orig_rm_off;
    int386 (0x31, &r, &r);
}
.code end
.np
You will also need to create the following assembler code module.
The first part provides the interrupt handling routine for the
real-mode interrupt handler.
The second provides the protected-mode version of the interrupt
handler.
.code begin
;**
;** bimo.asm:
;** Assembler code for real-mode and protected-mode
;** INT 0xC interrupt handlers to support the INT 0xC
;** interrupt in both modes
;**
.li .386
;**
;** The real-mode interrupt handler is in a 16-bit code
;** segment so that the assembler will generate the right
;** code.  We will copy this code down to a 16-bit segment
;** in low memory rather than executing it in place.
;**

_TEXT16 SEGMENT BYTE PUBLIC USE16 'CODE'
    ASSUME  cs:_TEXT16

    PUBLIC  rmhandler_
rmhandler_:
    push    es
    push    bx
    mov     bx,0B800h
    mov     es,bx                   ; ES = 0xB800
    sub     bx,bx                   ; BX = 0
    mov     WORD PTR es:[bx],0720h  ; Clear 2 char cells
    mov     WORD PTR es:[bx+2],0720h
    mov     BYTE PTR es:[bx],'R'    ; Write R to memory
    pop     bx
    pop     es
    push    ax
    push    dx
    mov     dx,03FAh
    in      al,dx                   ; Read ports so
    mov     dx,03F8h                ; interrupts can
    in      al,dx                   ; continue to be
    mov     dx,020h                 ; generated
    mov     al,dl
    out     dx,al                   ; Send EOI
    pop     dx
    pop     ax
    iret
_TEXT16 ENDS
.code break
;**
;** The protected-mode interrupt handler is in a 32-bit code
;** segment.  Even so, we have to be sure to force an IRETD
;** at the end of the handler, because MASM doesn't generate
;** one.  This handler will be called on a 32-bit stack by
;** DOS/4GW.
;**
;** _DATA is the flat model data segment, which we load into
;** ES so we can write to absolute address 0xB8000.  (In the
;** flat model, DS is based at 0.)
;**
_DATA   SEGMENT BYTE PUBLIC USE32 'DATA'
_DATA   ENDS
.code break

DGROUP GROUP _DATA

_TEXT   SEGMENT BYTE PUBLIC USE32 'CODE'
    ASSUME  cs:_TEXT

    PUBLIC  com1_init_
com1_init_:
    mov     ax,0F3h                 ; 9600,n,8,1
    mov     dx,0                    ; com1
    int     14h                     ; Initialize COM1
    mov     bx,03F8h                ; COM1 port space
    lea     dx,[bx+5]               ; line status reg
    in      al,dx
    lea     dx,[bx+4]               ; modem control reg
    in      al,dx
    or      al,8                    ; enable OUT2 int
    out     dx,al
    lea     dx,[bx+2]               ; int id register
    in      al,dx
    mov     dx,bx                   ; data receive reg
    in      al,dx
    in      al,21h                  ; interrupt mask reg
    and     al,0EFh                 ; force IRQ4 unmask
    out     21h,al
    lea     dx,[bx+1]               ; int enable reg
    mov     al,1
    out     dx,al                   ; enable received int
    ret
.code break

    PUBLIC  pmhandler_
pmhandler_:
    push    es
    push    bx
    mov     bx,DGROUP
    mov     es,bx
    mov     ebx,0B8000h             ; ES:EBX=flat:0B8000h
    mov     DWORD PTR es:[ebx],07200720h  ; Clear cells
    mov     BYTE PTR es:[ebx+2],'P' ; Write P to memory
    pop     bx
    pop     es
    push    ax
    push    dx
    mov     dx,03FAh
    in      al,dx                   ; Read ports so
    mov     dx,03F8h                ; interrupts can
    in      al,dx                   ; continue to be
    mov     dx,020h                 ; generated
    mov     al,dl
    out     dx,al                   ; Send EOI
    pop     dx
    pop     ax
    iretd
_TEXT   ENDS
    END
.code end
.*
.do end
