// bits.h:    Fixed-length sets of bit flags

#ifdef _USE_INT64
#ifdef __WATCOM_INT64__
typedef unsigned __int64 _Block;
#else
typedef unsigned long _Block;
#endif
#else
typedef unsigned short _Block;
#endif
const unsigned _BITS_PER_BYTE = 8;
const unsigned _WRDSIZ = _BITS_PER_BYTE * sizeof (_Block);

template<int nbits>
class bits
{
public:
    // Constructors
    bits()
      {reset();}
    bits(_Block n)
      {reset(); _bits[NWORDS-1] = n;}
    bits(const bits&);

    // Conversions
    _Block word(unsigned n = 0) const
      {return n < NWORDS ? _bits[n] : 0;}
    
    // Assignment
    bits& operator=(const bits&);

    int operator==(const bits&) const;
    int operator!=(const bits& b) const
      {return !operator==(b);};

    bits& set(unsigned);
    bits& set();
    bits& reset(unsigned);
    bits& reset();
    bits& toggle(unsigned);
    bits& toggle();
    bits operator~() const;
    int test(unsigned) const;
    int any() const;
    int none() const
        {return !any();}

    const bits& operator&=(const bits&);
    bits operator&(const bits&) const;

    const bits& operator|=(const bits&);
    bits operator|(const bits&) const;

    const bits& operator^=(const bits&);
    bits operator^(const bits&) const;

    int count() const;
    int size() const
      {return nbits;}
    
    // Shift
    const bits& operator<<=(unsigned);
    bits operator<<(unsigned) const;
    const bits& operator>>=(unsigned);
    bits operator>>(unsigned) const;

    friend ostream& operator<<(class ostream& os, const bits& b)
      {return b._print(os);}

private:
    enum {NWORDS = (nbits+_WRDSIZ-1) / _WRDSIZ};
    _Block _bits[(nbits+_WRDSIZ-1) / _WRDSIZ];

    static _Block _word(unsigned b) {return NWORDS - b/_WRDSIZ - 1;}
    static _Block _offset(unsigned b) {return b%_WRDSIZ;}
    static _Block _mask(unsigned b) {return _Block(1) << _offset(b);}
    void _cleanup();
    ostream& _print(ostream&) const;
    void _set(unsigned b) {_bits[_word(b)] |= _mask(b);}  // Caveat usor
    void _reset(unsigned b) {_bits[_word(b)] &= ~_mask(b);}  // Caveat usor
    int _test(unsigned b) const {return (_bits[_word(b)] & _mask(b)) != 0;}
};
typedef bits<128> dbits; //grb

template<int nbits>
bits<nbits>::bits(const bits<nbits>& b)
{
    for (int i = 0; i < NWORDS; ++i)
	    _bits[i] = b._bits[i];
}

template<int nbits>
bits<nbits>& bits<nbits>::set()
{
    unsigned nw = NWORDS;
    while (nw--)
        _bits[nw] = (_Block)~0u; //grb
    _cleanup();
    return *this;
}

template<int nbits>
bits<nbits>& bits<nbits>::set(unsigned b)
{
    if (b < nbits)
        _set(b);
    return *this;
}

template<int nbits>
bits<nbits>& bits<nbits>::reset()
{
    unsigned nw = NWORDS;
    while (nw--)
        _bits[nw] = 0u;
    return *this;
}

template<int nbits>
bits<nbits>& bits<nbits>::reset(unsigned b)
{
    if (b < nbits)
        _reset(b);
    return *this;
}

template<int nbits>
bits<nbits>& bits<nbits>::toggle()
{
    unsigned nw = NWORDS;
    while (nw--)
        _bits[nw] = ~_bits[nw];
    _cleanup();
    return *this;
}

template<int nbits>
bits<nbits>& bits<nbits>::toggle(unsigned b)
{
    if (b < nbits)
        _bits[_word(b)] ^= _mask(b);
    return *this;
}

template<int nbits>
int bits<nbits>::test(unsigned b) const
{
    if (b < nbits)
        return _test(b);
    else
        return 0;
}

template<int nbits>
int bits<nbits>::any() const
{
    for (int i = 0; i < NWORDS; ++i)
        if (_bits[i])
            return 1;
    return 0;
}

template<int nbits>
const bits<nbits>& bits<nbits>::operator&=(const bits<nbits>& b)
{
    for (int i = 0; i < NWORDS; ++i)
        _bits[i] &= b._bits[i];
    return *this;
}

template<int nbits>
const bits<nbits>& bits<nbits>::operator|=(const bits<nbits>& b)
{
    for (int i = 0; i < NWORDS; ++i)
        _bits[i] |= b._bits[i];
    _cleanup();
    return *this;
}

template<int nbits>
const bits<nbits>& bits<nbits>::operator^=(const bits<nbits>& b)
{
    for (int i = 0; i < NWORDS; ++i)
        _bits[i] ^= b._bits[i];
    _cleanup();
    return *this;
}

template<int nbits>
bits<nbits>& bits<nbits>::operator=(const bits<nbits>& b)
{
    if (this != &b)
    {
        for (int i = 0; i < NWORDS; ++i)
            _bits[i] = b._bits[i];
        _cleanup();  // Is this necessary?
    }
    return *this;
}

template<int nbits>
int bits<nbits>::operator==(const bits<nbits>& b) const
{
    for (int i = 0; i < NWORDS; ++i)
        if (_bits[i] != b._bits[i])
            return 0;
    return 1;
}

template<int nbits>
bits<nbits> bits<nbits>::operator&(const bits<nbits>& y) const
{
    bits b(*this);
    return b &= y;
}

template<int nbits>
bits<nbits> bits<nbits>::operator|(const bits<nbits>& y) const
{
    bits b(*this);
    return b |= y;
}

template<int nbits>
bits<nbits> bits<nbits>::operator^(const bits<nbits>& y) const
{
    bits b(*this);
    return b ^= y;
}

template<int nbits>
bits<nbits> bits<nbits>::operator~() const
{
    bits b(*this);
    b.toggle();
    b._cleanup();
    return b;
}

template<int nbits>
int bits<nbits>::count() const
{
    int sum = 0;
    for (int i = 0; i < nbits; ++i)
        if (test(i))
            ++sum;
    return sum;
}

template<int nbits>
void bits<nbits>::_cleanup()
{
    // Make sure unused bits don't get set
    for (int i = nbits; i < NWORDS*_WRDSIZ; ++i)
        _reset(i);
}

template<int nbits>
ostream& bits<nbits>::_print(ostream& os) const
{
    for (int i = nbits-1; i >= 0; --i)
        os << test(i);
    return os;
}

template<int nbits>
const bits<nbits>& bits<nbits>::operator<<=(unsigned n)
{
    for (int i = 0; i < NWORDS; ++i)
        _bits[i] <<= n;
    _cleanup();
    return *this;
}

template<int nbits>
bits<nbits> bits<nbits>::operator<<(unsigned n) const
{
    bits r(*this);
    return r <<= n;
}

template<int nbits>
const bits<nbits>& bits<nbits>::operator>>=(unsigned n)
{
    for (int i = 0; i < NWORDS; ++i)
        _bits[i] >>= n;
    return *this;
}

template<int nbits>
bits<nbits> bits<nbits>::operator>>(unsigned n) const
{
    bits r(*this);
    return r >>= n;
}
