

#ifndef _objset_h_ /* Wed Apr 20 11:06:03 1994 */
#define _objset_h_





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




// An object set is a derived class, rather than a typedef, so that we may
// support persistent sets, and for "finding" objects in sets.

#include "base/set.h"
#include "base/iofilter.h"
#include "base/stream.h"

class CL_ObjectSet: public CL_Set<CL_ObjectPtr> {

public:
    CL_ObjectSet (CL_ObjectIOFilter* builder = NULL);
    // Construct an ObjectSet whose iofilter is the given parameter.
    // The parameter specifies an object io filter; it will
    // only be used by the ReadFrom method when this set needs to be
    // read from a stream, to construct an object from its passive
    // representation in the stream. If the  parameter is NULL, the
    // ReadFrom method will always return FALSE. The builder object is
    // {\it not\/} owned by the Set, but it must
    // exist as long as the Set does.
    

    CL_ObjectSet (const CL_ObjectSet&);
    // Copy constructor.

    CL_ObjectPtr Find (CL_ObjectPtr p) const;
    // Determine whether the given object is in the set, and if so, return
    // a pointer to the contained object. If not, return \NULL. This method
    // is useful in situations where a compound object is stored in the
    // set, where equality testing for the contained object is based only
    // on some (not all) of its components.
    
    void DestroyContents ();
    // Invoke the destructors of all the contained objects, and set our
    // size to zero. Note that this is not the same as the inherited
    // method MakeEmpty, since the latter does not destroy contained
    // objects.



    bool ReadFrom (const CL_Stream&);
    // Override the method inherited from {\small\tt CL_Object}.

    bool WriteTo  (CL_Stream&) const;
    // Override the method inherited from {\small\tt CL_Object}.

    
    
    CL_ClassId ClassId() const {return _CL_ObjectSequence_CLASSID;};
    
    const char* ClassName () const {return "CL_ObjectSet";};
    
};


typedef CL_SetIterator<CL_ObjectPtr> CL_ObjectSetIterator;




inline bool CL_ObjectSet::ReadFrom (const CL_Stream& s)
{
    CL_ClassId id;
    if (!s.Read (id) || id != ClassId())
        return FALSE;
    return _idata ?
        ((CL_Sequence<CL_ObjectPtr> *)_idata)->ReadFrom (s) : FALSE;
}

inline bool CL_ObjectSet::WriteTo  (CL_Stream& s) const
{
    return _idata ? s.Write (ClassId()) &&
        ((CL_Sequence<CL_ObjectPtr>*)_idata)->WriteTo (s) : FALSE;
}


#endif /* _objctset_h_ */
