/* Test the include_alias pragma */

/* Later pragmas override earlier ones */
#pragma include_alias( "header.h", "hdr2.h" )
#pragma include_alias( "header.h", "hdr1.h" )

/* Macro expansion must work */
#define QFOO "foo.h"
#define QBAR "hdr3.h"
#pragma include_alias( QFOO, QBAR )

#define AFOO <foo.h>
#define ABAR <hdr3.h>
#pragma include_alias( AFOO, ABAR )
