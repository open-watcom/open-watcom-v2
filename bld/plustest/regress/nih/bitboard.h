#ifndef BITBOARD_H
#define BITBOARD_H


/* BitBoard.h -- Declarations for class BitBoard

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

log:    BITBOARD.H $
Revision 1.2  95/01/29  13:27:00  NT_Test_Machine
*** empty log message ***

Revision 1.1  93/04/08  09:50:32  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:11  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class BitBoard: public VIRTUAL Object {
        DECLARE_MEMBERS(BitBoard);
        union {
                unsigned long m[2];
                unsigned char c[8];
        };
public:
        static void init(const Class&);
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        BitBoard() {}
        BitBoard(unsigned i);
        BitBoard(unsigned long i, unsigned long j) { m[0] = i;  m[1] = j; }
        BitBoard operator~() const              { return BitBoard(~m[0], ~m[1]); }
        BitBoard operator-(const BitBoard& n) const     { return BitBoard(m[0]&~n.m[0], m[1]&~n.m[1]); }
        bool operator>(const BitBoard& n) const { return m[0] == (m[0]|n.m[0])
                                                && m[0] != n.m[0]
                                                && m[1] == (m[1]|n.m[1])
                                                && m[1] != n.m[1]; }
        bool operator<(const BitBoard& n) const { return n > *this; }
        bool operator>=(const BitBoard& n) const { return m[0] == (m[0]|n.m[0])
                                                && m[1] == (m[1]|n.m[1]); }
        bool operator<=(const BitBoard& n) const { return n >= *this; }
        bool operator==(const BitBoard& n) const { return m[0] == n.m[0] && m[1] == n.m[1]; }
        bool operator!=(const BitBoard& n) const { return m[0] != n.m[0] || m[1] != n.m[1]; }
        BitBoard operator|(const BitBoard& n) const { return BitBoard(m[0]|n.m[0], m[1]|n.m[1]); }
        BitBoard operator&(const BitBoard& n) const { return BitBoard(m[0]&n.m[0], m[1]&n.m[1]); }
        BitBoard operator^(const BitBoard& n) const { return BitBoard(m[0]^n.m[0], m[1]^n.m[1]); }
        void operator-=(const BitBoard& n)      { m[0] &= ~n.m[0]; m[1] &= ~n.m[1]; }
        void operator&=(const BitBoard& n)      { m[0] &= n.m[0]; m[1] &= n.m[1]; }
        void operator^=(const BitBoard& n)      { m[0] ^= n.m[0]; m[1] ^= n.m[1]; }
        void operator|=(const BitBoard& n)      { m[0] |= n.m[0]; m[1] |= n.m[1]; }
        unsigned count() const;
        bool includes(unsigned i) const;
        virtual unsigned capacity() const;
        virtual void deepenShallowCopy();       // {}
        virtual unsigned hash() const;
        virtual bool isEmpty() const;
        virtual bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual unsigned size() const;
        virtual const Class* species() const;
private:                            // shouldNotImplement()
        virtual int compare(const Object&) const;
};

extern BitBoard squareBitBoard[64];
extern BitBoard rankBitBoard[8];
extern BitBoard fileBitBoard[8];
extern unsigned char bit_count[256];

inline BitBoard::BitBoard(unsigned i)   { *this = squareBitBoard[i]; }
        
inline unsigned BitBoard::count() const
{
        register unsigned const char* p = c;
        register unsigned n = 0;
        n += bitCount(*p++);
        n += bitCount(*p++);
        n += bitCount(*p++);
        n += bitCount(*p++);
        n += bitCount(*p++);
        n += bitCount(*p++);
        n += bitCount(*p++);
        n += bitCount(*p++);
        return n;
}

inline bool BitBoard::includes(unsigned i) const
{
        return (m[0] & squareBitBoard[i].m[0]) || (m[1] & squareBitBoard[i].m[1]);
}

#endif
