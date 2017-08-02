//=============================================================================
//
//  OOPACK - a benchmark for comparing OOP vs. C-style programming.
//  Copyright (C) 1995 Arch D. Robison
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  For a copy of the GNU General Public License, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//=============================================================================
//
// OOPACK: a benchmark for comparing OOP vs. C-style programming.
// 
// Version: 1.7
// 
// Author: Arch D. Robison (robison@kai.com)
//         Kuck & Associates 
//         1906 Fox Dr.
//         Champaign IL 61820
//              
// Web Info: http://www.kai.com/oopack/oopack.html
//
// Last revised: November 21, 1995
//
// This benchmark program contains a suite of tests that measure the relative 
// performance of object-oriented-programming (OOP) in C++ versus just writing
// plain C-style code in C++.  All of the tests are written so that a 
// compiler can in principle transform the OOP code into the C-style code.  
// After you run this benchmark and discover just how much you are paying to 
// use object-oriented programming, you will probably say: OOP? ACK!
// (Unless, of course, you have Kuck & Associates' Photon C++ compiler.)
//
// TO COMPILE
//
//      Compile with your favorite C++ compiler.  E.g. ``CC -O2 oopack.C''.
//      On most machines, no special command-line options are required.  
//      For Suns only, you need to define the symbol ``sun4''.  
//      E.g. ``g++ -O -Dsun4 oopack.C''.
//
// TO RUN
//
//      To run the benchmark, run ``a.out Max=50000 Matrix=500 Complex=20000 Iterator=50000''.
//      This runs the four tests for the specified number of iterations.
//      E.g., the Max test is run for 50000 iterations.  You may want to 
//      adjust the number of iterations to be small enough to get
//      an answer in reasonable time, but large enough to get a reasonably
//      accurate answer.
//
// INTERPRETING THE RESULTS
//      
//      Below is an example command line and the program's output.
//
//      $ a.out Max=5000 Matrix=50 Complex=2000  Iterator=5000
//      OOPACK Version 1.7
//      
//      For results on various systems and compilers, examine this Web Page:
//          http://www.kai.com/oopack/oopack.html
//      
//      Report your results by sending e-mail to oopack@kai.com.
//      For a run to be accepted, adjust the number of iterations for each test
//      so that each time reported is greater than 10 seconds.
//      
//      Send this output, along with:
//      
//           * your 
//              + name ------------------- 
//              + company/institution ---- 
//      
//           * the compiler
//              + name ------------------- 
//              + version number --------- 
//              + options used ----------- 
//      
//           * the operating system
//              + name ------------------- 
//              + version number --------- 
//      
//           * the machine
//              + manufacturer ----------- 
//              + model number ----------- 
//              + processor clock speed -- 
//              + cache memory size ------ 
//      
//                                Seconds       Mflops         
//      Test       Iterations     C    OOP     C    OOP  Ratio
//      ----       ----------  -----------  -----------  -----
//      Max              5000    1.3   1.3    3.8   4.0    1.0
//      Matrix             50    1.5   2.8    8.6   4.5    1.9
//      Complex          2000    1.5   5.3   10.8   3.0    3.6
//      Iterator         5000    1.1   1.6    9.4   6.3    1.5
//      
//      The ``Test'' column gives the names of the four tests that are run.  
//      The ``Iterations'' column gives the number of iterations that a test 
//      was run.  The The two ``Seconds'' columns give the C-style 
//      and OOP-style running times for a test.  The two ``Mflops'' columns
//      give the corresponding megaflop rates.  The ``Ratio'' column gives
//      the ratio between the times.  The value of 1.5 at the bottom, for 
//      example, indicates that the OOP-style code for Iterator ran 1.5 times 
//      more slowly than the C-style code.
//
//      Beware that a low ``Ratio'' could indicate either that the OOP-style
//      code is compiled very well, or that the C-style code is compiled poorly.
//      OOPACK performance figures for KAI's Photon C++ and some other compilers
//      can be found in http://www.kai.com/oopack/oopack.html.
//
// Revison History
//      9/17/93         Version 1.0 released
//      10/5/93         Allow results to be printed even if checksums do not match.
//      10/5/93         Increased ``Tolerance'' to allow 10-second runs on RS/6000.  
//      10/5/93         Version 1.1 released
//      1/10/94         Change author's address from Shell to KAI
//      1/13/94         Added #define's for conditional compilation of individual tests
//      1/21/94         Converted test functions to virtual members of class Benchmark.
//     10/11/94         Added routine to inform user of command-line usage.
//     10/11/94         Version 1.5 released.
//     11/21/95         V1.6 Added "mail results to oopack@kai.com" message in output
//     11/28/95         V1.7 Added company/institution to requested information

