/* KeySortCltn.c  -- implementation of class KeySortCltn

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        S. M. Orlow
        Systex Inc.
        Beltsville, MD 20705

Contractor:
        K. E. Gorlen
        Bg. 12A, Rm. 2033
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-1111
        uucp: uunet!nih-csl!kgorlen
        Internet: kgorlen@alw.nih.gov
        February, 1987

Function:
        
Modification History:

log:    KEYSORTC.C $
Revision 1.1  90/05/20  04:20:00  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:58  kgorlen
 * Release for 1st edition.
 * 
*/

#include "nihclIO.h"
#include "Assoc.h"
#include "KeySortCltn.h"

#define THIS    KeySortCltn
#define BASE    SortedCltn
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(KeySortCltn,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\KEYSORTC.C 1.1 90/05/20 04:20:00 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

KeySortCltn::KeySortCltn(int size) : BASE(size) {}

#ifndef BUG_TOOBIG
// yacc stack overflow
KeySortCltn::KeySortCltn(const KeySortCltn& c) : BASE(c) {}
#endif

LookupKey* KeySortCltn::assocAt(int i)
{
        return LookupKey::castdown(at(i));
}

Object* KeySortCltn::keyAt(int i)
{
        LookupKey& assoc = *assocAt(i);
        return assoc.key();
}

Object* KeySortCltn::valueAt(int i)
{
        LookupKey& assoc = *assocAt(i);
        return assoc.value();
}

Object* KeySortCltn::atKey(Object& key)
{
        // binary search for key
        int i = findIndexOf(key);

        if (i > -1 && key.compare(*keyAt(i)) == 0)
                return valueAt(i);
        else 
                return nil; // key not found
}

Assoc* KeySortCltn::addAssoc(Object& key, Object& val)
{
        Assoc* assoc = new Assoc(key,val);
        SortedCltn::add(*assoc);
        return assoc;
}

Collection& KeySortCltn::addValuesTo(Collection& c) const
{
        DO(*this,LookupKey,as) c.add(*(as->value())); OD
        return c;
}

Collection& KeySortCltn::addKeysTo(Collection& c) const
{
        DO(*this,LookupKey,as) c.add(*(as->key())); OD
        return c;
}

Collection& KeySortCltn::addContentsBeforeTo(Object& key,Collection& c)
{
        if (isEmpty()) return c;
        int k = findIndexOf(key);
        if (k<0) return c;
        for (int i=0; i<=k; i++ )
                c.add(*at(i));
        return c;
}

void KeySortCltn::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
}

int KeySortCltn::occurrencesOfKey(const Object& key) const
{
        return SortedCltn::occurrencesOf(LookupKey((Object&)key));
}

bool KeySortCltn::includesKey(Object& key) const
{
        return occurrencesOfKey(key) > 0;
}

Object* KeySortCltn::removeKey(Object& key) { return SortedCltn::remove(LookupKey(key)); }

int KeySortCltn::findIndexOf(Object& key) const
/*
        if key is found return index of key
        else if key is before all keys return -1
                else return index of last key before given key
*/
{
        return SortedCltn::findIndexOf(LookupKey(key));
}

Range KeySortCltn::findRangeOfKey(Object& key) const
{
        return SortedCltn::findRangeOf(LookupKey(key));
}

KeySortCltn::KeySortCltn(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm)
{
}

KeySortCltn::KeySortCltn(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd)

{
}
