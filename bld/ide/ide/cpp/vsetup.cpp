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


#include "vsetup.hpp"
#include "wpshbttn.hpp"
#include "wradbttn.hpp"
#include "wcheckbx.hpp"
#include "weditbox.hpp"
#include "wtext.hpp"
#include "wcombox.hpp"
#include "wgroupbx.hpp"
#include "mtool.hpp"
#include "mfamily.hpp"
#include "mswitch.hpp"
#include "mrswitch.hpp"
#include "mcswitch.hpp"
#include "mc2swtch.hpp"
#include "mvswitch.hpp"
#include "mstate.hpp"
#include "mrstate.hpp"
#include "mcstate.hpp"
#include "mvstate.hpp"
#include "wmetrics.hpp"

#define TEST

WCLASS SwitchMap : public WObject
{
    public:
#ifdef TEST
        SwitchMap( WControl* ctrl );
#endif
        SwitchMap( MSwitch* sw, WBoolSwitch* ctl, MState* st, WEditBox* ctl2=NULL );
        ~SwitchMap() {}
        MSwitch* sw() { return _switch; }
        WBoolSwitch* ctl() { return _control; }
        WEditBox* ctl2() { return _control2; }
#ifdef TEST
        WControl* ctrl() { return _ctrl; }
#endif
    private:
        MSwitch*        _switch;
        WBoolSwitch*    _control;
        WEditBox*       _control2;
#ifdef TEST
        WControl*       _ctrl;
#endif
};

#ifdef TEST
SwitchMap::SwitchMap( WControl* ctrl )
    : _switch( NULL )
    , _control( NULL )
    , _control2( NULL )
    , _ctrl( ctrl )
{
}
#endif
SwitchMap::SwitchMap( MSwitch* sw, WBoolSwitch* ctl, MState* /*st*/, WEditBox* ctl2 )
    : _switch( sw )
    , _control( ctl )
    , _control2( ctl2 )
#ifdef TEST
    , _ctrl( NULL )
#endif
{
}

VSetup::VSetup( WWindow* parent, MTool* tool, WString& tagMask, WVList* states, const char* text, SwMode mode )
    : WDialog( parent, WRect( 20, 50, 600, 400 ) )
    , _tool( tool )
    , _tagMask( tagMask )
    , _states( states )
    , _mode( mode )
    , _curIndex( -1 )
    , _next( NULL )
    , _prev( NULL )
    , _text( text )
{
}

static unsigned calcControlHite( WVList *list, unsigned index,
                            unsigned editctl_hite, unsigned checkbox_hite,
                            unsigned radiobutton_hite, unsigned text_hite,
                            unsigned groupbox_hite ) {

    MSwitch*    sw;
    unsigned    ret;
    unsigned    cnt;

    sw = (MSwitch*)(*list)[index];
    if( streq( sw->className(), "MRSwitch" ) ) {
        ret = groupbox_hite + radiobutton_hite;
        WString& groupName = ((MRSwitch*)sw)->group();
        index++;
        cnt = list->count();
        for( ; index < cnt ; index++ ) {
            sw = (MSwitch*)(*list)[index];
            if( !streq( sw->className(), "MRSwitch" ) ) break;
            if( !(groupName == ((MRSwitch*)sw)->group()) ) break;
            ret += radiobutton_hite;
        }
    } else if( streq( sw->className(), "MCSwitch" ) ) {
        ret = checkbox_hite;
    } else if( streq( sw->className(), "MC2Switch" ) ) {
        ret = checkbox_hite;
    } else if( streq( sw->className(), "MVSwitch" ) ) {
        if( ((MVSwitch*)sw)->optional() ) {
            ret = checkbox_hite;
        } else {
            ret = text_hite;
        }
        ret += editctl_hite;
    } else {
        ret = 0;
    }
    return( ret );
}

