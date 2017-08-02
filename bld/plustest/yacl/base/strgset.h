

#ifndef _strgset_h_ /* Wed Jul 27 16:54:28 1994 */
#define _strgset_h_



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


#include "base/set.h"
#include "base/string.h"

class CL_StringSet: public CL_Set<CL_String> {

public:
    CL_StringSet StringsWithPrefix (const CL_String& s) const;
    // Return a set containing all those strings in this set that have
    // {\tt s} as prefix.
    
    //
    // ------------------------- Basic methods --------------------
    //
    CL_Object* Clone () const
        {return new CL_Set<CL_String> (*this);};
    // Override the method inherited from {\small\tt CL_Object}.
    
    const char* ClassName() const {return "CL_StringSet";};
    // Override the method inherited from {\small\tt CL_Object}.
    
    CL_ClassId ClassId() const
        { return _CL_StringSet_CLASSID;};
    // Override the method inherited from {\small\tt CL_Object}.



    // -------------------- End public protocol ---------------------------

};


typedef CL_SetIterator<CL_String>  CL_StringSetIterator;

#endif /* _strgset_h_ */
