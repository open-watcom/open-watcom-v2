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
* Description:  Draw kitchen furniture.
*
****************************************************************************/


#include "furnitu.hpp"

void cupboard::build( unsigned pos )
{
    // Assume 1" for door, 1" for handle
    box * body = new box( width(), depth() - 2, height() );
    // Same back left corner as whole cupboard
    // Try to get brown
    body->rgb( 255, 255, 0 );
    add( body );
    // Make door 1" smaller than body on all four sides
    box * door = new box( width() - 2, 1, height() - 2 );
    door->bottom( 1 );
    door->left( 1 );
    door->back( body->front() );
    door->rgb( body );
    add( door );
    box * handle = new box( 2, 1, 2 );
    handle->back( door->front() );
    handle->rgb( 64, 64, 64 );        // grey?
    if( pos & TOP ) {
        handle->top( door->top() - 3 );
    } else if( pos & BOTTOM ) {
        handle->bottom( door->bottom() + 3 );
    } else {
        handle->top( door->mid_height() + handle->width() / 2 );
    }
    if( pos & LEFT ) {
        handle->left( door->left() + 2 );
    } else if( pos & RIGHT ) {
        handle->right( door->right() - 2 );
    } else {
        handle->left( door->mid_width() );
        handle->move_right( handle->width() / 2 );
    }
    add( handle );
}

cupboard::cupboard( float width, float depth, float height, unsigned pos )
    : box( width, depth, height )
{
    build( pos );
}

cupboard::cupboard( cupboard& to_left, unsigned pos )
    : box( to_left.width(), to_left.depth(), to_left.height() )
{
    // Build a same-sized cupboard to the right of existing cupboard to_left
    build( pos );
    // Position in same place & same orientation as cupboard to_left
    position_to_right_of( to_left );
}

void fridge::make_fridge( handle_position pos )
{
    // Assume 2" for door, 2" for handle
    box * body = new box( width(), depth() - 4, height() );
    // Same back left corner as whole cupboard
    // Use a white fridge
    body->rgb( 255, 255, 255 );
    add( body );
    // Bottom door first
    box * door = new box( width(), 2, height() * .67 );
    door->back( body->front() );
    door->rgb( body );
    add( door );
    box * handle = new box( 2, 2, door->height() - 4 );
    handle->bottom( door->bottom() + 2 );
    handle->back( door->front() );
    handle->rgb( 64, 64, 64 );        // grey?
    switch( pos ) {
    case LEFT:
        handle->left( door->left() + 2 );
        break;
    case RIGHT:
        handle->right( door->right() - 2 );
        break;
    }
    add( handle );
    // Add freezer door
    door = new box( width(), 2, height() - door->height() );
    door->rgb( body );
    door->back( body->front() );
    door->top( top() );
    add( door );
    handle = new box( 2, 2, door->height() - 4 );
    handle->bottom( door->bottom() + 2 );
    handle->back( door->front() );
    handle->rgb( 64, 64, 64 );        // grey?
    switch( pos ) {
    case LEFT:
        handle->left( door->left() + 2 );
        break;
    case RIGHT:
        handle->right( door->right() - 2 );
        break;
    }
    add( handle );
}

fridge::fridge( float width, float depth, float height, handle_position pos )
    : box( width, depth, height )
{
    make_fridge( pos );
}

void table::top_and_four_legs( float leg_width )
{
    box * tabletop = new box( width(), depth(), 4 );
    tabletop->top( top() );
    tabletop->rgb( 0, 255, 255);
    add( tabletop );
    for( int i = 0; i < 4; ++ i ) {
        box * leg = new box( leg_width, leg_width, height() - tabletop->height() );
        leg->rgb( 192, 192, 192 );    // silver?
        leg->bottom( 0 );
        if( i < 2 ) {
            leg->left( left() );
        } else {
            leg->right( right() );
        }
        if( i & 1 ) {
            leg->front( front() );
        } else {
            leg->back( back() );
        }
        add( leg );
    }
}

table::table( float width, float depth, float height, float leg_width )
    : box( width, depth, height )
{
    top_and_four_legs( leg_width );
}

chair::chair( float chwidth, float chdepth, float chheight )
    : box( chwidth, chdepth, chheight )
{
    table * seat = new table( width(), depth(), height() / 2, 2 );
    add( seat );
    box * chair_back = new box( width(), 2, height() - seat->height() );
    chair_back->bottom( seat->top() );
    add( chair_back );
    change_color( color( 128, 128, 0 ) );
}
