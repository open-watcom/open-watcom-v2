#ifndef OBJECT_H
#define OBJECT_H


/* Object.h -- declarations for class Object and class Class

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Bg. 12A, Rm. 2033
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-1111
        uucp: uunet!nih-csl!kgorlen
        Internet: kgorlen@alw.nih.gov
        September, 1985

Function:
        
Define class Object, the root of the class tree, and class Class, the
class that describes class objects.

log:    OBJECT.H $
Revision 1.2  95/01/29  13:27:26  NT_Test_Machine
*** empty log message ***

Revision 1.1  94/09/14  14:28:58  Anthony_Scian
.

 * Revision 3.0  90/05/22  10:42:35  kgorlen
 * Release for 1st edition.
 * 
*/

// c_plusplus   defined by AT&T C++ Translator R2.0 and earlier
// __cplusplus  defined by AT&T C++ Translator R2.0 and later
// __GNUG__     defined by GNU C++
// _CPLUSPLUS_  defined by Oregon C++

// define if typedefs within a class have class scope
//#define NESTED_TYPES

#ifdef __GNUG__
// GNU C++ doesn't have endl
#define endl '\n';
#endif

#define bool int
#include <stddef.h>
#include <stdio.h>
#include <iostream.h>
#include "errors.h"
#undef bool

#ifdef MI
#define VIRTUAL virtual
#else
#define VIRTUAL
#endif

class Class;
class Dictionary;
class OIOifd;
class OIOofd;
class OIOin;
class OIOout;
class Object;
class OrderedCltn;
class ReadFromTbl;
class StoreOnTbl;
class IdentSet;

#define bool BOOL

typedef int bool;
const int YES = 1;
const int NO = 0;

inline char     ABS(char x)     { return x >= 0 ? x : -x; }
inline short    ABS(short x)    { return x >= 0 ? x : -x; }
inline int      ABS(int x)      { return x >= 0 ? x : -x; }
inline long     ABS(long x)     { return x >= 0 ? x : -x; }
inline float    ABS(float x)    { return x >= 0 ? x : -x; }
inline double   ABS(double x)   { return x >= 0 ? x : -x; }

inline char     MAX(char a,char b)      { return a >= b ? a : b; }
inline short    MAX(short a,short b)    { return a >= b ? a : b; }
inline int      MAX(int a,int b)        { return a >= b ? a : b; }
inline long     MAX(long a,long b)      { return a >= b ? a : b; }
inline float    MAX(float a,float b)    { return a >= b ? a : b; }
inline double   MAX(double a,double b)  { return a >= b ? a : b; }
inline void*    MAX(void* a,void* b)    { return a >= b ? a : b; }
inline unsigned char    MAX(unsigned char a, unsigned char b)   { return a >= b ? a : b; }
inline unsigned short   MAX(unsigned short a, unsigned short b) { return a >= b ? a : b; }
inline unsigned int     MAX(unsigned int a, unsigned int b)     { return a >= b ? a : b; }
inline unsigned long    MAX(unsigned long a, unsigned long b)   { return a >= b ? a : b; }

inline char     MIN(char a,char b)      { return a <= b ? a : b; }
inline short    MIN(short a,short b)    { return a <= b ? a : b; }
inline int      MIN(int a,int b)        { return a <= b ? a : b; }
inline long     MIN(long a,long b)      { return a <= b ? a : b; }
inline float    MIN(float a,float b)    { return a <= b ? a : b; }
inline double   MIN(double a,double b)  { return a <= b ? a : b; }
inline void*    MIN(void* a,void* b)    { return a <= b ? a : b; }
inline unsigned char    MIN(unsigned char a, unsigned char b)   { return a <= b ? a : b; }
inline unsigned short   MIN(unsigned short a, unsigned short b) { return a <= b ? a : b; }
inline unsigned int     MIN(unsigned int a, unsigned int b)     { return a <= b ? a : b; }
inline unsigned long    MIN(unsigned long a, unsigned long b)   { return a <= b ? a : b; }

