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


#include <time.h>
#include <iostream.h>

#define STEP    (1024*1024)
#define V0 -0.3
#define V1 -0.6
#define V2 -0.9
#define SUM 1e27

struct vector {
    float v[3];
    ~vector() {}
    vector( float x ) {
        v[0] = v[1] = v[2] = x;
    }
    vector( float *p ) {
        v[0] = p[0];
        v[1] = p[1];
        v[2] = p[2];
    }
    vector( float x, float y, float z ) {
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }
    vector( vector const &d ) {
        v[0] = d.v[0];
        v[1] = d.v[1];
        v[2] = d.v[2];
    }
    vector & operator =( vector const &d ) {
        v[0] = d.v[0];
        v[1] = d.v[1];
        v[2] = d.v[2];
        return *this;
    }
    vector operator *( const vector &d )
    {
        vector res(*this);
        res *= d;
        return res;
    }
    vector & operator *=( const vector &d )
    {
        v[0] *= d.v[0];
        v[1] *= d.v[1];
        v[2] *= d.v[2];
        return *this;
    }
    vector operator +( const vector &d )
    {
        vector res(*this);
        res += d;
        return res;
    }
    vector & operator +=( const vector &d )
    {
        v[0] += d.v[0];
        v[1] += d.v[1];
        v[2] += d.v[2];
        return *this;
    }
};

vector in_reg()
{
    float v0=V0, v1=V1, v2=V2;
    float sum0=SUM, sum1=SUM, sum2=SUM;
    for( int i = 0; i < STEP; ++i ) {
        sum0 += v0 * sum0 + v0;
        sum1 += v1 * sum1 + v1;
        sum2 += v2 * sum2 + v2;
    }
    return vector( sum0, sum1, sum2 );
}

vector in_arr()
{
    float v[3] = { V0, V1, V2 };
    float sum[3] = { SUM, SUM, SUM };
    for( int i = 0; i < STEP; ++i ) {
        sum[0] += v[0] * sum[0] + v[0];
        sum[1] += v[1] * sum[1] + v[1];
        sum[2] += v[2] * sum[2] + v[2];
    }
    return vector( sum );
}

vector in_vec()
{
    vector v(V0,V1,V2);
    vector sum(SUM);
    for( int i = 0; i < STEP; ++i ) {
        sum += v * sum + v;
    }
    return sum;
}

void timer( vector ( *rtn )() )
{
    clock_t start;
    clock_t stop;

    start = clock();
    rtn();
    stop = clock();
    cout << stop - start << endl;
}

void main()
{
    timer( in_reg );
    timer( in_arr );
    timer( in_vec );
}
