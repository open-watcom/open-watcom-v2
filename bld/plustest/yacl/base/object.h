

#ifndef _object_h_
#define _object_h_





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




// The class Object serves as the root of the inheritance tree of
// YACL. It provides for simple mechanisms for runtime type
// identification (via class id's or class names), cloning (copying)
// of objects (via the Clone() method), and dependent notification.
//
// Every object instance maintains two sets of  dependents, who are to be
// notified just before and just  after the  object  is modified. (It  is
// upto the individual  object  to decide  exactly what its  modification
// means;    for  example,  all  base  objects   regard "modification" as
// alteration of their   value in some way.)  An   object A can  register
// itself  as  a  (post-modification)  dependent  on   object B  via  the
// AddDependent() call on   B,  which  also  specifies  a  method on   A.
// Subsequently,  whenever B invokes  Notify()  on itself, the  specified
// method    on A  is   called, with  B  and  the    notification code as
// parameters.
// 
// Similarly,  object  A  can    register itself  as   a pre-modification
// dependent of object B via the call to AddPreChangeDependent(). If B is
// any   base  object that   has    a nonempty  set  of  pre-modification
// dependents, then B consults all  of these dependents before  modifying
// itself, and remains unaltered if any of the dependent bindings returns
// a FALSE value.
// 
// The  sets of dependents   are implemented  via  a  single  pointer  as
// instance variable, to minimize  overhead on  objects  that do not  use
// dependents. These sets are {\it not\/} copied when the object is copied.




#include "base/defs.h"
#include "base/classid.h"
#include "base/error.h"

#ifdef __GNUC__
#pragma interface
#endif

class __CLASSTYPE CL_Object;
class __CLASSTYPE CL_AbstractBinding;
class __CLASSTYPE CL_ByteArray;
class __CLASSTYPE CL_ByteString;
class __CLASSTYPE CL_String;
class __CLASSTYPE CL_Stream;
class __CLASSTYPE CL_Integer;
template <class K, class V>
class __CLASSTYPE CL_Map;


// The iostream classes are declared this way, rather than including
// iostream.h, to minimize preprocessing time:
#include <iosfwd.h>


typedef CL_Object*  CL_ObjectPtr;
typedef CL_Object*  (*CL_Creator) (); // For persistence support

typedef long        CL_ClassId; // This class id representation may be
                                // changed in future.

struct CL_DependStruct;

class __CLASSTYPE CL_Object {

public:
    
    CL_Object();
    // Default constructor.

    CL_Object (const CL_Object&);
    // Copy constructor.
    
    virtual ~CL_Object();
    // Destructor, declared virtual, so the destructors of all derived
    // classes are virtual.


    // ------------------ Object identification --------------------------

    virtual const char* ClassName() const { return "CL_Object";};
    // Return the class name of this object.
    
    virtual CL_ClassId ClassId () const {return _CL_Object_CLASSID;};
    // Return the class id of this object.

    virtual bool IsA (const CL_Object& obj) const
        {return ClassId() == obj.ClassId();};
    // Return TRUE if our this object's class is the same as, or derived
    // from, that of {\tt obj}. The current implementation only checks
    // equality of ClassId values, so it really doesn't take care of derived
    // classes.

    bool CheckClassType (const CL_Object& o, const char* msg) const;
    // Check that {\tt o} has the same type as this object (via {\tt
    // IsA}), and issue an error message containing {\tt msg} otherwise.
    // Return  the value returned by {\tt IsA}.
    
    // ------------------- Copying -----------------------------------

    virtual CL_Object* Clone () const;
    // Return a copy of ourselves. Whether this is a shallow or deep
    // copy depends on the derived class. All primitive (i.e.
    // non-container) subclasses must return a deep copy. The caller
    // must destroy the returned object. The default implementation issues a
    // "Not implemented" error message.

    
    // -------------------- Comparison operators -------------------

    
    virtual bool operator<  (const CL_Object& obj) const
    { return this < &obj; };
    // This and operator {\tt ==} constitute the two basic comparison
    // operators.
    // This operator must be implemented by every derived class that can
    // be thought of as defining an "orderable" type. The default
    // implementation uses addresses for comparison; therefore, if the
    // derived class doesn't care about ordering, this operator need not
    // be overridden.
    