static class NIHCL {
private:                        // static member variables
        static int initCount;           // see NIHCL::NIHCL() and NIHCL::~NIHCL()
        static bool init;               // YES if NIHCL initialization complete
        static unsigned char    char_bit_mask[sizeof(char)*8];
        static unsigned short   short_bit_mask[sizeof(short)*8];
        static unsigned int     int_bit_mask[sizeof(int)*8];
        static unsigned char    bit_count[256];
        static unsigned char    bit_reverse[256];
private:                        // static member functions
        static void initTables();       // initialize tables
public:                         // static member functions
        NIHCL();
        ~NIHCL();
        static unsigned char charBitMask(int i)         { return char_bit_mask[i]; }
        static unsigned short shortBitMask(int i)       { return short_bit_mask[i]; }
        static unsigned int intBitMask(int i)           { return int_bit_mask[i]; }
        static unsigned char bitCount(unsigned i)       { return bit_count[i]; }
        static unsigned char bitReverse(unsigned i)     { return bit_reverse[i]; }
        static void initialize();       // library initialization
        static bool initialized()       { return init; }
        static void setError(int error, int sev ...);   // set an NIHCL error condition
} NIHCL_init;

class ClassList : public NIHCL {
        Class** clp;
        friend class Class;
public:
        ClassList(const char*, ...);
        ~ClassList() {}         // that's right -- don't delete clp
};

#ifdef MI

#define DECLARE_CASTDOWN(classname) \
        static classname& castdown(Object& p) \
                { return *(classname*)(&p ? p._safe_castdown(*desc()) : 0); } \
        static const classname& castdown(const Object& p) \
                { return *(const classname*)(&p ? p._safe_castdown(*desc()) : 0); } \
        static classname* castdown(Object* p) \
                { return (classname*)(p ? p->_safe_castdown(*desc()) : 0); } \
        static const classname* castdown(const Object* p) \
                { return (const classname*)(p ? p->_safe_castdown(*desc()) : 0); } \

#else

#define DECLARE_CASTDOWN(classname) \
        static classname& castdown(Object& p)                   { return (classname&)p; } \
        static const classname& castdown(const Object& p)       { return (const classname&)p; } \
        static classname* castdown(Object* p)                   { return (classname*)p; } \
        static const classname* castdown(const Object* p)       { return (const classname*)p; } \

#endif

#define DECLARE_MEMBERS(classname) \
private: \
        static Class classDesc; \
public: \
        DECLARE_CASTDOWN(classname) \
        static const Class* desc()  { return &classDesc; } \
        static classname* readFrom(OIOin& strm) { return castdown(desc()->readFrom(strm)); } \
        static classname* readFrom(OIOifd& fd) { return castdown(desc()->readFrom(fd)); } \
        classname(OIOin&); \
        classname(OIOifd&); \
        virtual const Class* isA() const; \
        virtual Object* shallowCopy() const; \
        virtual void* _castdown(const Class&) const; \
protected: \
        void deepenVBase(); \
        void storeVBaseOn(OIOofd&) const; \
        void storeVBaseOn(OIOout&) const; \
/* WATCOM private: */ public: \
        static Object* reader(OIOin& strm); \
        static Object* reader(OIOifd& fd) \

/*
The STRINGIZE preprocessor macro converts a name into a character
string.  It can use one of three possible ways to accomplish this.
ANSI C preprocessors, which define the symbol __STDC__, recognize "#".
If you define the symbol BS_NL, the name will be separated from
surrounding double quotes by "\\\n"; otherwise, the name is simply
embedded in double quotes.
*/
#if defined(mc300)
#define   BS_NL
#endif

#ifdef __STDC__

#define STRINGIZE(s) #s

#else
#ifdef BS_NL

#define STRINGIZE(s) "\
s\
"

#else

#define STRINGIZE(s) "s"

#endif
#endif

#define _DEFINE_CLASS(classname) \
Object* classname::reader(OIOin& strm)  { return new classname(strm); } \
Object* classname::reader(OIOifd& fd)   { return new classname(fd); } \
Object* classname::shallowCopy() const  { return new classname(*this); } \

#define _DEFINE_ABSTRACT_CLASS(classname) \
extern const int NIHCL_RDABSTCLASS; \
Object* classname::reader(OIOin&)       { setError(NIHCL_RDABSTCLASS,DEFAULT,STRINGIZE(classname)); return 0; } \
Object* classname::reader(OIOifd&)      { setError(NIHCL_RDABSTCLASS,DEFAULT,STRINGIZE(classname)); return 0; } \
Object* classname::shallowCopy() const  { derivedClassResponsibility("shallowCopy"); return nil; } \

