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
* Description:  ctrldef/ectrldef tag sequence
*   :ctrldef / :ectrldef
*   Follows :docprof
*   May contain :pbutton, :ctrl
*
****************************************************************************/

#include "ctrldef.hpp"
#include "ctrltag.hpp"
#include "document.hpp"
#include "pbutton.hpp"
#include "ctrl.hpp"

CtrlDef::~CtrlDef()
{
    for( ConstChildrenIter itr = children.begin(); itr != children.end(); ++itr ) {
        delete *itr;
    }
}
/***************************************************************************/
Lexer::Token CtrlDef::parse( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::FLAG )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            document->printError( ERR1_TAGSYNTAX );
        tok = document->getNextToken();
    }
    tok = document->getNextToken();
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( tok == Lexer::WORD ||
            tok == Lexer::ENTITY ||
            tok == Lexer::PUNCTUATION )
            document->printError( ERR1_HEADTEXT );
        else if( tok == Lexer::WHITESPACE )
            tok = document->getNextToken();
        else if( tok == Lexer::COMMAND ) {
            switch( lexer->cmdId() ) {
            case Lexer::COMMENT:
                break;
            case Lexer::IMBED:
                {
                std::wstring* fname( new std::wstring( lexer->text() ) );
                fname = document->addFileName( fname );
                document->pushInput( new IpfFile( fname ) );
                break;
                }
            default:
                document->printError( ERR1_TAGCONTEXT );
                break;
            }
            tok = document->getNextToken();
        }
        else if( tok == Lexer::TAG ) {
            if( lexer->tagId() == Lexer::ECTRLDEF ) {
                tok = document->getNextToken();
                while( tok != Lexer::TAGEND ) {
                    if( tok == Lexer::ATTRIBUTE )
                        document->printError( ERR1_ATTRNOTDEF );
                    else if( tok == Lexer::FLAG )
                        document->printError( ERR1_ATTRNOTDEF );
                    else if( tok == Lexer::ERROR_TAG )
                        throw FatalError( ERR_SYNTAX );
                    else if( tok == Lexer::END )
                        throw FatalError( ERR_EOF );
                    else
                        document->printError( ERR1_TAGSYNTAX );
                    tok = document->getNextToken();
                }
                break;
            }
            else if( lexer->tagId() == Lexer::PBUTTON ) {
                PButton* pb( new PButton( document, 0, document->dataName(), \
                    document->dataLine(), document->dataCol() ) );
                appendChild( pb );
                tok = pb->parse( lexer );
            }
            else if( lexer->tagId() == Lexer::CTRL ) {
                Ctrl* ctrl( new Ctrl( document, 0, document->dataName(), \
                    document->dataLine(), document->dataCol() ) );
                appendChild( ctrl );
                tok = ctrl->parse( lexer );
            }
            else
                document->printError( ERR1_TAGCONTEXT );
        }
        else if( tok == Lexer::ERROR_TAG ) {
            document->printError( ERR1_TAGNOTDEF );
            tok = document->getNextToken();
        }
        else if( tok == Lexer::ERROR_ENTITY ) {
            document->printError( ERR1_TAGNOTDEF );
            tok = document->getNextToken();
        }
    }
    return document->getNextToken();
}
/***************************************************************************/
void CtrlDef::build( Controls* ctrls )
{
    for( ConstChildrenIter itr = children.begin(); itr != children.end(); ++itr )
        (*itr)->build( ctrls );
}

