// -ox-d1-ml
// generates a const table that requires a segment reloc (far ptr)
// to a R/W segment
// can we setup the tables to only use near pointers when an object is
// near?  AFS (affects Windows 16-bit programs)

struct S {
    S();
    ~S();
};

S x;