#define _DEFINE_CLASS_ALWAYS(classname,version,identification,initor1,initor2) \
Class classname::classDesc(STRINGIZE(classname),\
        ClassList(0,BASE_CLASSES,0), \
        ClassList(0,MEMBER_CLASSES-0,0), \
        ClassList(0,VIRTUAL_BASE_CLASSES-0,0), \
        version, identification, sizeof(classname), \
        classname::reader, classname::reader, \
        initor1, initor2 ); \
const Class* classname::isA() const     { return &classDesc; } \
void classname::deepenVBase() \
{ \
        if (Class::_deepenVBase((void*)this)) \
                classname::deepenShallowCopy(); \
} \
void classname::storeVBaseOn(OIOofd& fd) const \
{ \
        if (Class::_storeVBase((void*)this)) classname::storer(fd); \
} \
void classname::storeVBaseOn(OIOout& strm) const \
{ \
        if (Class::_storeVBase((void*)this)) classname::storer(strm); \
} \

#define _DEFINE_CASTDOWN(classname) \
void* classname::_castdown(const Class& target) const \
{ \
        if (&target == desc()) return (void*)this; \
        return BASE::_castdown(target); \
} \

#define DEFINE_CLASS(classname,version,identification,initor1,initor2) \
_DEFINE_CLASS(classname) \
_DEFINE_CLASS_ALWAYS(classname,version,identification,initor1,initor2) \
_DEFINE_CASTDOWN(classname) \

#define DEFINE_ABSTRACT_CLASS(classname,version,identification,initor1,initor2) \
_DEFINE_ABSTRACT_CLASS(classname) \
_DEFINE_CLASS_ALWAYS(classname,version,identification,initor1,initor2) \
_DEFINE_CASTDOWN(classname) \

#define DEFINE_CLASS_MI(classname,version,identification,initor1,initor2) \
_DEFINE_CLASS(classname) \
_DEFINE_CLASS_ALWAYS(classname,version,identification,initor1,initor2) \

#define DEFINE_ABSTRACT_CLASS_MI(classname,version,identification,initor1,initor2) \
_DEFINE_ABSTRACT_CLASS(classname) \
_DEFINE_CLASS_ALWAYS(classname,version,identification,initor1,initor2) \

