/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <stddef.h>
#include <string.h>

int variable;
void tester( void ) { variable = 4; }
extern int far * cdecl Ono( int );
static int cdecl Wintermute( char *, int );
static int dowahDitty( char *one, char *two, int three, long four );

/* PharLap reported these bugs */

int a2by2[3][5] = {
{  1,  2,  3,  4,  5 },
{  6,  7,  8,  9, 10 }, /* try printing a2by2[1][2] (8) and a2by2[2][1] (12) */
{ 11, 12, 13, 14, 15 }
};

typedef enum color { RED, BLUE, GREEN } color;

color RGB_1;    /* WOMP dumps this as typeless */
color RGB_2;
color RGB_3;

typedef struct foo_bits {
    char        f1;
    unsigned    f2 : 1;
    unsigned    f3 : 2;
    unsigned    f4 : 3;
    int         f5;
} foo_bits;

foo_bits abits = { 0, 1, 2, 3, 4 };     /* all report 0! */

/* end of PharLap bugs */


typedef struct {
    char    yo;
    int     ho;
    long    mo;
} Neuromancer;

typedef struct {
    char    a : 1;
    int     b : 5;
} Flower;

typedef union {
    long    l;
    short   s;
} Wind;

#pragma aux Power_Calloc value struct struct caller;
extern Flower Power_Calloc( Wind );
#pragma aux Power_Ralloc value struct struct routine;
extern Flower Power_Ralloc( Wind );

extern void main( void ) {
    long    one;
    char *  two;
    int far * three;
    Neuromancer   four;
    Flower  five;
    Wind six;
    static char seven[77];

    printf( "this is a program to test\n" );
    printf( "how the linker handles simple situations.\n");
    tester();
    {
        char  gibson;

        gibson = 4;
        Ono( gibson );
    }
    if( variable == 4 ) {
        int simstim;

        simstim = 3;
        Ono( simstim ); {
            long cyberspace;
            cyberspace = 5;
            Ono( (int)cyberspace ); }
    }
    two = "five";
    one = 1;
    three = Ono( (int)one );
    four.ho = Wintermute( two, 12 );
    five.a = 1;
    five.b = -one;
    six.s = -five.b;
    six.l = 0x12345L;
    dowahDitty( "hi", "there", 5, 6 );
    five = Power_Calloc( six );
    five = Power_Ralloc( six );
    strcpy( seven, two );
    printf( "variable has a value of %d\n", variable );
}

static unsigned long    Molly;

static int cdecl Wintermute( char *tessier, int dude )
/****************************************************/
{
    int     destroyer;

    dude=dude;
    destroyer = 3;
    printf( "ashpool says %s \n", tessier );
    return( 0 );
}

extern int far * cdecl Ono( int Sendai )
/*********************************/
{
    Sendai = Sendai;
    return( 0 );
}

static int dowahDitty( char *one, char *two, int three, long four )
{
    one = one;
    two = two;
    three = four >> 2;
    four = three << 2;
    return( four + three );
}

Flower Power_Calloc( Wind w ) {

    Flower x;

    x.b = w.l;
    x.a = w.s;
    return( x );
}

Flower Power_Ralloc( Wind w ) {

    Flower x;

    x.b = w.l;
    x.a = w.s;
    return( x );
}

