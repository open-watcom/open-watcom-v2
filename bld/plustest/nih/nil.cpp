/* Nil.c -- implementation of the nil object

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
        
Declarations and member functions for the nil object.

log:    NIL.C $
Revision 1.1  90/05/20  15:34:22  Anthony_Scian
.

 * Revision 3.0  90/05/20  11:34:10  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class Nil : public VIRTUAL Object {
        DECLARE_MEMBERS(Nil);
        Nil(const Nil&) {}
public: 
        Nil() {}
        virtual int compare(const Object&) const; // compare objects 
        virtual Object* copy() const;           // copy returns nil 
        virtual Object* deepCopy();             // copy returns nil 
        virtual void dumpOn(ostream& strm =cerr) const;
        virtual unsigned hash() const;          // calculate object hash 
        virtual bool isEqual(const Object&) const;      // equality test 
        virtual void printOn(ostream& strm =cout) const;
private:                                        // shouldNotImplement 
        virtual void deepenShallowCopy();
        virtual void storer(OIOout&) const;
        virtual void storer(OIOofd&) const;
};

extern const int NIHCL_RDUNKCLASS;

Object* const Object::nil = 0;      // initialized by NIHCL::NIHCL()

#define THIS    Nil
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

Object* Nil::reader(OIOin&)             { setError(NIHCL_RDUNKCLASS,DEFAULT,"Nil"); return nil; }
Object* Nil::reader(OIOifd&)            { setError(NIHCL_RDUNKCLASS,DEFAULT,"Nil"); return nil; }
Object* Nil::shallowCopy() const        { return nil; }

_DEFINE_CLASS_ALWAYS(Nil,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\NIL.C 1.1 90/05/20 15:34:22 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL)
_DEFINE_CASTDOWN(Nil)

bool Nil::isEqual(const Object& ob) const { return (&ob==nil); }

unsigned Nil::hash() const { return 0; }

int Nil::compare(const Object& ob) const
{
        assertArgSpecies(ob,classDesc,"compare");
        return 0;
}

Object* Nil::copy() const { return nil; }

Object* Nil::deepCopy() { return nil; }

void Nil::dumpOn(ostream& strm) const { strm << "NIL"; }

void Nil::printOn(ostream& strm) const { strm << "NIL"; }

void Nil::deepenShallowCopy()   { shouldNotImplement("deepenShallowCopy"); }

Nil::Nil(OIOin&) {}

Nil::Nil(OIOifd&) {}

void Nil::storer(OIOout&) const
/*
The Nil object is always implicitly stored as object number zero; all
references to nil are stored as @0.
*/
{
        shouldNotImplement("storer");
}

void Nil::storer(OIOofd&) const
{
        shouldNotImplement("storer");
}

//----------------------------------------------------------------------

// NIH Class Library Initialization

#include <errlib.h>

int NIHCL::initCount = 0;
bool NIHCL::init = NO;
unsigned char NIHCL::char_bit_mask[sizeof(char)*8];
unsigned short NIHCL::short_bit_mask[sizeof(short)*8];
unsigned int NIHCL::int_bit_mask[sizeof(int)*8];
unsigned char NIHCL::bit_count[256];
unsigned char NIHCL::bit_reverse[256];


void NIHCL::initTables()
{
        register unsigned i,j;
        
        for (i=0, j=1; i<sizeof(char)*8; i++, j <<= 1) char_bit_mask[i] = j;
        for (i=0, j=1; i<sizeof(short)*8; i++, j <<= 1) short_bit_mask[i] = j;
        for (i=0, j=1; i<sizeof(int)*8; i++, j <<= 1) int_bit_mask[i] = j;

        for (i=0; i<256; i++) {
                bit_count[i] = 0;
                j = i;
                while (j != 0) {
                        bit_count[i]++;
                        j &= j-1;
                }
        }
        
        for (i=0; i<256; i++) {
                bit_reverse[i] = 0;
                j = i;
                register unsigned char m = 0x80;
                while (j != 0) {
                        if ((j&1) != 0) bit_reverse[i] |= m;
                        j >>= 1;
                        m >>= 1;
                }
        }
}

extern void NIHCL__errinit();   // error facility initializer for NIHCL

NIHCL::NIHCL()
// Called once for every module that includes Object.h
{
        if (initCount++) return;
        NIHCL__errinit();                       // initialize the NIHCL error handler 
        seterropt(ERROR,WARNING,NO,3,NULL);
        initTables();                           // initialize NIHCL tables
        Object** nilp = (Object**)&Object::nil; // create the nil object and
        *nilp = new Nil;                        // initialize the pointer to it 
}

NIHCL::~NIHCL()
// Called once for every module that includes Object.h
{
        if (--initCount) return;
}

void NIHCL::initialize()        // NIHCL initialization, called once from _main 
                                // AFTER execution of all static constructors
{
        if (initialized()) return;
        Class::initialize();    // initialize NIHCL classes
        init = YES;
}