class Object : public NIHCL {           // abstract class
public:
        static Object& castdown(Object& p)              { return p; }
        static const Object& castdown(const Object& p)  { return p; }
        static Object* castdown(Object* p)              { return p; }
        static const Object* castdown(const Object* p)  { return p; }
        static const Class* desc();
        virtual const Class* isA() const = 0;
        virtual Object* shallowCopy() const = 0;
/* WATCOM private: */ public:
        static Object* reader(OIOin& strm);
        static Object* reader(OIOifd& fd);
public:                 // static member variables
        static Object* const nil;               // pointer to sole instance of nil object
public:                 // static member functions
        static Object* readFrom(OIOifd& fd);
        static Object* readFrom(OIOin& strm);
protected:              // constructors for object I/O
        Object(OIOifd&);
        Object(OIOin&);
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
        friend class OIOout;
        friend class OIOofd;
protected:
        Object() {}
        void ambigCheck(void*&, void*&, const Class&) const;    // check for ambiguous castdown()
public:
        void assertArgClass(const Class& expect, const char* fname) const;      // validate non-member function argument class
        void assertArgClass(const Object& ob, const Class& expect, const char* fname) const;    // validate member function argument class
        void assertArgSpecies(const Class& expect, const char* fname) const;    // validate non-member function argument species
        void assertArgSpecies(const Object& ob, const Class& expect, const char* fname) const;  // validate member function argument species
        void assertClass(const Class& expect) const;            // validate object class
        void assertSpecies(const Class& expect) const;          // validate object species
        const char* className() const;                          // return class name 
        Object* deepCopy() const;                               // copy with distinct instance variables 
        void derivedClassResponsibility(const char*) const;     // unimplemented virtual function 
        void invalidArgClass(const Class& expect, const char* fname) const; // invalid non-member function argument class
        void invalidArgClass(const Object& ob, const Class& expect, const char* fname) const;  // invalid member function argument class
        void invalidArgSpecies(const Class& expect, const char* fname) const;   // invalid non-member function argument species error
        void invalidArgSpecies(const Object& ob, const Class& expect, const char* fname) const;  // invalid member function argument species error
        void invalidClass(const Class& expect) const;           // invalid object class error
        void invalidSpecies(const Class& expect) const;         // invalid object species error
        bool isKindOf(const Class&) const;                      // YES if MemberOf class or a superclass 
        bool isMemberOf(const Class& clid) const        { return isA()==&clid; }
        bool isSame(const Object& ob) const             { return this==&ob; }
        bool isSpecies(const Class& clid) const         { return species()==&clid; }
        void shouldNotImplement(const char*) const;             /* class cannot implement this function */
        void storeMemberOn(OIOofd&) const;                      // store object member in binary on file
        void storeMemberOn(OIOout&)     const;                  // store object member on stream 
        void storeOn(OIOofd&) const;                            // store object in binary on file
        void storeOn(OIOout&) const;                            // store object on stream
        void* _safe_castdown(const Class&) const;               // checked call to _castdown()
        virtual Object* addDependent(Object&);                  // add dependent object 
        virtual unsigned capacity() const;                      // subclass capacity 
        virtual void changed();                                 // notify dependents of change 
        virtual void changed(const Object&);                    // notify dependents of change 
        virtual int compare(const Object&) const = 0;           // compare objects 
        virtual Object* copy() const;                           // copy defaulted as shallowCopy 
        virtual void deepenShallowCopy() = 0;                   // convert shallow copy to deep copy 
        virtual OrderedCltn& dependents() const;                // return list of dependent objects 
        virtual void destroyer();                               // destroy object
        virtual void dumpOn(ostream& strm =cerr) const;         // printOn() with class name
        virtual unsigned hash() const = 0;                      // calculate object hash 
        virtual bool isEqual(const Object&) const = 0;          // equality test 
        virtual void scanFrom(istream& strm);                   // parse object from stream 
        virtual void printOn(ostream& strm =cout) const = 0;    // print object on stream 
        virtual void release();                                 // remove all dependent objects 
        virtual Object* removeDependent(const Object&);         // remove dependent object 
        virtual unsigned size() const;                          // # of objects in array/container subclass
        virtual const Class* species() const;                   // return species class descriptor address 
        virtual void update(const Object&, const Object&);      // object change notification 
        virtual void* _castdown(const Class&) const;            // cast this to derived class
};

#ifndef NESTED_TYPES
typedef void (*initorTy)(const Class&);
#endif

class StoreOnTblMgr;
class ReadFromTblMgr;

