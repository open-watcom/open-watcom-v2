#include "fail.h"
#if ( ! defined( M_I86 ) && ! defined( M_I386 ) ) \
  ||( defined(__386__) && ! defined(__OS2__) )

ALWAYS_PASS

#else
// positive test for special PC pointers
//

static void same_char           // TEST IF SAME CHARACTER
    ( char c1                   // - char[1]
    , char c2                   // - char[2]
    , unsigned line_no )        // - line #
{
    if( c1 != c2 ) {
        fail(line_no);
    }
}

static void same_unsigned       // TEST IF SAME UNSIGNED
    ( unsigned u1               // - value[1]
    , unsigned u2               // - value[2]
    , unsigned line_no )        // - line #
{
    if( u1 != u2 ) {
        fail(line_no);
    }
}


// -------------------- STRUCTURES -------------------

struct LINK {
    LINK __based((__segment)__self) *next;
    unsigned value;
    LINK( LINK*, unsigned );
};

LINK::LINK( LINK*p, unsigned v )
    : next(p), value(v)
{
}


// -------------------- DATA -------------------------

static char char_file_scope = 'q';
static char array_file_scope[] = { 'r', 's', 't', 'u', 'v' };

static void *base_ptr = array_file_scope;

static char __based( __segname( "MyArea" ) ) char_area = 'j';
static char __based( __segname( "MyArea" ) ) array_area[]
    = { 'a', 'b', 'c', 'd', 'e' };

static char __based( __segname( "_DATA" ) ) char_data = 'j';
static char __based( __segname( "_DATA" ) ) array_data[]
    = { 'a', 'b', 'c', 'd', 'e' };

static char __based( __segname( "_CODE" ) ) char_code = 'j';
static char __based( __segname( "_CODE" ) ) array_code[]
    = { 'a', 'b', 'c', 'd', 'e' };

static char __based( __segname( "_TEXT" ) ) char_text = 'j';
static char __based( __segname( "_TEXT" ) ) array_text[]
    = { 'a', 'b', 'c', 'd', 'e' };

static __segment seg_myarea = __segname( "MyArea" );
static __segment seg_data   = __segname( "_DATA"  );
static __segment seg_code   = __segname( "_CODE"  );
static __segment seg_text   = __segname( "_TEXT"  );




// --------------------- TEST ADDR OF --------------------

static void addrs_of()
{
    char *p01 = &char_file_scope;           // regular
    same_char( *p01, 'q', __LINE__ );

    char __far16 *p02 = &char_file_scope;   // far16
    same_char( *p02, 'q', __LINE__ );

    char __based( base_ptr ) *p03 = &array_file_scope[2];
    same_char( *p03, 't', __LINE__ );

    char _far *p04;
    p04 = p01; same_char( *p04, 'q', __LINE__ );
    p04 = p02; same_char( *p04, 'q', __LINE__ );
    p04 = p03; same_char( *p04, 't', __LINE__ );

    same_char( 'j', char_area, __LINE__ );
    same_char( 'c', array_area[2], __LINE__ );

    same_char( 'j', char_data, __LINE__ );
    same_char( 'c', array_data[2], __LINE__ );

    same_char( 'j', char_code, __LINE__ );
    same_char( 'c', array_code[2], __LINE__ );

    same_char( 'j', char_text, __LINE__ );
    same_char( 'c', array_text[2], __LINE__ );
}

// --------------------- TEST __SELF ---------------------

static void selfs()
{
    LINK l657( 0, 657 );
    LINK l658( &l657, 658 );
    LINK l659( &l658, 659 );
    LINK l660( &l659, 660 );
    LINK *p = &l660;

    same_unsigned( p->value, 660, __LINE__ );
    same_unsigned( p->next->value, 659, __LINE__ );
    same_unsigned( p->next->next->value, 658, __LINE__ );
    same_unsigned( p->next->next->next->value, 657, __LINE__ );
}

// --------------------- TEST __segment -------------------

static void seg_test(
    __segment seg_var,
    char __far* seg_beg,
    char __far* elements,
    unsigned line_no )
{
    char __based( seg_var ) *pseg;
    pseg = (unsigned)(elements - seg_beg);
    same_char( 'a', *(pseg+0), line_no );
    same_char( 'b', *(pseg+1), line_no );
    same_char( 'c', *(pseg+2), line_no );
    same_char( 'd', *(pseg+3), line_no );
    same_char( 'e', *(pseg+4), line_no );
}

static void segs()
{
    seg_test( seg_myarea
            , (char __far*)( char __based( seg_myarea ) *)0
            , array_area
            , __LINE__ );
    seg_test( seg_data
            , (char __far*)( char __based( seg_data ) *)0
            , array_data
            , __LINE__ );
    seg_test( seg_code
            , (char __far*)( char __based( seg_code ) *)0
            , array_code
            , __LINE__ );
    seg_test( seg_text
            , (char __far*)( char __based( seg_text ) *)0
            , array_text
            , __LINE__ );
}


// ----------------------- MAIN-LINE ----------------------

int main()
{
    addrs_of();
    selfs();
    segs();

    _PASS;
}
#endif
