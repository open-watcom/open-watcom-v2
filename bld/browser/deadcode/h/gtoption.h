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


#ifndef __GTOPTION_H__
#define __GTOPTION_H__

#include <wcolour.hpp>
#include <wlines.hpp>
#include <wtptlist.hpp>     // TemplateList

enum TreeGroupType { TreeGroupLine, TreeGroupNode };
enum TreeGroupUses {
        TreeGroupNone       = 0x00,
        TreeGroupColour     = 0x01,
        TreeGroupStyle      = 0x02,
        TreeGroupThickness  = 0x04
};

class TreeOption;
class TreeOptionGroup;
class TreeOptionCollection;
class ProjectModel;

typedef TemplateList<TreeOptionGroup *> TreeGroupList;
typedef TemplateList<TreeOption *> TreeOptionList;

/////////////////////////// TreeOptionCollection ///////////////////////

class TreeOptionCollection
{
public:
            TreeOptionCollection(){}
    virtual ~TreeOptionCollection(){}

            void                makeDefault( void ) const;
            void                useDefault( void );
    virtual int                 numGroups( void ) const = 0;
    virtual char *              name( void ) const = 0;
    virtual TreeOptionGroup *   getGroup( int index ) const = 0;

            uint_16             getUses( int grp );
            Color               getColour( int grp, int ix );
            WLineStyle          getStyle( int grp, int ix );
            int                 getThickness( int grp, int ix );
};

///////////////////////// TreeOptionGroup ////////////////////////////

class TreeOptionGroup
{
public:
            TreeOptionGroup( char * name, TreeGroupType type,
                             TreeGroupUses uses );
            TreeOptionGroup( TreeOptionGroup * def );
            ~TreeOptionGroup();

            int             addOption( TreeOption * def );
            int             addOption( const TreeOption & opt,
                                       TreeOption * defaultOpt );
            int             addOption( char * name, Color clr,
                                       WLineStyle stl, int thk );
            void            readOption( int index, TreeOption & opt );

            TreeGroupType   getType( void ) const { return _type; }
            uint_16         getUses( void ) const { return _uses; }
            TreeOption *    getOption( int index ) { return _options[ index ]; }
            void            setUses( uint_16 u ) { _uses = u; }
            int             numOpts( void ) { return _options.count(); }
            char *          name( void ) const { return _name; }

            void            makeDefault( void );
            void            useDefault( void );
            TreeOptionGroup *   getDefault( void ) { return _default; }

private:
            char *          _name;
            TreeGroupType   _type;
            uint_16         _uses;
            TreeOptionList  _options;

            TreeOptionGroup *  _default;
};

///////////////////////////// TreeOption ////////////////////////////////

class TreeOption
{
public:
            TreeOption( char * name, Color clr, WLineStyle stl, int thk );
            TreeOption( TreeOption * def );
            TreeOption( TreeOption const & clone, TreeOption * def );
            ~TreeOption() {}

            void            setOption( const TreeOption & opt );
            void            setOption( Color, WLineStyle, int );
            void            readOption( TreeOption & opt ) const;

            Color           getColour( void ) const { return _colour; }
            WLineStyle      getStyle( void ) const { return _penStyle; }
            int             getThickness( void ) const { return _penThickness; }
            char *          name( void ) const { return _name; }

            void            makeDefault( void ) const;
            void            useDefault( void );
            TreeOption *    getDefault( void ) { return _default; }
protected:
            char *          _name;
            Color           _colour;
            WLineStyle      _penStyle;
            int             _penThickness;

            TreeOption *    _default;
};


#endif // __GTOPTION_H__
