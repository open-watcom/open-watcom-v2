/* LookupKey.c -- implementation of Dictionary LookupKey

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
        
LookupKey is an abstract class for managing the key object of an Assoc.
It is used to implement class Dictionary.

log:    LOOKUPKE.C $
Revision 1.1  90/05/20  04:20:12  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:09  kgorlen
 * Release for 1st edition.
 * 
*/

#include "LookupKey.h"
#include "nihclIO.h"

#define THIS    LookupKey
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(LookupKey,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\LOOKUPKE.C 1.1 90/05/20 04:20:12 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

LookupKey::LookupKey(Object& newKey)
{
        akey = &newKey;
}

Object* LookupKey::key() const { return akey; }

Object* LookupKey::key(Object& newkey)
{
        Object* temp = akey;
        akey = &newkey;
        return temp;
}

bool LookupKey::isEqual(const Object& ob) const { return ob.isEqual(*akey); }

unsigned LookupKey::hash() const { return akey->hash(); }

int LookupKey::compare(const Object& ob) const { return ob.compare(*akey); }

void LookupKey::deepenShallowCopy()
{
        akey = akey->deepCopy();
}

void LookupKey::dumpOn(ostream& strm) const
{
        strm << className() << '[';
        key()->dumpOn(strm);
        strm << "=>";
        value()->dumpOn(strm);
        strm << "]\n";
}

void LookupKey::printOn(ostream& strm) const
{
        key()->printOn(strm);
        strm << "=>";
        value()->printOn(strm);
}

const Object* LookupKey::value() const
{
        derivedClassResponsibility("value"); return 0;
}

Object* LookupKey::value()
{
        derivedClassResponsibility("value"); return 0;
}

Object* LookupKey::value(Object& /*newvalue*/)
{
        derivedClassResponsibility("value"); return 0;
}

LookupKey::LookupKey(OIOin& strm)
        : BASE(strm)
{
        akey = Object::readFrom(strm);
}

void LookupKey::storer(OIOout& strm) const
{
        BASE::storer(strm);
        akey->storeOn(strm);
}

LookupKey::LookupKey(OIOifd& fd)
        : BASE(fd)
{
        akey = Object::readFrom(fd);
}

void LookupKey::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        akey->storeOn(fd);
}
