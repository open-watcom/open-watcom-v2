/* Object.c -- implementation of class Object

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

Operations applicable to all objects.
        
log:    OBJECT.C $
Revision 1.1  93/02/16  15:59:00  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:35  kgorlen
 * Release for 1st edition.
 * 
*/

#include <ctype.h>
#include <string.h>
//#include "nihclconfig.h"
#include "nihclIO.h"
#include "OIOTbl.h"
#include "Object.h"
#include "Dictionary.h"
#include "LookupKey.h"
#include "IdentDict.h"
#include "IdentSet.h"
#include "String.ho"
#include "Assoc.h"
#include "AssocInt.h"
#include "OrderedCltn.h"

extern const int NIHCL_AMBIGCASTDN,NIHCL_DRVDCLASSRSP,NIHCL_ILLEGALMFCN,NIHCL_BADARGCL,
        NIHCL_BADARGSP,NIHCL_BADARGCLM,NIHCL_BADARGSPM,NIHCL_BADCASTDN,NIHCL_BADCLASS,
        NIHCL_BADSTMBR,NIHCL_BADSPEC,NIHCL_NOISA,NIHCL_RDABSTCLASS;

#define THIS    Object
#define BASE_CLASSES
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

/* DEFINE_CLASS(); */

Object* Object::reader(OIOin&)
{
        setError(NIHCL_RDABSTCLASS,DEFAULT,"Object");
        return 0;
}

Object* Object::reader(OIOifd&)
{
        setError(NIHCL_RDABSTCLASS,DEFAULT,"Object");
        return 0;
}

static Class class_Object("Object",
        ClassList(0,0), ClassList(0,0), ClassList(0,0),
        1,      // version
        "header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\OBJECT.C 1.1 93/02/16 15:59:00 Anthony_Scian Exp Locker: NT_Test_Machine $",
        sizeof(Object), Object::reader, Object::reader, NULL, NULL );

const Class* Object::desc()     { return &class_Object; }

void* Object::_castdown(const Class& target) const
{
        if (&target == &class_Object) return (void*)this;
        return 0;
}

void* Object::_safe_castdown(const Class& target) const
{
        void* p = _castdown(target);
        if (p == 0) setError(NIHCL_BADCASTDN,DEFAULT,this,className(),target.name());
        return p;
}

void Object::ambigCheck(void*& p, void*& q, const Class& target) const
{
        if (p == 0 || p == q) return;
        if (q == 0) { q = p;  return; }
        setError(NIHCL_AMBIGCASTDN,DEFAULT,this,className(),target.name());
}

bool Object::isKindOf(const Class& clid) const
{
        return isA()->_isKindOf(clid);
}

unsigned Object::size() const { return 0; }

unsigned Object::capacity() const { return 0; }

const Class* Object::species() const    { return isA(); }

Object* Object::copy() const { return shallowCopy(); }

static IdentDict* deepCopyDict =0;      // object ID -> object copy dictionary for deepCopy()
static IdentSet* deepCopyVBaseSet =0;   // deepened virtual base set for deepCopy()

Object* Object::deepCopy() const
{
        bool firstObject = NO;
        if (deepCopyDict == 0) {
                deepCopyDict = new IdentDict;
                deepCopyDict->add(*new Assoc(*nil,*nil));
                firstObject = YES;
        }
        Assoc* asc = (Assoc*)deepCopyDict->assocAt(*this);
        if (asc == 0) {                         // object has not been copied 
                Object* copy = shallowCopy();   // make a shallow copy 
                deepCopyDict->add(*new Assoc(*(Object*)this,*copy));    // add to dictionary 
                copy->deepenShallowCopy();      // convert shallow copy to deep copy 
                if (firstObject) {              // delete the deepCopy dictionary 
                        DO(*deepCopyDict,Assoc,asc) delete asc; OD
                        delete deepCopyDict;
                        deepCopyDict = 0;
                        delete deepCopyVBaseSet;  // and the deepen VBase IdentSet
                        deepCopyVBaseSet = 0;
                }
                return copy;
        }
        else return asc->value();       // object already copied, just return object reference 
}

bool Class::_deepenVBase(void* p)
{
        if (deepCopyVBaseSet == 0) {
                deepCopyVBaseSet = new IdentSet(256);
                deepCopyVBaseSet->add(*(Object*)p);
                return YES;
        }
        if (deepCopyVBaseSet->includes(*(Object*)p)) return NO;
        deepCopyVBaseSet->add(*(Object*)p);
        return YES;
}

// error reporting 

void Object::derivedClassResponsibility(const char* fname) const
{
        setError(NIHCL_DRVDCLASSRSP,DEFAULT,this,className(),fname);
}

void Object::destroyer() {}

void Object::shouldNotImplement(const char* fname) const
{
        setError(NIHCL_ILLEGALMFCN,DEFAULT,this,className(),fname);
}

