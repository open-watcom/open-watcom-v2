#include <stdio.h>
#include <process.h>

void main()
  {
     int   process_id;
     int   status;

     process_id = spawnl( P_NOWAIT, "child.exe",
                "child", "parm", NULL );
     cwait( &status, process_id, WAIT_CHILD );
  }