void VSetup::initialize()
{
    static const char ok[] = { "OK" };
    static const char cancel[] = { "Cancel" };
    static const char resetcfg[] = { "Rese&t" };
    static const char resetdev[] = { "Copy&Dev" };
    static const char resetrel[] = { "Copy&Rel" };

    unsigned            editctl_hite;
    unsigned            checkbox_hite;
    unsigned            radiobutton_hite;
    unsigned            groupbox_hite;
    WPoint              avgCh;
    WPoint              maxCh;
    WRect               sc;
    int                 frame_width;
    int                 frame_hite;
    int                 text_hite;
    int                 button_width;
    int                 button_hite;
    int                 bxoff;
    int                 byoff;
    int                 yy;
    WString             n;
    int                 thoff;
    int                 maxw;
    int                 maxh;
    WVList              famList;
    int                 k;
    int                 icount;
    setSystemFont( FALSE );
#ifndef TEST
    show();
#endif
    WSystemMetrics::screenCoordinates( sc );
    size( (WOrdinal)((long)sc.w()*4/5), (WOrdinal)((long)sc.h()) );
    textMetrics( avgCh, maxCh );

    frame_width = WSystemMetrics::dialogFrameWidth();
    frame_hite = WSystemMetrics::dialogFrameHeight();
    text_hite = maxCh.y() * 5/4;

    editctl_hite = text_hite + avgCh.y() / 2;
    checkbox_hite = text_hite;
    radiobutton_hite = text_hite;
    groupbox_hite = frame_hite + text_hite + text_hite / 2;

    button_width = 50 * avgCh.x() / 4;
    button_hite = 14 * avgCh.y() / 8;
    bxoff = button_width + frame_width;
    byoff = button_hite * 5/4;

    startWait();
    _tagMask.concat( '*' );     //make this work for all switches
    yy = frame_hite;

    _tool->name( n );
    n.concat( " Switches" );
    setText( n );

    //
    // Create the text at the top of the dialog
    //
    WText* head = new WText( this, WRect(frame_width,yy,-frame_width,text_hite), _text );
    head->show();
    yy += text_hite;

    //
    // Create the drop down list box at the top of the dialog
    //
    _combo = new WComboBox( this, WRect( frame_width, yy, -frame_width, maxCh.y()*12 ), NULL, CStyleDefault | WStyleVScroll | WStyleHScroll );
    _combo->onChanged( this, (cbw)&VSetup::pickFamily );
    thoff = text_hite * 3/2;
    yy += thoff;

    maxw = getTextExtentX( _text );
    maxh = 0;
    _tool->addFamilies( famList );
    k = 0;
    icount = famList.count();
    for( int i=0; i<icount; i++ ) {
        MFamily         *fam;
        WVList          myList;
        int             jcount;

        fam = (MFamily*)famList[i];
        fam->addSwitches( myList, _tagMask, TRUE );
        jcount = myList.count();
        if( jcount > 0 ) {
            WVList      *swList;
            WString     famName;
            WString     title;
            WWindow     *w;
            int         YBEG;
            int         XBEG;
            int         xoff;
            int         yoff;
            WRect       rr;
            int         wid;

            swList = new WVList();
            _families.add( swList );
            fam->name( famName );
            famName.concat( " Switches" );
            title.printf( "%d. %s", k+1, (const char*)famName );
#ifdef TEST
            w = this;
            YBEG = yy;
#else
            w = new WWindow( this, WRect(frame_width,yy,-frame_width,-button_hite*3/2), NULL, WStyleSimple );
            YBEG = frame_hite;
#endif
            XBEG = frame_width;
            xoff = XBEG;
            yoff = YBEG;
            _screens.add( w );
            w->getRectangle( rr );
            rr.w( rr.w() - 2*XBEG );
            rr.h( rr.h() - button_hite*3/2
                  - frame_hite*2
                  - WSystemMetrics::captionSize() );
            wid = rr.w()/2;
            for( int j=0; j<jcount; ) {
                MSwitch         *sw;

                sw = (MSwitch*)myList[j];
                if( sw->text().size() > 0 && sw->text()[0] != ' ' ) {
                    MState      *st;
                    WString     text;

                    st = findState( sw, _mode );
                    //
                    // decide if we should start putting controls in
                    // the second column
                    //
                    if( xoff == XBEG ) {
                        unsigned        newypos;
                        newypos = yoff;
                        newypos += calcControlHite( &myList, j, editctl_hite,
                                    checkbox_hite, radiobutton_hite, text_hite,
                                    groupbox_hite );
                        if( sw->panel() == 1 || newypos > rr.h() ) {
                            yoff = YBEG;
                            xoff = XBEG + rr.w()/2 + frame_width;
                        }
                    }
                    sw->displayText( text );
                    //
                    // Create a radio button group
                    //
                    if( streq( sw->className(), "MRSwitch" ) ) {
                        WString&        groupName = ((MRSwitch*)sw)->group();
                        WRect           rg(xoff,yoff,wid,10);
                        WGroupBox*      curGroup;
                        int             XGBEG;
                        int             YGBEG;
                        int             ygoff;
                        int             xgoff;
                        WStyle          style;

                        curGroup = new WGroupBox( w, rg, groupName );
#ifdef TEST
                        swList->add( new SwitchMap( curGroup ) );
#endif
                        XGBEG = frame_width;
                        YGBEG = text_hite;
                        ygoff = YGBEG;
                        xgoff = XGBEG;
                        style = RStyleGroupFirst;
                        for( ; j<jcount; ) {
                            sw = (MSwitch*)myList[j];
                            st = findState( sw, _mode );
                            if( !streq( sw->className(), "MRSwitch" ) ) break;
                            if( !(groupName == ((MRSwitch*)sw)->group()) ) break;
                            if( j+1 < jcount ) {
                                MSwitch* nsw = (MSwitch*)myList[j+1];
                                if( !streq( nsw->className(), "MRSwitch" ) ||
                                    !(groupName == ((MRSwitch*)nsw)->group()) ) {
                                    style = RStyleGroupLast;
                                }
                            } else {
                                style = RStyleGroupLast;
                            }
                            sw->displayText( text );
                            WRect rbrect( xoff+xgoff, yoff+ygoff, wid-2*xgoff, radiobutton_hite );
                            WRadioButton* t = new WRadioButton( w, rbrect, text, style );
                            swList->add( new SwitchMap( sw, t, st ) );
#ifndef TEST
                            t->show();
#endif
                            ygoff += radiobutton_hite;
                            j++;
                            style = RStyleRadioButton;
                        }
                        rg.h( ygoff+frame_hite );
                        curGroup->move( rg );
#ifndef TEST
                        curGroup->show();
#endif
                        yoff += rg.h() + text_hite / 2;
                    //
                    // Create a check box
                    //
                    } else if( streq( sw->className(), "MCSwitch" )
                               || streq( sw->className(), "MC2Switch" ) ) {
                        WCheckBox* t = new WCheckBox( w, WRect(xoff,yoff,wid,checkbox_hite), text );
                        swList->add( new SwitchMap( sw, t, st ) );
#ifndef TEST
                        t->show();
#endif
                        yoff += checkbox_hite;
                        j++;
                    //
                    // Create an edit control
                    //
                    } else if( streq( sw->className(), "MVSwitch" ) ) {
                        WBoolSwitch* tt = NULL;
                        if( ((MVSwitch*)sw)->optional() ) {
                            tt = new WCheckBox( w, WRect(xoff,yoff,wid, checkbox_hite), text );
#ifndef TEST
                            tt->show();
#endif
                            yoff += checkbox_hite;
                        } else {
                            WText* t = new WText( w, WRect(xoff,yoff,wid,text_hite), text );
#ifdef TEST
                            swList->add( new SwitchMap( t ) );
#else
                            t->show();
#endif
                            yoff += text_hite;
                        }
                        int INDENT = frame_width * 2;
                        WEditBox* t = new WEditBox( w, WRect(xoff+INDENT,yoff,wid-INDENT,editctl_hite), "value" );
                        swList->add( new SwitchMap( sw, tt, st, t ) );
#ifndef TEST
                        t->show();
#endif
                        yoff += editctl_hite;
                        j++;
                    } else {
                        j++;    //this should not happen!
                    }
                    if( xoff+wid > maxw ) maxw = xoff+wid;
                    if( yoff > maxh ) maxh = yoff;
                } else {
                    j++;
                }
            }
            initControls( swList, _mode, TRUE );
            _combo->insertString( title, k );
            k += 1;
        }
    }
    if( k > 1 ) {
        _combo->show();
    }

    int xoff = frame_width;

    //
    // create the push buttons at the bottom of the dialog
    //
    WDefPushButton* bOk = new WDefPushButton( this, WRect( xoff, -byoff, button_width, button_hite), ok );
    xoff += bxoff;
    bOk->onClick( this, (cbw)&VSetup::okButton );
    bOk->show();

    WPushButton* bCancel = new WPushButton( this, WRect( xoff, -byoff, button_width, button_hite), cancel );
    xoff += bxoff;
    bCancel->onClick( this, (cbw)&VSetup::cancelButton );
    bCancel->show();

    WPushButton* bResetCfg = new WPushButton( this, WRect( xoff, -byoff, button_width, button_hite), resetcfg );
    xoff += bxoff;
    bResetCfg->onClick( this, (cbw)&VSetup::resetCfgButton );
    bResetCfg->show();

    WPushButton* bResetDev = new WPushButton( this, WRect( xoff, -byoff, button_width, button_hite), resetdev );
    xoff += bxoff;
    bResetDev->onClick( this, (cbw)&VSetup::resetDevButton );
    bResetDev->enable( _mode != SWMODE_DEBUG );
    bResetDev->show();

    WPushButton* bResetRel = new WPushButton( this, WRect( xoff, -byoff, button_width, button_hite), resetrel );
    xoff += bxoff;
    bResetRel->onClick( this, (cbw)&VSetup::resetRelButton );
    bResetRel->enable( _mode != SWMODE_RELEASE );
    bResetRel->show();

    //
    // create the next and previous push buttons if this dialog has
    // more than one panel
    //
    if( k > 1 ) {
        _prev = new WPushButton( this, WRect( xoff, -byoff, button_width, button_hite), "&<<" );
        xoff += bxoff;
        _prev->onClick( this, (cbw)&VSetup::prevButton );
        _prev->show();

        _next = new WPushButton( this, WRect( xoff, -byoff, button_width, button_hite), "&>>" );
        xoff += bxoff;
        _next->onClick( this, (cbw)&VSetup::nextButton );
        _next->show();
    }

    if( maxw < xoff ) maxw = xoff;

    //
    // hide the drop down list box at the top of the dialog and move all
    // the controls up if this dialog has only one panel
    //
    if( k <= 1 ) {
        yy -= thoff;
        maxh -= thoff;
        for( int ii=0; ii<_families.count(); ii++ ) {
            shiftFamily( ii, 0, -thoff );
        }
    }

#ifdef TEST
    maxw += 2 * frame_width; //add wwindow offsets
    maxh += button_hite*3/2;
#else
    maxw += 3 * frame_width; //add wwindow offsets
    maxh += button_hite*3/2;
#endif

    maxw += 2 * frame_width; //add for frame thickness
    maxh += frame_hite*2 + WSystemMetrics::captionSize(); //add frame thickness and title bar

    size( (WOrdinal)maxw, (WOrdinal)maxh );
    sc.x( 0 );
    sc.y( 0 );
    sc.w( maxw );
    sc.h( maxh );
    move( sc );

    _combo->select( 0 );
    pickFamily( _combo );
    show();
    enableButtons();
    stopWait();
    _combo->setFocus();
}


