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


#ifndef __GTDRAWOP_H__
#define __GTDRAWOP_H__

#include <wcolrdlg.hpp>
#include <wtptlist.hpp>
#include <wcolour.hpp>
#include <wlines.hpp>

#include "gtoption.h"

class WPushButton;
class WCheckBox;
class OutputDevice;

class TreeDrawOptions : public WDialog
{
public:
            TreeDrawOptions( WWindow * parent, TreeOptionCollection * coll,
                             int x, int y );
            ~TreeDrawOptions();

    virtual void        initialize( void );
            bool        paint();
            void        prepare( void );


            class Option;
            class OptionGroup;

            typedef TemplateList<Option*>       OptionList;
            typedef TemplateList<OptionGroup*>  GroupList;

            void            checkNoContend( uint_16 uses,
                                            TreeGroupType type,
                                            bool state );
            void            applyChanges( void );

            class Option : public WObject
            {
            public:
                    Option( OptionGroup * parentGrp, int y, TreeOption * opt );

                    ~Option();

                    void            paint( OutputDevice * );
                    void            getRect( WRect & r ) const;
                    void            enableSet( bool enable = TRUE );
                    void            applyChanges( void );
                    void            makeDefault( void );
                    void            useDefault( void );

                    char *          name( void ) { return _clone->name(); }

            private:
                    void            setSelected( WWindow * );

                    Color           _colour;
                    WLineStyle      _penStyle;
                    int             _penThickness;

                    OptionGroup *   _parentGroup;
                    int             _y;
                    WPushButton *   _set;

                    TreeOption *    _clone;
            };

            class OptionGroup : public WObject
            {
            public:
                    OptionGroup( TreeDrawOptions * parent, int y,
                                 TreeOptionGroup * clone );
                    ~OptionGroup();

                    void        paint( OutputDevice * );
                    void        getRect( WRect & r ) const { r = _rect; }

                    void        addOption( TreeOption * );
                    TreeDrawOptions * getParent( void ) const { return _parent; }
                    TreeGroupType     getType( void ) const { return _clone->getType(); }
                    uint_16     getUses( void ) const { return _uses; }
                    void        enableBox( uint_8 uses, bool state );
                    void        applyChanges( void );
                    void        makeDefault( void );
                    void        useDefault( void );

                    char *      name( void ) { return _clone->name(); }

            private:
                    void        boxSelected( WWindow * box );

                    TreeDrawOptions *   _parent;
                    TreeOptionGroup *   _clone;

                    WRect               _rect;
                    int                 _nextY;
                    uint_16             _uses;

                    OptionList          _options;

                    WCheckBox *         _colour;
                    WCheckBox *         _style;
                    WCheckBox *         _thickness;
            };

protected:
            void            drawButtons( void );
private:
            OptionGroup *   addGroup( TreeOptionGroup * group );
            void    defaultButton( WWindow * );
            void    makeDefaultButton( WWindow * );
            void    okButton( WWindow * );
            void    cancelButton( WWindow * );

            GroupList   _groups;
            WRect       _rect;
            int         _nextY;
            TreeOptionCollection *_coll;
};

#endif // __GTDRAWOP_H__
