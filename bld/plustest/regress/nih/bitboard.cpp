/* BitBoard.c -- member functions of class BitBoard

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
        January, 1986

Function:
        

log:    BITBOARD.C $
Revision 1.1  90/05/20  04:19:12  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:09  kgorlen
 * Release for 1st edition.
 * 
*/

#include "BitBoard.h"
#include "nihclIO.h"

#define THIS    BitBoard
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(BitBoard,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\BITBOARD.C 1.1 90/05/20 04:19:12 Anthony_Scian Exp Locker: NT_Test_Machine $",BitBoard::init,NULL);

BitBoard squareBitBoard[64];
BitBoard rankBitBoard[8];
BitBoard fileBitBoard[8];
unsigned char bit_count[256];

void BitBoard::init(const Class&)
{
        register BitBoard* p = squareBitBoard;
        register unsigned i,j,k;
        for (i=0; i<8; i++) {
                for (j=0, k=1; j<8; j++, k+=k) {
                        (*p++).c[i] = k;
                }
        }
        p = rankBitBoard;
        register BitBoard* q = fileBitBoard;
        for (i=0, k=1; i<8; i++, k+=k) {
                (*p++).c[i] = 0xFF;
                for (j=0; j<8; j++) {
                        (*q).c[j] = k;
                }
                q++;
        }
}

unsigned BitBoard::capacity() const { return sizeof(BitBoard)*8; }

void BitBoard::deepenShallowCopy()      {}

unsigned BitBoard::hash() const { return m[0]^m[1]; }
        
bool BitBoard::isEmpty() const { return m[0]==0 && m[1]==0; }
        
bool BitBoard::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && *this==castdown(ob);
}

const Class* BitBoard::species() const { return &classDesc; }

void BitBoard::printOn(ostream& strm) const
{
        for (register int i=0; i<8; i++) {
                strm << '\n';
                for (register int j=7; j>=0; j--) {
                        if (includes(8*i+j)) strm << " *";
                        else strm << " .";
                }
        }
}               

unsigned BitBoard::size() const { return count(); }

BitBoard::BitBoard(OIOin& strm)
        : BASE(strm)
{
        strm >> m[0] >> m[1];
}

void BitBoard::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << m[0] << m[1];
}

BitBoard::BitBoard(OIOifd& fd)
        : BASE(fd)
{
        fd.get((char*)c,8);
}

void BitBoard::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd.put((char*)c,8);
}

int BitBoard::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}
