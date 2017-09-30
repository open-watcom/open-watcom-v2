

#ifndef _clntset_h_ /* Tue Mar  8 15:35:57 1994 */
#define _clntset_h_





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




// This is a set of bindings and corresponding parameters, used for maintaining
// client sets for notification.

#include "base/objset.h"
#include "base/binding.h"

    
class CL_ClientSet: public CL_ObjectSet {

public:
    ~CL_ClientSet ();

    bool Add    (const CL_AbstractBinding& binding, long parameter);
    // Add the given binding-parameter pair to this set. Return FALSE if
    // either memory allocation failed or the pair was already in the set,
    // and TRUE otherwise.

    bool Remove   (const CL_AbstractBinding& binding);
    // Remove the given binding, and its associated parameter, from the set.
    // Return FALSE if there was no such binding in the set, and TRUE
    // otherwise.

    bool Includes (const CL_AbstractBinding& binding) const;
    // Return whether this set includes the given binding.


    long& CodeFor (const CL_AbstractBinding& binding) const;
    // Return a reference to the parameter value associated with the given
    // binding. The caller of this method may modify the return value. If
    // the set does not contain the given binding, a reference to an
    // undefined long value is returned.
    
    void NotifyAll (CL_Object& source) const;
    // Execute all the contained bindings, with the given object as first
    // parameter, and the remembered second parameter. Ignore the return
    // values of the executed bindings.
    
    bool Permits (CL_Object& source) const;
    // Execute all the contained bindings, with the given object as first
    // parameter, and the remembered second parameter; return TRUE if all
    // of them return TRUE, and FALSE otherwise. The notification process
    // stops as soon as one of the bindings returns FALSE, so that
    // subsequent bindings in the set are not invoked. Here, each
    // binding's return value is viewed as "permission" from the client to
    // perform some operation, and this method's return value is TRUE only
    // if all contained bindings permit it.

    // Disallow some inherited methods!
    
    bool Add    (const CL_ObjectPtr&)
        {NotImplemented ("Add object"); return FALSE;};
    // This method issues a "Not implemented" error message, and must not be
    // called. It is declared here to override the inherited method with
    // the same signature.
    
    CL_ObjectPtr Remove   (const CL_ObjectPtr&)
        {NotImplemented ("Remove object"); return FALSE;};
    // This method issues a "Not implemented" error message, and must not be
    // called. It is declared here to override the inherited method with
    // the same signature.
    
    bool Includes (const CL_ObjectPtr&) const
        {NotImplemented ("Remove object"); return FALSE;};
    // This method issues a "Not implemented" error message, and must not be
    // called. It is declared here to override the inherited method with
    // the same signature.
    
    const char* ClassName () const {return "CL_ClientSet";};
    // Override method inherited from CL_Object.

};


#endif /* _clntset_h_ */
