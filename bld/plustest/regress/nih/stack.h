#ifndef STACK_H
#define STACK_H


/* Stack.h -- declarations for class Stack

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

log:    stack.h $
Revision 1.2  95/01/29  13:27:38  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:21:30  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:28  kgorlen
 * Release for 1st edition.
 * 
*/

#include "OrderedCltn.h"

class Stack: public SeqCltn {
        DECLARE_MEMBERS(Stack);
        OrderedCltn contents;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Stack(unsigned size =DEFAULT_CAPACITY);
        Stack(const Stack&);
        bool operator==(const Stack& s) const
                { return contents == s.contents; }
        bool operator!=(const Stack& s) const { return !(*this==s); }
        void operator=(const Stack& s)  { contents = s.contents; }
        Object*& operator[](int i)      { return contents.at(size()-i-1); }
        const Object *const& operator[](int i) const    { return contents.at(size()-i-1); }
        void push(Object& ob)           { contents.addLast(ob); }
        Object* pop()                   { return contents.removeLast(); }
        Object* top() const             { return contents.last(); }
        virtual Object* add(Object& ob);
        virtual Object*& at(int i);
        virtual const Object *const& at(int i) const;
        virtual unsigned capacity() const;
        virtual void deepenShallowCopy();
        virtual unsigned hash() const;
        virtual bool isEmpty() const;
        virtual Object* last() const;
        virtual void reSize(unsigned newSize);
        virtual void removeAll();
        virtual Object* removeLast();
        virtual unsigned size() const;
private:                            // shouldNotImplement();
        virtual void atAllPut(Object& ob);
        virtual int indexOfSubCollection(const SeqCltn& cltn, int start=0) const;
        virtual Object* remove(const Object&);
        virtual void replaceFrom(int start, int stop, const SeqCltn& replacement, int startAt =0);
};

#endif
