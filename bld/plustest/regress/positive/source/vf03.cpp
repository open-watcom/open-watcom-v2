#include "fail.h"

// Section: 10.2
// Filename: c1020601.C
// Keywords: section_10.2, virtual_functions
//
// Purpose: 
//
//    Positive test for chapter 10, Section 2, para 6, line 1: 
//
//	  "... virtual functions used with multiple base-classes"
//				
// Description: 
//
//    Set up the following hierarchy of classes:
//
//						      B1
//					      B2	   B3
//						      B4
//
//			   L1							R1
//		   L2		L3					R2		R3
//			   L4							R4
//							
//						      D1
//					      D2	   D3
//						      D4
//
//	   Class B"i" defines virtual function vf"i", for i = 1,2,3,4.
//
//	   Class L2 overrides vf2(), L3 overrides vf3(), 
//	   R1 overrides vf1(), and R4 overrides vf4().
//
//	   Note that each virtual function in the "B" diamond is 
//	   overridden only once, so every virtual function has a
//	   unique final overrider.
//
//	   The functions are accessed in a D4 object through various 
//	   pointers.
//
// DWP:  June 1st, 1993
//
// ARM:  COMPATIBLE
//
// Status: no known problems
//


//	B1 - B4 hierarchy
//
struct B1 {
	virtual int vf1() { return iv+1; }
	int iv;
	B1() : iv(1) {}
};

struct B2 : virtual B1 {
	virtual int vf2() { return iv+2; }
};

struct B3 : virtual B1 {
	virtual int vf3() { return iv+3; }
};

struct B4 : B2, B3 {
	virtual int vf4() { return iv+4; }
};


// Left Diamond
//
struct L1 : virtual B4 {};
struct L2 : virtual L1 {
	int vf2() { return iv + 200; }
};
struct L3 : virtual L1 {
	int vf3() { return iv + 300; }
};
struct L4 : L2, L3 {
};

// Right Diamond
//
struct R1 : virtual B4 {
	int vf1() { return iv + 100; }
};
struct R2 : virtual R1 {};
struct R3 : virtual R1 {};
struct R4 : R2, R3 {
	int vf4() { return iv + 400; }
};

struct D1 : L4, R4 {};
struct D2 : virtual D1 {};
struct D3 : virtual D1 {};
struct D4 : D2, D3 {};

void chkVal( unsigned line, char const *, int v, int should )
{
    if( v != should ) {
	fail( line );
    }
}

int main()
{
	D4 od4;

	chkVal(__LINE__, "od4.vf1()", od4.vf1(), 101);
	chkVal(__LINE__, "od4.vf2()", od4.vf2(), 201);
	chkVal(__LINE__, "od4.vf3()", od4.vf3(), 301);
	chkVal(__LINE__, "od4.vf4()", od4.vf4(), 401);

	//	Check access through pointer to right "diamond" lattice
	//
	R1* rptr = new D4;
	chkVal(__LINE__, "rptr->vf1()", rptr->vf1(), 101);	// should call R1::vf1
	chkVal(__LINE__, "rptr->vf2()", rptr->vf2(), 201);	// should call L2::vf2
	chkVal(__LINE__, "rptr->vf3()", rptr->vf3(), 301);  // should call L3::vf3
	chkVal(__LINE__, "rptr->vf4()", rptr->vf4(), 401);	// should call R4::vf4
	
	//	Check access through pointer to left "diamond" lattice
	//
	L3* lptr = new D1;
	chkVal(__LINE__, "lptr->vf1()", lptr->vf1(), 101);	// should call R1::vf1
	chkVal(__LINE__, "lptr->vf2()", lptr->vf2(), 201);	// should call L2::vf2
	chkVal(__LINE__, "lptr->vf3()", lptr->vf3(), 301);  // should call L3::vf3
	chkVal(__LINE__, "lptr->vf4()", lptr->vf4(), 401);	// should call R4::vf4
	
	_PASS;
}
