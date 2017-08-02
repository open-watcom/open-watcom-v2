

#ifndef _iofilter_h_ /* Thu Jul  7 15:49:50 1994 */
#define _iofilter_h_




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


#include "base/object.h"

class CL_ObjectIOFilter: public CL_Object {

public:
    virtual CL_ObjectPtr RestoreFrom (const CL_Stream& s);
    // "Polymorphic" restoration: the returned pointer is to a newly
    // created object which can be of any class derived from CL_Object.
    // The default implementation returns an object whose type is
    // indicated by the class id seen in the stream.

    virtual bool RestoreFrom (const CL_Stream& s, CL_Object& o);
    // Restore the object o from the stream. With the default
    // implementation, the type of object restored
    // matches the type of o. If the stream contains an object that is not
    // of o's class, the restoration fails.
    
    virtual bool       SaveTo (CL_Stream& s, const CL_Object& o) const;
    // Write the value of o to the stream. The default implementation uses
    // o's {\tt WriteTo} method.
    
    virtual bool   SaveTo (CL_Stream& s, CL_ObjectPtr o) const;
    // Write the value determined by the pointer {\tt o} into the stream.
    // The default implementation simply forwards the call to the stream.
    
};




template <class Base>
class CL_IOFilter: public CL_ObjectIOFilter {

public:
    virtual CL_Object* RestoreFrom (const CL_Stream& s);

    virtual bool RestoreFrom (const CL_Stream& s, CL_Object& o);
    
};


#endif /* _iofilter_h_ */
