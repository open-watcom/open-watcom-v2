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


#include "box.hpp"

static float point::world_scale = 12 * 12;      // ie 12 feet
static float point::vertical_shift = 0;

// Functions which could be inlined
point::point( float x0, float y0, float z0 ) : x(x0), y(y0), z(z0) { }
point::point( point& src ) : x(src.x), y(src.y), z(src.z) { }
void point::move_left( float amount ) { x -= amount; }
void point::move_right( float amount ) { x += amount; }
void point::move_up( float amount ) { y += amount; }
void point::move_down( float amount ) { y -= amount; }
void point::move_forward( float amount ) { z -= amount; }
void point::move_back( float amount ) { z += amount; }

color::color(unsigned char red,unsigned char green,unsigned char blue)
                : r(red), g(green), b(blue) { }
color::color(const color& src) : r(src.r), g(src.g), b(src.b) { }
color::color() : r(0), g(0), b(0) { }
void color::rgb(unsigned char red, unsigned char green, unsigned char blue)
                { r = red; g = green; b = blue; }
void color::rgb(color& src) { rgb( src.r, src.g, src.b); }
void color::rgb(color* src) { rgb( src->r, src->g, src->b); }
void color::red( unsigned char intensity ) { r = intensity; }
void color::green( unsigned char intensity ) { g = intensity; }
void color::blue( unsigned char intensity ) { b = intensity; }

float box::width() { return size_vector.x; }
float box::height() { return size_vector.y; }
float box::depth() { return size_vector.z; }
void box::left( float x0 ) { x = x0; }
void box::right( float x0 ) { x = x0 - size_vector.x; }
void box::front( float z0 ) { z = z0 - size_vector.z; }
void box::back( float z0 ) { z = z0; }
void box::top( float y0 ) { y = y0 - size_vector.y; }
void box::bottom( float y0 ) { y = y0; }
float box::left() { return x; }
float box::right() { return x + size_vector.x; }
float box::top() { return y + size_vector.y; }
float box::bottom() { return y; }
float box::front() { return( z + size_vector.z ); }
float box::back() { return z; }
float box::mid_width() { return x + size_vector.x / 2; }
float box::mid_height() { return y + size_vector.y / 2; }
float box::mid_depth() { return z + size_vector.z / 2; }


void point::rotate( orientation move )
{
    float       temp;

    switch( move % 4 ) {
    case FRONT:
        // nothing to do
        break;
    case RIGHT:
        temp = x;
        x = z;
        z = -temp;
        break;
    case BACK:
        x = -x;
        z = -z;
        break;
    case LEFT:
        temp = x;
        x = -z;
        z = temp;
        break;
    }
}

void point::moveto( float x0, float y0, float z0 )
{
    x = x0;
    y = y0;
    z = z0;
}

void point::moveto( point& origin )
{
    x = origin.x;
    y = origin.y;
    z = origin.z;
}

void point::shift( float xdelta, float ydelta, float zdelta )
{
    x += xdelta;
    y += ydelta;
    z += zdelta;
}

void point::shift( point& vector )
{
    shift( vector.x, vector.y, vector.z );
}

box::box( float across, float depth, float height )
    : point(0,0,0), size_vector(across, height, depth), facing(FRONT)
{
    children = 0;
    sibling = 0;
}

box::~box()
{
    delete children;
    delete sibling;
}

static void point::world_size( float wide, float deep, float high )
{
    // To keep proportions correct, just remember the largest of the dimensions

    world_scale = wide;
    if( world_scale < deep ) {
        world_scale = deep;
    }
    if( world_scale < high ) {
        world_scale = high;
        vertical_shift = 0;
    } else {
        vertical_shift = (world_scale - high) / 2;
    }
}

void box::add(box* child)
{
    child->sibling = children;
    children = child;
}

void box::orient( orientation rotation )
{
    facing = (orientation) ((facing + rotation) % 4);
}

void box::orient( box * other )
{
    orient( other->facing );
}

void box::position_to_right_of( box& to_left )
{
    // Move to same position & facing as to_left
    moveto( to_left );
    orient( to_left.facing );
    // Now move across by the width of to_left
    point shift_vector( to_left.width(), 0, 0 );
    shift_vector.rotate( facing );
    shift( shift_vector );
}

void box::change_color( color newclr )
{
    rgb( newclr );
    for( box * curr = children; curr; curr = curr->sibling ) {
        curr->change_color( newclr );
    }
}

void box::draw( point& origin, orientation face )
{
    // First, combine the rotations
    face = (orientation) ((face + facing) % 4);
    if( children ) {
        for( box * curr = children; curr; curr = curr->sibling ) {
            point curr_origin = *curr;
            curr_origin.rotate( face );
            curr_origin.shift( origin );
            curr->draw( curr_origin, face );
        }
    } else {
        // Only draw outer box if there is no children
        point new_size = size_vector;
        new_size.rotate( face );
        point other_corner = origin;
        other_corner.shift( new_size );
        draw_box( origin.x / world_scale, other_corner.x / world_scale,
                  (origin.y + vertical_shift) / world_scale,
                  (other_corner.y + vertical_shift) / world_scale,
                  origin.z / world_scale, other_corner.z / world_scale,
                  r, g, b );
    }
}

