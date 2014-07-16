/* Class.c -- implementation of class descriptor objects

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
        
Functions pertaining to class Class.  Most of these are defined inline
in object.h.

Modification History:

log:    CLASS.C $
Revision 1.1  92/11/10  12:10:54  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:16  kgorlen
 * Release for 1st edition.
 * 
*/

#include <stdarg.h>
#include "Object.h"
#include "String.ho"
#include "Dictionary.h"
#include "Assoc.h"
#include "OrderedCltn.h"
#include "IdentSet.h"
#include "nihclIO.h"
#include "OIOTbl.h"

extern const int NIHCL_RDBADSIG,NIHCL_RDBADTYP,NIHCL_RDWRONGCLASS,NIHCL_RDUNKCLASS,NIHCL_UNKCLASS;

#define THIS    Class
#define BASE    Object
#define BASE_CLASSES    BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

/* DEFINE_CLASS */

Object* Class::reader(OIOin&)
{
        setError(NIHCL_RDUNKCLASS,DEFAULT,"Class");
        return 0;
}
Object* Class::reader(OIOifd&)
{
        setError(NIHCL_RDUNKCLASS,DEFAULT,"Class");
        return 0;
}

static Class class_Class("Class",
        ClassList(0,BASE_CLASSES,0), ClassList(0,0), ClassList(0,0),
        1,      // version
        "header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\CLASS.C 1.1 92/11/10 12:10:54 Anthony_Scian Exp Locker: NT_Test_Machine $",
        sizeof(Class), Class::reader, Class::reader, NULL, NULL);

const Class* Class::desc()      { return &class_Class; }

const Class* Class::isA() const { return &class_Class; }

Object* Class::shallowCopy() const      { return new Class(*this); }

_DEFINE_CASTDOWN(Class)

// head of the list of all Classes, built by the static constructor of class Class 
static Class* allClasses =0;

static Dictionary classDictionary;      // Dictionary of all Classes 
Dictionary& Class::dictionary = classDictionary;

unsigned long Class::readFrom_level = 0;        // reset readFromTbl when 0
unsigned long Class::storeOn_level = 0;         // reset storeOnTbl when 0
unsigned long Class::addObjectFlag = 0;         // add object to readFromTbl when 0
IdentSet* Class::storeVBaseTbl = 0;             // table used by storeOn()
ReadFromTbl* Class::readFromTbl = 0;            // tables used by readFrom()
StoreOnTbl* Class::storeOnTbl = 0;              // tables used by storeOn()

#ifdef sparc
// Use magic name to make stdarg work
#define arg0 __builtin_va_alist
#endif

ClassList::ClassList(const char* arg0, ...)
{
        va_list ap;
        unsigned nargs = 0;
        va_start(ap, arg0);
        do nargs++; while (va_arg(ap, Class*));
        va_end(ap);
        Class** p = clp = new Class*[nargs];
        va_start(ap, arg0);
        while (nargs--) *p++ = va_arg(ap, Class*);
        va_end(ap);
}

Class::Class(const char* name,
        const ClassList& bases, const ClassList& members, const ClassList& vbases,
        unsigned version, const char* ident, unsigned size,
        Object* (*reader)(OIOin&),
        Object* (*binreader)(OIOifd&),
        initorTy initor1, initorTy initor2)
:
        class_name(name),
        class_ident(ident),
        class_bases(bases.clp),
        class_members(members.clp),
        class_vbases(vbases.clp),
        class_version(version),
        inst_size(size),
        inst_reader(reader),
        inst_binreader(binreader),
        class_initor2(initor2)
{
        nextClass = allClasses; allClasses = this;
        class_signature = 0;
        class_number = 0;
//      if (initor1 != 0) initor1(*this);       MASSCOMP cc bug
        initorTy initfun = initor1;
        if (initfun != 0) initfun(*this);
}

Class::Class(const Class& c)
:
        class_name(c.class_name),
        class_bases(c.class_bases),
        class_members(c.class_members),
        class_vbases(c.class_vbases),
        class_version(c.class_version),
        class_ident(c.class_ident),
        class_signature(c.class_signature),
        inst_size(c.inst_size),
        inst_reader(c.inst_reader),
        inst_binreader(c.inst_binreader),
        class_initor2(c.class_initor2),
        class_number(c.class_number)
{
}

Class::Class(OIOifd&)
:
        class_bases(0),
        class_members(0),
        class_vbases(0),
        class_version(0),
        inst_size(0),
        inst_reader(0),
        inst_binreader(0),
        class_initor2(0)
{
}

Class::Class(OIOin&)
:
        class_bases(0),
        class_members(0),
        class_vbases(0),
        class_version(0),
        inst_size(0),
        inst_reader(0),
        inst_binreader(0),
        class_initor2(0)
{
}

int Class::compare(const Object& ob) const      // compare Class names 
{
        assertArgSpecies(ob,class_Class,"compare");
        return strcmp(name(),castdown(ob).name());
}

unsigned Class::hash() const    { return (const unsigned)this; }

bool Class::isEqual(const Object& ob) const     { return this == castdown(&ob); }

