/*++

Copyright (c) 1990,91  Microsoft Corporation

Module Name:

    packoff.h

Abstract:

    This file turns packing of structures off.  (That is, it enables
    automatic alignment of structure fields.)  An include file is needed
    because various compilers do this in different ways.

    packoff.h is the complement to packon.h.  An inclusion of packoff.h
    MUST ALWAYS be preceded by an inclusion of packon.h, in one-to-one
    correspondence.

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
#    pragma unpacked
#  elif defined(__HIGHC__)
#    pragma Pop(Align_members)
#  elif defined(__WATCOMC__) && (__WATCOMC__ >= 1000)
#    pragma pack(__pop);
#  elif defined(_MSC_VER) && (_MSC_VER >= 800)  /* Visual C 4+ */
#    pragma warning(disable:4103)
#    pragma pack(pop)
#  else
#    pragma pack()
#  endif
#endif