//=============================================================================

#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//
// The source-code begins with the benchmark computations themselves and
// ends with code for collecting statistics.  Each benchmark ``Foo'' is
// a class FooBenchmark derived from class Benchmark.  The relevant methods
// are:
//
//      init - Initialize the input data for the benchmark
//
//      c_style - C-style code
//
//      oop_style - OOP-style code
//
//      check - computes number of floating-point operations and a checksum.
//
const int BenchmarkListMax = 4;

class Benchmark {
public:
    void time_both( int iteration_count, double limit ) const;
    void time_one( void (Benchmark::*function)() const, int iterations, double& sec, double& Mflop, double& checksum ) const;
    virtual const char * name() const = 0;
    virtual void init() const = 0;
    virtual void c_style() const = 0; 
    virtual void oop_style() const = 0;
    virtual void check( int iterations, double& flops, double& checksum ) const = 0;
    static Benchmark * find( const char * name );
private:
    static Benchmark * list[BenchmarkListMax];
    static int count;
protected:
    Benchmark() {list[count++] = this;}
};

// The initializer for Benchmark::count *must* precede the declarations
// of derived of class Benchmark.
int Benchmark::count = 0;
Benchmark * Benchmark::list[BenchmarkListMax];

//
// The ``iterations'' argument is the number of times that the benchmark 
// computation was called.  The computed checksum that ensures that the
// C-style code and OOP code are computing the same result.  This 
// variable also prevents really clever optimizers from removing the
// the guts of the computations that otherwise would be unused.
//

// Each of the following symbols must be defined to enable a test, or
// undefined to disable a test.  The reason for doing this with the
// preprocessor is that some compilers may choke on specific tests.
#define HAVE_MAX 1
#define HAVE_MATRIX 1
#define HAVE_COMPLEX 1 
#define HAVE_ITERATOR 1

const int N = 1000;

#if HAVE_MAX
//=============================================================================
//
// Max benchmark
//
// This benchmark measures how well a C++ compiler inlines a function that 
// returns the result of a comparison.  
//
// The functions C_Max and OOP_Max compute the maximum over a vector.
// The only difference is that C_Max writes out the comparison operation
// explicitly, and OOP_Max calls an inline function to do the comparison.
//
// This benchmark is included because some compilers do not compile 
// inline functions into conditional branches as well as they might.  
//
const int M = 1000;             // Dimension of vector
double U[M];                    // The vector
double MaxResult;               // Result of max computation

class MaxBenchmark: public Benchmark {
private:
    const char * name() const {return "Max";}
    void init() const;
    void c_style() const; 
    void oop_style() const;
    void check( int iterations, double& flops, double& checksum ) const;
} TheMaxBenchmark;

void MaxBenchmark::c_style() const              // Compute max of vector (C-style)
{               
    double max = U[0];
    for( int k=1; k<M; k++ )                    // Loop over vector elements
        if( U[k] > max )  
            max=U[k];
    MaxResult = max;
}

inline int Greater( double i, double j ) 
{
    return i>j;
}

void MaxBenchmark::oop_style() const            // Compute max of vector (OOP-style)
{       
    double max = U[0];
    for( int k=1; k<M; k++ )                    // Loop over vector elements
        if( Greater( U[k], max ) ) 
            max=U[k];
    MaxResult = max;
}

void MaxBenchmark::init() const                         
{
    for( int k=0; k<M; k++ ) 
        U[k] = k&1 ? -k : k;
}

void MaxBenchmark::check( int iterations, double& flops, double& checksum ) const
{
    flops = (double)M*iterations;
    checksum = MaxResult;
}
#endif /* HAVE_MAX */

