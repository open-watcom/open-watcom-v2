#include <sys\stat.h>
#include <sys\types.h>
#include <fcntl.h>

void main()
  {
    int handle;

    /* open a file for output                  */
    /* replace existing file if it exists      */

    handle = open( "file",
                O_WRONLY | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );

    /* read a file which is assumed to exist   */

    handle = open( "file", O_RDONLY );

    /* append to the end of an existing file   */
    /* write a new file if file does not exist */

    handle = open( "file",
                O_WRONLY | O_CREAT | O_APPEND,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
  }
