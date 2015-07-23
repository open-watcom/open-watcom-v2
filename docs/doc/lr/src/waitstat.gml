.np
If
.arg status
is not
.mono NULL,
it points to a word that will be filled in with the termination status
word and return code of the terminated child process.
.np
If the child process terminated normally, then the low order byte of the
status word will be set to 0, and the high order byte will contain the
low order byte of the return code that the child process passed to the
.kw DOSEXIT
function.
The
.kw DOSEXIT
function is called whenever
.kw main
returns, or
.kw exit
, or
.kw _Exit
or
.kw _exit
are explicity called.
.np
If the child process did not terminate normally, then the high order byte
of the status word will be set to 0, and the low order byte will contain
one of the following values:
.begnote $setptnt 6
.termhd1 Value
.termhd2 Meaning
.note 1
Hard-error abort
.note 2
Trap operation
.note 3
SIGTERM signal not intercepted
.endnote
.begnote
.note Note:
This implementation of the status value follows the OS/2 model and
differs from the Microsoft implementation.
Under Microsoft, the return code is returned in the low order byte and
it is not possible to determine whether a return code of 1, 2, or 3
imply that the process terminated normally.
For portability to Microsoft compilers, you should ensure that the
application that is waited on does not return one of these values.
The following shows how to handle the status value in a portable
manner.
.code begin
cwait( &status, process_id, WAIT_CHILD );

#if defined(__WATCOMC__)
switch( status & 0xff ) {
case 0:
    printf( "Normal termination exit code = %d\n", status >> 8 );
    break;
case 1:
    printf( "Hard-error abort\n" );
    break;
case 2:
    printf( "Trap operation\n" );
    break;
case 3:
    printf( "SIGTERM signal not intercepted\n" );
    break;
default:
    printf( "Bogus return status\n" );
}

#else if defined(_MSC_VER)
switch( status & 0xff ) {
case 1:
    printf( "Possible Hard-error abort\n" );
    break;
case 2:
    printf( "Possible Trap operation\n" );
    break;
case 3:
    printf( "Possible SIGTERM signal not intercepted\n" );
    break;
default:
    printf( "Normal termination exit code = %d\n", status );
}

#endif
.code end
.endnote
