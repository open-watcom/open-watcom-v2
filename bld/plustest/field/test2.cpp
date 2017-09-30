#define BAD
#include <math.h>
#include <stdlib.h>
#include <iostream.h>

#pragma inline_depth 0;

#include "field.h"

# define Assert(ex)	{if (!(ex)){(void)printf("Failed test: file \"%s\", line %d\n", __FILE__, __LINE__);exit(1);}}

static int flag = 0;	// Used in test for virtuality of destructor

main(){
    #if defined(__386__)
    __chipbug = (__typeof(__chipbug)) ~0;
    #endif
  register int i;
  FILE *tmp = tmpfile();

// Default constructor
  {
    ifield a, b;
    Assert(a.Length() == 0);	// Default constructor creates length 0 ifield
    b = ifield(10, 0);		// length 0 ifield may be assigned to
    ifield c;
    Assert(fwrite(b, tmp));
    rewind(tmp);
    Assert(fread(c, tmp));	// length 0 ifield may be fread to
    Assert(c.Length() == 10);
    for (i=0; i<10; i++) 
      Assert(c[i] == 0);
  }

// Constructor with size argument (only)
  {
    ifield a(7);
    Assert(a.Length() == 7);	// Correct number of elements allocated
  }

// Constructor with size and initialization
  {
    ifield a(5, 10);
    Assert(a.Length() == 5);	// Correct length
    for (i=0; i<5; i++) 
      Assert(a[i] = 10);	// Correct data
  }

// Conversion from C array
  {
    static const int data[5] = { 5, 4, 3, 2, 1};
    ifield a(5, data);
    Assert(a.Length() == 5);	// Correct length
    for (i=0; i<5; i++) 
      Assert(a[i] == data[i]);	// Correct data
  }

// Copy constructor
  {
    ifield a(22, 3);
    ifield b(a);
    Assert(b.Length() == 22);	// Correct length
    for (i=0; i<22; i++) 
      Assert(b[i] == 3);	// Correct data
  }

// Conversion from field
  {
    field q(10, 3);
    ifield d = q;
    Assert(d.Length() == 10);	// Correct length
    for (i=0; i<10; i++) 
      Assert(d[i] == 3);	// Correct data
  }

// Is destructor virtual?

  {
    class Derived_Field : public ifield {
      private:
        int *flag;
      public:
        Derived_Field(void) : flag(&::flag) { *flag = 1; }
        ~Derived_Field(void){ *flag = 0; }
    } *derived;
    derived = new Derived_Field;
    ifield *base = derived;
    delete base;
    Assert(flag == 0);		// Was the correct destructor called?
  }

// Test of assignment
  {
    ifield a(7, 2);
    ifield b(3, 10);
    a = b;
    Assert(a.Length() == 3);	// Correct length
    for (i=0; i<3; i++) 
      Assert(a[i] == 10);	// Correct data
  }

// Test of assignment of int
  {
    ifield a(7, 2);
    a = 5;
    Assert(a.Length() == 7);	// Correct length
    for (i=0; i<7; i++) 
      Assert(a[i] == 5);	// Correct data
  }

// Subscript and pointer conversion operators
  {
    ifield a(10, 0);
    ifield b;
    Assert(a[0] == 0);	
    Assert((a[0] = 2, a[0]) == 2);	// Correct access
    Assert(&a[1+2] == &a[1]+2);		// Correct reference
    b = a;
    Assert(&a[0] != &b[0]);		// Uniqueness
    Assert(&a[0] == (int*)a);	// Correspondence
    Assert(&a[3] == (int*)a + 3);
    int& ref = a[0];
    b = a;
    Assert(&ref == &a[0]);
  }

// Operations
  {
    static const int data[] = {0, 1, 2, 3, 4, 5};
    ifield a;
    size_t l = sizeof(data)/sizeof(double);
    a = ifield(l, data); 

// Unary operators
    {
      ifield b = +a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]);

      b = -a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == -a[i]);

      b = ~a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == ~a[i]);

      b = !a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == !a[i]);
    }

