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


#ifndef vwenum_h
#define vwenum_h

#include <wvlist.hpp>
#include <wcombox.hpp>
#include <wpshbttn.hpp>
#include "projmodl.h"
#include "wbrdefs.h"
#include "viewsym.h"

class WCheckBox;
class ObjectPickList;
class WTextBox;
class WListBox;
class WText;
class EnumElement;

class ViewEnum : public ViewSymbol
{
public:
                        ViewEnum();
                        ViewEnum( int x, int y, WBRWindow *parent,
                                  const Symbol *info, bool derived=FALSE );
                        ~ViewEnum();
    static ViewSymbol * create( int x, int y, WBRWindow *parent,
                                const Symbol *info );
protected:
    virtual void        setup();
    virtual void        refresh();

private:
    void                showElements( WWindow * );
    void                loadElements();

// callbacks

    void                selectElement( WObject * );
    void                selectValueStyle( WWindow * );
    void                makeDefault( WWindow * );

    ProjectModel *      _projectModel;
    ObjectPickList *    _elementsList;
    WCheckBox *         _showElements;
    WText *             _elementDesc;
    static bool         _elementsOn;
    WVList              _elements;
    WComboBox *         _comboBoxFormatStyle;
    WPushButton *       _pushbuttonMakeDefault;
};

#endif // vwenum_h
