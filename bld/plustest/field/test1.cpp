#include <math.h>
#include <stdlib.h>
#include <iostream.h>

#pragma inline_depth 0;

#include "field.h"

# define Assert(ex)	{if (!(ex)){(void)printf("Failed test: file \"%s\", line %d\n", __FILE__, __LINE__);exit(1);}}

static int flag = 0;	// Used in test for virtuality of destructor

double ret_double( double *p )
{
    return *p;
}

double force_double( double x )
{
    return ret_double( &x );
}

main(){
    #if defined(__386__)
    __chipbug = (__typeof(__chipbug)) ~0;
    #endif
  register int i;
  FILE *tmp = tmpfile();

// Default constructor
  {
    field a, b;
    Assert(a.Length() == 0);	// Default constructor creates length 0 field
    b = field(10, 0.0);		// length 0 field may be assigned to
    field c;
    Assert(fwrite(b, tmp));
    rewind(tmp);
    Assert(fread(c, tmp));	// length 0 field may be fread to
    Assert(c.Length() == 10);
    for (i=0; i<10; i++) 
      Assert(c[i] == 0.0);
  }

// Constructor with size argument (only)
  {
    field a(7);
    Assert(a.Length() == 7);	// Correct number of elements allocated
  }

// Constructor with size and initialization
  {
    field a(5, 10.0);
    Assert(a.Length() == 5);	// Correct length
    for (i=0; i<5; i++) 
      Assert(a[i] = 10.0);	// Correct data
  }

// Conversion from C array
  {
    static const double data[5] = { 5., 4., 3., 2., 1.};
    field a(5, data);
    Assert(a.Length() == 5);	// Correct length
    for (i=0; i<5; i++) 
      Assert(a[i] == data[i]);	// Correct data
  }

// Copy constructor
  {
    field a(22, 3.1416);
    field b(a);
    Assert(b.Length() == 22);	// Correct length
    for (i=0; i<22; i++) 
      Assert(b[i] == 3.1416);	// Correct data
  }

// Conversion from ifield
  {
    ifield q(10, 3);
    field d = q;
    Assert(d.Length() == 10);	// Correct length
    for (i=0; i<10; i++) 
      Assert(d[i] == 3.0);	// Correct data
  }

// Is destructor virtual?

  {
    class Derived_Field : public field {
      private:
        int *flag;
      public:
        Derived_Field(void) : flag(&::flag) { *flag = 1; }
        ~Derived_Field(void){ *flag = 0; }
    } *derived;
    derived = new Derived_Field;
    field *base = derived;
    delete base;
    Assert(flag == 0);		// Was the correct destructor called?
  }

// Test of assignment
  {
    field a(7, 2.);
    field b(3, 10.);
    a = b;
    Assert(a.Length() == 3);	// Correct length
    for (i=0; i<3; i++) 
      Assert(a[i] == 10.0);	// Correct data
  }

// Test of assignment of double
  {
    field a(7, 2.);
    a = 5.;
    Assert(a.Length() == 7);	// Correct length
    for (i=0; i<7; i++) 
      Assert(a[i] == 5.0);	// Correct data
  }

// Subscript and pointer conversion operators
  {
    field a(10, 0.0);
    field b;
    Assert(a[0] == 0.0);	
    Assert((a[0] = 2., a[0]) == 2.);	// Correct access
    Assert(&a[1+2] == &a[1]+2);		// Correct reference
    b = a;
    Assert(&a[0] != &b[0]);		// Uniqueness
    Assert(&a[0] == (double*)a);	// Correspondence
    Assert(&a[3] == (double*)a + 3);
    double &ref = a[0];
    b = a;
    Assert(&ref == &a[0]);
  }

// Operations
  {
    static const double data[] = {1., 2., 3., 4., 5.};
    field a;
    size_t l = sizeof(data)/sizeof(double);
    a = field(l, data); 
    Assert(a.Length() == 5);	// Correct length
    for (i=0; i<5; i++) 
      Assert(a[i] == data[i]);	// Correct data

// Unary operators
    {
      field b = +a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]);
      b = -a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == -a[i]);
    }

// Operators with scalar constants
    {
      field b = a*3.0;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3.0*a[i]);

      b = 3.0*a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3.0*a[i]);

      b = a;
      b *= 3.0;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3.0*a[i]);


      b = a/3.0;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]/3.0);

      b = 3.0/a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3.0/a[i]);

      b = a;
      b /= 3.0;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]/3.0);


      b = a+3.0;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]+3.0);

      b = 3.0+a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3.0+a[i]);

      b = a;
      b += 3.0;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]+3.0);


      b = a-3.0;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]-3.0);

      b = 3.0-a;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == 3.0-a[i]);

      b = a;
      b -= 3.0;
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == a[i]-3.0);
    }

// Operators with other double fields
    {
      field b = a;
      field c;


      c = a * b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] * b[i]);

      c = a;
      c *= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] * b[i]);


      c = a / b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] / b[i]);

      c = a;
      c /= b;
      Assert(c.Length() == l);
      for (i=0; i<l; i++)
        Assert(c[i] == a[i] / b[i]);


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
    }