class Class : public VIRTUAL Object {   // class descriptor object 
private:                        // static member variables
        static unsigned long readFrom_level;    // reset readFromTbl when 0
        static unsigned long storeOn_level;     // reset storeOnTbl when 0
        static unsigned long addObjectFlag;     // add object to readFromTbl when 0
        friend Object::Object(OIOin&);
        friend Object::Object(OIOifd&);
        static IdentSet* storeVBaseTbl;         // table used by storeOn()
        friend class StoreOnTblMgr;
        friend class ReadFromTblMgr;
public:                         // static member variables
        static Dictionary& dictionary;          // class descriptor dictionary
        static ReadFromTbl* readFromTbl;        // tables used by readFrom()
        static StoreOnTbl* storeOnTbl;          // tables used by storeOn()
public:
#ifdef NESTED_TYPES
        typedef void (*initorTy)(const Class&);
#endif
        static const Class* desc();
        virtual const Class* isA() const;
        virtual Object* shallowCopy() const;
/* WATCOM private: */ public:                   // static member functions
        static Object* reader(OIOin& strm);
        static Object* reader(OIOifd& fd);
public:                         // static member functions
        DECLARE_CASTDOWN(Class)
        static void initialize();       // class initialization
        static const Class* lookup(const char* name);
        static unsigned long readFromLevel()    { return readFrom_level; }
        static unsigned long storeOnLevel()     { return storeOn_level; }
        static bool _deepenVBase(void*);
        static bool _storeVBase(void*);
private:                        // member variables
        const char* class_name;                 // class name 
        const char* class_ident;                // class RCS identification header
        /* WATCOM const */ Class* */* WATCOM const*/ class_bases;       // pointer to base class list
        /* WATCOM const */ Class* */* WATCOM const*/ class_members;     // pointer to member class list
        /* WATCOM const */ Class* */* WATCOM const*/ class_vbases;      // pointer to virtual base class list
        /* WATCOM const */ unsigned class_version;              // class version number 
        /* WATCOM const */ unsigned inst_size;          // sizeof instance variables 
        Object* (*/* WATCOM const*/ inst_reader)(OIOin&);       // object reader function 
        Object* (*/* WATCOM const*/ inst_binreader)(OIOifd&);  // binary reader function
        /* WATCOM const */ initorTy class_initor2;              // phase 2 class initor
        Class* nextClass;                       // link for list of all Class objects 
        unsigned long class_signature;          // class signature
        unsigned class_number;                  // class number, used by storeOn()
private:                        // private member functions
        unsigned number(unsigned n)     { return class_number = n; }
        friend class StoreOnTbl;
        Object* readObject(OIOin&) const;
        friend class OIOin;
        Object* readObject(OIOifd&) const;
        friend class OIOifd;
        void addSignature(unsigned long);
        void computeSignature();
public:
        Class(const char* name,
                const ClassList& bases, const ClassList& members, const ClassList& vbases,
                unsigned version, const char* ident, unsigned size,
                Object* (*reader)(OIOin&), 
                Object* (*binreader)(OIOifd&), 
                initorTy initor1 =0, initorTy initor2 =0);
        Class(const Class&);
        Class(OIOifd&);
        Class(OIOin&);
        const char* name() const                        { return class_name; }
        /* WATCOM const */ Class** baseClasses() const          { return class_bases; }
        /* WATCOM const */ Class** memberClasses() const                { return class_members; }
        /* WATCOM const */ Class** virtualBaseClasses() const   { return class_vbases; }
        const char* ident() const                       { return class_ident; }
        unsigned number() const                         { return class_number; }
        unsigned long signature() const {
                if (class_signature == 0) ((Class*)this)->computeSignature();
                return class_signature;
        }
        unsigned version() const                        { return class_version; }
        Object* readFrom(OIOifd& fd) const;             // read binary object from file
        Object* readFrom(OIOin&) const;                 // read object from stream 
        bool _isKindOf(const Class&) const;
        virtual int compare(const Object&) const;       // compare class names 
        virtual void dumpOn(ostream& strm =cerr) const;
        virtual unsigned hash() const;
        virtual bool isEqual(const Object& ob) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual unsigned size() const;
        virtual void* _castdown(const Class&) const;
private:                        // shouldNotImplement
        virtual void deepenShallowCopy();
        virtual void storer(OIOout&) const;
        virtual void storer(OIOofd&) const;
};

inline const char* Object::className() const    { return isA()->name(); }
                
inline Object* Object::readFrom(OIOifd& fd)             { return desc()->readFrom(fd); }
inline Object* Object::readFrom(OIOin& strm)    { return desc()->readFrom(strm); }

inline void Object::assertArgClass(const Class& expect, const char* fname) const
{
        if (!isKindOf(expect)) invalidArgClass(expect,fname);
}

inline void Object::assertArgClass(const Object& ob, const Class& expect, const char* fname) const
{
        if (!(ob.isKindOf(expect))) invalidArgClass(ob,expect,fname);
}

inline void Object::assertArgSpecies(const Class& expect, const char* fname) const
{
        if (!isSpecies(expect)) invalidArgSpecies(expect,fname);
}

inline void Object::assertArgSpecies(const Object& ob, const Class& expect, const char* fname) const
{
        if (!(ob.isSpecies(expect))) this->invalidArgSpecies(ob,expect,fname);
}

inline void Object::assertClass(const Class& expect) const
{
        if (!isKindOf(expect)) invalidClass(expect);
}

inline void Object::assertSpecies(const Class& expect) const
{
        if (!isSpecies(expect)) invalidSpecies(expect);
}

inline istream& operator>>(istream& strm, Object& ob)
{
        ob.scanFrom(strm);
        return strm;
}

inline ostream& operator<<(ostream& strm, const Object& ob)
{
        ob.printOn(strm);
        return strm;
}

#endif
