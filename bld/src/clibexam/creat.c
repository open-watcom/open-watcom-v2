#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>

void main()
  {
    int handle;

    handle = creat( "file", S_IWRITE | S_IREAD );
    if( handle != -1 ) {

      /* process file */

      close( handle );
    }
  }
