#ifndef KEYSORTCLTNH
#define KEYSORTCLTNH

/* KeySortCltn.h -- header file for class KeySortCltn

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        S. M. Orlow
        Systex, Inc.
        Beltsville, MD 20705

Contractor:
        K. E. Gorlen
        Computer Systems Laboratory, DCRT
        National Institutes of Health
        Bethesda, MD 20892

Modification History:

log:    KEYSORTC.H $
Revision 1.1  90/05/20  04:20:02  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:59  kgorlen
 * Release for 1st edition.
 * 
*/

#include "SortedCltn.h"
#include "LookupKey.h"

class Assoc;

class KeySortCltn: public SortedCltn {
        DECLARE_MEMBERS(KeySortCltn);
#ifndef BUG_38
// internal <<AT&T C++ Translator 2.00 06/30/89>> error: bus error (or something nasty like that)
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd& fd) const   { SortedCltn::storer(fd); };
        virtual void storer(OIOout& strm) const { SortedCltn::storer(strm); };
#endif
public:
        KeySortCltn(int size =DEFAULT_CAPACITY);
#ifndef BUG_TOOBIG
// yacc stack overflow
        KeySortCltn(const KeySortCltn&);
#endif

        LookupKey* assocAt(int i);
        Object* keyAt(int i);
        Object* valueAt(int i);
        Object* atKey(Object& key);

        Assoc* addAssoc(Object& key,Object& val);
        Collection& addValuesTo(Collection&) const;
        Collection& addKeysTo(Collection&) const;
        Collection& addContentsBeforeTo(Object&,Collection&);

        virtual void deepenShallowCopy();
        virtual int findIndexOf(Object& key) const;
        virtual Range findRangeOfKey(Object& key) const;
        virtual bool includesKey(Object& key) const;
        virtual int occurrencesOfKey(const Object& key) const;
        virtual Object* removeKey(Object& key);
};

#endif
