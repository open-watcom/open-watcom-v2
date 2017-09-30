



/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */





#include "base/bitset.h"
#include "base/bytestrm.h"
#include "base/intset.h"

static const short BitsInLong = sizeof (ulong)*8;

// The following inline functions are used for indexing into the array of
// ulong's, and assume that a ulong has 32 bits.
inline long __Div32 (long x)
{
    return (x >> 5) & ((1L << 27) - 1);
}

inline long __Mod32 (long x)
{
    return x & ((1L << 5) - 1);
}

inline ulong __Mul32 (long x)
{
    return x << 5;
}


static ulong __FAR BitTable [32] = {
    0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x00000100, 0x00000200, 0x00000400, 0x00000800,
    0x00001000, 0x00002000, 0x00004000, 0x00008000,
    0x00010000, 0x00020000, 0x00040000, 0x00080000,
    0x00100000, 0x00200000, 0x00400000, 0x00800000,
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000
};

static short BitCount (ulong n)
{
    short count = 0;
    while (n != 0) {
        n &= n-1;
        count++;
    }
    return count;
}



inline bool DoAdd (ulong _rep[], long value)
{
    ulong mask = (1L << (__Mod32 (value)));
    long index = __Div32 (value);
    if (_rep [index] & mask)
        return FALSE;
    _rep[index] |= mask;
    return TRUE;
}


CL_DEFINE_CLASS(CL_BitSet, _CL_Sequence_CLASSID);

CL_BitSet::CL_BitSet  (long max_size)
{
    long div = __Div32 (max_size);
    long mod = __Mod32 (max_size);
    _count = (mod == 0) ? div : div+1;
    _rep = new ulong [_count];
    if (!_rep) {
        _count = 0;
        return;
    }
    register ulong* p = _rep;
    for (register long i = 0; i < _count; i++)
        *p++ = 0L;
    _size = 0;
}

CL_BitSet::CL_BitSet  (const CL_BitSet& o)
{
    _count = o._count;
    _rep = new ulong [_count];
    if (!_rep) {
        _count = 0;
        return;
    }
    register ulong *p, *q;
    p = _rep; q = o._rep;
    for (long i = 0; i < _count; i++)
        *p++ = *q++;
    _size = o._size;
}


CL_BitSet::CL_BitSet (long lo, long hi, long max_size)
{
    long div = __Div32 (max_size);
    long mod = __Mod32 (max_size);
    _count = (mod == 0) ? div : div+1;
    _rep = new ulong [_count];
    if (!_rep) {
        _count = 0;
        return;
    }
    register ulong* p = _rep;
    long i;
    for (i = 0; i < _count; i++)
        *p++ = 0L;
    _size = 0;
    for (i = lo; i <= hi; i++)
        Add (i);
}

CL_BitSet::CL_BitSet  (CL_ByteArray& o)
{
    CL_ByteStream s (o);
    ReadFrom (s);
}


// Protected constructor
CL_BitSet::CL_BitSet (long wordCount, ulong* array)
{
    _rep = array;
    _size = 0;
    _count = wordCount;
    for (long i = 0; i < _count; i++) {
        _size += BitCount (_rep[i]);
    }
}


CL_BitSet::~CL_BitSet ()
{
    if (_rep)
        delete [] _rep;
}



// ----------------------- Element methods ------------------

bool CL_BitSet::Includes     (const long& value) const
{
    if (value < 0 || value >= __Mul32 (_count))
        return FALSE;
    return (_rep [__Div32 (value)] &
            (1L << (__Mod32 (value)))) ? TRUE : FALSE;
}

long CL_BitSet::Remove    (const long& value)
{
    if (!PrepareToChange())
        return FALSE;
    if (value < 0 || value >= __Mul32 (_count))
        return 0;
    long index = __Div32(value);
    ulong mask = 1L << __Mod32 (value);
    if (!(_rep [index] & mask))
        return 0;
    _rep[index] &= ~mask;
    _size --;
    Notify();
    return value;
}



bool CL_BitSet::Add  (const long& value)
{
    if (!PrepareToChange())
        return FALSE;
    if (value < 0 || value >= __Mul32 (_count))
        return FALSE;
    if (DoAdd (_rep, value)) {
        _size++;
        Notify();
        return TRUE;
    }
    return FALSE;
}


