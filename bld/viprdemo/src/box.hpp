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
* Description:  Low level classes for 3D objects.
*
****************************************************************************/


// For our purposes, view the world down the Z axis
//
//      Y
//      |
//      |
//      |
//      *------X
//    /
//  /
//Z
//
// So other words used for clarity are 'across', 'height' and 'depth'

extern "C" void draw_box( float x1, float x2, float y1, float y2,
                          float z1, float z2,
                          unsigned red, unsigned green, unsigned blue );

class point {
private:
    static float        world_scale;
    static float        vertical_shift;

protected:
    float       x;
    float       y;
    float       z;

public:
    enum orientation {
        FRONT = 0, RIGHT, BACK, LEFT }; // anticlockwise
    point( float x0, float y0, float z0 );
    point( point& src );
    // Define the world
    static void world_size( float wide, float deep, float high );
    // Shifts
    void moveto( float x0, float y0, float z0 );
    void moveto( point& origin );
    void shift( float x0, float y0, float z0 );
    void shift( point& vector );
    void move_left( float amount );
    void move_right( float amount );
    void move_up( float amount );
    void move_down( float amount );
    void move_forward( float amount );
    void move_back( float amount );

    // Rotates -- only about Y axis, only by 90 degree increments
    void rotate( orientation );

                                friend class box;
};

class color {
protected:
    unsigned char       r, g, b;
public:
    color(unsigned char red,unsigned char green,unsigned char blue);
    color(const color& src);
    color();
    void rgb(unsigned char r, unsigned char g, unsigned char b);
    void rgb(color& src);
    void rgb(color* src);
    void red( unsigned char intensity = 255 );
    void green( unsigned char intensity = 255 );
    void blue( unsigned char intensity = 255 );
};

class box : public point, public color {

private:
    point       size_vector;
    orientation facing;
    box *       children;
    box *       sibling;

public:
    box( float across, float depth, float height );
    ~box();
    void        draw( point& origin, orientation face);
    void        add(box* child);

    // Dimensions
    float width();
    float height();
    float depth();
    // Change rgb of entire piece
    void change_color( color newclr );
    // Change orientation
    void orient( orientation rotation );
    void orient( box * other );
    // Assign relative position
    void left( float x0 );
    void right( float x0 );
    void front( float z0 );
    void back( float z0 );
    void top( float y0 );
    void bottom( float y0 );

    void position_to_right_of( box& to_left );

    // Position -- notice names are overloaded
    float left();
    float right();
    float top();
    float bottom();
    float front();
    float back();
    float mid_width();
    float mid_height();
    float mid_depth();

};
