

#ifndef _builder_h_ /* Wed Apr 20 22:49:51 1994 */
#define _builder_h_





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



// An ObjectBuilder is an object that can reconstruct another object from
// a given data stream. This file contains an abstract class declaration;
// each class that needs to be stored and retrieved must declare its own
// derived version of ObjectBuilder.

#include "base/object.h"

class CL_ObjectBuilder: public CL_Object {

public:
    virtual CL_Object* BuildFrom (const CL_Stream&) = 0;
    // Build a new object from its representation in the stream, and
    // return it. The caller of this method then owns the new object, and
    // must destroy it when done.

    const char* ClassName () const {return "CL_ObjectBuilder";};
    
};



// A Builder is a template-based specialization of ObjectBuilder, and
// simply uses the object's ReadFrom method. This can be used in most
// cases, obviating the need for defining an explicit ObjectBuilder class
// for every client class.

template <class Base>
class CL_Builder: public CL_ObjectBuilder {

public:
    CL_Object* BuildFrom (const CL_Stream& s)
        { Base* p = new Base; p->ReadFrom (s); return p; };

    const char* ClassName () const {return "CL_Builder";};
    
};



#endif /* _builder_h_ */
