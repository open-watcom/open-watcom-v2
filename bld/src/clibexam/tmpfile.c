#include <stdio.h>

static FILE *TempFile;

void main()
  {
    TempFile = tmpfile();
    /* . */
    /* . */
    /* . */
    fclose( TempFile );
  }