VSetup::~VSetup()
{
    show( WWinStateHide );
    int icount = _families.count();
    for( int i=0; i<icount; i++ ) {
        WVList* swList = (WVList*)_families[i];
        swList->deleteContents();
    }
    _families.deleteContents();
}

MState* VSetup::findState( MSwitch* sw, SwMode mode )
{
    int kcount = _states->count();
    for( int k=0; k<kcount; k++ ) {
        MState* t = (MState*)(*_states)[k];
        if( mode == t->mode() && sw == t->sw() ) {
            return t;
        }
    }
    return NULL;
}

void VSetup::initControls( WVList* swList, SwMode mode, bool useStates )
{
    int icount = swList->count();
    for( int i=0; i<icount; i++ ) {
        SwitchMap* m = (SwitchMap*)(*swList)[i];
        MSwitch* sw = m->sw();
        MState* st = NULL;
        if( useStates ) {
            st = findState( sw, mode );
        }
        if( !sw ) {
        } else if( streq( sw->className(), "MRSwitch" ) ) {
            bool b = FALSE;
            if( st ) {
                b = ((MRState*)st)->state();
            } else {
                b = ((MRSwitch*)sw)->state( mode );
            }
            m->ctl()->setCheck( b );
        } else if( streq( sw->className(), "MCSwitch" ) ) {
            bool b = FALSE;
            if( st ) {
                b = ((MCState*)st)->state();
            } else {
                b = ((MCSwitch*)sw)->state( mode );
            }
            m->ctl()->setCheck( b );
        } else if( streq( sw->className(), "MC2Switch" ) ) {
            bool b = FALSE;
            if( st ) {
                b = ((MCState*)st)->state();
            } else {
                b = ((MC2Switch*)sw)->state( mode );
            }
            m->ctl()->setCheck( b );
        } else if( streq( sw->className(), "MVSwitch" ) ) {
            if( ((MVSwitch*)sw)->optional() ) {
                bool b = FALSE;
                if( st ) {
                    b = ((MVState*)st)->state();
                } else {
                    b = ((MVSwitch*)sw)->state( mode );
                }
                m->ctl()->setCheck( b );
            }
            WString* v;
            if( st ) {
                v = &((MVState*)st)->value();
            } else {
                v = &((MVSwitch*)sw)->value( mode );
            }
            m->ctl2()->setText( *v );
        }
    }
}

