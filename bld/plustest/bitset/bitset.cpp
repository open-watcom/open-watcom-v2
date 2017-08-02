// bitset.cpp

#include <iostream.h>
#include "bitset.h"

Bitset::Bitset(unsigned nb)
{
    nbits = nb; 
    bits = new unsigned[numwords(nb)];
    reset();
}

ostream& operator<<(ostream& os, const Bitset& bs)
{
    for (int i = 0; i < bs.nbits; ++i)
        os << bs.test(i);
    return os;
}

Bitset& Bitset::set()
{
    unsigned nw = numwords(nbits);
    while (nw--)
        bits[nw] = ~0u;
    cleanup();
    return *this;
}

Bitset& Bitset::set(unsigned b)
{
    if (b < nbits)
        bits[word(b)] |= mask(b);
    return *this;
}

Bitset& Bitset::set(unsigned start, unsigned stop)
{
    for (int i = start; i < nbits && i <= stop; ++i)
        set(i);
    return *this;
}

Bitset& Bitset::reset()
{
    unsigned nw = numwords(nbits);
    while (nw--)
        bits[nw] = 0u;
    return *this;
}

Bitset& Bitset::reset(unsigned b)
{
    if (b < nbits)
        _reset(b);
    return *this;
}

Bitset& Bitset::reset(unsigned start, unsigned stop)
{
    for (int i = start; i < nbits && i <= stop; ++i)
        _reset(i);
    return *this;
}

Bitset& Bitset::toggle()
{
    unsigned nw = numwords(nbits);
    while (nw--)
        bits[nw] = ~bits[nw];
    cleanup();
    return *this;
}

Bitset& Bitset::toggle(unsigned b)
{
    if (b < nbits)
        bits[word(b)] ^= mask(b);
    return *this;
}

Bitset& Bitset::toggle(unsigned start, unsigned stop)
{
    for (int i = start; i < nbits && i <= stop; ++i)
        toggle(i);
    return *this;
}

int Bitset::test(unsigned b) const
{
    if (b < nbits)
        return (bits[word(b)] & mask(b)) != 0;
    else
        return 0;
}

Bitset& Bitset::set_intersect(const Bitset& b)
{
    unsigned nw = numwords(nbits);
    unsigned nw2 = numwords(b.nbits);
    for (int i = 0; i < nw && i < nw2; ++i)
        bits[i] &= b.bits[i];
    return *this;
}

Bitset& Bitset::set_union(const Bitset& b)
{
    unsigned nw = numwords(nbits);
    unsigned nw2 = numwords(b.nbits);
    for (int i = 0; i < nw && i < nw2; ++i)
        bits[i] |= b.bits[i];
    cleanup();
    return *this;
}

Bitset& Bitset::set_sym_diff(const Bitset& b)
{
    unsigned nw = numwords(nbits);
    unsigned nw2 = numwords(b.nbits);
    for (int i = 0; i < nw && i < nw2; ++i)
        bits[i] ^= b.bits[i];
    cleanup();
    return *this;
}

Bitset& Bitset::set_diff(const Bitset& b)
{
    unsigned nw = numwords(nbits);
    unsigned nw2 = numwords(b.nbits);
    for (int i = 0; i < nw && i < nw2; ++i)
        bits[i] &= ~b.bits[i];
    return *this;
}

Bitset& Bitset::operator=(const Bitset& b)
{
    if (this != &b)
    {
        unsigned nw = numwords(nbits);
        unsigned nw2 = numwords(b.nbits);
        for (int i = 0; i < nw && i < nw2; ++i)
            bits[i] = b.bits[i];
        cleanup();
    }
    return *this;
}

Bitset::Bitset(const Bitset& b)
{
    nbits = b.nbits;
    bits = new unsigned[numwords(nbits)];
    for (int i = 0; i < numwords(nbits); ++i)
           bits[i] = b.bits[i];
}

int Bitset::equal(const Bitset& b) const
{
    if (is_null() && b.is_null())
        return 1;

    // Else, must be the same size to be equal
    if (nbits == b.nbits)
    {
        for (int i = 0; i < numwords(nbits); ++i)
            if (bits[i] != b.bits[i])
                return 0;
        return 1;
    }
    return 0;
}

Bitset operator&(const Bitset& x, const Bitset& y)
{
    if (x.nbits < y.nbits)
    {
        Bitset b(x);
        return b &= y;
    }
    else
    {
        Bitset b(y);
        return b &= x;
    }
}

Bitset operator|(const Bitset& x, const Bitset& y)
{
    if (x.nbits > y.nbits)
    {
        Bitset b(x);
        return b |= y;
    }
    else
    {
        Bitset b(y);
        return b |= x;
    }
}

Bitset operator^(const Bitset& x, const Bitset& y)
{
    if (x.nbits > y.nbits)
    {
        Bitset b(x);
        return b ^= y;
    }
    else
    {
        Bitset b(y);
        return b ^= x;
    }
}

Bitset operator-(const Bitset& x, const Bitset& y)
{
    Bitset b(x);
    return b -= y;
}

Bitset Bitset::operator~() const
{
    Bitset b(*this);
    b.toggle();
    b.cleanup();
    return b;
}

int Bitset::is_null() const
{
    for (int i = 0; i < numwords(nbits); ++i)
        if (bits[i])
            return 0;
    return 1;
}

int Bitset::subsetof(const Bitset& b) const
{
    if (is_null())
        return 1;
    else
    {
        Bitset r(*this);
        r &= b;
        return equal(r);
    }
}

int Bitset::count() const
{
    int sum = 0;
    for (int i = 0; i < nbits; ++i)
        if (test(i))
            ++sum;
    return sum;
}

void Bitset::cleanup()
{
    // Make sure unused bits don't get set
    for (int i = nbits; i < numwords(nbits)*WRDSIZ; ++i)
        _reset(i);
}

Bitset::operator const void*() const
{
    return is_null() ? 0 : this;
}

// GRB Bitref::operator unsigned()
Bitref::operator int() // GRB
{
    return bs.test(bit);
}

Bitref& Bitref::operator=(unsigned value)
{
    if (value)
        bs.set(bit);
    else
        bs.reset(bit);
    return *this;
}

ostream& operator<<(ostream& os, const Bitref& br)
{
    return os << br.bs.test(br.bit);
}

// tbitset.cpp:              Test the Bitset Class
