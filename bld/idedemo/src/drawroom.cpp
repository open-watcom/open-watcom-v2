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
* Description:  Draw the room.
*
****************************************************************************/


#include "furnitu.hpp"
#include "room.h"

static box *    Room = 0;

static box * make_room( float wide, float deep, float high )
{
    box * room = new box( wide * 12, deep * 12, high * 12 );
    room->world_size( room->width(), room->depth(), room->height() );
    // Draw some walls and a floor
    box * floor = new box( room->width(), room->depth(), 1 );
    floor->top( 0 );
    floor->rgb( 192, 192, 192 );
    room->add( floor );
    box * wall = new box( 4, room->depth(), room->height() );
    wall->blue( 128 );
    wall->right( 0 );
    room->add( wall );
    wall = new box( room->width(), 4, room->height() );
    wall->front( 0 );
    wall->blue( 128 );
    room->add( wall );
    return( room );
}

static chair * add_chair()
{
    return new chair( 16, 16, 40 );
}

static void table_and_chairs()
{
    table * tab = new table( 32, 70, 32, 4 );
    tab->right( Room->right() - 12 );
    tab->front( Room->front() - 12 );
    Room->add( tab );
    chair * ch = add_chair();
    ch->orient( point::FRONT );
    ch->left( tab->mid_width() - ch->width() / 2 );
    ch->back( tab->back() - 12 );
    Room->add( ch );
    ch = add_chair();
    ch->orient( point::RIGHT );
    ch->left( tab->left() - 12 );
    ch->back( tab->mid_depth() + 12 );
    Room->add( ch );
#if 0
    ch = add_chair();
    ch->orient( point::BACK );
    ch->left( tab->mid_width() + ch->width() / 2 );
    ch->back( Room->front() );
    Room->add( ch );
    ch = add_chair();
    ch->orient( point::LEFT );
    ch->left( Room->right() );
    ch->back( tab->mid_depth() - 12 );
    Room->add( ch );
#endif
}

static box * set_of_cupboards(
    int         num,
    float       total_width,
    float       depth,
    float       height,
    unsigned    handle_pos )
{
    box * set = new box( total_width, depth, height );
           cupboard * c = new cupboard( total_width / num, depth, height,
                                        handle_pos + cupboard::RIGHT );
    set->add( c );
    for( int i = 1; i < num; ++ i ) {
        c = new cupboard( *c, handle_pos +
                                (i & 1 ? cupboard::LEFT : cupboard::RIGHT ) );
        set->add( c );
    }
    return set;
}

void define_room()
{
    Room = make_room( 12, 12, 8 );
    fridge * frig = new fridge( 36, 30, 66, fridge::LEFT );
    frig->orient( point::RIGHT );
    frig->back( frig->width() );
    Room->add( frig );
    // Put cupboards on back wall
    box * pantry = set_of_cupboards( 2, 32, 16, Room->height(), cupboard::MIDDLE );
    pantry->orient( point::FRONT );
    pantry->right( Room->right() );
    pantry->bottom( 0 );
    pantry->back( Room->back() );
    Room->add( pantry );
    // Leave enough room for fridge & door to open
    float set_width = Room->width() - frig->width() - frig->depth()
                        - pantry->width();
    box * set = set_of_cupboards( 4, set_width, 12, 36, cupboard::BOTTOM );
    set->orient( point::FRONT );
    set->right( pantry->left() );
    set->top( Room->top() );
    set->back( Room->back() );
    Room->add( set );
    // Add some along the left wall
    set = set_of_cupboards( 6, Room->depth() * 2/3, 12, 36, cupboard::BOTTOM );
    set->orient( point::RIGHT );
    set->top( Room->top() );
    set->back( Room->front() );
    set->left( 0 );
    Room->add( set );
    box * set2 = set_of_cupboards( 3, Room->depth() / 3, 12, 24, cupboard::BOTTOM );
    set2->position_to_right_of( *set );
    set2->top( Room->top() );
    Room->add( set2 );
    // Now add some lower cupboards
    box * lower = set_of_cupboards( 6, set->width(), 24, 36, cupboard::TOP );
    lower->orient( set );
    lower->bottom( 0 );
    lower->left( 0 );
    lower->back( Room->front() );
    Room->add( lower );
    // And add a countertop
    box * countertop = new box( lower->width(), lower->depth() + 1, 2 );
    countertop->red();
    countertop->orient( lower );
    countertop->bottom( lower->top() );
    countertop->left( lower->left() );
    countertop->back( lower->back() );
    Room->add( countertop );
    table_and_chairs();
}

void finish_room()
{
    delete Room;
    Room = 0;
}

void draw_room()
{
    point origin(0,0,0);
    Room->draw( origin, point::FRONT );
}