    virtual bool operator== (const CL_Object& obj) const
    { return this == &obj; }
    // The default implementation checks for equality of addresses.


    virtual bool operator<= (const CL_Object& obj) const
        { return (*this == obj || *this < obj);};
    // This and the other three relational operators ({\tt >}, {\tt >=}
    // and {\tt !=}) have
    // default implementations that invoke the operators {\tt ==} and {\tt
    // <}. Thus a derived class that defines an "orderable" object need
    // only override the {\tt ==} and {\tt <} operators.

    virtual bool operator>  (const CL_Object& obj) const
        {return (! (*this <= obj));} ;

    virtual bool operator>= (const CL_Object& obj) const
        {return (! (*this < obj));};

    virtual bool operator!= (const CL_Object& obj) const
        {return (! (*this == obj));};

    virtual short Compare (const CL_Object& obj) const
        { return (*this == obj) ? 0 : ((*this < obj) ? -1 : 1);};
    // This method defines "strcmp"-style comparison. The default
    // implementation invokes (only) the operators {\tt ==} and {\tt <} on this
    // object, in that order. Therefore, if a derived class chooses to
    // override only the < operator, then == is still used on addresses.

    
    enum ComparisonOperator {OP_EQUAL = 0, OP_LESSTHAN, OP_GTRTHAN,
                             OP_LESSEQ, OP_GTREQ, OP_PREFIX,
                             OP_CONTAINS, OP_NOTEQUAL};
    virtual bool CompareWith (const CL_Object& obj,
                              ComparisonOperator op) const;
    // ``Indirect'' comparison, given a comparison operator:
    // compare ourselves with the given object (after ensuring
    // correct class id), and return a boolean value according to whether
    // the required relationship holds between us and the given
    // object. E.g., given objects p and q, {\small\tt p.CompareWith (q,
    // OP_LESSTHAN)} yields TRUE if p is less than q. For the PREFIX and
    // CONTAINS operators on non-string objects, the {\small\tt
    // AsString()} method 
    // is applied to both operands before doing the comparison.

    virtual bool CompareWith (const CL_String& obj,
                              ComparisonOperator op) const;
    // This is similar to Compare, except the given object is a string, so we
    // convert ourselves into a string (via the {\small\tt AsString()}
    // method on this object) before comparing.
    
    // ---------------------- Assignment ----------------------------

    virtual void operator= (const CL_Object&)
        { NotImplemented ("op= (const CL_Object&)");};
    // The default implementation issues a "Not implemented" error message.

    
    // ------------------- Passive representations of object -----------

    // ----- Representation as a printable string ---
    
    virtual CL_String AsString () const; 
    // Return a representation of this object in string
    // form. The default implementation issues a "Not implemented"
    // warning message via the {\small\tt NotImplemented} method, and
    // returns the null string; this method must be overridden by derived
    // classes.


    virtual void IntoStream (ostream& strm) const;
    // Write this object's string representation onto the given {\small\tt
    // ostream}.  The default implementation invokes {\small\tt
    // AsString()} and writes the result to the stream; this method may be
    // overridden by derived classes if necessary.


    virtual void FromStream (istream& stream) {};
    // Read this object's value from its string representation in the
    // given {\small\tt istream}.  The default implementation does
    // nothing; this method must be overridden by derived classes.


    // ----- Saving and restoration in binary form ----
    
    virtual long StorableFormWidth () const {return 0;};
    // Return the number of bytes in the binary storable representation
    // of this object. The default implementation returns 0.

    virtual bool ReadFrom (const CL_Stream&);
    // Read and reconstruct ourselves from the binary representation in
    // the given stream. Return TRUE on success, FALSE if failed for any
    // reason, including when a pre-change dependent disallows the change.
    // The default implementation issues a "Not implemented" message.

