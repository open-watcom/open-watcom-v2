#include <stdlib.h>
#include <process.h>

void main()
  {
     int   process_id, status;

     process_id = spawnl( P_NOWAIT, "child.exe",
                "child", "parm", NULL );
     wait( &status );
  }
