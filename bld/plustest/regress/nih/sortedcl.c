/* SortedCltn.c -- implementation of sorted collection

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
        
A SortedCltn is a Collection of objects ordered as defined by the
virtual function "compare", which the objects must implement.  The "add"
function locates the position at which to insert the object by
performing a binary search, then invokes the private function
"OrderedCltn::addAtIndex" to insert the object after shifting up all the
objects after it in the array; therefore, a SortedCltn is not efficient
for a large number of objects.

log:    SORTEDCL.C $
Revision 1.1  90/05/20  20:45:46  Anthony_Scian
.

 * Revision 3.0  90/05/20  16:45:35  kgorlen
 * Release for 1st edition.
 * 
*/

#include "SortedCltn.h"
#include "nihclIO.h"

#define THIS    SortedCltn
#define BASE    OrderedCltn
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(SortedCltn,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\SORTEDCL.C 1.1 90/05/20 20:45:46 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

SortedCltn::SortedCltn(unsigned size) : BASE(size) {}

void SortedCltn::operator=(const SortedCltn& s)
{
        OrderedCltn::operator=(s);
}

Object* SortedCltn::add(Object& ob)
{
        if (size()==0) {                // add first object to collection 
                OrderedCltn::add(ob);
                return &ob;
        }

        int i = findIndexOf(ob);

        if (i == -1 || ob.compare(*contents[i]) != 0)
                OrderedCltn::addAtIndex(i + 1, ob);
        else
//              An object(s) equal to the argument "ob" already exists.
//              Add another one, after the sequence.
                OrderedCltn::addAtIndex(findIndexOfLastKey(ob, i) + 1, ob);

        return &ob;
}
        
int SortedCltn::findIndexOf(const Object& key) const
{
//      This algorithm is adapted from Horowitz & Sahni,
//      "Fundamentals of Data Structures", 1976, Section 7.1,
//      algorithm BINSRCH.
//      This function will return (on failure to find):
//              (-1) if the key is less than all other keys; 
//              otherwise, returns i, such that i is the greatest index
//              whose key is less than the key value.
//      On successful search the algorithm returns an index to a
//      key which equals the key argument.  However, this is not
//      guaranteed to be the first key of a sequence, when such a
//      sequence exists.

        int l = 0;
        int u = size() - 1;
        int m = 0;
        int c;

        if (key.compare(*contents[0]) < 0)
                return -1;

        while (l <= u) {
                m = (l + u) >> 1;
                if ((c = key.compare(*contents[m])) > 0)
                        l = m + 1;
                else
                if (c == 0)
                        return m;
                else
                        u = m - 1;
        }

//      Binary search will leave the final index searched either
//      just greater than the key or just less than, depending 
//      upon the relation of the search key to the existing
//      collection.  Must adjust here, in case it was placed just
//      to the right.

        if (m > 0 && c < 0)
                m--;

        return m; // Key not found.
}

Object* SortedCltn::addAfter(const Object&, Object& /*newob*/)
{
        shouldNotImplement("addAfter"); return 0;
}

Object* SortedCltn::addAllLast(const OrderedCltn&)
{
        shouldNotImplement("addAllLast"); return 0;
}

Object* SortedCltn::addBefore(const Object&, Object& /*newob*/)
{
        shouldNotImplement("addBefore"); return 0;
}

Object* SortedCltn::addLast(Object&)
{
        shouldNotImplement("addLast"); return 0;
}

void SortedCltn::atAllPut(Object&)
{
        shouldNotImplement("atAllPut");
}

int SortedCltn::indexOfSubCollection(const SeqCltn& /*cltn*/, int /*start*/) const
{
        shouldNotImplement("indexOfSubCollection"); return 0;
}

void SortedCltn::replaceFrom(int /*start*/, int /*stop*/, const SeqCltn& /*replacement*/, int /*startAt*/)
{
        shouldNotImplement("replaceFrom");
}

void SortedCltn::sort()
{
        shouldNotImplement("sort");
}
 
SortedCltn Collection::asSortedCltn() const
{
        SortedCltn cltn(MAX(size(),DEFAULT_CAPACITY));
        addContentsTo(cltn);
        return cltn;
}

SortedCltn::SortedCltn(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm)
{
}

SortedCltn::SortedCltn(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd)
{
}

Range SortedCltn::findRangeOf(const Object& key) const
{
        int i = SortedCltn::findIndexOf(key);

        if (i == -1 || key.compare(*contents[i]) != 0)
//              Give the caller the place where "key" should
//              be placed, if not found.
                return Range(i, 0);
        int k = findIndexOfFirstKey(key, i);
        int l = findIndexOfLastKey(key, i);
        return Range(k, l - k + 1);
}

int SortedCltn::findIndexOfFirstKey(const Object& key, const int index) const
{
        register int j;
        for (j = index - 1; j > -1; j--)
                if (key.compare(*contents[j]) != 0)
                        break;
        return (j + 1);

}

int SortedCltn::findIndexOfLastKey(const Object& key, const int index) const
{
        register int max = size();
        register int j;
        for (j = index + 1; j < max; j++)
                if (key.compare(*contents[j]) != 0)
                        break;
        return (j - 1);

}

unsigned SortedCltn::occurrencesOf(const Object& key) const
{
        Range r = findRangeOf(key);
        return unsigned(r.length());
}

Object* SortedCltn::remove(const Object& ob)
{
        Range r = findRangeOf(ob);
        int count = r.length();
        if (count == 0) return nil;
        return removeAtIndex(r.firstIndex());
}
