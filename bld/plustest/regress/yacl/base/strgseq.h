

#ifndef _strgseq_h_ /* Fri May 20 19:15:07 1994 */
#define _strgseq_h_







/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */





// The string sequence is a derived class, rather than a typedef, in order
// to work around the quirks of the template instantiation mechanism. If
// it were a typedef, we could not use it as return value for any of the
// {\small\tt CL_String} methods, but we need to (in the {\small\tt Split}
// method).

#include "base/sequence.h"
#include "base/string.h"

class CL_StringSequence: public CL_Sequence<CL_String> {

public:
    //
    // ------------------------ Creation and destruction --------------
    //
    CL_StringSequence (long initial_size = 0);

    CL_StringSequence (const CL_String data[], long count);
    // Create ourselves from a C-style array of Strings.
    
    CL_StringSequence (const char* data[], long count);
    // Create ourselves from a C-style array of character  pointers.
    
    CL_StringSequence (const CL_Sequence<CL_String>& s);
    // Copy constructor.

    const char* ClassName () const {return "CL_StringSequence";};
    
    CL_ClassId ClassId () const { return _CL_StringSequence_CLASSID; };
    
};



#endif /* _strgseq_h_ */
