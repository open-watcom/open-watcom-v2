// Status: bug fixed
struct T {
    T( int );
};

struct S {
    S( T );
    S( S const & );
};

int N;

#pragma on ( dump_rank )

S a1(1);
S a2(N);

/************************************************************************************

Overloaded Symbol Resolution Result: FNOV_NONAMBIGUOUS
Symbol: 'array::array( spec )' occurs 2 times
Ranked Arguments:
    'this qualifier': ''
    [1]: 'int <id> '
Matching Functions:
[1]: 'array::array( spec )'
    'this': OV_RANK_EXACT
    [1]: OV_RANK_UD_CONV
        UsrDef Input: Not Exact(1)
        UsrDef Output:
Rejected Functions:
[1]: 'array::array( array const & )'
    'this': OV_RANK_EXACT
    [1]: OV_RANK_UD_CONV
        UsrDef Input: Not Exact(1) Trivial(1)
        UsrDef Output: Not Exact(1)


Overloaded Symbol Resolution Result: FNOV_AMBIGUOUS
Symbol: 'array::array( array const & )' occurs 2 times
Ranked Arguments:
    'this qualifier': ''
    [1]: 'int & <id> '
Matching Functions:
[1]: 'array::array( spec )'
    'this': OV_RANK_EXACT
    [1]: OV_RANK_UD_CONV
        UsrDef Input: Not Exact(1)
        UsrDef Output: Not Exact(1)
[2]: 'array::array( array const & )'
    'this': OV_RANK_EXACT
    [1]: OV_RANK_UD_CONV
        UsrDef Input: Not Exact(1) Trivial(1)
        UsrDef Output: Not Exact(1)
File: abug02.cpp
(15,10): Error! E303: constructor is ambiguous for operands used
  ambiguous function: array::array( spec ) defined at: (6,5)
  ambiguous function: array::array( array const & ) defined at: (7,5)
  source conversion type is "int (lvalue)"
  target conversion type is "array"


******************************************************************************************/