long CL_BitSet::Largest () const
{
    // This implementation is likely to be more efficient than simply using
    // ItemWithRank.
    if (_size <= 0)
        return -1;
    register short i = _count-1;
    register ulong* p = _rep;
    while (i >= 0 && *p == 0)
        i--, p--;
    if (i < 0) {
        // Can't be: we checked for size > 0 already
        CL_Error::Warning ("CL_BitSet::Largest: internal error");
        return -1;
    }
    register ulong data = *p;
    short j;
    for (j = 31; j >= 0; j--) {
        if (data & BitTable[j])
            break;
    }
    return __Mul32(i) + j;
}


long CL_BitSet::Smallest () const
{
    // This implementation is likely to be more efficient than simply using
    // ItemWithRank.
    if (_size <= 0)
        return -1;
    register short i = 0;
    register ulong* p = _rep;
    while (i < _count && *p == 0)
        i++, p++;
    if (i >= _count) {
        // Can't be: we checked for size > 0 already
        CL_Error::Warning ("CL_BitSet::Smallest: internal error");
        return 0;
    }
    register ulong data = *p;
    short j;
    for (j = 0; j < 32; j++) {
        if (data & BitTable[j])
            break;
    }
    return __Mul32 (i) + j;
}




long  CL_BitSet::Successor (long n) const
{
    if (_size <= 0)
        return -1;
    long index = __Div32 (n);
    short offs = __Mod32 (n);
    if (index >= _count)
        return -1;
    else if (index < 0)
        index = 0;
    register ulong word = _rep[index];
    register short j;
    // See if next larger element is in the same long cell as n
    for (j = offs+1; j < BitsInLong; j++)
        if (BitTable [j] & word)
            return __Mul32 (index) + j;

    // No, it isn't. Scan the rest of the cells, from index+1 up
    register long i;
    for (i = index+1; i < _count; i++) {
        word = _rep[i];
        if (word)
            break;
    }
    if (i >= _count)
        return -1;
    for (j = 0; j < BitsInLong; j++)
        if (word & BitTable[j])
            return __Mul32 (i) + j;
    // Huh? We found a non-zero cell, but no 1-bit in it??
    CL_Error::Warning ("BitSet::Successor: internal error");
    return -1;
}


long  CL_BitSet::SmallestNonMember () const
{
    if (_size <= 0)
        return 0;
    if (_size == __Mul32 (_count))
        return -1;
    register long i = 0;
    register ulong* p = _rep;
    while (i < _count && *p == (ulong) -1)
        i++, p++;
    if (i >= _count)
        return __Mul32 (_count);
    register ulong data = ~ (*p);
    short j;
    for (j = 0; j < 32; j++) {
        if (data & BitTable[j])
            break;
    }
    return __Mul32(i) + j;
}


long CL_BitSet::RankOf (const long& value) const
{
    if (_size <= 0 || value <= 0)
        return 0;
    long index = minl (__Div32(value), _count-1);
    register long mod32 = __Mod32 (value);
    ulong mask = (-1L) << mod32; // Shift in zeros at right
    long rank = 0;
    for (register long i = 0; i < index; i++)
        rank += BitCount (_rep[i]);
    register ulong tmp = _rep[index];
    rank += BitCount (tmp & ~mask);
    return rank;
}



long CL_BitSet::ItemWithRank (long rank) const
{
    if (_size <= 0)
        return -1;
    rank = maxl (0, minl (rank, _size - 1));
    long r = 0, l = 0;
    long i;
    for (i = 0; i < _count; i++) {
        l = BitCount (_rep[i]);
        if (r + l > rank)
            break;
        r += l;
    }
    if (i >= _count) // We've looked at all the words in _rep
        i = _count-1;
    long m = _rep[i];
    short j;
    for (j = 0; j < 32; j++) {
        if (m & 1) {
            if (r == rank)
                break;
            r++;
        }
        m >>= 1;
    }
    return __Mul32(i) + j;
}



CL_BitSet CL_BitSet::operator + (long value) const
{
    CL_BitSet s (*this);
    s.Add (value);
    return s;
}


// void CL_BitSet::operator= (const CL_Set<long>& s)
// {
//     if (!PrepareToChange ())
//         return;
//     if (this == &s)
//         return;
//     MakeEmpty();
//     long n = s.Size();
//     for (long i = 0; i < n; i++)
//         Add (s.ItemWithRank(i));
//     Notify ();
// }

// void CL_BitSet::operator= (const CL_Sequence<long>& s)
// {
//     if (!PrepareToChange ())
//         return;
//     MakeEmpty();
//     register long n = s.Size();
//     for (long i = 0; i < n; i++)
//         Add (s[i]);
//     Notify ();
// }

// ----------------------- Set methods ----------------------