// Operators with scalar constants
    {
      ifield b = a*3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3*a[i]);

      b = 3*a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3.0*a[i]);

      b = a;
      b *= 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3*a[i]);


      b = a/3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]/3);

      b = 3/(a+1);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3/(a[i]+1));

      b = a;
      b /= 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]/3);


      b = a%3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]%3);

      b = 3%(a+1);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3%(a[i]+1));

      b = a;
      b %= 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]%3);


      b = a+3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]+3);

      b = 3+a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3+a[i]);

      b = a;
      b += 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]+3);


      b = a-3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]-3);

      b = 3-a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3-a[i]);

      b = a;
      b -= 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]-3);


      b = a^3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]^3));

      b = 3^a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (3^a[i]));

      b = a;
      b ^= 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]^3));


      b = a&3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]&3));

      b = 3&a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (3&a[i]));

      b = a;
      b &= 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]&3));


      b = a|3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]|3));

      b = 3|a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (3|a[i]));

      b = a;
      b |= 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]|3));


      b = a<3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]<3);

      b = 3<a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3<a[i]);


      b = a>3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]>3);

      b = 3>a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3>a[i]);


      b = a<=3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]<=3);

      b = 3<=a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3<=a[i]);


      b = a>=3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]>=3);

      b = 3>=a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3>=a[i]);


      b = a<<3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]<<3);

      b = 3<<a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3<<a[i]);

      b = a;
      b <<= 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]<<3);


      b = a>>3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]>>3);

      b = 3>>a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3>>a[i]);

      b = a;
      b >>= 3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]>>3);


      b = a&&3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]&&3));

      b = 3&&a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (3&&a[i]));


      b = a||3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]||3));

      b = 3||a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (3||a[i]));


      b = a==3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]==3));

      b = 3==a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (3==a[i]));


      b = a!=3;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]!=3));

      b = 3!=a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (3!=a[i]));
    }

// Operators with other integer ifields
    {
      ifield b = a;
      ifield c;


      c = a * b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] * b[i]);

      c = a;
      c *= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] * b[i]);


      c = a / (b+1);
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] / (b[i]+1));

      c = a;
      c /= (b+1);
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] / (b[i]+1));


      c = a % (b+1);
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] % (b[i]+1));

      c = a;
      c %= (b+1);
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] % (b[i]+1));


      c = a + b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] + b[i]);

      c = a;
      c += b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] + b[i]);


      c = a - b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] - b[i]);

      c = a;
      c -= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] - b[i]);


      c = a ^ b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] ^ b[i]));

      c = a;
      c ^= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] ^ b[i]));


      c = a | b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] | b[i]));

      c = a;
      c |= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] | b[i]));


      c = a & b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] & b[i]));

      c = a;
      c &= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] & b[i]));


      c = a << b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] << b[i]);

      c = a;
      c <<= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] << b[i]);


      c = a >> b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] >> b[i]);

      c = a;
      c >>= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] >> b[i]);


      c = a == b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] == b[i]));


      c = a != b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] != b[i]));


      c = a < b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] < b[i]));


      c = a > b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] > b[i]));


      c = a <= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] <= b[i]));


      c = a >= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] >= b[i]));


      c = a || b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] || b[i]));


      c = a && b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == (a[i] && b[i]));
    }

// Operators returning field
    {
      ifield b = a;
      ifield c = 2*a;
      ifield ib, ic;


      ib = (a == 2);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] == 2));
      }

      ib = (2 == a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2 == a[i]));
      }

      ib = (a == b);
      ic = (a == c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] == b[i]));
        Assert(ic[i] == (a[i] == c[i]));
      }


      ib = (a != 2);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] != 2));
      }

      ib = (2 != a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2 != a[i]));
      }

      ib = (a != b);
      ic = (a != c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] != b[i]));
        Assert(ic[i] == (a[i] != c[i]));
      }


      ib = (a < 2);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] < 2));
      }

      ib = (2 < a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2 < a[i]));
      }

      ib = (a < b);
      ic = (a < c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] < b[i]));
        Assert(ic[i] == (a[i] < c[i]));
      }


      ib = (a > 2);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] > 2));
      }

      ib = (2 > a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2 > a[i]));
      }

      ib = (a > b);
      ic = (a > c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] > b[i]));
        Assert(ic[i] == (a[i] > c[i]));
      }


      ib = (a <= 2);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] <= 2));
      }

      ib = (2 <= a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2 <= a[i]));
      }

      ib = (a <= b);
      ic = (a <= c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] <= b[i]));
        Assert(ic[i] == (a[i] <= c[i]));
      }


      ib = (a >= 2);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] >= 2));
      }

      ib = (2 >= a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2 >= a[i]));
      }

      ib = (a >= b);
      ic = (a >= c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] >= b[i]));
        Assert(ic[i] == (a[i] >= c[i]));
      }
    }

