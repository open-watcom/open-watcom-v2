/*================================================================\
|                                                                 |
|      OS/2 Physical Device Driver Example Code                   |
|                  for Open Watcom C/C++                          |
|                                                                 |
|  HEADER.C (Header portion of driver, must be first in DATA SEG) |
|                                                                 |
|  This device driver provides a high-resolution timer for OS/2   |
|                                                                 |
|  The source code was adapted from the Fall 1991 issue of        |
|  IBM Personal Systems Developer magazine.                       |
|                                                                 |
|                                                                 |
|  Adapted for Open Watcom C/C++                                  |
|  Original Author: Rick Fishman                                  |
|                   Code Blazers, Inc.                            |
|                   4113 Apricot                                  |
|                   Irvine, CA 92720                              |
|                                                                 |
\================================================================*/

#include "devdefs.h"
#include "devhdr.h"

// Ensure that the header is located at the beginning of the driver

#pragma data_seg ( "_HEADER", "DATA" ) ;

// Declare the device driver header

extern VOID Strategy();

DEVHEADER HRTimer = {
      FENCE,
      DAW_CHARACTER | DAW_OPENCLOSE | DAW_LEVEL1,
      Strategy,
      0,
      "WTIMER$ "
      "",
      CAP_NULL
};
