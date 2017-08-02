#ifndef LOOKUPKEY_H
#define LOOKUPKEY_H


/* LookupKey.h -- declarations for Dictionary LookupKey

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

log:    LOOKUPKE.H $
Revision 1.2  95/01/29  13:27:20  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:20:14  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:11  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class LookupKey: public VIRTUAL Object {
        DECLARE_MEMBERS(LookupKey);
        Object* akey;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        LookupKey(Object& newKey =*nil);
        virtual int compare(const Object&) const;
        virtual void deepenShallowCopy();
        virtual void dumpOn(ostream& strm =cerr) const;
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual Object* key() const;
        virtual Object* key(Object& newkey);
        virtual void printOn(ostream& strm =cout) const;
        virtual Object* value();                    // derivedClassResponsibility()
        virtual const Object* value() const;        // derivedClassResponsibility()
        virtual Object* value(Object& newvalue);    // derivedClassResponsibility()
};

#endif