#if HAVE_MATRIX
//=============================================================================
//
// Matrix benchmark
//
// This benchmark measures how well a C++ compiler performs constant propagation and 
// strength-reduction on classes.  C_Matrix multiplies two matrices using C-style code; 
// OOP_Matrix does the same with OOP-style code.  To maximize performance on most RISC 
// processors, the benchmark requires that the compiler perform strength-reduction and 
// constant-propagation in order to simplify the indexing calculations in the inner loop.
//
const int L = 50;               // Dimension of (square) matrices.

double C[L*L], D[L*L], E[L*L];  // The matrices to be multiplied.

class MatrixBenchmark: public Benchmark {
private:
    const char * name() const {return "Matrix";}
    void init() const;
    void c_style() const; 
    void oop_style() const;
    void check( int iterations, double& flops, double& checksum ) const;
} TheMatrixBenchmark;

void MatrixBenchmark::c_style() const {         // Compute E=C*D with C-style code.
    for( int i=0; i<L; i++ )
        for( int j=0; j<L; j++ ) {
            double sum = 0;
            for( int k=0; k<L; k++ )
                sum += C[L*i+k]*D[L*k+j];
            E[L*i+j] = sum;
        }
}

class Matrix {  // Class Matrix represents a matrix stored in row-major format (same as C).
private:
    double *data;       // Pointer to matrix data
public:
    int rows, cols;     // Number of rows and columns

    Matrix( int rows_, int cols_, double * data_ ) :
        rows(rows_), cols(cols_), data(data_)
    {}

    double& operator()( int i, int j ) {        // Access element at row i, column j
        return data[cols*i+j];
    }
};

void MatrixBenchmark::oop_style() const {       // Compute E=C*D with OOP-style code.
    Matrix c( L, L, C );                        // Set up three matrices
    Matrix d( L, L, D );
    Matrix e( L, L, E );
    for( int i=0; i<e.rows; i++ )               // Do matrix-multiplication
        for( int j=0; j<e.cols; j++ ) {
            double sum = 0;
            for( int k=0; k<e.cols; k++ )
                sum += c(i,k)*d(k,j);
            e(i,j) = sum;
        }
}

void MatrixBenchmark::init() const      
{
    for( int j=0; j<L*L; j++ ) {
        C[j] = j+1;
        D[j] = 1.0/(j+1);
    }
}

void MatrixBenchmark::check( int iterations, double& flops, double& checksum ) const
{
    double sum = 0;
    for( int k=0; k<L*L; k++ )
        sum += E[k];
    checksum = sum;
    flops = 2.0*L*L*L*iterations;
}
#endif /* HAVE_MATRIX */

#if HAVE_ITERATOR
//=============================================================================
//
// Iterator benchmark 
//
// Iterators are a common abstraction in object-oriented programming, which
// unfortunately may incur a high cost if compiled inefficiently.
// The iterator benchmark below computes a dot-product using C-style code
// and OOP-style code.  All methods of the iterator are inline, and in 
// principle correspond exactly to the C-style code.
//
// Note that the OOP-style code uses two iterators, but the C-style
// code uses a single index.  Good common-subexpression elimination should,
// in principle, reduce the two iterators to a single index variable, or
// conversely, good strength-reduction should convert the single index into 
// two iterators!  
//
double A[N];
double B[N];
double IteratorResult;

class IteratorBenchmark: public Benchmark {
private:
    const char * name() const {return "Iterator";}
    void init() const;
    void c_style() const; 
    void oop_style() const;
    void check( int iterations, double& flops, double& checksum ) const;
} TheIteratorBenchmark;

void IteratorBenchmark::c_style() const                 // Compute dot-product with C-style code
{
    double sum = 0;
    for( int i=0; i<N; i++ )
        sum += A[i]*B[i];
    IteratorResult = sum;
}

class Iterator {                // Iterator for iterating over array of double
private:
    int index;                  // Index of current element
    const int limit;            // 1 + index of last element
    double * const array;       // Pointer to array
public:
    double look() {return array[index];}        // Get current element
    void next() {index++;}                      // Go to next element
    int done() {return index>=limit;}           // True iff no more elements
    Iterator( double * array1, int limit1 ) : 
        array(array1), 
        limit(limit1), 
        index(0) 
    {}
};

void IteratorBenchmark::oop_style() const               // Compute dot-product with OOP-style code
{
    double sum = 0;
    for( Iterator ai(A,N), bi(B,N); !ai.done(); ai.next(), bi.next() )
        sum += ai.look()*bi.look();
    IteratorResult = sum;
}