void CL_BitSet::MakeEmpty ()
{
    if (!PrepareToChange())
        return;
    ulong* p = _rep;
    for (long i = 0; i < _count; i++)
        *p++ = (ulong) 0;
    _size = 0;
    Notify();
}



// Make this the universal set:
void CL_BitSet::MakeUniversal ()
{
    if (!PrepareToChange())
        return;
    register ulong* p = _rep;
    for (register long i = 0; i < _count; i++)
        *p++ = (ulong) (-1);
    _size = __Mul32 (_count);
    Notify();
}


bool CL_BitSet::IsUniversal () const
{
    return Size() == __Mul32 (_count);
}

CL_IntegerSet CL_BitSet::AsSet () const
{
    CL_IntegerSet aSet;
    
    long offset = 0;
    for (long i = 0; i < _count; i++, offset += BitsInLong) {
        register ulong mask = 1;
        register ulong data = _rep[i];
        for (short j = 0; j < BitsInLong; j++) {
            if (data & mask)
                aSet.Add (offset + j);
            mask <<= 1;
        }
    }
    return aSet;
}


            
CL_BitSet& CL_BitSet::operator = (const CL_BitSet& o)
{
    if (!PrepareToChange())
        return *this;
    if (_count != o._count) {
        delete [] _rep;
        _rep = new ulong[_count = o._count];
        if (!_rep) {
            _count = 0;
            return *this;
        }
    }
    for (long i = 0; i < _count; i++)
        _rep[i] = o._rep[i];
    _size = o._size;
    Notify();
    return *this;
}





static void DoOr (ulong& w1, ulong w2)
{
    w1 |= w2;
}


static void DoAnd (ulong& w1, ulong w2)
{
    w1 &= w2;
}


static void DoDiff (ulong& w1, ulong w2)
{
    w1 &= ~w2;
}



CL_BitSet CL_BitSet::_DoOp (const CL_BitSet& s, void (*opr)(ulong&, ulong))
    const
{
    long cnt = maxl (s._count, _count);
    ulong* new_rep = new ulong[cnt];
    if (! new_rep)
        return *this; // No memory
    long i;
    for (i = 0; i < cnt; i++)
        new_rep[i] = (i < _count) ? _rep[i] : 0;
    for (i = 0; i < s._count; i++) {
        (*opr) (new_rep[i], s._rep[i]);
    }
    CL_BitSet o (cnt, new_rep);
    return o;
}


// Union:
CL_BitSet CL_BitSet::operator + (const CL_BitSet& s) const
{
    return _DoOp (s, DoOr);
}



// Difference:
CL_BitSet CL_BitSet::operator - (const CL_BitSet& s) const
{
    return _DoOp (s, DoDiff);
}



// Intersection:
CL_BitSet CL_BitSet::operator * (const CL_BitSet& s) const
{
    return _DoOp (s, DoAnd);
}

// Complement:
CL_BitSet CL_BitSet::operator ~ () const
{
    CL_BitSet s (*this);
    for (long i = 0; i < _count; i++)
        s._rep[i] = ~s._rep[i];
    s._size = s._count * BitsInLong - _size;
    return s;
}





// CL_Set<long> CL_BitSet::operator + (const CL_Set<long>& s) const
// {
//     CL_Set<long> t = s;
//     for (long i = 0; i < _count; i++) {
//         ulong b = _rep[i];
//         for (short j = 0; j < BitsInLong; j++) {
//             if (b & 1)
//                 t.Add (i*BitsInLong + j);
//             b >>= 1;
//         }
//     }
//     return t;
// }
// 
// 
// 
// // Difference:
// CL_Set<long> CL_BitSet::operator - (const CL_Set<long>& s) const
// {
//     CL_Set<long> t;
//     for (long i = 0; i < _count; i++) {
//         ulong b = _rep[i];
//         for (short j = 0; j < BitsInLong; j++) {
//             if (b & 1)
//                 t.Add (i*BitsInLong + j);
//             b >>= 1;
//         }
//     }
//     return t - s;
// }
// 
// 
// 
// // Intersection:
// CL_Set<long> CL_BitSet::operator * (const CL_Set<long>& s) const
// {
//     CL_Set<long> t;
//     for (long i = 0; i < _count; i++) {
//         ulong b = _rep[i];
//         for (short j = 0; j < BitsInLong; j++) {
//             if (b & 1)
//                 t.Add (i*BitsInLong + j);
//             b >>= 1;
//         }
//     }
//     return t * s;
// }
// 
// 
// 
// 
// 
bool CL_BitSet::operator == (const CL_IntegerSet& o) const
{
    if (Size() != o.Size())
        return FALSE;
    for (long i = 0; i < _count; i++) {
        ulong b = _rep[i];
        for (short j = 0; j < BitsInLong; j++) {
            if (b & 1)
                if (!o.Includes (i*BitsInLong + j))
                    return FALSE;
            b >>= 1;
        }
    }
    return TRUE;
}


