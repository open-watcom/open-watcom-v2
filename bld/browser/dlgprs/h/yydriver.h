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



// Note -- there is no protection on this header as it is included multiple
//         times with different definitions of YYPARSER.

#include <wstd.h>

#include "scancm.h"

#define YYTABTYPE       int_16
#define YYCHKTYPE       YYTABTYPE
#define YYACTTYPE       YYTABTYPE
#define YYPLENTYPE      YYTABTYPE
#define YYPLHSTYPE      YYTABTYPE


class Scanner;

class YYPARSER {
public:
                                        YYPARSER( const char * fileName );
                                        ~YYPARSER();

            int                         yyparse();
            void                        yyerror( const char * msg );
            int                         yyabort();
            int                         yylex();
private:

            Scanner *                   _scanner;

            YYSTYPE *                   yyvp;
            YYSTYPE                     yyval;
            YYSTYPE                     yylval;

static      const YYCHKTYPE YYFAR       yychktab[];
static      const YYACTTYPE YYFAR       yyactab[];
static      const YYPLENTYPE YYFAR      yyplentab[];
static      const YYPLHSTYPE YYFAR      yyplhstab[];
};
