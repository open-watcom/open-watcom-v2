/*++

Copyright (c) 1990,91  Microsoft Corporation

Module Name:

    packon.h

Abstract:

    This file turns packing of structures on.  (That is, it disables
    automatic alignment of structure fields.)  An include file is needed
    because various compilers do this in different ways.

    The file packoff.h is the complement to this file.

Author:

    Chuck Lenzmeier (chuckl) 4-Mar-1990

Revision History:

    15-Apr-1991 JohnRo
        Created lint-able variant.

    20-Oct-1997 G.Vanem
        Added Metaware support

    05-Jul-1999 G.Vanem
        Added LADsoft support

    01-Nov-2000 G. Vanem
        Added Visual C/C++ support

--*/

#if !(defined(lint) || defined(_lint))
#  if defined(_MSC_VER) && (_MSC_VER >= 700)
#    pragma warning(disable:4103)
#  endif

#  if defined(__CCDL__)
#    pragma packed
#  elif defined(__HIGHC__)
#    pragma Off(Align_members)
#  elif defined(__WATCOMC__) && (__WATCOMC__ >= 1000)
#    pragma pack(__push,1);
#  elif defined(_MSC_VER) && (_MSC_VER >= 800)  /* Visual C 4+ */
#    pragma warning(disable:4103)
#    pragma pack(push,1)
#  else
#    pragma pack(1)   /* x86, GNU compiler; MIPS, MIPS compiler */
#  endif
#endif

