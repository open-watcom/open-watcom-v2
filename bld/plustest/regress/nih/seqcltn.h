#ifndef SEQCLTN_H
#define SEQCLTN_H


/* SeqCltn.h -- declarations for abstract sequential collections

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

log:    SEQCLTN.H $
Revision 1.2  95/01/29  13:27:36  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:21:16  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:13  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Collection.h"

class SeqCltn: public Collection {
        DECLARE_MEMBERS(SeqCltn);
protected:
        SeqCltn();
        void indexRangeErr() const;
public:
        virtual Object* add(Object&) = 0;
        virtual Object*& at(int) = 0;
        virtual const Object *const& at(int) const = 0;
        virtual void atAllPut(Object& ob) = 0;
        virtual int compare(const Object&) const;
        virtual Object* doNext(Iterator&) const;
        virtual Object* first() const;
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual int indexOf(const Object& ob) const;
        virtual int indexOfSubCollection(const SeqCltn& cltn, int start=0) const = 0;
        virtual Object* last() const;
        virtual unsigned occurrencesOf(const Object&) const;
        virtual Object* remove(const Object&) = 0;
        virtual void removeAll() = 0;
        virtual void replaceFrom(int start, int stop, const SeqCltn& replacement, int startAt =0) = 0;
        virtual unsigned size() const = 0;
        virtual const Class* species() const;
};

#endif
