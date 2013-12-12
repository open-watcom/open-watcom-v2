.func getpid
.synop begin
#include <process.h>
int getpid(void);
.synop end
.desc begin
The
.id &func.
function returns the process id for the current process.
.desc end
.return begin
The
.id &func.
function returns the process id for the current process.
.return end
.* .see begin
.* .seelist getpid _threadid
.* .see end
.exmp begin
#include <stdio.h>
#include <process.h>

void main()
  {
    unsigned int process_id;
    auto char filename[13];
.exmp break
    process_id = getpid();
    /* use this to create a unique file name */
    sprintf( filename, "TMP%4.4x.TMP", process_id );
  }
.exmp end
.class POSIX 1003.1
.system
