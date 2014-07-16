/* IdentDict.c -- implementation of Identifier Dictionary

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
        October, 1985

Function:
        
An IdentDict is like a Dictionary, except keys are compared using
isSame() rather than isEqual().

log:    IDENTDIC.C $
Revision 1.1  90/05/20  04:19:50  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:48  kgorlen
 * Release for 1st edition.
 * 
*/

#include "IdentDict.h"
#include "LookupKey.h"

#define THIS    IdentDict
#define BASE    Dictionary
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(IdentDict,0,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\IDENTDIC.C 1.1 90/05/20 04:19:50 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

IdentDict::IdentDict(unsigned size) : BASE(size) {}

#ifndef BUG_TOOBIG
// yacc stack overflow
IdentDict::IdentDict(const IdentDict& c) : BASE(c) {}
#endif

void IdentDict::operator=(const IdentDict& d)
{
        this->Dictionary::operator=(d);
}

int IdentDict::findIndexOf(const Object& ob) const
/*
Search this IdentDict for a LookupKey with the same key object as the
argument.

Enter:
        ob = pointer to LookupKey to search for

Returns:
        index of object if found or of nil slot if not found
        
Algorithm L, Knuth Vol. 3, p. 519
*/
{
        register int i;
        Object* keyob = LookupKey::castdown(ob).key();
        for (i = h((int)keyob); contents[i]!=nil; i = (i-1)&mask) {
                if (LookupKey::castdown(contents[i])->key()->isSame(*keyob)) return i;
        }
        return i;
}

Object* IdentDict::atKey(const Object& key) const
{
        return Dictionary::atKey(LookupKey((Object&)key));
}

Object* IdentDict::atKey(const Object& key, Object& newValue)
{
        return Dictionary::atKey(LookupKey((Object&)key), newValue);
}

LookupKey* IdentDict::assocAt(const Object& key) const
{
        return Dictionary::assocAt(LookupKey((Object&)key));
}

bool IdentDict::includesKey(const Object& key) const
{
        return Dictionary::includesKey(LookupKey((Object&)key));
}

IdentDict::IdentDict(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm)
{
}

IdentDict::IdentDict(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd)
{
}
