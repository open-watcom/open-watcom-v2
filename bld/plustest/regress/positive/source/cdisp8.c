// takes a long to compile under WPP386 with all text included
#include "fail.h"
#include <limits.h>

#pragma inline_depth(0)
#if INT_MAX == SHRT_MAX
#define USE	D20
#else
#define USE	D50
#endif

struct U1 {
    int u1;
};
struct U2 {
    int u2;
};
struct U3 {
    int u3;
};
struct U4 {
    int u4;
};

struct V : virtual U1, virtual U2 {
    int v;
    void virtual foo( int x, int line )
    {
	if( v != x ) {
	    fail(line);
	}
    }
};

struct BAD : virtual V {
    void virtual foo( int x, int line )
    {
	V::foo( x, line );
    }
    BAD()
    {
	V *pv = this;

	v = -33;
	pv->foo( v, __LINE__ );		// MSC7 fails here
    }
};
struct FAILURE : virtual BAD {
    FAILURE()
    {
	V *pv = this;

	v = -42;
	pv->foo( v, __LINE__ );		// MetaWare C++ fails here
    }
};
struct D0 : virtual FAILURE, virtual BAD, virtual U2, virtual U1 {
    int d0;
};
struct D1 : virtual D0 {
    int d1;
};
struct D2 : virtual D1 {
    int d2;
};
struct D3 : virtual D2 {
    int d3;
};
struct D4 : virtual D3 {
    int d4;
};
struct D5 : virtual D4 {
    int d5;
};
struct D6 : virtual D5 {
    int d6;
};
struct D7 : virtual D6 {
    int d7;
};
struct D8 : virtual D7 {
    int d8;
};
struct D9 : virtual D8 {
    int d9;
};
struct D10 : virtual D9 {
    int d10;
};
struct D11 : virtual D10 {
    int d11;
};
struct D12 : virtual D11 {
    int d12;
};
struct D13 : virtual D12 {
    int d13;
};
struct D14 : virtual D13 {
    int d14;
};
struct D15 : virtual D14 {
    int d15;
};
struct D16 : virtual D15 {
    int d16;
};
struct D17 : virtual D16 {
    int d17;
};
struct D18 : virtual D17 {
    int d18;
};
struct D19 : virtual D18 {
    int d19;
};
struct D20 : virtual D19 {
    int d20;
};
struct D21 : virtual D20 {
    int d21;
};
struct D22 : virtual D21 {
    int d22;
};
struct D23 : virtual D22 {
    int d23;
};
struct D24 : virtual D23 {
    int d24;
};
struct D25 : virtual D24 {
    int d25;
};
struct D26 : virtual D25 {
    int d26;
};
struct D27 : virtual D26 {
    int d27;
};
struct D28 : virtual D27 {
    int d28;
};
struct D29 : virtual D28 {
    int d29;
};
struct D30 : virtual D29 {
    int d30;
};
struct D31 : virtual D30 {
    int d31;
};
struct D32 : virtual D31 {
    int d32;
};
struct D33 : virtual D32 {
    int d33;
};
struct D34 : virtual D33 {
    int d34;
};
struct D35 : virtual D34 {
    int d35;
};
struct D36 : virtual D35 {
    int d36;
};
struct D37 : virtual D36 {
    int d37;
};
struct D38 : virtual D37 {
    int d38;
};
struct D39 : virtual D38 {
    int d39;
};
struct D40 : virtual D39 {
    int d40;
};
struct D41 : virtual D40 {
    int d41;
};
struct D42 : virtual D41 {
    int d42;
};
struct D43 : virtual D42 {
    int d43;
};
struct D44 : virtual D43 {
    int d44;
};
struct D45 : virtual D44 {
    int d45;
};
struct D46 : virtual D45 {
    int d46;
};
struct D47 : virtual D46 {
    int d47;
};
struct D48 : virtual D47 {
    int d48;
};
struct D49 : virtual D48 {
    int d49;
};
struct D50 : virtual D49 {
    int d50;
};
#if 0
struct D51 : virtual D50 {
    int d51;
};
struct D52 : virtual D51 {
    int d52;
};
struct D53 : virtual D52 {
    int d53;
};
struct D54 : virtual D53 {
    int d54;
};
struct D55 : virtual D54 {
    int d55;
};
struct D56 : virtual D55 {
    int d56;
};
struct D57 : virtual D56 {
    int d57;
};
struct D58 : virtual D57 {
    int d58;
};
struct D59 : virtual D58 {
    int d59;
};
struct D60 : virtual D59 {
    int d60;
};
struct D61 : virtual D60 {
    int d61;
};
struct D62 : virtual D61 {
    int d62;
};
struct D63 : virtual D62 {
    int d63;
};
struct D64 : virtual D63 {
    int d64;
};
struct D65 : virtual D64 {
    int d65;
};
struct D66 : virtual D65 {
    int d66;
};
struct D67 : virtual D66 {
    int d67;
};
struct D68 : virtual D67 {
    int d68;
};
struct D69 : virtual D68 {
    int d69;
};
struct D70 : virtual D69 {
    int d70;
};
struct D71 : virtual D70 {
    int d71;
};
struct D72 : virtual D71 {
    int d72;
};
struct D73 : virtual D72 {
    int d73;
};
struct D74 : virtual D73 {
    int d74;
};
struct D75 : virtual D74 {
    int d75;
};
struct D76 : virtual D75 {
    int d76;
};
struct D77 : virtual D76 {
    int d77;
};
struct D78 : virtual D77 {
    int d78;
};
struct D79 : virtual D78 {
    int d79;
};
struct D80 : virtual D79 {
    int d80;
};
struct D81 : virtual D80 {
    int d81;
};
struct D82 : virtual D81 {
    int d82;
};
struct D83 : virtual D82 {
    int d83;
};
struct D84 : virtual D83 {
    int d84;
};
struct D85 : virtual D84 {
    int d85;
};
struct D86 : virtual D85 {
    int d86;
};
struct D87 : virtual D86 {
    int d87;
};
struct D88 : virtual D87 {
    int d88;
};
struct D89 : virtual D88 {
    int d89;
};
struct D90 : virtual D89 {
    int d90;
};
struct D91 : virtual D90 {
    int d91;
};
struct D92 : virtual D91 {
    int d92;
};
struct D93 : virtual D92 {
    int d93;
};
struct D94 : virtual D93 {
    int d94;
};
struct D95 : virtual D94 {
    int d95;
};
struct D96 : virtual D95 {
    int d96;
};
struct D97 : virtual D96 {
    int d97;
};
struct D98 : virtual D97 {
    int d98;
};
struct D99 : virtual D98 {
    int d99;
};
#endif
struct DISRUPT : virtual U2, virtual U1, virtual U3, virtual U4, virtual FAILURE {
};
struct FINAL : DISRUPT, virtual USE {
    void virtual foo( int x, int line )
    {
	V::foo( x, line );
    }
};

int main()
{
    static FINAL dummy;

    _PASS;
}
