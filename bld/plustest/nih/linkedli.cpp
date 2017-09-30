/* LinkedList.c -- implementation of singly-linked list

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
        
LinkedLists are ordered by the sequence in which objects are added and removed
from them.  Object elements are accessible by index.

log:    LINKEDLI.C $
Revision 1.1  92/11/10  12:44:22  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:06  kgorlen
 * Release for 1st edition.
 * 
*/

#include "LinkedList.h"
#include "nihclIO.h"

#define THIS    LinkedList
#define BASE    SeqCltn
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(LinkedList,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\LINKEDLI.C 1.1 92/11/10 12:44:22 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_DBLLNK,NIHCL_CLTNEMPTY,NIHCL_MISSINGLNK,NIHCL_OBNOTFOUND,NIHCL_INDEXRANGE;

/*
Derived classes must override these definitions of linkCastdown() when
adding objects to a LinkedList that are multiply derived from class
Link.
*/

Link& LinkedList::linkCastdown(Object& p) const
{
        return Link::castdown(p);
}

LinkedList::LinkedList()
{
        firstLink = lastLink = Link::nilLink;
        count = 0;
}

#ifndef BUG_TOOBIG
// yacc stack overflow

LinkedList::LinkedList(const LinkedList& l)
{
        firstLink = l.firstLink;
        lastLink = l.lastLink;
        count = l.count;
}

#endif

bool LinkedList::operator==(const LinkedList& ll) const
{
        if (count != ll.count) return NO;
        else {
                register Link* p = firstLink;
                register Link* q = ll.firstLink;
                while (!p->isListEnd()) {
                        if (!(p->isEqual(*q))) return NO;
                        p = p->nextLink();
                        q = q->nextLink();
                }
        }
        return YES;
}

Object* LinkedList::add(Link& ob)
{
        register Link* lnk = &ob;
        if (lnk->nextLink() != NULL) errDblLnk("add",*lnk);
        if (count == 0) firstLink = lnk;
        else lastLink->nextLink(lnk);
        lastLink = lnk;
        lnk->nextLink(Link::nilLink);
        count++;
        return &ob;
}

Object* LinkedList::add(Object& ob)
{
        assertArgClass(ob,*Link::desc(),"add");
        return add(Link::castdown(ob));
}

Object* LinkedList::addAfter(Link& afterLink, Link& newLink)
{
        if (newLink.nextLink() != NULL) errDblLnk("addAfter",newLink);
        if (afterLink.nextLink() == NULL || count == 0)
                setError(NIHCL_MISSINGLNK,DEFAULT,afterLink.className(),this,className(),
                        afterLink.className(),&afterLink,newLink.className(),&newLink);
        newLink.nextLink(afterLink.nextLink());
        afterLink.nextLink(&newLink);
        if (lastLink == &afterLink) lastLink = &newLink;
        count++;
        return &newLink;
}

Object* LinkedList::addAfter(Object& afterLink, Object& newLink)
{
        assertArgClass(afterLink,*Link::desc(),"add");
        assertArgClass(newLink,*Link::desc(),"add");
        return addAfter(Link::castdown(afterLink), Link::castdown(newLink));
}

Collection& LinkedList::addContentsTo(Collection& cltn) const
{
        register Link* p = firstLink;
        while (!p->isListEnd()) {
                register Link* t = linkCastdown(p->copy());
                cltn.add(*t);
                p = p->nextLink();
        }
        return cltn;
}

Object* LinkedList::addFirst(Link& ob)
{
        assertArgClass(ob,*Link::desc(),"addFirst");
        if (count == 0) return add(ob);
        else {
                register Link* lnk = &ob;
                if (lnk->nextLink() != NULL) errDblLnk("addFirst",*lnk);
                lnk->nextLink(firstLink);
                firstLink = lnk;
                count++;
                return &ob;
        }
}

Object* LinkedList::addFirst(Object& ob)
{
        assertArgClass(ob,*Link::desc(),"addFirst");
        return addFirst(Link::castdown(ob));
}

Object* LinkedList::addLast(Link& ob) { return add(ob); }

Object* LinkedList::addLast(Object& ob) { return add(ob); }

Object* LinkedList::operator[](int i)
/*
Note that we can't return an Object*& because of MI: a Link* may need
to be adjusted to become an Object*.
*/
{
        if ((unsigned)i >= count) indexRangeErr();
        if (i==0) return firstLink;
        else {
                register Link* p = firstLink;
                for (register int j=i-1; j>0; j--) p = p->nextLink();
                return p->next;
        }
}

const Object *const LinkedList::operator[](int i) const
{
        if ((unsigned)i >= count) indexRangeErr();
        if (i==0) return firstLink;
        else {
                register Link* p = firstLink;
                for (register int j=i-1; j>0; j--) p = p->nextLink();
                return p->next;
        }
}

Object*& LinkedList::at(int)
// Can't implement at() because of MI -- see operator[]().
{
        shouldNotImplement("at");
        return (Object*&)nil;
}

const Object *const& LinkedList::at(int) const
// Can't implement at() because of MI -- see operator[]().
{
        shouldNotImplement("at");
        return (Object*&)nil;
}

void LinkedList::atAllPut(Object&) { shouldNotImplement("atAllPut"); }

void LinkedList::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
        register Link* p = firstLink;
        firstLink = lastLink = Link::nilLink;
        count = 0;
        while (!p->isListEnd()) {
                add(*(p->deepCopy()));
                p = p->nextLink();
        }
}