bool CL_BitSet::operator == (const CL_BitSet& o) const
{
    if (_size != o._size)
        return FALSE;
    for (long i = 0; i < minl (o._count, _count); i++) {
        if (_rep[i] != o._rep[i])
            return FALSE;
    }
    return TRUE;
}



bool CL_BitSet::IncludesAll (const CL_IntegerSet& o) const
{
    long n = o.Size();
    for (long i = 0; i < n; i++)
        if (!Includes (o.ItemWithRank(i)))
            return FALSE;
    return TRUE;
}


bool CL_BitSet::IncludesAll (const CL_BitSet& o) const
{
    if (_size < o._size)
        return FALSE;
    register long n;
    if (o._count > _count) {
        for (register long i = _count; i < o._count; i++)
            if (o._rep[i] != 0)
                return FALSE;
        n = _count;
    }
    else
        n = o._count;  // n is the smaller of the two counts
    for (register long i = 0; i < n; i++) {
        register ulong p = _rep[i];
        register ulong q = o._rep[i];
        if (p | q != p)
            return FALSE;
    }
    return TRUE;
}



// ------------------------ Save/restore -------------------------

long CL_BitSet::StorableFormWidth() const
{
    return sizeof (CL_ClassId) + sizeof (long) + sizeof (ulong) +
        _count*sizeof (ulong);
}



bool CL_BitSet::ReadFrom (const CL_Stream& s)
{
    if (!PrepareToChange())
        return 0;
    if (!ReadClassId (s))
        return FALSE;
    long  new_count;
    if (!s.Read (new_count))
        return FALSE;
    if (!s.Read (_size))
        return FALSE;
    if (new_count != _count) {
        ulong* new_rep   = new ulong [new_count];
        if (!new_rep) 
            return FALSE; // No memory
        if (_rep)
            delete [] _rep;
        _rep = new_rep;
        _count = new_count;
    }
    if (!s.Read ((uchar*) _rep, _count*sizeof(ulong)))
        return FALSE;
//     _size = 0;
//     for (long i = 0; i < _count; i++) {
//         _size += BitCount (_rep[i]);
//     }
    Notify();
    return TRUE;
}

bool CL_BitSet::WriteTo  (CL_Stream& s) const
{
    return s.Write (ClassId())  &&
        s.Write (_count) && s.Write (_size) &&
        s.Write ((uchar*) _rep, _count*sizeof(ulong));
}




// ---------------------- BitSetIterator methods ----------------------

CL_BitSetIterator::CL_BitSetIterator (const CL_BitSet& o)
: _set (o)
{
    _count = _index = 0;
    _tblIndex = 0;
}





void CL_BitSetIterator::Reset ()
{
    _index = _count = 0;
    _tblIndex = 0;
}



void CL_BitSetIterator::BeginFromRank (long l)
{
    register ulong* p = _set._rep;
    _count = maxl (0, l);
    if (!p)
        return;
    register long i;
    for (i = 0; i < _set._count; i++, p++) {
        short q = BitCount (*p);
        if (l < q) break;
        l -= q;
    }
    _index = i;
    if (i < _set._count) {
        register ulong r = _set._rep[i];
        register short j;
        for (j = 0; j < BitsInLong; j++)
            if (BitTable[j] & r) {
                l--;
                if (l < 0)
                    break;
            }
        _tblIndex = j;
            
    }
}



bool CL_BitSetIterator::More () const
{
    return _count < _set.Size();
}


long CL_BitSetIterator::Next ()
{
    if (_count >= _set.Size())
        return -1;
    ulong* p = _set._rep;
    while (_index < _set._count) {
        ulong mask =  BitTable[_tblIndex];
    if (mask & p[_index]) break;
        _tblIndex++;
        if (_tblIndex >= BitsInLong) {
            _tblIndex = 0;
            _index++;
        }
    }
    if (_index < _set._count) {
        long ret = __Mul32 (_index) + _tblIndex;
        _count++;
        _tblIndex++;
        if (_tblIndex >= BitsInLong) {
            _tblIndex = 0;
            _index++;
        }
        return ret;
    }

    return -1;
}






