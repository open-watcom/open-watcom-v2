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


#include "vcompdlg.hpp"

#include "wpshbttn.hpp"
#include "wtext.hpp"
#include "weditbox.hpp"
#include "wpickcbx.hpp"
#include "wmsgdlg.hpp"
#include "wmetrics.hpp"

#include "mconfig.hpp"
#include "mproject.hpp"
#include "mcompon.hpp"
#include "mrule.hpp"
#include "mtarget.hpp"
#include "wfiledlg.hpp"
#include "wfilenam.hpp"
#include "wradbttn.hpp"
#include "wgroupbx.hpp"

extern char _viperError[];


VCompDialog::VCompDialog( WWindow* parent, const char* text, MProject* project, const char* filter )
    : WDialog( parent, WRect( 100, 50, 400, 150 ), text )
    , _project( project )
{
    _browseDialog = new WFileDialog( this, filter );
}

void VCompDialog::initialize()
{

    int         sx;
    int         sy;
    WPoint      average;
    WPoint      max;

    const int   rb_hite = 10;  //height of a radio button
    const int   space = 10;
    const int   button_width = 50;
    const int   button_hite = 14;

    this->textMetrics( average, max );
    sx = average.x() / 4;
    sy = average.y() / 8;

    WText* t1 = new WText( this, WRect( 7*sx, 2*sy, 164*sx, 9*sy ),
                            "Target name:" );
    t1->show();

    _eName = new WEditBox( this, WRect( 7*sx, 12*sy, 250*sx, 13*sy ), *_fn );
    _eName->select();
    _eName->show();

    WPushButton* bBrowse = new WPushButton( this,
                WRect( 265*sx, 11*sy, button_width * sx, button_hite * sy ),
                "&Browse..." );
    bBrowse->onClick( this, (cbw)&VCompDialog::browseButton );
    bBrowse->show();

    WText* t2 = new WText( this, WRect( 167*sx, 31*sy, 86*sx, 9*sy ),
                            "Image Type:" );
    t2->show();

    WRadioButton        *button;
    WRadioButton        *checkedbutton;
    int                 icount;
    WStyle              style;
    MTarget             *curtarg;
    int                 i;
    bool                singletarg;

    curtarg = NULL;
    if( _comp != NULL ) {
        WPickList& targets = _config->targets();
        icount = targets.count();
        for( i=0; i < icount; i++ ) {
            curtarg = (MTarget *)targets[i];
            if( curtarg->mask() == _comp->mask() ) break;
        }
        if( i == icount ) curtarg = NULL;
    }

    WVList &list = _config->targetOSs();
    icount = list.count();
    if( icount == 1 ) {
        // use a static field instead of a radio button when there is
        // only one target installed
        singletarg = TRUE;
        i = 0;
        _cur_os = *(WString *)list[0];
        WText* t3 = new WText( this, WRect( 11*sx, 41*sy, 135*sx, rb_hite * sy ),
                            _cur_os.gets() );
        t3->show();
    } else {
        singletarg = FALSE;
        for( i=0; i < icount; i ++ ) {
            if( i == 0 ) {
                if( icount == 1 ) {
                    style = RStyleRadioButton;
                } else {
                    style = RStyleGroupFirst;
                }
            } else if( i == icount - 1 ) {
                style = RStyleGroupLast;
            } else {
                style = RStyleRadioButton;
            }
            button = new WRadioButton( this,
                   WRect( 11*sx, ( 41 + rb_hite * i ) * sy, 135*sx, rb_hite * sy ),
                   ( (WString *)list[i] )->gets(), style );
            button->show();
            button->onClick( this, (cbw)&VCompDialog::osButton );

            if( curtarg == NULL ) {
                if( i == 0 ) {
                    checkedbutton = button;
                }
            } else {
                if( curtarg->targetOS() == *(WString *)list[i] ) {
                    checkedbutton = button;
                }
            }
        }
    }

    WGroupBox           *gbox;

    if( i < 5 ) i = 5;
    int boxtop = 29;
    int boxhite = i * rb_hite + 20;
    int boxbottom = boxtop + boxhite;
    gbox = new WGroupBox( this,
                          WRect( 7*sx, boxtop * sy, 146*sx, boxhite * sy ),
                          "Target Environment" );
    gbox->show();


    _imagelist = new WListBox( this,
                 WRect( 166*sx, 44*sy, 150*sx, ( boxbottom - 44 ) * sy ),
                 LStyleNoIntegral );
    _imagelist->onDblClick( this, (cbw)&VCompDialog::okButton );
    _imagelist->show();


    WDefPushButton* bOk;
    boxbottom += space;
    bOk = new WDefPushButton( this,
          WRect( 105*sx, boxbottom * sy, button_width * sx, button_hite * sy ),
          "&OK" );
    bOk->onClick( this, (cbw)&VCompDialog::okButton );
    bOk->show();

    WPushButton* bCancel;
    bCancel = new WPushButton( this,
           WRect( 186*sx, boxbottom * sy, button_width * sx, button_hite * sy ),
           "&Cancel" );
    bCancel->onClick( this, (cbw)&VCompDialog::cancelButton );
    bCancel->show();

    if( singletarg ) {
        updateCurOs();
    } else {
        checkedbutton->setCheck( TRUE );
#ifdef __OS2__
    // this is a kludge until GUI starts calling our callback function
    // when the check state is set under OS/2
    osButton( checkedbutton );
#endif
    }
    if( curtarg != NULL ) {
        icount = _tgList.count();
        MTarget         *tmptarg;
        for( i=0; i < icount; i++ ) {
            tmptarg = (MTarget *)_tgList[i];
            if( curtarg->imageName() == tmptarg->imageName() ) {
                _imagelist->select( i );
                break;
            }
        }
    }
    int xsize = 330 * sx;
    int ysize = WSystemMetrics::dialogFrameWidth() * 2
                + WSystemMetrics::captionSize()
                + ( boxbottom + button_hite + space ) * sy;
    size( (WOrdinal)xsize, (WOrdinal) ysize );
    centre();
    show();
    _eName->setFocus();
}

