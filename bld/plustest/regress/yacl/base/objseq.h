

#ifndef _objseq_h_ /* Wed Apr 20 22:52:00 1994 */
#define _objseq_h_





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




// The ObjectSequence is a derived class, rather than a typedef, so that
// we can support storage and retrieval of object sequences.


#include "base/sequence.h"
#include "base/iofilter.h"


class CL_ObjectSequence: public CL_Sequence<CL_ObjectPtr> {

public:

    CL_ObjectSequence (long initial_size = 0,
                       CL_ObjectIOFilter* builder = NULL);
    // The second parameter specifies an object builder function; it will
    // only be used by the ReadFrom method when this sequence needs to be
    // read from a stream, to construct an object from its passive
    // representation in the stream. If the second parameter is NULL, the
    // ReadFrom method will always return FALSE. The builder object is
    // {\it not\/} owned by the sequence, but it must
    // exist as long as the sequence does.
    

    CL_ObjectSequence (const CL_ObjectPtr data[], long count,
                       CL_ObjectIOFilter* builder = NULL);
    // Create ourselves from a C-style array.
    
    CL_ObjectSequence (const CL_ObjectSequence& s);
    // Copy constructor.

    ~CL_ObjectSequence ();
    // Destructor.

    
    void DestroyContents ();
    // Invoke the destructors of all the contained objects, and set our
    // size to zero. Note that this is not the same as the inherited
    // method MakeEmpty, since the latter does not destroy contained
    // objects.

    bool ReadFrom (const CL_Stream& s);
    // Reconstruct this ObjectSequence from the given stream, using the
    // builder specified at construction time.
    

    CL_ClassId ClassId() const {return _CL_ObjectSequence_CLASSID;};
    
    const char* ClassName () const {return "CL_ObjectSequence";};
    
protected:
    CL_ObjectIOFilter* _builder;
};


#endif /* _objseq_h_ */