bool Class::_isKindOf(const Class& clid) const
{
    if (this == &clid) return YES;
    /* WATCOM const */ Class** bp = baseClasses();
    while (*bp) if ((*bp++)->_isKindOf(clid)) return YES;
    return NO;
}

void Class::deepenShallowCopy() { shouldNotImplement("deepenShallowCopy"); }

void Class::dumpOn(ostream& strm) const
{
        strm << className() << '[' << name() << '\n';
        strm << "ident: " << ident() << '\n';
        strm << "version: " << version() << '\n';
        strm << "signature: " << signature() << '\n';
        strm << "number: " << number() << '\n';
        strm << "base classes:";
        /* WATCOM const */ Class** p = baseClasses();
        while (*p) strm << ' ' << (*p++)->name();
        strm << '\n';
        strm << "member classes:";
        p = memberClasses();
        while (*p) strm << ' ' << (*p++)->name();
        strm << '\n';
        strm << "virtual base classes:";
        p = virtualBaseClasses();
        while (*p) strm << ' ' << (*p++)->name();
        strm << '\n';
        strm << "]\n";
}

void Class::printOn(ostream& strm) const        { strm << class_name; }

unsigned Class::size() const    { return inst_size; }
        
void Class::addSignature(unsigned long s)
{
// Assumes that an unsigned long is at least 32 bits
        class_signature = ((class_signature << 3) + (class_signature >> (32-3) & 7) + s) & 0xFFFFFFFF;
}

void Class::computeSignature()
{
        if (this == Object::desc()) {
                class_signature = class_version;
                return;
        }
        Class** p = (Class**)class_bases;
        while (*p) addSignature((*p++)->signature());
        if (class_signature == 0) class_signature++;
        p = (Class**)class_members;
        while (*p) addSignature((*p++)->signature());
        if (class_version != 0) addSignature(class_version);
        if (class_signature == 0) class_signature++;
}

const Class* Class::lookup(const char* name)
// Find the class descriptor for class "name"
{
        LookupKey* asc = dictionary.assocAt(String(name));
        if (asc == 0) return 0;
        return castdown(asc->value());
}

class ReadFromTblMgr {
        friend Object* Class::readFrom(OIOin& strm) const;
        friend Object* Class::readFrom(OIOifd& fd) const;
        ReadFromTblMgr() {
                if (Class::readFrom_level++ == 0) Class::readFromTbl = new ReadFromTbl;
        }
        ~ReadFromTblMgr() {
                if (--Class::readFrom_level == 0) {
                        delete Class::readFromTbl;
                        Class::readFromTbl = 0;
                }
        }
};

Object* Class::readFrom(OIOin& strm) const
{
        ReadFromTblMgr tbl;
        return strm.readObject(*this);
}

Object* Class::readObject(OIOin& strm) const
// Called by OIOin::readObject()
{
#ifdef DEBUG_OBJIO
        cerr << "readFrom: read instance of class " << name();
#endif
        addObjectFlag = 0;      // causes Object::Object(OIOin&) to do Class::readFromTbl->add()
        return (*inst_reader)(strm);
}

Object::Object(OIOin&)
{
        if (Class::addObjectFlag++ == 0) {
                int objectNum = Class::readFromTbl->add(*this); // add object to readFromTbl
#ifdef DEBUG_OBJIO
                cerr << ", object #" << objectNum << "\n";
#endif
        }
}

Object* Class::readFrom(OIOifd& fd) const
{
        ReadFromTblMgr tbl;
        return fd.readObject(*this);
}

Object* Class::readObject(OIOifd& fd) const
// Called by OIOifd::readObject()
{
#ifdef DEBUG_OBJIO
        cerr << "readFrom: read instance of class " << name();
#endif
        addObjectFlag = 0;    // causes Object::Object(OIOifd&) to do Class::readFromTbl->add();
        return (*inst_binreader)(fd);
}

Object::Object(OIOifd&)
{
        if (Class::addObjectFlag++ == 0) {
                int objectNum = Class::readFromTbl->add(*this); // add object to readFromTbl
#ifdef DEBUG_OBJIO
                cerr << ", object #" << objectNum << "\n";
#endif
        }
}

void Class::initialize()        // class initialization
{
        register Class* clp = allClasses;   // add all classes to class dictionary

#ifdef DEBUG
while (clp != NULL) { clp->dumpOn(); clp = clp->nextClass; }
clp = allClasses;
#endif

        while (clp != NULL) {
                clp->signature();       // compute class signatures
                dictionary.add(*new Assoc(*new String(clp->name()), *clp));
                clp = clp->nextClass;
        }
        clp = allClasses;       // call the initor2 functions
        while (clp != NULL) {
                if (clp->class_initor2 != 0) (*clp->class_initor2)(*clp);
                clp = clp->nextClass;
        }
}

bool Class::_storeVBase(void* p)
{
        if (storeVBaseTbl == 0) {
                storeVBaseTbl = new IdentSet(256);
                storeVBaseTbl->add(*(Object*)p);
                return YES;
        }
        if (storeVBaseTbl->includes(*(Object*)p)) return NO;
        storeVBaseTbl->add(*(Object*)p);
        return YES;
}

void Class::storer(OIOout&) const { shouldNotImplement("storer"); }

void Class::storer(OIOofd&) const { shouldNotImplement("storer"); }