void Object::invalidArgClass(const Class& expect, const char* fname) const
{
        setError(NIHCL_BADARGCL,DEFAULT,fname,expect.name(),fname,className());
}

void Object::invalidArgClass(const Object& ob, const Class& expect, const char* fname) const
{
        setError(NIHCL_BADARGCLM,DEFAULT,className(),fname,expect.name(),className(),fname,ob.className());
}

void Object::invalidArgSpecies(const Class& expect, const char* fname) const
{
        setError(NIHCL_BADARGSP,DEFAULT,fname,expect.name(),fname,species()->className());
}

void Object::invalidArgSpecies(const Object& ob, const Class& expect, const char* fname) const
{
        setError(NIHCL_BADARGSPM,DEFAULT,className(),fname,expect.name(),className(),fname,ob.species()->name());
}

void Object::invalidClass(const Class& expect) const
{
        setError(NIHCL_BADCLASS,DEFAULT,expect.name(),className());
}

void Object::invalidSpecies(const Class& expect) const
{
        setError(NIHCL_BADSPEC,DEFAULT,expect.name(),species()->name());
}

void Object::dumpOn(ostream& strm) const
{
        strm << className() << '[';
        printOn(strm);
        strm << ']' << endl;
}

void Object::scanFrom(istream&) { derivedClassResponsibility("scanFrom"); }

// Object I/O

class StoreOnTblMgr {
        friend void Object::storeOn(OIOout& strm) const;
        friend void Object::storeOn(OIOofd& fd) const;
        StoreOnTblMgr() {
                if (Class::storeOn_level++ == 0) Class::storeOnTbl = new StoreOnTbl;
        }
        ~StoreOnTblMgr() {
                if (--Class::storeOn_level == 0) {
                        delete Class::storeOnTbl;
                        Class::storeOnTbl = 0;
                        delete Class::storeVBaseTbl;
                        Class::storeVBaseTbl = 0;
                }
        }
};

void Object::storeOn(OIOout& strm) const
{
        StoreOnTblMgr tbl;
        strm.storeObject(*this);
}

void Object::storeMemberOn(OIOout& strm) const
{
        if (Class::storeOnLevel() == 0) setError(NIHCL_BADSTMBR,DEFAULT,this,className());
        Class::storeOnTbl->addMember(*this);
        storer(strm);
}


void Object::storer(OIOout& /*strm*/) const     // store class Object 
{
}

// binary object I/O

void Object::storeOn(OIOofd& fd) const
{
        StoreOnTblMgr tbl;
        fd.storeObject(*this);
}

void Object::storeMemberOn(OIOofd& fd) const
{
        if (Class::storeOnLevel() == 0) setError(NIHCL_BADSTMBR,DEFAULT,this,className());
        Class::storeOnTbl->addMember(*this);
        storer(fd);
}

void Object::storer(OIOofd& /*fd*/) const       // store Object on file descriptor
{
}

// Object Dependence Relationships 

static IdentDict* dependDict =0;        // object ID -> dependents list 
        
Object* Object::addDependent(Object& ob)
{
        if (dependDict == 0) dependDict = new IdentDict;
        if (!(dependDict->includesKey(*this)))
                dependDict->add(*new Assoc(*this,*new OrderedCltn));
        OrderedCltn::castdown(dependDict->atKey(*this))->add(ob);
        return &ob;
}

Object* Object::removeDependent(const Object& ob)
{
        if (dependDict == 0) return nil;
        Object* val = dependDict->atKey(*this);
        if (val == nil) return nil;
        OrderedCltn* depList = OrderedCltn::castdown(val);
        Object* dependent = depList->removeId(ob);
        if (depList->size() == 0) release();
        return dependent;
}
        
OrderedCltn& Object::dependents() const
{
        if (dependDict == 0) return *new OrderedCltn(1);
        Assoc* asc = (Assoc*)dependDict->assocAt(*this);
        if (asc == 0) return *new OrderedCltn(1);
        return *new OrderedCltn(*OrderedCltn::castdown(asc->value()));
}
        
void Object::release()
{
        if (dependDict != 0 && dependDict->includesKey(*this)) {
                Assoc* asc = (Assoc*)dependDict->removeKey(*this);
                OrderedCltn* oc = OrderedCltn::castdown(asc->value());
                delete oc;
                delete asc;
                if (dependDict->size() == 0) {
                        delete dependDict;
                        dependDict = 0;
                }
        }
}

void Object::changed(const Object& param)
{
        OrderedCltn* depList = &dependents();
        DO(*depList,Object,depob) depob->update(*this,param); OD
        delete depList;
}

void Object::changed()  { changed(*nil); }

void Object::update(const Object& /*dependent*/, const Object& /*param*/)
{
        derivedClassResponsibility("update");
}
