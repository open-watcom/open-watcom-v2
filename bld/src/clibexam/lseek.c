
long int file_posn;
int handle;

/* get current file position */
file_posn = lseek( handle, 0L, SEEK_CUR );
  /* or */
file_posn = tell( handle );

/* return to previous file position */
file_posn = lseek( handle, file_posn, SEEK_SET );


#include <stdio.h>
#include <io.h>

int read_record( int  handle,
                 long rec_numb,
                 int  rec_size,
                 char *buffer )
  {
    if( lseek( handle, rec_numb * rec_size, SEEK_SET )
         == -1L ) {
      return( -1 );
    }
    return( read( handle, buffer, rec_size ) );
  }

.blktext begin
The function in this example assumes records are numbered starting
with zero and that
.arg rec_size
contains the size of a record in the file
(including the carriage-return character in text files).
.blktext end