void IteratorBenchmark::init() const    
{
    for( int i=0; i<N; i++ ) {
        A[i] = i+1;
        B[i] = 1.0/(i+1);
    }
}

void IteratorBenchmark::check( int iterations, double& flops, double& checksum ) const {
    flops = 2*N*iterations;
    checksum = IteratorResult;
}
#endif /* HAVE_ITERATOR */

#if HAVE_COMPLEX
//=============================================================================
//
// Complex benchmark
//
// Complex numbers are a common abstraction in scientific programming.
// This benchmark measures how fast they are in C++ relative to the same
// calculation done by explicitly writing out the real and imaginary parts.
// The calculation is a complex-valued ``SAXPY'' operation.
//
// The complex arithmetic is all inlined, so in principle the code should
// run as fast as the version using explicit real and imaginary parts.
//
class ComplexBenchmark: public Benchmark {
private:
    const char * name() const {return "Complex";}
    void init() const;
    void c_style() const; 
    void oop_style() const;
    void check( int iterations, double& flops, double& checksum ) const;
} TheComplexBenchmark;

class Complex {
public:
    double re, im;
    Complex( double r, double i ) : re(r), im(i) {}
    Complex() {}
};

inline Complex operator+( Complex a, Complex b )        // Complex add
{
    return Complex( a.re+b.re, a.im+b.im );
}

inline Complex operator*( Complex a, Complex b )        // Complex multiply
{
    return Complex( a.re*b.re-a.im*b.im, a.re*b.im+a.im*b.re );
}

Complex X[N], Y[N];                             // Arrays used by benchmark

void ComplexBenchmark::c_style() const          // C-style complex-valued SAXPY operation
{
    double factor_re = 0.5;
    double factor_im = 0.86602540378443864676;
    for( int k=0; k<N; k++ ) {
        Y[k].re = Y[k].re + factor_re*X[k].re - factor_im*X[k].im;
        Y[k].im = Y[k].im + factor_re*X[k].im + factor_im*X[k].re;
    }
}

void ComplexBenchmark::oop_style() const        // OOP-style complex-valued SAXPY operation 
{
    Complex factor( 0.5, 0.86602540378443864676 );
    for( int k=0; k<N; k++ )
        Y[k] = Y[k] + factor*X[k];
}

void ComplexBenchmark::init() const     
{
    for( int k=0; k<N; k++ ) {
        X[k] = Complex( k+1, 1.0/(k+1) );
        Y[k] = Complex( 0, 0 );
    }
}

void ComplexBenchmark::check( int iterations, double& flops, double& checksum ) const {
    double sum = 0;
    for( int k=0; k<N; k++ )
        sum += Y[k].re + Y[k].im;
    checksum = sum;
    flops = 8*N*iterations;
}
#endif /* HAVE_COMPLEX */

//=============================================================================
// End of benchmark computations.  
//=============================================================================

// All the code below is for running and timing the benchmarks.                                              
#if defined(sun4) && !defined(CLOCKS_PER_SEC)
// Sun/4 include-files seem to be missing CLOCKS_PER_SEC.
#define CLOCKS_PER_SEC 1000000  
#endif

//
// TimeOne
//
// Time a single benchmark computation.
//
// Inputs
//      function = pointer to function to be run and timed.
//      iterations = number of times to call function.
//
// Outputs
//      sec = Total number of seconds for calls of function.
//      Mflop = Megaflop rate of function.
//      checksum = checksum computed by function.
//     
void Benchmark::time_one( void (Benchmark::*function)() const, int iterations, double& sec, double& Mflop, double& checksum ) const
{
    // Initialize and run code once to load caches
    init();
    (this->*function)();

    // Initialize and run code.
    init();
    clock_t t0 = clock();
    for( int k=0; k<iterations; k++ ) 
        (this->*function)();
    clock_t t1 = clock();

    // Update checksum and compute number of floating-point operations.
    double flops;
    check( iterations, flops, checksum );

    sec = (t1-t0) / (double)CLOCKS_PER_SEC;
    Mflop = flops/sec*1e-6;

}

//
// The variable ``C_Seconds'' is the time in seconds in which to run the 
// C-style benchmarks.
//
double C_Seconds = 1;   

