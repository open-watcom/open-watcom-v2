#ifndef DICTIONARY_H
#define DICTIONARY_H


/* Dictionary.h -- declarations for Set of Associations

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Computer Systems Laboratory, DCRT
        National Institutes of Health
        Bethesda, MD 20892

log:    DICTIONA.H $
Revision 1.2  95/01/29  13:27:02  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:30  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:27  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Set.h"

class Assoc;
class LookupKey;
class OrderedCltn;

class Dictionary: public Set {
        DECLARE_MEMBERS(Dictionary);
#ifndef BUG_38
// internal <<AT&T C++ Translator 2.00 06/30/89>> error: bus error (or something nasty like that)
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd& fd) const   { Set::storer(fd); };
        virtual void storer(OIOout& strm) const { Set::storer(strm); };
#endif
public:
        Dictionary(unsigned size =DEFAULT_CAPACITY);
        void operator=(const Dictionary&);
        bool operator==(const Dictionary&) const;
        bool operator!=(const Dictionary& d) const { return !(*this == d); }
        virtual Object* add(Object&);
        virtual Assoc* addAssoc(Object& key, Object& value);
        virtual Collection& addContentsTo(Collection&) const;
        virtual Collection& addKeysTo(Collection&) const;
        virtual Collection& addValuesTo(Collection&) const;
        virtual LookupKey* assocAt(const Object& key) const;
        virtual Object* atKey(const Object& key) const;
        virtual Object* atKey(const Object& key, Object& newValue);
        virtual bool includesAssoc(const LookupKey& asc) const;
        virtual bool includesKey(const Object& key) const;
        virtual bool isEqual(const Object&) const;
        virtual Object* keyAtValue(const Object& val) const;
        virtual unsigned occurrencesOf(const Object& val) const;
        virtual Object* remove(const Object& asc);
        virtual LookupKey* removeAssoc(const LookupKey& asc);
        virtual LookupKey* removeKey(const Object& key);
        virtual const Class* species() const;
private:                                // shouldNotImplement()
        virtual int compare(const Object&) const;
};

#endif