void VSetup::enableButtons()
{
    int index = _combo->selected();
    if( _prev ) _prev->enable( (index>0) ? TRUE : FALSE );
    if( _next ) _next->enable( (index+1<_screens.count()) ? TRUE : FALSE );
}

void VSetup::okButton( WWindow* )
{
    for( int k=_states->count(); k>0; ) {
        k--;
        MState* st = (MState*)(*_states)[k];
        if( st->mode() == _mode ) {
            delete _states->removeAt( k );
        }
    }
    int icount = _families.count();
    for( int i=0; i<icount; i++ ) {
        WVList* swList = (WVList*)_families[i];
        int jcount = swList->count();
        for( int j=0; j<jcount; j++ ) {
            SwitchMap* m = (SwitchMap*)(*swList)[j];
            MSwitch* sw = m->sw();
            if( !sw ) {
            } else if( streq( sw->className(), "MRSwitch" ) ) {
                bool b = m->ctl()->checked();
                if( b != ((MRSwitch*)sw)->state( _mode ) ) {
                    MRState* st = new MRState( _tool, _mode, (MRSwitch*)sw, b );
                    _states->add( st );
                }
            } else if( streq( sw->className(), "MCSwitch" ) ) {
                bool b = m->ctl()->checked();
                if( b != ((MCSwitch*)sw)->state( _mode ) ) {
                    MCState* st = new MCState( _tool, _mode, (MCSwitch*)sw, b );
                    _states->add( st );
                }
            } else if( streq( sw->className(), "MC2Switch" ) ) {
                bool b = m->ctl()->checked();
                if( b != ((MC2Switch*)sw)->state( _mode ) ) {
                    MCState* st = new MCState( _tool, _mode, (MCSwitch*)sw, b );
                    _states->add( st );
                }
            } else if( streq( sw->className(), "MVSwitch" ) ) {
                bool b = FALSE;
                if( ((MVSwitch*)sw)->optional() ) {
                    b = m->ctl()->checked();
                }
                WString v;
                m->ctl2()->getText( v );
                v.trim();
                if( b != ((MVSwitch*)sw)->state( _mode ) || !(((MVSwitch*)sw)->value( _mode ) == v) ) {
                    MVState* st = new MVState( _tool, _mode, (MVSwitch*)sw, b, &v );
                    _states->add( st );
                }
            }
        }
    }
    quit( TRUE );
}

