

#ifndef _intseq_h_ /* Tue Nov 23 09:38:24 1993 */
#define _intseq_h_





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




#include "base/integer.h"
#include "base/sequence.h"

class CL_IntegerSequence: public CL_Sequence<long>  {

public:
    CL_IntegerSequence (long initial_size = 0);

    CL_IntegerSequence (long data[], long count);
    // Create ourselves from a C-style array of long values.
    
    CL_IntegerSequence (const CL_Sequence<long>& s);
    // Copy constructor.

    const char* ClassName () const {return "CL_IntegerSequence";};

    CL_ClassId ClassId () const { return _CL_IntegerSequence_CLASSID; };

    CL_Object* Clone () const {return new CL_IntegerSequence (*this);};
    
};




#endif /* _intseq_h_ */
