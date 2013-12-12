.func _harderr _hardresume _hardretn
#include <dos.h>
void _harderr( int (__far *handler)() );
void _hardresume( int action );
void _hardretn( int error );
.synop end
.desc begin
The &func routine installs a critical error handler (for INT 0x24)
to handle hardware errors.
This critical error handler will call the user-defined function
specified by
.arg handler
when a critical error occurs (for example, attempting to open a file
on a floppy disk when the drive door is open).
The parameters to this function are as follows:
.millust begin
int handler( unsigned deverror,
             unsigned errcode,
             unsigned __far *devhdr );
.millust end
.np
The low-order byte of
.arg errcode
can be one of the following values:
.begnote $compact
.termhd1 Value
.termhd2 Meaning
.note 0x00
Attempt to write to a write-protected disk
.note 0x01
Unknown unit
.note 0x02
Drive not ready
.note 0x03
Unknown command
.note 0x04
CRC error in data
.note 0x05
Bad drive-request structure length
.note 0x06
Seek error
.note 0x07
Unknown media type
.note 0x08
Sector not found
.note 0x09
Printer out of paper
.note 0x0A
Write fault
.note 0x0B
Read fault
.note 0x0C
General failure
.endnote
.np
The
.arg devhdr
argument points to a device header control-block that contains
information about the device on which the error occurred.
Your error handler may inspect the information in this control-block
but must not change it.
.cp 14
.np
If the error occurred on a disk device, bit 15 of the
.arg deverror
argument will be 0 and the
.arg deverror
argument will indicate the following:
.begnote $compact
.termhd1 Bit
.termhd2 Meaning
.note bit 15
0 indicates disk error
.note bit 14
not used
.note bit 13
0 indicates "Ignore" response not allowed
.note bit 12
0 indicates "Retry" response not allowed
.note bit 11
0 indicates "Fail" response not allowed
.note bit 9,10
location of error
.begnote $compact
.termhd1 Value
.termhd2 Meaning
.note 00
MS-DOS
.note 01
File Allocation Table (FAT)
.note 10
Directory
.note 11
Data area
.endnote
.note bit 8
0 indicates read error, 1 indicates write error
.endnote
.np
The low-order byte of
.arg deverror
indicates the drive where the error occurred;
(0 = drive A, 1 = drive B, etc.).
.np
The handler is very restricted in the type of system calls that
it can perform.
System calls 0x01 through 0x0C, and 0x59 are the only system
calls allowed to be issued by the handler.
Therefore, many of the standard C run-time functions such as
stream I/O and low-level I/O cannot be used by the handler.
Console I/O is allowed (e.g., cprintf, cputs).
.np
The handler must indicate what action to take by returning one of
the following values or calling
.kw _hardresume
with one of the following values:
.begnote $setptnt 22
.termhd1 Value
.termhd2 Meaning
.note _HARDERR_IGNORE
Ignore the error
.note _HARDERR_RETRY
Retry the operation
.note _HARDERR_ABORT
Abort the program issuing INT 0x23
.note _HARDERR_FAIL
Fail the system call that is in progress (DOS 3.0 or higher)
.endnote
.np
Alternatively, the handler can return directly to the application
program rather than returning to DOS by using the
.kw _hardretn
function.
The application program resumes at the point just after the failing
I/O function request.
The
.kw _hardretn
function should be called only from within a user-defined hardware
error-handler function.
.np
The
.arg error
argument of
.kw _hardretn
should be a DOS error code.
See
.us The MS-DOS Encyclopedia
or
.us Programmer's PC Sourcebook, 2nd Edition,
for more detailed information on DOS error codes that may be returned
by a given DOS function call.
.np
If the failing I/O function request is an INT 0x21 function greater
than or equal to function 0x38,
.kw _hardretn
will return to the application with the carry flag set and the AX
register set to the
.kw _hardretn
.arg error
argument.
If the failing INT 0x21 function request is less than function 0x38 abd
the function can return an error, the AL register will be set to 0xFF
on return to the application.
If the failing INT 0x21 function does not have a way of returning an
error condition (which is true of certain INT 0x21 functions below
0x38), the
.arg error
argument of
.kw _hardretn
is not used, and no error code is returned to the application.
.desc end
.return begin
These functions do not return a value.
The
.kw _hardresume
and
.kw _hardretn
functions do not return to the caller.
.return end
.see begin
.seelist _harderr _chain_intr _dos_getvect _dos_setvect
.see end
.exmp begin
#include <stdio.h>
#include <conio.h>
#include <dos.h>

#if defined(__DOS__) && defined(__386__)
    #define FAR __far
#else
    #if defined(__386__)
        #define FAR
    #else
        #define FAR __far
    #endif
#endif
.exmp break
int FAR critical_error_handler( unsigned deverr,
                                unsigned errcode,
                                unsigned FAR *devhdr )
  {
    cprintf( "Critical error: " );
    cprintf( "deverr=%4.4X errcode=%d\r\n",
             deverr, errcode );
    cprintf( "devhdr = %Fp\r\n", devhdr );
    return( _HARDERR_IGNORE );
  }
.exmp break
main()
  {
    FILE *fp;

    _harderr( critical_error_handler );
    fp = fopen( "a:tmp.tmp", "r" );
    printf( "fp = %p\n", fp );
  }
.exmp output
Critical error: deverr=1A00 errcode=2
devhdr = 0070:01b6
fp = 0000
.exmp end
.class DOS
.system