// Sum
    {
      int sum = 0;
      for (i=0; i<l; i++)
        sum += data[i];
      Assert(a.Sum() == sum);
    }

// Transcendentals
    {
      ifield b, c, d;
      field ib;

      b = abs(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == (a[i]>0? a[i] : -a[i]));

      b = a + 1;
      div(a, b, c, d);
      Assert(c.Length() == l);
      Assert(d.Length() == l);
      for (i=0; i<l; i++){
        Assert(c[i] == a[i]/b[i]);
        Assert(d[i] == a[i]%b[i]);
      }

      a.rand();
      for (i=1; i<l; i++){
        Assert(a[i] != a[i-1]);
      }
    }
  }

// FORTRAN-like functions
  {
    static const int data1[] = {2, 1, 3, 4};
    static const int data2[] = {3, 4, 2, 1};
    static const size_t l = 4;
    const ifield a(l, data1), b(l, data2);
    ifield c;


    c = dim(a, 2);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > 2? a[i] - 2 : 0));

    c = dim(2, a);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] < 2? 2 - a[i] : 0));

    c = dim(a, b);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > b[i]? a[i] - b[i] : 0));


    c = min(a, 2);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] < 2? a[i] : 2));

    c = min(2, a);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] < 2? a[i] : 2));

    c = min(a, b);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] < b[i]? a[i] : b[i]));


    c = max(a, 2);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > 2? a[i] : 2));

    c = max(2, a);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > 2? a[i] : 2));

    c = max(a, b);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > b[i]? a[i] : b[i]));

    Assert(min(a) == 1);
    Assert(max(b) == 4);


    c = sign(a, 2);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == a[i]);

    c = sign(2, a);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == 2);

    c = sign(a, b);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == a[i]);
  }

// Shifts
  {
    static const int data[] = {1, 2, 3, 4};
    ifield a(4, data), b, c;

    b = a.Shift(-2);
    Assert(b.Length() == 4);
    for (i=0; i<2; i++)
      Assert(b[i] = a[i+2]);

    b = a.Shift(1);
    Assert(b.Length() == 4);
    for (i=1; i<4; i++)
      Assert(b[i] = a[i-1]);
  }

// Assembly, Scatter, Gather
  {
     static const int data_a[] = {1, 2, 3, 4};
     static const int data_b[] = {4, 3, 2, 1};
     static const int data_ia[] = {0, 2, 1, 3};
     static const int data_ib[] = {0, 1, 3, 2};
     const ifield ca(4, data_a);
     const ifield cb(4, data_b);
     const ifield ia(4, data_ia);
     const ifield ib(4, data_ib);
     ifield a, b;

// Assembly, first form
     a = ca;
     a.Assemble(cb, ia, ib);
     for (i=0; i<4; i++)
       Assert(a[ia[i]] == ca[ia[i]] + cb[ib[i]]);

// Assembly, second form
     a = ca;
     a.Assemble(cb, ia);
     for (i=0; i<4; i++)
       Assert(a[ia[i]] == ca[ia[i]] + cb[i]);

// Assembly, third form
     a = ca;
     a.Assemble(1, ia);
     for (i=0; i<4; i++)
       Assert(a[ia[i]] == ca[ia[i]] + 1);


// Scatter, first form
     a = ca;
     a.Scatter(cb, ia, ib);
     for (i=0; i<4; i++)
       Assert(a[ia[i]] == cb[ib[i]]);

// Scatter, second form
     a = ca;
     a.Scatter(cb, ia);
     for (i=0; i<4; i++)
       Assert(a[ia[i]] == cb[i]);

// Scatter, third form
     a = ca;
     a.Scatter(1, ia);
     for (i=0; i<4; i++)
       Assert(a[ia[i]] == 1);

// Gather, first form
    a = ca.Gather(ib);
    Assert(a.Length() == 4);
    for (i=0; i<4; i++)
      Assert(a[i] == ca[ib[i]]);

// Scatter, fourth form
    a = ca;
    a.Scatter(cb, 0, 3, 2);
    for (i=0; i<2; i++)
      Assert(a[i<<1] == cb[i]);

// Gather, second form
    a = ca.Gather(0, 3, 2);
    Assert(a.Length() == 2);
    for (i=0; i<2; i++)
      Assert(a[i] == ca[i<<1]);
  }   

// test the Free function
  {
    ifield a(27, 2);
    a.Free();
    Assert(a.Length() == 0);
  }

  cout << "*****Test completed successfully*****\n";
  fclose(tmp);
  exit(0);
}