void VSetup::cancelButton( WWindow* )
{
    quit( FALSE );
}

void VSetup::resetCfgButton( WWindow* )
{
    int i = _combo->selected();
    WVList* swList = (WVList*)_families[i];
    initControls( swList, _mode, FALSE );
}

void VSetup::resetDevButton( WWindow* )
{
    int i = _combo->selected();
    WVList* swList = (WVList*)_families[i];
    initControls( swList, SWMODE_DEBUG, TRUE );
}

void VSetup::resetRelButton( WWindow* )
{
    int i = _combo->selected();
    WVList* swList = (WVList*)_families[i];
    initControls( swList, SWMODE_RELEASE, TRUE );
}

void VSetup::prevButton( WWindow* )
{
    int index = _combo->selected();
    if( index > 0 ) {
        index -= 1;
        _combo->select( index );
    }
    enableButtons();
}

void VSetup::nextButton( WWindow* )
{
    int index = _combo->selected();
    if( index+1 < _screens.count() ) {
        index += 1;
        _combo->select( index );
    }
    enableButtons();
}

void VSetup::shift( WWindow* w, int xoff, int yoff )
{
    WRect r = w->getAutosize();
    w->move( (WOrdinal)(r.x() + xoff), (WOrdinal)(r.y() + yoff) );
}