    virtual bool WriteTo  (CL_Stream&) const;
    // Write the passive binary representation of this object into the
    // given stream. Return TRUE if successful, FALSE otherwise. The
    // default implementation issues a "Not implemented" error message.

    // ------------------ Dependents and notification -------------------


    // ----- Post-change dependency methods:
    
    void Notify ();
    // Notify all our dependents by calling the associated methods,
    // with ourselves as parameter.
    
    void AddDependent (const CL_AbstractBinding& binding, long code);
    // Add a dependent to this object's dependent list. The given
    // method will be called on the given object when Notify() is
    // called. A binding can only appear once in our dependent set; so we
    // cannot add the same binding with different codes as dependents.
    
    void RemoveDependent (const CL_AbstractBinding& binding);
    // Remove a binding from our dependent list.
    
    bool HasDependent (const CL_AbstractBinding& b) const;
    // Is the given object dependent on us?

    long& PreChangeCode (const CL_AbstractBinding& b) const;
    // Return a reference to the notification code associated with the
    // given binding, if the latter is in our pre-change dependency set.
    // Return a reference to a zero-valued integer otherwise. The return
    // value may be modified by the caller of this method.


    // ----- Pre-change dependency methods:

    bool PrepareToChange ();
    // Ask all our pre-change dependents whether it's ok to change our
    // value; return FALSE if any of them returns FALSE, and TRUE if
    // all of them return TRUE.

    void AddPreChangeDependent (const CL_AbstractBinding& binding, long code);
    // Add a pre-change dependent.
    
    void RemovePreChangeDependent (const CL_AbstractBinding& binding);
    // Remove a binding from our dependent list
    
    bool HasPreChangeDependent (const CL_AbstractBinding& b) const;
    // Is the given object dependent on us?

    long& PostChangeCode (const CL_AbstractBinding& b) const;
    // Return a reference to the notification code associated with the
    // given binding, if the latter is in our post-change dependency set.
    // Return a reference to a zero-valued integer otherwise. The return
    // value may be modified by the caller of this method.

    // ------------------- End public protocol ----------------------------

protected:
    void NotImplemented (const char* method_name) const;

    bool ReadClassId (const CL_Stream&) const;

    static CL_Map<long,long>& _ClassIdMap;
    
private:
    CL_DependStruct* _dependSet;
};


class CL_ClassIdEntryMaker: public CL_Object {

public:
    CL_ClassIdEntryMaker (CL_ClassId id, CL_Creator func);

    ~CL_ClassIdEntryMaker ();

};


#define CL_DEFINE_CLASS(cls, cls_id)                            \
     CL_Object* CL_Creator__ ## cls ()                          \
     {                                                          \
         return new cls;                                        \
     }                                                          \
     static CL_ClassIdEntryMaker CL_AddEntry__ ## cls ## __instance \
     (cls_id, &CL_Creator__ ## cls)



// --------------------------- Operator definitions --------------

istream& operator>> (istream& s, CL_Object& o); // Uses the FromStream
                                                // method

ostream& operator<< (ostream& s, const CL_Object& o); // Uses the IntoStream
                                                      // method


// -------------------------- Inline methods ----------------------

inline CL_Object::CL_Object()
{
    _dependSet = NULL;
}


inline CL_Object::CL_Object (const CL_Object&)
{
    _dependSet = NULL;
}

inline bool CL_Object::CheckClassType (const CL_Object& o, const char*
                                       msg) const
{
    if (!IsA (o)) {
        CL_Error::Warning ("%s: Invalid class '%s'", msg, o.ClassName());
        return FALSE;
    }
    return TRUE;
}

inline CL_Object* CL_Object::Clone () const
{
    NotImplemented ("Clone"); return NULL;
}


inline void CL_Object::NotImplemented (const char* method_name) const
{
    CL_Error::Warning ("Class '%s' does not implement method '%s'\n",
                       ClassName(), method_name);
}
    


#endif
