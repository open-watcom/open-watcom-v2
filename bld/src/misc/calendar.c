#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef __OS2__
 #define INCL_SUB
 #include <os2.h>
#else
 #include <dos.h>
#endif


#define FEBRUARY        1
#define NARROW          3
#define WIDE            4


static int   Jump[ 12 ] =       { 1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6 };
static int   MonthDays[ 12 ] =  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static char  *MonthName[ 12 ] = { "January", "February", "March", "April",
                                  "May", "June", "July", "August",
                                  "September", "October", "November", "December" };
static char  *WideTitle =       { "Sun Mon Tue Wed Thu Fri Sat" };
static char  *NarrowTitle =     { "Su Mo Tu We Th Fr Sa" };


int main( void )
/**************/

{
    time_t                      curr_time;
    register struct tm          *tyme;

    ClearScreen();

    /* get today's date */

    curr_time = time( NULL );
    tyme = localtime( &curr_time );

    /* draw calendar for this month */

    Calendar( tyme->tm_mon, tyme->tm_year, 10, 26, WIDE, WideTitle );

    /* draw calendar for last month */

    tyme->tm_mon--;
    if( tyme->tm_mon < 0 ) {
        tyme->tm_mon = 11;
        tyme->tm_year--;
    }
    Calendar( tyme->tm_mon, tyme->tm_year, 5, 3, NARROW, NarrowTitle );

    /* draw calendar for next month */

    tyme->tm_mon += 2;
    if( tyme->tm_mon > 11 ) {
        tyme->tm_mon -= 12;
        tyme->tm_year++;
    }
    Calendar( tyme->tm_mon, tyme->tm_year, 5, 56, NARROW, NarrowTitle );

    PosCursor( 20, 1 );
    return( 0 );
}


void Calendar( month, year, row, col, width, title )
/**************************************************/

    int                         month;
    int                         year;
    int                         row;
    int                         col;
    int                         width;
    char                        *title;
{
    register int                start;
    register int                days;
    register int                box_width;
    register char               *str;
    register int                i;

    box_width = 7 * width - 1;
    Box( row, col, box_width, 8 );
    str = MonthName[ month ];
    PosCursor( row - 1, col + 1 + ( box_width - strlen( str ) - 5 ) / 2 );
    printf( "%s %d\n", str, 1900 + year );
    fflush( stdout );
    PosCursor( row + 1, col + 1 );
    printf( title );
    fflush( stdout );

    start = year + year / 4 + Jump[ month ];
    if( ( year % 4 == 0 ) && ( month <= FEBRUARY ) ) {
        --start;
    }
    start = start % 7 + 1;
    if( ( year % 4 == 0 ) && ( month == FEBRUARY ) ) {
        days = 29;
    } else {
        days = MonthDays[ month ];
    }
    row += 3;
    for( i = 1; i <= days; ++i ) {
        PosCursor( row, col + width * start - 2 );
        printf( "%2d", i );
        fflush( stdout );
        if( start == 7 ) {
            printf( "\n" );
            fflush( stdout );
            ++row;
            start = 1;
        } else {
            ++start;
        }
    }
}


void Box( row, col, width, height )
/*********************************/

    int                         row;
    int                         col;
    int                         width;
    int                         height;
{
    register int                i;

    Line( row, col, width, 'Ú', 'Ä', '¿' );
    Line( row + 1, col, width, '³', ' ', '³' );
    Line( row + 2, col, width, 'Ã', 'Ä', '´' );
    for( i = 3; i <= height; ++i ) {
        Line( row + i, col, width, '³', ' ', '³' );
    }
    Line( row + height + 1, col, width, 'À', 'Ä', 'Ù' );
}


void Line( row, col, width, left, centre, right )
/***********************************************/

    int                         row;
    int                         col;
    int                         width;
    char                        left;
    char                        centre;
    char                        right;
{
    char                        buffer[ 80 ];

    buffer[ 0 ] = left;
    memset( &buffer[ 1 ], centre, width );
    buffer[ width + 1 ] = right;
    buffer[ width + 2 ] = '\0';
    PosCursor( row, col );
    printf( buffer );
    fflush( stdout );
}


void PosCursor( row, col )
/************************/

    int                         row;
    int                         col;
{
#ifdef __OS2__
    VioSetCurPos( row, col, 0 );
#else
    union REGS                  regs;

    regs.w.dx = ( row << 8 ) + col - 0x0101;
    regs.h.bh = 0;
    regs.h.ah = 2;
#ifdef __386__
    int386( 0x10, &regs, &regs );
#else
    int86( 0x10, &regs, &regs );
#endif
#endif
}


void ClearScreen( void )
/**********************/

{
#ifdef __OS2__
    VioWrtNChar( " ", 2000, 0, 0, 0 );
#else
    union REGS                  regs;

    regs.w.cx = 0;
    regs.w.dx = 0x1850;
    regs.h.bh = 7;
    regs.w.ax = 0x0600;
#ifdef __386__
    int386( 0x10, &regs, &regs );
#else
    int86( 0x10, &regs, &regs );
#endif
#endif
}
