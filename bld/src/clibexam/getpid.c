#include <stdio.h>
#include <process.h>

void main()
  {
    unsigned int process_id;
    auto char filename[13];

    process_id = getpid();
    /* use this to create a unique file name */
    sprintf( filename, "TMP%4.4x.TMP", process_id );
  }
