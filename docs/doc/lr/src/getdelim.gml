.func getdelim
.synop begin
#include <stdio.h>
ssize_t getdelim( char **line, size_t *n, int delim, FILE *fp );
.synop end
.desc begin
The
.id &funcb.
function reads in text from a stream 
.arg fp
up to and including a delimiter
.arg delim
and returns the resulting text in a buffer pointed to by
.arg line
when complete.  The buffer pointed to by
.arg line
should be of the size pointed to by
.arg n
initially.
.np
The buffer pointed to by 
.arg line
can initially be null and
.arg n
should reflect the size of 
.arg line
and be set to null.  When 
.id &funcb.
is invoked, it will check if the buffer pointed to by
.arg line
is allocated and large enough.  If not, it will first call
.id realloc
to resize the buffer appropriately and adjust the value pointed to by
.arg n
to reflect the new size of 
.arg line
after reallocation.  If allocation fails, errno will be set to ENOMEM.
.desc end
.return begin
The function returns the number of characters read into the buffer or -1
on either error or if no further data is available.
.return end
.exmp begin
.blktext begin
The following program would print out each line in a text file "test.txt" .
.blktext end
.blkcode begin
#include <stdio.h>

void main()
  {
    char *line;
    size_t n;
    FILE *fp;
    
    fp = fopen("test.txt", "r");
    while(getdelim(&line, &n, '\n', fp) >= 0) {
        printf("> %s", line);
    }
    fclose(fp);
  }
.blkcode end
.exmp end
.class POSIX
.system
