.func popen
.synop begin
#include <stdio.h>
FILE *popen( const char *command, const char *mode );
.ixfunc2 '&Direct' &func
.synop end
.desc begin
The
.id &func.
function executes the command specified by
.arg command
and creates a pipe between the calling process and the executed
command.
.np
Depending on the
.arg mode
argument, the stream pointer returned may be used to read from or
write to the pipe.
.np
The executed command has an environment the same as its parents.
The command will be started as follows:
.millust begin
execl("/bin/sh", "sh", "-c", command, (char *) NULL);
.millust end
.np
The
.arg mode
argument to
.id &func.
is a string that specifies an I/O mode for the pipe.
.begnote
.termhd1 Mode
.termhd2 Meaning
.note "r"
The calling process will read from the standard output of the child
process using the stream pointer returned by
.id &func.
.
.note "w"
The calling process will write to the standard input of the child
process using the stream pointer returned by
.id &func.
.
.endnote
.np
A stream opened by
.id &func.
should be closed by the
.kw pclose
function.
.desc end
.return begin
The
.id &func.
function returns a non-NULL stream pointer upon successful
completion.
If
.id &func.
is unable to create either the pipe or the subprocess, a
.mono NULL
stream pointer is returned and
.kw errno
is set appropriately.
.return end
.error begin
.begterm 12
:DTHD.Constant
:DDHD.Meaning
.term EINVAL
The
.arg mode
argument is invalid.
.endterm
.np
.id &func.
may also set
.kw errno
values as described by the
.kw pipe
,
.kw fork
, and
.kw execl
functions.
.error end
.see begin
.seelist &function. pclose pipe execl
.see end
.exmp begin
/*
 * Executes the 'ls' command and prints all
 * its output preceded by an arrow.
 */
#include <stdio.h>

int main()
{
    FILE *fp;
    char readbuf[256];
    
    fp = popen("ls", "r");
    if(fp == NULL) {
        printf("Failed to open pipe\n");
        exit(1);
    }
    
    while(fgets(readbuf, 256, fp))
        printf("-> %s", readbuf);
        
    pclose(fp);
    
    return 0;
}
.exmp end
.class POSIX 1003.1
.system