void VSetup::shiftFamily( int index, int xoff, int yoff )
{
#ifdef TEST
    WVList* swList = (WVList*)_families[ index ];
    for( int i=0; i<swList->count(); i++ ) {
        SwitchMap* sm = (SwitchMap*)(*swList)[i];
        if( sm->ctl() ) shift( sm->ctl(), xoff, yoff );
        if( sm->ctl2() ) shift( sm->ctl2(), xoff, yoff );
        if( sm->ctrl() ) shift( sm->ctrl(), xoff, yoff );
    }
#else
    WWindow* w = (WWindow*)_screens[ index ];
    w->shift( xoff, yoff );
#endif
}

void VSetup::hideFamily( int index )
{
#ifdef TEST
    WVList* swList = (WVList*)_families[ index ];
    for( int i=0; i<swList->count(); i++ ) {
        SwitchMap* sm = (SwitchMap*)(*swList)[i];
        if( sm->ctl() ) sm->ctl()->show( WWinStateHide );
        if( sm->ctl2() ) sm->ctl2()->show( WWinStateHide );
        if( sm->ctrl() ) sm->ctrl()->show( WWinStateHide );
    }
#else
    WWindow* g = (WWindow*)_screens[ index ];
    g->show( WWinStateHide );
#endif
}

void VSetup::showFamily( int index )
{
#ifdef TEST
    WVList* swList = (WVList*)_families[ index ];
    for( int i=0; i<swList->count(); i++ ) {
        SwitchMap* sm = (SwitchMap*)(*swList)[i];
        if( sm->ctl() ) sm->ctl()->show();
        if( sm->ctl2() ) sm->ctl2()->show();
        if( sm->ctrl() ) sm->ctrl()->show();
    }
#else
    WWindow* g = (WWindow*)_screens[ index ];
    g->show();
#endif
}

void VSetup::pickFamily( WWindow* combo )
{
    int newIndex = ((WComboBox*)combo)->selected();
    if( _curIndex != newIndex ) {
        if( _curIndex >= 0 ) {
            hideFamily( _curIndex );
        }
        _curIndex = newIndex;
        if( _curIndex >= 0 ) {
            showFamily( _curIndex );
        }
        enableButtons();
    }
}

bool VSetup::process()
{
    return WDialog::process() == TRUE;
}