VCompDialog::~VCompDialog()
{
    delete _browseDialog;
}

bool VCompDialog::legalExt()
{
    MTarget             *curtarg;
    int i = _imagelist->selected();
    if( i >= 0 ) {
        curtarg = (MTarget *)_tgList[i];
        WString ext1( curtarg->tgtMask().ext() );
        WString ext2( _fn->ext() );
        if( ext2.size() == 0 ) {
            _fn->setExt( ext1 );
            return TRUE;
        }
        if( ext1 == ext2 ) {
            return TRUE;
        }
        WString n; curtarg->name( n );
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "'%s' is not a legal file extension for '%s'", (const char*)ext2, (const char*)n );
    } else {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Select an image type." );
    }
    return FALSE;
}

bool VCompDialog::findRule()
{
    int i = _imagelist->selected();
    if( i >= 0 ) {
        MTarget* tg = (MTarget*)_tgList[i];
        *_mask = tg->mask();
        *_rulep = _config->findMatchingRule( *_fn, *_mask );
        return TRUE;
    }
    return FALSE;
}

bool VCompDialog::checkName()
{
    WFileName fn( *_fn );
    fn.absoluteTo( _project->filename() );
    int icount = _project->components().count();
    for( int i=0; i<icount; i++ ) {
        MComponent* m = (MComponent*)_project->components()[i];
        if( m != _comp ) {
            WFileName cn;
            m->target()->absName( cn );
            if( fn.match( cn, matchDrive|matchDir ) ) {
                if( fn.match( cn, matchFName ) ) {
                    WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                        "Targets '%s' and '%s' have the same name and must be in different directories.", (const char*)*_fn, (const char*)*m->target() );
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

void VCompDialog::updateCurOs( void )
{
    MTarget     *curtarg;
    int         pos;
    WString     curname;

    WPickList&  targets = _config->targets();
    int icount = _imagelist->count();
    for( int i=0; i < icount; i++ ) {
        _imagelist->deleteString( 0 );
    }
    pos = 0;
    icount = targets.count();
    _tgList.reset();
    for( i=0; i< icount; i++ ) {
        curtarg = (MTarget *)targets[i];
        if( _cur_os == curtarg->targetOS() ) {
            curname.printf( "%s [%s]", curtarg->imageName().gets(),
                                       curtarg->tgtMask().ext() );
            _imagelist->insertString( curname.gets(), pos );
            _tgList.insertAt( pos, curtarg );
            pos++;
        }
    }
}

void VCompDialog::osButton( WWindow *item )
{

    item->getText( _cur_os );
    updateCurOs();
}

void VCompDialog::okButton( WWindow* )
{
    _eName->getText( *_fn );
    _fn->toLower();
    if( _fn->isMask() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "'%s' cannot be a wildcard", (const char*)*_fn );
    } else if( !_fn->legal() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "'%s' is not a legal filename", (const char*)*_fn );
    } else if( !checkName() ) {
        //error issued by checkName()
    } else if( streq( _fn->ext(), ".tgt" ) ) {
        quit( TRUE );
    } else if( !legalExt() ) {
        //error issued by legalExt()
    } else if( !findRule() ) {
        //this will work since extension has been verified
    } else {
        quit( TRUE );
    }
}

void VCompDialog::cancelButton( WWindow* )
{
    quit( FALSE );
}

void VCompDialog::browseButton( WWindow* )
{
    WWindow *old = WWindow::hasFocus();
//    NYI - let the user enter lists of targets to add
//    WFileNameList     fn_list;
//    if( _browseDialog->getOpenFileName( fn_list, "", "Enter target filename", WFOpenNew ) ) {
//      WFileName fn( fn_list.cString( 0, -1 ) );
    WFileName fn;
    fn = _browseDialog->getOpenFileName( NULL, "Enter target filename",
                                         WFOpenNew );
        if( fn.size() > 0 ) {
            WFileName cwd; cwd.getCWD( TRUE );
            int len = cwd.size();
            if( len > 0 ) {
                if( strnicmp( cwd, fn, len ) == 0 ) {
                    fn.chop( len );
                }
            }
            _eName->setText( fn );
        }
    old->setFocus();
}

bool VCompDialog::process( WFileName& fn, MRule** rulep, WString& mask, MComponent* comp )
{
    _fn = &fn;
    _rulep = rulep;
    _mask = &mask;
    _comp = comp;

    WVList &list = _config->targetOSs();
    if( list.count() == 0 ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "No targets are currently installed" );
        return( FALSE );
    }
    return WDialog::process() == TRUE;
}