//
// The variable ``Tolerance'' is the maximum allowed relative difference 
// between the C and OOP checksums.  Machines with multiply-add 
// instructions may produce different answers when they use those 
// instructions rather than separate instructions.
//
// There is nothing magic about the 32, it's just the result of tweaking.
//
const double Tolerance = 128*DBL_EPSILON;

Benchmark * Benchmark::find( const char * name ) {
    for( int i=0; i<count; i++ )
        if( strcmp( name, list[i]->name() )== 0 )
            return list[i];
    return NULL;        
}

//
// Benchmark::time_both
//
// Runs the C and Oop versions of a benchmark computation, and print the 
// results.
//
// Inputs
//      name = name of the benchmark
//      c_style = benchmark written in C-style code
//      oop_style = benchmark written in OOP-style code
//      check = routine to compute checksum on answer
//
void Benchmark::time_both( int iterations, double limit ) const {
    // Run the C-style code. 
    double c_sec, c_Mflop, c_checksum;
    time_one( &Benchmark::c_style, iterations, c_sec, c_Mflop, c_checksum );
   
    // Run the OOP-style code. 
    double oop_sec, oop_Mflop, oop_checksum;
    time_one( &Benchmark::oop_style, iterations, oop_sec, oop_Mflop, oop_checksum );

    // Compute execution-time ratio of OOP to C.  This is also the 
    // reciprocal of the Megaflop ratios.                                
    double ratio = oop_sec/c_sec;
 
    // Compute the absolute and relative differences between the checksums
    // for the two codes.
    double diff = c_checksum - oop_checksum;
    double min = c_checksum < oop_checksum ? c_checksum : oop_checksum;
    double rel = diff/min;

    // If the relative difference exceeds the tolerance, print an error-message,
    // otherwise print the statistics.
    if( rel > Tolerance || rel < -Tolerance ) {
        printf( "%-10s: warning: relative checksum error of %g between C (%g) and oop (%g)\n",
                name(), rel, c_checksum, oop_checksum );
    } 
    if( ratio > limit ) {
        printf( "%-10s %10d  %5.1f %5.1f  %5.1f %5.1f  %5.1f\n",
                name(), iterations, c_sec, oop_sec, c_Mflop, oop_Mflop, ratio );
    }
}

const char * Version = "Version 1.7"; // The OOPACK version number 

void Usage( int /*argc*/, char * argv[] ) {
    printf( "Usage:\t%s test1=iterations1 test2=iterations2 ...\n", argv[0] ); 
    printf( "E.g.:\ta.out  Max=5000 Matrix=50 Complex=2000  Iterator=5000\n" );
    exit(1);
}

int main( int argc, char * argv[] ) 
{
    // The available benchmarks are automatically put into the list of available benchmarks
    // by the constructor for Benchmark.

    // Check if user does not know command-line format.
    if( argc==1 ) {
        Usage( argc, argv );
    }
    int i;
#if 0
    for( i=1; i<argc; i++ ) {
        if( !isalpha(argv[i][0]) )
            Usage( argc, argv );
    }
#endif

    // Print header.
    printf("%-10s %10s  %11s  %11s  %5s\n", "", "", "Seconds  ", "Mflops  ", "" );
    printf("%-10s %10s  %5s %5s  %5s %5s  %5s\n",
           "Test", "Iterations", " C ", "OOP", " C ", "OOP", "Ratio" );
    printf("%-10s %10s  %11s  %11s  %5s\n", "----", "----------", "-----------", "-----------", "-----" );

    for( i=1; i<argc; i++ ) {
        const char * test_name = strtok( argv[i], "=" );
        const char * rhs =  strtok( NULL, "" );
        const char * limit =  strtok( argv[++i], "" );
        if( rhs==NULL ) {
            printf("missing iteration count for test '%s'\n", test_name );
        } else {
            int test_count = (int)strtol( rhs, 0, 0 );
            Benchmark * b = Benchmark::find( test_name );
            double test_limit = (double)strtod( limit, 0 );
            if( b==NULL ) {
                printf("skipping non-existent test = '%s'\n", test_name );
            } else {
                b->time_both( test_count, test_limit );
            }
        }
    }

    /* Print blank line. */
    printf("\n");

    return 0;
}
