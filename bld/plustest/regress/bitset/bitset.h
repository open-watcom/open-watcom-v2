// bitset.h:    Bit arrays / sets

class ostream;
class Bitset;

// Bitref is a helper class to enable a symmetric Bitset::operator[]()
class Bitref
{
    class Bitset &bs;
    unsigned bit;
    
    Bitref(Bitset & _bs, unsigned _bit) : bs(_bs), bit(_bit) {}

public:
    Bitref& operator=(unsigned);
    // GRB operator unsigned(); 
    operator int(); //GRB
    friend ostream& operator<<(ostream&, const Bitref&);
    friend class Bitset;    // Because Bitref(_bs,_bit) is private
};

class Bitset
{
public:
    Bitset(unsigned);
    Bitset(const Bitset&);
    ~Bitset() {delete [] bits;}

    // Assignment
    Bitset& operator=(const Bitset&);

    // Bit operations
    Bitset& set(unsigned);
    Bitset& set(unsigned, unsigned);
    Bitset& set();
    Bitset& reset(unsigned);
    Bitset& reset(unsigned, unsigned);
    Bitset& reset();
    Bitset& toggle(unsigned);
    Bitset& toggle(unsigned, unsigned);
    Bitset& toggle();
    int test(unsigned b) const;

    // Set operations
    Bitset& operator&=(const Bitset& b) {return set_intersect(b);}
    Bitset& set_intersect(const Bitset&);
    Bitset& operator|=(const Bitset& b) {return set_union(b);}
    Bitset& set_union(const Bitset&);
    Bitset& operator^=(const Bitset& b) {return set_sym_diff(b);}
    Bitset& set_sym_diff(const Bitset&);
    Bitset& operator-=(const Bitset& b) {return set_diff(b);}
    Bitset& set_diff(const Bitset&);
    Bitset operator~() const;
    int is_null() const;
    operator const void*() const;
    int count() const;
    int size() const {return nbits;}
    int subsetof(const Bitset&) const;
    
    // Equality
    int operator==(const Bitset& b) const {return equal(b);}
    int equal(const Bitset&) const;
    int operator!=(const Bitset& b) const {return !equal(b);};
    int not_equal(const Bitset& b) const {return !equal(b);};

    // Subscript
    Bitref operator[](unsigned b) {return Bitref(*this,b);}

    // Friends
    friend ostream& operator<<(ostream&, const Bitset&);
    friend Bitset operator&(const Bitset&, const Bitset&);
    friend Bitset operator|(const Bitset&, const Bitset&);
    friend Bitset operator^(const Bitset&, const Bitset&);
    friend Bitset operator-(const Bitset&, const Bitset&);
    friend int operator<=(const Bitset& x, const Bitset& y);
    friend int operator>=(const Bitset& x, const Bitset& y);
    friend int operator<(const Bitset& x, const Bitset& y);
    friend int operator>(const Bitset& x, const Bitset& y);

private:
    unsigned *bits;
    unsigned nbits;

    enum {WRDSIZ = 16};
    static unsigned word(unsigned b) {return b / WRDSIZ;}
    static unsigned offset(unsigned b) {return WRDSIZ - b%WRDSIZ - 1;}
    static unsigned mask(unsigned b) {return 1u << offset(b);}
    static unsigned numwords(unsigned nb) {return (nb+WRDSIZ-1) / WRDSIZ;}
    void cleanup();
    void _reset(unsigned b) {bits[word(b)] &= ~mask(b);}  // Caveat usor
};

inline int operator<=(const Bitset& x, const Bitset& y)
      {return x.subsetof(y);}
inline int operator>=(const Bitset& x, const Bitset& y)
      {return y.subsetof(x);}
inline int operator<(const Bitset& x, const Bitset& y)
      {return x != y && x.subsetof(y);}
inline int operator>(const Bitset& x, const Bitset& y)
      {return x != y && y.subsetof(x);}

