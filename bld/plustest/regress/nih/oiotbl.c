/* OIOTbl.c -- Implementation of tables used for Object I/O

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
        Internet:kgorlen@alw.nih.gov
        July, 1988

Function:
        
Tables to keep track of multiple references to objects and classes.
Used by storeOn()/readFrom().

Modification History:

log:    OIOTBL.C $
Revision 1.1  90/05/20  04:20:26  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:24  kgorlen
 * Release for 1st edition.
 * 
*/

#include "OIOTbl.h"
#include "AssocInt.h"

extern const int NIHCL_STMBRREF,NIHCL_STREFMBR;

StoreOnTbl::StoreOnTbl(unsigned size)
        : d(size), c(Class::dictionary.size())
{
        d.add(*new AssocInt(*Object::nil,0));   // add nil object
        objNum = classNum = 0;
}

StoreOnTbl::~StoreOnTbl()
{
        DO(d,AssocInt,asc) delete asc; OD
        DO(c,Class,cl) cl->number(0); OD
}

bool StoreOnTbl::add(const Object& obj, int& num)
{
        AssocInt* asc = (AssocInt*)d.assocAt(obj);
        if (asc == 0) { // object has not been stored
                d.add(*new AssocInt((Object&)obj, (num = ++objNum)));
                return YES;     // need to store object
        }
// return number of stored object
        num = (int)Integer::castdown(asc->value())->value(); 
        if (num == -1) setError(NIHCL_STREFMBR,DEFAULT,&obj,obj.className());
        return NO;              // don't need to store object -- use reference
}

bool StoreOnTbl::addClass(const Class* cp, unsigned short& num)
{
        if ((num = cp->number()) == 0) { 
                c.add(*(Class*)cp);
                ((Class*)cp)->number(num = ++classNum);
                return YES;     // need to store class
        }
        return NO;      // don't need to store class -- use reference
}

void StoreOnTbl::addMember(const Object& obj)
{
        AssocInt* asc = (AssocInt*)d.assocAt(obj);
        if (asc == 0) {                 // object has not been stored 
                d.add(*new AssocInt((Object&)obj,-1));  // add member object to IdentDict
                return;
        }
// Error -- tried to store a member object that has already been stored
        int objectNum = (int)Integer::castdown(asc->value())->value(); 
        setError(NIHCL_STMBRREF,DEFAULT,objectNum,&obj,obj.className());
}

ReadFromTbl::ReadFromTbl(unsigned size)
        : obs(size), cls(Class::dictionary.size())
{
        obs.add(*Object::nil);          // nil is @0
        cls.add(*Object::nil);
}

int ReadFromTbl::add(Object& obj)
{
        int objectNum = obs.size();
        if (objectNum == obs.capacity()) obs.reSize(objectNum+1024);
        obs.add(obj);
        return objectNum;
}

int ReadFromTbl::add(const Class* cl)
{
        int classNum = cls.size();
        cls.add(*(Class*)cl);
        return classNum;
}

Object* ReadFromTbl::objectAt(int i)
{
        return obs[i];
}

const Class* ReadFromTbl::classAt(int i)
{
        return Class::castdown(cls[i]);
}
