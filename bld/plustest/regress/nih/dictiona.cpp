/* Dictionary.c -- implementation of Set of Associations

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
        
A Dictionary is a Set of Associations.  A Dictionary returns the value
of an association given its key.

log:    DICTIONA.C $
Revision 1.1  92/11/10  12:15:20  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:26  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Dictionary.h"
#include "LookupKey.h"
#include "Assoc.h"

#define THIS    Dictionary
#define BASE    Set
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(Dictionary,0,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\DICTIONA.C 1.1 92/11/10 12:15:20 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_DUPKEY,NIHCL_KEYNOTFOUND;

Dictionary::Dictionary(unsigned size) : BASE(size) {}

void Dictionary::operator=(const Dictionary& d)
{
        this->Set::operator=(d);
}

bool Dictionary::operator==(const Dictionary& d) const
{
        if (size() != d.size()) return NO;
        DO(*this,LookupKey,a) if (!d.includesAssoc(*a)) return NO; OD
        return YES;
}

Object* Dictionary::add(Object& ob)
{
        assertArgClass(ob,*LookupKey::desc(),"add");
        return Set::add(ob);
}

Assoc* Dictionary::addAssoc(Object& key, Object& value)
{
        Assoc* a = new Assoc(key,value);
        Assoc* b = Assoc::castdown(Set::add(*a));
        if (a != b) {
                delete a;
                setError(NIHCL_DUPKEY,DEFAULT,this,className(),"addAssoc",key.className(),&key);
        }
        return b;
}

Collection& Dictionary::addContentsTo(Collection& cltn) const
{
        DO(*this,LookupKey,a) cltn.add(*(a->value())); OD
        return cltn;
}

Object* Dictionary::remove(const Object& ob)
{
        assertArgClass(ob,*LookupKey::desc(),"remove");
        return Set::remove(ob);
}

Object* Dictionary::atKey(const Object& key) const
{
        register Object* p = findObjectWithKey(key);
        if (p==nil) setError(NIHCL_KEYNOTFOUND,DEFAULT,this,className(),key.className(),&key);
        else return LookupKey::castdown(p)->value();
        /* WATCOM */ return 0;
}

Object* Dictionary::atKey(const Object& key, Object& newValue)
{
        register Object* p = findObjectWithKey(key);
        if (p==nil) setError(NIHCL_KEYNOTFOUND,DEFAULT,this,className(),key.className(),&key);
        else return LookupKey::castdown(p)->value(newValue);
        /* WATCOM */ return 0;
}

LookupKey* Dictionary::assocAt(const Object& key) const
{
        Object* lk = findObjectWithKey(key);
        if (lk == nil) return 0;
        return LookupKey::castdown(lk);
}

Collection& Dictionary::addKeysTo(Collection& cltn) const
{
        DO(*this,LookupKey,a) cltn.add(*a->key()); OD
        return cltn;
}

Collection& Dictionary::addValuesTo(Collection& cltn) const
{
        return addContentsTo(cltn);
}

Object* Dictionary::keyAtValue(const Object& val) const
{
        DO(*this,LookupKey,a) if (val.isEqual(*a->value())) return a->key(); OD
        return nil;
}

unsigned Dictionary::occurrencesOf(const Object& val) const
{
        unsigned n =0;
        DO(*this,LookupKey,a) if (val.isEqual(*a->value())) n++; OD
        return n;
}

bool Dictionary::includesAssoc(const LookupKey& asc) const
{
        register Object* p = findObjectWithKey(asc);
        if (p==nil) return NO;
        return asc.value()->isEqual(*(LookupKey::castdown(p)->value()));
}

bool Dictionary::includesKey(const Object& key) const
{
        if (findObjectWithKey(key) == nil) return NO;
        else return YES;
}

bool Dictionary::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && *this==castdown(ob);
}

const Class* Dictionary::species() const { return &classDesc; }

LookupKey* Dictionary::removeAssoc(const LookupKey& asc)
{
        return LookupKey::castdown(remove(asc));
}

LookupKey* Dictionary::removeKey(const Object& key)
{
        LookupKey* lk = assocAt(key);
        if (lk == 0) setError(NIHCL_KEYNOTFOUND,DEFAULT,this,className(),key.className(),&key);
        return removeAssoc(*lk);
}

Dictionary::Dictionary(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd)
{
}

Dictionary::Dictionary(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm)
{
}

int Dictionary::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}