// Operators returning ifield
    {
      field b = a;
      field c = 2.0*a;
      ifield ib, ic;


      ib = (a == 2.0);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] == 2.0));
      }

      ib = (2.0 == a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2.0 == a[i]));
      }

      ib = (a == b);
      ic = (a == c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] == b[i]));
        Assert(ic[i] == (a[i] == c[i]));
      }


      ib = (a != 2.0);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] != 2.0));
      }

      ib = (2.0 != a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2.0 != a[i]));
      }

      ib = (a != b);
      ic = (a != c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] != b[i]));
        Assert(ic[i] == (a[i] != c[i]));
      }


      ib = (a < 2.0);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] < 2.0));
      }

      ib = (2.0 < a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2.0 < a[i]));
      }

      ib = (a < b);
      ic = (a < c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] < b[i]));
        Assert(ic[i] == (a[i] < c[i]));
      }


      ib = (a > 2.0);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] > 2.0));
      }

      ib = (2.0 > a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2.0 > a[i]));
      }

      ib = (a > b);
      ic = (a > c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] > b[i]));
        Assert(ic[i] == (a[i] > c[i]));
      }


      ib = (a <= 2.0);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] <= 2.0));
      }

      ib = (2.0 <= a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2.0 <= a[i]));
      }

      ib = (a <= b);
      ic = (a <= c);
      Assert(ib.Length() == l);
      Assert(ic.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] <= b[i]));
        Assert(ic[i] == (a[i] <= c[i]));
      }


      ib = (a >= 2.0);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (a[i] >= 2.0));
      }

      ib = (2.0 >= a);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(ib[i] == (2.0 >= a[i]));
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
      double sum = 0.0;
      for (i=0; i<l; i++)
        sum += data[i];
      Assert(a.Sum() == sum);
    }

// Transcendentals
    {
      double __max = max(a);
      Assert( __max > 4.9 && __max < 5.1 );
      field aa = a/max(a);
      field b, c;
      ifield ib;
      int j;
      double cc;

      b = acos(aa);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == acos(aa[i]));

      b = asin(aa);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(asin(aa[i])));

      b = atan(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(atan(a[i])));

      aa = a+1.0;
      b = atan2(a, aa);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(atan2(a[i], aa[i])));

      b = atan2(a, 3.);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(atan2(a[i], 3.)));

      b = atan2(3., a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(atan2(3.0, a[i])));

      b = ceil(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == ceil(a[i]));

      b = cos(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(cos(a[i])));

      b = cosh(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == cosh(a[i]));

      b = exp(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == exp(a[i]));

      b = fabs(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == fabs(a[i]));

      b = floor(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == floor(a[i]));

      b = frexp(a, ib);
      Assert(b.Length() == l);
      Assert(ib.Length() == l);
      for (i=0; i<l; i++){
        Assert(b[i] == frexp(a[i], &j));
        Assert(ib[i] == j);
      }

      b = fmod(a, 3.0);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == fmod(a[i], 3.0));

      b = fmod(3.0, a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == fmod(3.0, a[i]));

      b = ldexp(a, ib);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == ldexp(a[i], ib[i]));

      b = log(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(log(a[i])));

      b = log10(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(log10(a[i])));

      b = modf(a, c);
      Assert(b.Length() == l);
      Assert(c.Length() == l);
      for (i=0; i<l; i++){
        Assert(b[i] == modf(a[i], &cc));
        Assert(c[i] == cc);
      }

      b = pow(a, 2.5);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == pow(a[i], 2.5));

      b = pow(2.5, a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == pow(2.5, a[i]));

      b = pow(a, aa);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == pow(a[i], aa[i]));

      b = sin(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(sin(a[i])));

      b = sinh(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == sinh(a[i]));

      b = sqrt(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == sqrt(a[i]));

      b = tan(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == force_double(tan(a[i])));

      b = tanh(a);
      Assert(b.Length() == l);
      for (i=0; i<l; i++)
        Assert(b[i] == tanh(a[i]));
    }
  }

// FORTRAN-like functions
  {
    static const double data1[] = {2., 1., 3., 4.};
    static const double data2[] = {3., 4., 2., 1.};
    static const size_t l = 4;
    const field a(l, data1), b(l, data2);
    field c;


    c = dim(a, 2.);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > 2.? a[i] - 2. : 0.0));

    c = dim(2., a);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] < 2.? 2. - a[i] : 0.0));

    c = dim(a, b);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > b[i]? a[i] - b[i] : 0.0));


    c = min(a, 2.);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] < 2.? a[i] : 2.0));

    c = min(2., a);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] < 2.? a[i] : 2.0));

    c = min(a, b);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] < b[i]? a[i] : b[i]));


    c = max(a, 2.);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > 2.? a[i] : 2.0));

    c = max(2., a);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > 2.? a[i] : 2.0));

    c = max(a, b);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == (a[i] > b[i]? a[i] : b[i]));

    Assert(min(a) == 1.0);
    Assert(max(b) == 4.0);


    c = sign(a, 2.);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == a[i]);

    c = sign(2., a);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == 2.0);

    c = sign(a, b);
    Assert(c.Length() == l);
    for (i=0; i<l; i++)
      Assert(c[i] == a[i]);
  }

// Shifts
  {
    static const double data[] = {1., 2., 3., 4.};
    field a(4, data), b, c;

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
     static const double data_a[] = {1., 2., 3., 4.};
     static const double data_b[] = {4., 3., 2., 1.};
     static const int data_ia[] = {0, 2, 1, 3};
     static const int data_ib[] = {0, 1, 3, 2};
     const field ca(4, data_a);
     const field cb(4, data_b);
     const ifield ia(4, data_ia);
     const ifield ib(4, data_ib);
     field a, b;

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
     a.Assemble(1.0, ia);
     for (i=0; i<4; i++)
       Assert(a[ia[i]] == ca[ia[i]] + 1.0);


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
     a.Scatter(1.0, ia);
     for (i=0; i<4; i++)
       Assert(a[ia[i]] == 1.0);

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
    field a(27, 2.);
    a.Free();
    Assert(a.Length() == 0);
  }

  cout << "*****Test completed successfully*****\n";
  fclose(tmp);
  return(0);
}