Object* LinkedList::first() const
{
        if (count==0) errEmpty("first");
        else return firstLink;
        return 0;
}

unsigned LinkedList::hash() const
{
        register unsigned h = count;
        register Link* p = firstLink;
        while (!p->isListEnd()) {
                h^= p->hash();
                p = p->nextLink();
        }
        return h;
}

bool LinkedList::includes(const Object& ob) const
{
        return (indexOf(ob) != -1);
}

int LinkedList::indexOf(const Object& ob) const
{
        register int i = 0;
        register Link* p = firstLink;
        while (!p->isListEnd()) {
                if (p->isEqual(ob)) return i;
                p = p->nextLink();
                i++;
        }
        return -1;
}

int LinkedList::indexOfSubCollection(const SeqCltn& /*cltn*/, int /*start*/) const
{       shouldNotImplement("indexOfSubCollection"); return 0;   }

bool LinkedList::isEmpty() const        { return count==0; }

bool LinkedList::isEqual(const Object& a) const
{
        return a.isSpecies(classDesc) && *this==castdown(a);
}

const Class* LinkedList::species() const { return &classDesc; }

Object* LinkedList::last() const
{
        if (count==0) errEmpty("last");
        else return lastLink;
        return 0;
}

Object* LinkedList::doNext(Iterator& pos) const
{
        if (pos.ptr == 0) {
                if (count == 0) return 0;
                else {
                        pos.ptr = firstLink;
                        pos.index = 1;
                        return firstLink;
                }
        }
else    if (pos.ptr == lastLink) return 0;
else            {
                pos.ptr = linkCastdown(pos.ptr)->nextLink();
                pos.index++;
                return pos.ptr;
        }
}

unsigned LinkedList::occurrencesOf(const Object& ob) const
{
        register unsigned n=0;
        register Link* p = firstLink;
        while (!p->isListEnd()) {
                if (p->isEqual(ob)) n++;
                p = p->nextLink();
        }
        return n;
}

Object* LinkedList::remove(const Link& ob)
{
        if (count==0) errEmpty("remove");
        register Link* lnk = &(Link&)ob;
        if (lnk == firstLink) {
                firstLink = lnk->nextLink();
                if (lnk == lastLink) lastLink = firstLink;
                goto wrapup;
        }
        else {
                register Link* p = firstLink;
                while (!p->isListEnd()) {
                        if (lnk == p->nextLink()) {
                                p->nextLink(lnk->nextLink());
                                if (lnk == lastLink) lastLink = p;
                                goto wrapup;
                        }
                        p = p->nextLink();
                }
                errNotFound("remove",ob);
        }
wrapup:
        lnk->nextLink(NULL);
        count--;
        return lnk;
}

Object* LinkedList::remove(const Object& ob)
{
        assertArgClass(ob,*Link::desc(),"remove");
        return remove(Link::castdown(ob));
}

void LinkedList::removeAll()
{
        while (count) remove(*firstLink);
}

Object* LinkedList::removeFirst()       { return remove(*firstLink); }

Object* LinkedList::removeLast()        { return remove(*lastLink); }
        
void LinkedList::replaceFrom(int /*start*/, int /*stop*/, const SeqCltn& /*replacement*/, int /*startAt*/)
{
        shouldNotImplement("replaceFrom");
}

void LinkedList::reSize(unsigned /*newSize*/) {}

unsigned LinkedList::size() const       { return count; }
        
LinkedList::LinkedList(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm)
{
        firstLink = lastLink = Link::nilLink;
        count = 0;
        unsigned n;
        strm >> n;
        while (n--) add(*Object::readFrom(strm));
}

void LinkedList::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << size();
        DO(*this,Object,ob) ob->storeOn(strm); OD
}

LinkedList::LinkedList(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd)
{
        firstLink = lastLink = Link::nilLink;
        count = 0;
        unsigned n;
        fd >> n;
        while (n--) add(*Object::readFrom(fd));
}

void LinkedList::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << size();
        DO(*this,Object,ob) ob->storeOn(fd); OD
}

void LinkedList::errDblLnk(const char* fn, const Link& lnk) const
{
        setError(NIHCL_DBLLNK,DEFAULT,lnk.className(),className(),this,className(),fn,lnk.className(),&lnk);
}

void LinkedList::errEmpty(const char* fn) const
{
        setError(NIHCL_CLTNEMPTY,DEFAULT,this,className(),fn);
}

void LinkedList::errNotFound(const char* fn, const Object& ob) const
{
        setError(NIHCL_OBNOTFOUND,DEFAULT,this,className(),fn,ob.className(),&ob);
}
