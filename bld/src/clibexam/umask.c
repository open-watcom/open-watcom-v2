#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>

void main()
  {
    int old_mask;

    /* set mask to create read-only files */
    old_mask = umask( S_IWUSR | S_IWGRP | S_IWOTH |
                      S_IXUSR | S_IXGRP | S_IXOTH );
  }
