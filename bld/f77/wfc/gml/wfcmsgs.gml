:cmt *****************************************************************************
:cmt *
:cmt *                            Open Watcom Project
:cmt *
:cmt * Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
:cmt *    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
:cmt *
:cmt *  ========================================================================
:cmt *
:cmt *    This file contains Original Code and/or Modifications of Original
:cmt *    Code as defined in and that are subject to the Sybase Open Watcom
:cmt *    Public License version 1.0 (the 'License'). You may not use this file
:cmt *    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
:cmt *    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
:cmt *    provided with the Original Code and Modifications, and is also
:cmt *    available at www.sybase.com/developer/opensource.
:cmt *
:cmt *    The Original Code and all software distributed under the License are
:cmt *    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
:cmt *    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
:cmt *    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
:cmt *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
:cmt *    NON-INFRINGEMENT. Please see the License for the specific language
:cmt *    governing rights and limitations under the License.
:cmt *
:cmt *  ========================================================================
:cmt *
:cmt * Description:  Fortran compiler diagnostic messages.
:cmt *
:cmt *     UTF-8 encoding, ¥
:cmt *
:cmt *****************************************************************************
:cmt

:cmt    Word usage:
:cmt
:cmt    'parameter' is used for macro parms
:cmt    'argument' is used for function arguments

:cmt    GML Macros used (see MSGMACS.GML):

:cmt        :ansi <n>   warning if extensions enabled, error otherwise
:cmt        :ansierr    ignored if extensions enabled, error otherwise
:cmt        :ansiwarn <n>   ignored if extensions enabled, warn otherwise
:cmt        :warning <n>    message is a warning with a specific level
:cmt        :info       informational message
:cmt
:cmt        :msgsym <sym>    internal symbolic name for message
:cmt        :msgtxt <text>   text for message
:cmt        :msgattr <value> attribute value for message
:cmt
:cmt        :msglvl     start of header title for a group of messages
:cmt        :emsglvl    end of header title for a group of messages
:cmt        :errbad     start of an example that generates an error msg
:cmt        :eerrbad    end of an example that generates an error msg
:cmt        :errgood    start of an example that compiles clean
:cmt        :eerrgood   end of an example that compiles clean
:cmt
:cmt        .kw     highlight a keyword
:cmt        .id     highlight an identifier
:cmt        .ev     highlight an environment variable
:cmt        .us     italicize a phrase
:cmt        .np     start a new paragraph

:cmt    The following substitutions are made:
:cmt        &incvarup   environment variable for include path
:cmt        &wclname    Compile and Link utility name

:cmt    Note for translators:

:cmt    Japanese error messages are supported via the :MSGJTXT tag.
:cmt    If there is no :MSGJTXT. for a particular :MSGSYM. then the
:cmt    message will come out in English.  Translation may proceed
:cmt    by translating error messages that do not have the :MSGJTXT.
:cmt    tag present and adding the :MSGJTXT. tag with the Japanese
:cmt    text after the :MSGTXT. tag.  If the :MSGJTXT. has no text
:cmt    then the error message must also be translated.  This has
:cmt    been found to be easier when searching for messages that
:cmt    still need to be translated.

:cmt -------------------------------------------------------------------
:MSGGRP. AR
:MSGGRPSTR. AR-
:MSGGRPTXT. Subprogram Arguments
:cmt -------------------------------------------------------------------
:MSGSYM. AR_BAD_COUNT
:MSGTXT. invalid number of arguments to intrinsic function %s1
:MSGJTXT. 組込み関数%s1で定義された引数の数とそれを呼出すために指定した引数の数が一致していません。
:MSGATTR. 2
 The number of actual arguments specified in the argument list for the
 intrinsic function %s1 does not agree with the dummy argument list.
 Consult the Language Reference for information on intrinsic functions and
 their arguments.
:MSGSYM. AR_DUPLICATE_PARM
:MSGTXT. dummy argument %s1 appears more than once
:MSGJTXT. 同じ仮引数%s1が2度以上使われてはいけません。
:MSGATTR. 0
 The same dummy argument %s1 is named more than once in the dummy
 argument list.
:MSGSYM. AR_ENTRY_TOO_LATE
:MSGTXT. dummy argument %s1 must not appear before definition of ENTRY %s2
:MSGJTXT. 仮引数%s1は,ENTRY %s2の定義の前になければなりません。
:MSGATTR. 2
 The dummy argument %s1 has appeared in an executable statement before
 its appearance in the definition of %s2 in an ENTRY statement.
 This is illegal.
:eMSGGRP. AR
:cmt -------------------------------------------------------------------
:MSGGRP. BD
:MSGGRPSTR. BD-
:MSGGRPTXT. Block Data Subprograms
:cmt -------------------------------------------------------------------
:MSGSYM. BD_BLKDAT_NOT_COMMON
:MSGTXT. %s1 was initialized in a block data subprogram but is not in COMMON
:MSGJTXT. %s1はBLOCK DATA副ﾌﾟﾛｸﾞﾗﾑで初期値されましたが, COMMONﾌﾞﾛｯｸにありません。
:MSGATTR. 0
 The variable or array element, %s1, was initialized in a BLOCK DATA
 subprogram but was not specified in a named COMMON block.
:MSGSYM. BD_IN_BLOCK_DATA
:MSGTXT. %s1 statement is not permitted in a BLOCK DATA subprogram
:MSGJTXT. %s1文はBLOCK DATA副ﾌﾟﾛｸﾞﾗﾑの中には書けません。
:MSGATTR. 0
 The statement, %s1, is not allowed in a BLOCK DATA subprogram.
 The only statements which are allowed to appear are:
 IMPLICIT, PARAMETER, DIMENSION, COMMON, SAVE, EQUIVALENCE, DATA, END,
 and type statements.
:eMSGGRP. BD
:cmt -------------------------------------------------------------------
:MSGGRP. CC
:MSGGRPSTR. CC-
:MSGGRPTXT. Source Format and Contents
:cmt -------------------------------------------------------------------
:MSGSYM. CC_BAD_CHAR
:MSGTXT. invalid character encountered in source input
:MSGJTXT. ﾌﾟﾛｸﾞﾗﾑの中に使ってはいけない文字があります。
:MSGATTR. 1
 The indicated statement contains an invalid character.
 Valid characters are: letters, digits, $, *, ., +, &minus., /, :, =, (, ),
 !, %,  ', and ,(comma).
 Any character may be used inside a character or hollerith string.
:MSGSYM. CC_NOT_DIGITS
:MSGTXT. invalid character in statement number columns
:MSGJTXT. 文番号桁に不適切な文字があります。
:MSGATTR. 0
 A column in columns 1 to 5 of the indicated statement contains a
 non-digit character.
 Columns 1 to 5 contain the statement number label.
 It is made up of digits from 0 to 9 and is greater than 0 and less
 than or equal to 99999.
:MSGSYM. CC_NOT_INITIAL
:MSGTXT. character in continuation column, but no statement to continue
:MSGJTXT. 継続桁に文字がありますが,継続する行がありません。
:MSGATTR. 1
 The character in column 6 indicates that this line is a continuation
 of the previous statement but there is no previous statement to
 continue.
:MSGSYM. CC_SET_EXTEND
:MSGTXT. character encountered is not FORTRAN 77 standard
:MSGJTXT. FORTRAN 77の標準ｾｯﾄにはない文字があります。
:MSGATTR. 0
 A non-standard character was encountered in the source input stream.
 This is most likely caused by the use of lower case letters.
:MSGSYM. CC_STMTNO_ON_CONT
:MSGTXT. columns 1-5 in a continuation line must be blank
:MSGJTXT. 継続行のｶﾗﾑ1から5はﾌﾞﾗﾝｸでなければなりません。
:MSGATTR. 0
 When column 6 is marked as a continuation statement to the previous
 line, columns 1 to 5 must be left blank.
:MSGSYM. CC_TOO_MANY_CONT
:MSGTXT. more than 19 continuation lines is not FORTRAN 77 standard
:MSGJTXT. 19行を超える継続行はFORTRAN 77標準ではありません。
:MSGATTR. 0
 More than 19 continuation lines is an extension to the FORTRAN 77 language.
:MSGSYM. CC_EOL_COMMENT
:MSGTXT. end-of-line comment is not FORTRAN 77 standard
:MSGJTXT. 終端行注訳はFORTRAN 77標準ではありません。
:MSGATTR. 0
 End-of-line comments are an extension to the FORTRAN 77 language.
 End-of-line comments start with the exclamation mark (!) character.
:MSGSYM. CC_D_IN_COLUMN_1
:MSGTXT. D in column 1 is not FORTRAN 77 standard
:MSGJTXT. 1ｶﾗﾑ目のDはFORTRAN 77標準ではありません。
:MSGATTR. 0
 A "D" in column 1 signifies a debug statement that is compiled
 when the "__debug__" macro symbol is defined.
 If the "__debug__" macro symbol is not defined, the statement is ignored.
 The "c$define" compiler directive or the "define" compiler option can be
 used to define the "__debug__" macro symbol.
:MSGSYM. CC_CONT_OVERFLOW
:MSGTXT. too many continuation lines
:MSGJTXT. 継続行が多すぎます。
:MSGATTR. 0
 The limit on the number of continuation lines has been reached.
 This limit depends on the size of each continuation line.
 A minimum of 61 continuation lines is permitted.
 If the "xline" option is used, a minimum of 31 continuation lines is
 permitted.
:eMSGGRP. CC
:cmt -------------------------------------------------------------------
:MSGGRP. CM
:MSGGRPSTR. CM-
:MSGGRPTXT. COMMON Blocks
:cmt -------------------------------------------------------------------
:MSGSYM. CM_ALREADY_IN_COM
:MSGTXT. %s1 already in COMMON
:MSGJTXT. %s1は他のCOMMONﾌﾞﾛｯｸにもあります。
:MSGATTR. 2
 The variable or array name, %s1, has already been specified in this or
 another COMMON block.
:MSGSYM. CM_COMMON
:MSGTXT. initializing %s1 in COMMON outside of block data subprogram is not FORTRAN 77 standard
:MSGJTXT. BLOCKDATA副ﾌﾟﾛｸﾞﾗﾑの外側のCOMMONﾌﾞﾛｯｸで%s1を初期化することは,FORTRAN 77標準ではありません。
:MSGATTR. 0
 The symbol %s1, in a named COMMON block, has been initialized outside of a
 block data subprogram.
 This is an extension to the FORTRAN 77 language.
:MSGSYM. CM_MIXED_COMMON
:MSGTXT. character and non-character data in COMMON is not FORTRAN 77 standard
:MSGJTXT. FORTRAN 77標準ｾｯﾄではCOMMONﾌﾞﾛｯｸの中に文字や数値を同時に含めることはできせん。
:MSGATTR. 0
 The FORTRAN 77 standard specifies that a COMMON block cannot contain
 both numeric and character data.
 Allowing COMMON blocks to contain both numeric and character data is an
 extension to the FORTRAN 77 standard.
:MSGSYM. CM_NAMED_DIFF_SIZE
:MSGTXT. COMMON block %s1 has been defined with a different size
:MSGJTXT. COMMONﾌﾞﾛｯｸ%s1は異なる大きさで定義されていました。
:MSGATTR. 0
 The COMMON block %s1 has been defined with a different size in another
 subprogram.
 A named COMMON block must define the same amount of storage units
 where ever named.
:MSGSYM. CM_BLKDATA_ALREADY
:MSGTXT. named COMMON block %s1 appears in more than one BLOCK DATA subprogram
:MSGJTXT. 名前の付いたCOMMONﾌﾞﾛｯｸ%s1が,2個以上のBLOCK DATA副ﾌﾟﾛｸﾞﾗﾑ にあります。
:MSGATTR. 0
 The named COMMON block, %s1, may not appear in more than one BLOCK
 DATA subprogram.
:MSGSYM. CM_BLANK_DIFF_SIZE
:MSGTXT. blank COMMON block has been defined with a different size
:MSGJTXT. 名前のないCOMMONﾌﾞﾛｯｸが異なる大きさで定義されていました。
:MSGATTR. 0
 The blank COMMON block has been defined with a different size in
 another subprogram.
 This is legal but a warning message is issued.
:eMSGGRP. CM
:cmt -------------------------------------------------------------------
:MSGGRP. CN
:MSGGRPSTR. CN-
:MSGGRPTXT. Constants
:cmt -------------------------------------------------------------------
:MSGSYM. CN_DOUBLE_COMPLEX
:MSGTXT. DOUBLE PRECISION COMPLEX constants are not FORTRAN 77 standard
:MSGJTXT. FORTRAN 77標準ｾｯﾄでは倍精度の複素数型の定数は使えません。
:MSGATTR. 1
 Double precision complex numbers are an extension to the FORTRAN 77
 language.
 The indicated number is a complex number and at least one of the
 parts, real or imaginary, is a double precision constant.
 Both real and imaginary parts will be double precision.
:MSGSYM. CN_FLOAT
:MSGTXT. invalid floating-point constant %s1
:MSGJTXT. %s1は正しい浮動小数点定数ではありません。
:MSGATTR. 2
 The floating-point constant %s1 is invalid.
 Refer to the chapter entitled "Names, Data Types and Constants"
 in the Language Reference.
:MSGSYM. CN_ZERO_LEN
:MSGTXT. zero length character constants are not allowed
:MSGJTXT. 長さがｾﾞﾛの文字定数は使えません。
:MSGATTR. 0
 FORTRAN 77 does not allow character constants of length 0 (i.e., an
 empty string).
:MSGSYM. CN_BAD_HEX_OCT
:MSGTXT. invalid hexadecimal/octal constant
:MSGJTXT. 16進数／8進数の定数が無効です。
:MSGATTR. 2
 An invalid hexadecimal or octal constant was specified.
 Hexadecimal constants can only contain digits or the letters 'a' through
 'f' and 'A' through 'F'.
 Octal constants can only contain the digits '0' through '7'.
:MSGSYM. CN_HEX_OCT
:MSGTXT. hexadecimal/octal constant is not FORTRAN 77 standard
:MSGJTXT. 16進数／8進数の定数はFORTRAN 77の標準ではありません。
:MSGATTR. 2
 Hexadecimal and octal constants are extensions to the FORTRAN 77 standard.
:eMSGGRP. CN
:cmt -------------------------------------------------------------------
:MSGGRP. CO
:MSGGRPSTR. CO-
:MSGGRPTXT. Compiler Options
:cmt -------------------------------------------------------------------
:MSGSYM. CO_ALREADY_OPEN
:MSGTXT. %s1 is already being included
:MSGJTXT. %s1は既に読み込まれています。
:MSGATTR. 0
 An attempt has been made to include a file that is currently being
 included in the program.
:MSGSYM. CO_BAD_NO
:MSGTXT. '%s1' option cannot take a NO prefix
:MSGJTXT. '%s1'ｵﾌﾟｼｮﾝの前にNOは付けられません。
:MSGATTR. 0
 The compiler option %s1, cannot have the NO prefix specified.
 The NO prefix is used to negate an option.
 Certain options, including all options that require a value cannot have a
 NO prefix.
:MSGSYM. CO_NEED_EQUALS
:MSGTXT. expecting an equals sign following the %s1 option
:MSGJTXT. %s1ｵﾌﾟｼｮﾝの後に＝が必要です。
:MSGATTR. 0
 The compiler option %s1, requires an equal sign to be between the
 option keyword and its associated value.
:MSGSYM. CO_WANT_NUMBER
:MSGTXT. the '%s1' option requires a number
:MSGJTXT. '%s1'ｵﾌﾟｼｮﾝには数字の値が必要です。
:MSGATTR. 0
 The compiler option %s1 and an equal sign has been detected but the
 required associated value is missing.
:MSGSYM. CO_NOT_RECOG
:MSGTXT. option '%s1' not recognized - ignored
:MSGJTXT. '%s1'ｵﾌﾟｼｮﾝは使えません。無視しました。
:MSGATTR. 0
 The option %s1 is not a recognized compiler option and has been ignored.
 Consult the User's Guide for a complete list of compiler options.
:MSGSYM. CO_NOT_IN_SOURCE
:MSGTXT. '%s1' option not allowed in source input stream
:MSGJTXT. '%s1'ｵﾌﾟｼｮﾝはﾌﾟﾛｸﾞﾗﾑの中に書けません。ｺﾏﾝﾄﾞ行で指示してください。
:MSGATTR. 0
 The option %s1 can only be specified on the command line.
 Consult the User's Guide for a description of which options are allowed
 in the source input stream.
:MSGSYM. CO_MACRO_NESTING_EXCEEDED
:MSGTXT. nesting level exceeded for compiler directives
:MSGJTXT. ｺﾝﾊﾟｲﾗｰ疑似命令のﾈｽﾄﾚﾍﾞﾙが深すぎます。
:MSGATTR. 0
 Use of the C$IFDEF or C$IFNDEF compiler directives has caused the maximum
 nesting level to be exceeded.
 The maximum nesting level is 16.
:MSGSYM. CO_MACRO_STRUCTURE_MISMATCH
:MSGTXT. mismatching compiler directives
:MSGJTXT. ｺﾝﾊﾟｲﾗｰ疑似命令が対応していません。
:MSGATTR. 0
 This error message is issued if, for example, a C$ENDIF directive is used and
 no matching C$IFDEF or C$IFNDEF precedes it.
 Incorrect nesting of C$IFDEF, C$IFNDEF, C$ELSE and C$ENDIF directives will
 also cause this message to be issued.
:MSGSYM. CO_DATA_IN_BAD_FILE
:MSGTXT. DATA option not allowed
:MSGJTXT. 読み込むﾌﾟﾛｸﾞﾗﾑの中にDATAｺﾝﾊﾟｲﾗｰｵﾌﾟｼｮﾝを含めてはいけません。
:MSGATTR. 0
 A source file has been included into the current program through the
 use of the INCLUDE compiler option.
 This included source file cannot contain the DATA compiler option.
:MSGSYM. CO_LIMIT_TOO_BIG
:MSGTXT. maximum limit exceeded in the '%s1' option - option ignored
:MSGJTXT. '%s1'の最大限界値をｵｰﾊﾞ-しました。ｵﾌﾟｼｮﾝは無視されました。
:MSGATTR. 0
 The user has specified a value on an option which exceeds the maximum allowed
 value.
:MSGSYM. CO_DATA_WITH_OBJ
:MSGTXT. DATA option not allowed with OBJECT option
:MSGJTXT. DATAｵﾌﾟｼｮﾝとOBJECTｵﾌﾟｼｮﾝは同時に指定できません。
:MSGATTR. 0
 The DATA compiler option can not appear a file that is compiled with the
 OBJECT option.
:eMSGGRP. CO
:cmt -------------------------------------------------------------------
:MSGGRP. CP
:MSGGRPSTR. CP-
:MSGGRPTXT. Compiler Errors
:cmt -------------------------------------------------------------------
:MSGSYM. CP_TERMINATE
:MSGTXT. program abnormally terminated
:MSGJTXT. ﾌﾟﾛｸﾞﾗﾑが異常終了しました。
:MSGATTR. 0
 This message is issued during the execution of the program.
 If you are running FORTRAN 77, this message indicates that an internal error
 has occurred in the compiler.
 Please report this error and any other helpful information about the program
 being compiled to Watcom so that the problem can be fixed.
 .pc
 If you are running an application compiled by the Watcom FORTRAN 77
 optimizing compiler, this message may indicate a problem with the compiler
 or a problem with your program.
 Try compiling your application with the "debug" option.
 This causes the generation of run-time checking code to validate, for
 example, array subscripts and will help ensure that your program is not
 in error.
:MSGSYM. CP_BAD_PARM_REGISTER
:MSGTXT. argument %d1 incompatible with register
:MSGJTXT. 引数%d1がﾚｼﾞｽﾀと一致しません。
:MSGATTR. 0
 The register specified in an auxiliary pragma for argument number %d1
 is invalid.
:MSGSYM. CP_BAD_RETURN_REGISTER
:MSGTXT. subprogram %s1 has invalid return register
:MSGJTXT. 副ﾌﾟﾛｸﾞﾗﾑ%s1に間違ったﾘﾀｰﾝﾚｼﾞｽﾀがあります。
:MSGATTR. 0
 The register specified in an auxiliary pragma for the return value of
 function %s1 is invalid.
 This error is issued when, for example, an auxiliary pragma is used to
 specify EAX as the return register for a double precision function.
:MSGSYM. CP_LOW_ON_MEMORY
:MSGTXT. low on memory - unable to fully optimize %s1
:MSGJTXT. ﾒﾓﾘｰ不足です。- %s1を最適化できませんでした。
:MSGATTR. 0
 There is not enough memory for the code generator to fully optimize
 subprogram %s1.
:MSGSYM. CP_BACK_END_ERROR
:MSGTXT. internal compiler error %d1
:MSGJTXT. ｺﾝﾊﾟｲﾗ内部ｴﾗｰです。 %d1
:MSGATTR. 0
 This error is an internal code generation error.
 Please report the specified internal compiler error number and any other
 helpful information about the program being compiled to Watcom so that
 the problem can be fixed.
:MSGSYM. CP_BAD_SAVE
:MSGTXT. illegal register modified by %s1
:MSGJTXT. %s1で変更したレジスタは正しくありません。
:MSGATTR. 0
 An illegal register was said to be modified by %s1 in the auxiliary pragma
 for %s1.
 In a 32-bit flat memory model, the base pointer register EBP and segment
 registers CS, DS, ES, and SS cannot be modified.
 In small data models, the base pointer register (32-bit EBP or 16-bit BP)
 and segment registers CS, DS, and SS cannot be modified.
 In large data models, the base pointer register (32-bit EBP or 16-bit BP)
 and segment registers CS, and SS cannot be modified.
:MSGSYM. CP_ERROR
:MSGTXT. %s1
:MSGJTXT. %s1
:MSGATTR. 0
 The message specified by %s1 indicates an error during the code generation
 phase.
 The most probable cause is an invalid instruction in the in-line assembly
 code specified in an auxiliary pragma.
:MSGSYM. CP_FATAL_ERROR
:MSGTXT. fatal: %s1
:MSGJTXT. 致命的エラー: %s1
:MSGATTR. 0
 The specified error indicates that the code generator has been abnormally
 terminated.
 This message will be issued if any internal limit is reached or a keyboard
 interrupt sequence is pressed during the code generation phase.
:MSGSYM. CP_MEMORY_NOT_FREED
:MSGTXT. dynamic memory not freed
:MSGJTXT. 動的ﾒﾓﾘｰが解放されませんでした。
:MSGATTR. 0
 This message indicates an internal compiler error.
 Please report this error and any other helpful information about the program
 being compiled to Watcom so that the problem can be fixed.
:MSGSYM. CP_FREEING_UNOWNED_MEMORY
:MSGTXT. freeing unowned dynamic memory
:MSGJTXT. 確保していない動的ﾒﾓﾘｰが解放されました。
:MSGATTR. 0
 This message indicates an internal compiler error.
 Please report this error and any other helpful information about the program
 being compiled to Watcom so that the problem can be fixed.
:MSGSYM. CP_AUTO_EQUIV_TOO_LARGE
:MSGTXT. The automatic equivalence containing %s1 exceeds 32K limit
:MSGJTXT. %s1を含む自動等価変数が32Kの限界を超えました。
:MSGATTR. 0
 In 16-bit environments, the size of an equivalence on the stack must not
 exceed 32767 bytes.
:MSGSYM. CP_AUTO_RET_TOO_LARGE
:MSGTXT. The return value of %s1 exceeds 32K limit
:MSGJTXT. %s1の戻り値が32Kの限界を超えました。
:MSGATTR. 0
 In 16-bit environments, the size of the return value of a function must not
 exceed 32767 bytes.
:MSGSYM. CP_AUTO_VAR_TOO_LARGE
:MSGTXT. The automatic variable %s1 exceeds 32K limit
:MSGJTXT. 自動変数%s1が32Kの限界を超えました。
:MSGATTR. 0
 In 16-bit environments, the size of any variable on the stack must not
 exceed 32767 bytes.
:eMSGGRP. CP
:cmt -------------------------------------------------------------------
:MSGGRP. CV
:MSGGRPSTR. CV-
:MSGGRPTXT. Character Variables
:cmt -------------------------------------------------------------------
:MSGSYM. CV_BAD_LEN
:MSGTXT. CHARACTER variable %s1 with length (*) not allowed in this expression
:MSGJTXT. この式の中に長さ(*)を指定した文字変数-%s1は使えません。
:MSGATTR. 0
 The length of the result of evaluating the expression is
 indeterminate.
 One of the operands has an indeterminate length and the result is
 being assigned to a temporary.
:MSGSYM. CV_CHARSTAR_ILLEGAL
:MSGTXT. character variable %s1 with length (*) must be a subprogram argument
:MSGJTXT. 長さに(*)を指定した文字変数%s1は副ﾌﾟﾛｸﾞﾗﾑの仮引数以外に使えません。
:MSGATTR. 0
 The character variable %s1 with a length specification (*) can only be
 used to declare dummy arguments in the subprogram.
 The length of a dummy argument assumes the length of the
 corresponding actual argument.
:MSGSYM. CV_OVERLAPING_ASSGT
:MSGTXT. left and right hand sides overlap in a character assignment statement
:MSGJTXT. 文字の代入文において左右の文字部分列が重なり合って、互いに内容をこわしてしまいます。
:MSGATTR. 0
 The expression on the right hand side defines a substring of a
 character variable and tries to assign it to an overlapping part of
 the same character variable.
:eMSGGRP. CV
:cmt -------------------------------------------------------------------
:MSGGRP. DA
:MSGGRPSTR. DA-
:MSGGRPTXT. Data Initialization
:cmt -------------------------------------------------------------------
:MSGSYM. DA_BAD_DO_VAR
:MSGTXT. implied DO variable %s1 must be an integer variable
:MSGJTXT. DO形並びの変数%s1はINTEGER変数でなければなりません。
:MSGATTR. 2
 The implied DO variable %s1 must be declared as a variable of type
 INTEGER or must have an implicit INTEGER type.
:MSGSYM. DA_BAD_RPT_SPEC
:MSGTXT. repeat specification must be a positive integer
:MSGJTXT. 繰返しの指定はﾌﾟﾗｽの整数でなければなりません。
:MSGATTR. 2
 The repeat specification in the constant list of the DATA statement
 must be an unsigned positive integer.
:MSGSYM. DA_BAD_VAR_IN_EXPR
:MSGTXT. %s1 appears in an expression but is not an implied DO variable
:MSGJTXT. DATAの式の中に使われている%s1はDO形並びの変数でなければなりません。
:MSGATTR. 2
 The variable %s1 is used to express the array elements in the DATA
 statement but the variable is not used as an implied DO variable.
:MSGSYM. DA_BLANK_INIT
:MSGTXT. %s1 in blank COMMON block cannot be initialized
:MSGJTXT. 名前の無いCOMMONﾌﾞﾛｯｸにある%s1にはDATAで初期値を与えられません。
:MSGATTR. 0
 A blank or unnamed COMMON block is a COMMON statement with the block
 name omitted.
 The entries in blank COMMON blocks cannot be initialized using DATA
 statements.
:MSGSYM. DA_HEX_CONST
:MSGTXT. data initialization with hexadecimal constant is not FORTRAN 77 standard
:MSGJTXT. FORTRAN 77標準では16進数で初期値は与えられません。
:MSGATTR. 2
 Data initialization with hexadecimal constants is an extension to the
 FORTRAN 77 language.
:MSGSYM. DA_ILL_NAME
:MSGTXT. cannot initialize %s1 %s2
:MSGJTXT. DATAで%s1 %s2には初期値を与えられません。
:MSGATTR. 0
 Symbol %s2 was used as a %s1.
 It is illegal for such a symbol to be initialized in a DATA statement.
 The DATA statement can only be used to initialize variables, arrays,
 array elements, and substrings.
:MSGSYM. DA_IN_TYPE_STMT
:MSGTXT. data initialization in %s1 statement is not FORTRAN 77 standard
:MSGJTXT. FORTRAN 77標準では%s1ｽﾃｰﾄﾒﾝﾄで初期値を与えられません。
:MSGATTR. 2
 Data initialization in type specification statements is an extension
 to the FORTRAN 77 language.
 These include: CHARACTER, COMPLEX, DOUBLE PRECISION, INTEGER,
 LOGICAL, and REAL.
:MSGSYM. DA_NOT_ENOUGH
:MSGTXT. not enough constants for list of variables
:MSGJTXT. DATAの中の変数に対応する定数の数が足りません。
:MSGATTR. 0
 There are not enough constants specified to initialize all of the
 names listed in the DATA statement.
:MSGSYM. DA_TOO_MUCH
:MSGTXT. too many constants for list of variables
:MSGJTXT. 変数に対して定数の数が多過ぎます。
:MSGATTR. 0
 There are too many constants specified to initialize the names listed
 in the DATA statement.
:MSGSYM. DA_TYPE_MISMATCH
:MSGTXT. cannot initialize %s1 variable %s2 with %s3 constant
:MSGJTXT. %s1変数に%s2定数を初期値として入れられません。
:MSGATTR. 0
 The constant of type %s3 cannot be used to initialize the variable %s2 of
 type %s1.
:MSGSYM. DA_DATA_TWICE
:MSGTXT. entity can only be initialized once during data initialization
:MSGJTXT. DATAの中で1個の変数に1度以上、初期値を与えてはいけません。
:MSGATTR. 0
 An attempt has been made to initialize an entity more than once in
 DATA statements.
:eMSGGRP. DA
:cmt -------------------------------------------------------------------
:MSGGRP. DM
:MSGGRPSTR. DM-
:MSGGRPTXT. Dimensioned Variables
:cmt -------------------------------------------------------------------
:MSGSYM. DM_SYM_PARM
:MSGTXT. using %s1 incorrectly in dimension expression
:MSGJTXT. 次元の定義式には%s1型は使えません。整数式のみです。
:MSGATTR. 2
 The name used as a dimension declarator has been previously declared
 as type %s1 and cannot be used as a dimension declarator. A dimension
 declarator must be an integer expression.
:MSGSYM. DM_RANGE_ERR_ON_DIMN
:MSGTXT. array or array element (possibly substring) associated with %s1 too small
:MSGJTXT. 配列または配列要素(文字列の一部など)と思われる%s1のｻｲｽﾞが小さすぎます。
:MSGATTR. 0
 The dummy argument, array %s1, is defined to be larger than the size
 of the actual argument.
:eMSGGRP. DM
:cmt -------------------------------------------------------------------
:MSGGRP. DO
:MSGGRPSTR. DO-
:MSGGRPTXT. DO-loops
:cmt -------------------------------------------------------------------
:MSGSYM. DO_BACKWARDS_DO
:MSGTXT. statement number %i1 already defined in line %d2 - DO loop is backwards
:MSGJTXT. DOの終りを示す文番号%i1は既に%d2行目で定義されています。終わり文番号は、DO文より後ろです。
:MSGATTR. 2
 The statement number to indicate the end of the DO control structure
 has been used previously in the program unit and cannot be used to
 terminate the DO loop.
 The terminal statement named in the DO statement must follow the DO
 statement.
:MSGSYM. DO_ENDING_BAD
:MSGTXT. %s1 statement not allowed at termination of DO range
:MSGJTXT. %s1文はDO範囲の終りとして使えません。
:MSGATTR. 0
 A non-executable statement cannot be used as the terminal statement
 of a DO loop.
 These statements include: all declarative statements,
 ADMIT, AT END, BLOCK DATA, CASE, DO, ELSE, ELSE IF, END, END AT END,
 END BLOCK, END GUESS, END IF, END LOOP, END SELECT, END WHILE, ENTRY,
 FORMAT, FUNCTION, assigned GO TO, unconditional GO TO, GUESS,
 arithmetic and block IF, LOOP, OTHERWISE, PROGRAM, RETURN, SAVE,
 SELECT, STOP, SUBROUTINE, UNTIL, and WHILE.
:MSGSYM. DO_NESTING_BAD
:MSGTXT. improper nesting of DO loop
:MSGJTXT. 外側のDOﾙｰﾌﾟが終わる前に内側のDOﾙｰﾌﾟは正しく終わらなければなりません。
:MSGATTR. 0
 A nested DO loop has not been properly terminated before the
 termination of the outer DO loop.
:MSGSYM. DO_BAD_ENDDO
:MSGTXT. ENDDO cannot terminate DO loop with statement label
:MSGJTXT. ENDDOではﾗﾍﾞﾙ付きのDOﾙｰﾌﾟを終えることはできません。
:MSGATTR. 0
 The ENDDO statement can only terminate a DO loop in which no
 statement label was specified in the defining DO statement.
:MSGSYM. DO_DO_EXT
:MSGTXT. this DO loop form is not FORTRAN 77 standard
:MSGJTXT. このDOﾙｰﾌﾟの形式はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 As an extension to FORTRAN 77, the following forms of the DO loop are
 also supported.
 .autonote
 .note
 A DO loop with no statement label specified in the defining DO
 statement.
 .note
 The DO WHILE form of the DO statement.
 .endnote
:MSGSYM. DO_NO_COMMA_OR_VAR
:MSGTXT. expecting comma or DO variable
:MSGJTXT. DOと終了を示す番号行の次はｺﾝﾏかDO変数がなければなりません。
:MSGATTR. 1
 The item following the DO keyword and the terminal statement-label
 (if present) must be either a comma or a DO variable.
 A DO variable is an integer, real or double precision variable name.
 The DO statement syntax is as follows:
 .millust begin
 DO <tsl> <,> DO-var = ex, ex <, ex>
 .millust end
:MSGSYM. DO_PARM_REDEFINED
:MSGTXT. DO variable cannot be redefined while DO loop is active
:MSGJTXT. DOﾙｰﾌﾟの中でDO変数の値を実行中に変えてはいけません。
:MSGATTR. 2
 The DO variable named in the DO statement cannot have its value
 altered by a statement in the DO loop structure.
:MSGSYM. DO_0INC
:MSGTXT. incrementation parameter for DO-loop cannot be zero
:MSGJTXT. DOﾙｰﾌﾟの増分にｾﾞﾛを指定してはいけません。
:MSGATTR. 0
 The third expression in the DO statement cannot be zero.
 This expression indicates the increment to the DO variable each
 iteration of the DO loop.
 If the increment expression is not specified a value of 1 is assumed.
:eMSGGRP. DO
:cmt -------------------------------------------------------------------
:MSGGRP. EC
:MSGGRPSTR. EC-
:MSGGRPTXT. Equivalence and/or Common
:cmt -------------------------------------------------------------------
:MSGSYM. EC_PAST_BEG
:MSGTXT. equivalencing %s1 has caused extension of COMMON block %s2 to the left
:MSGJTXT. EQUIVALENCEされた%s1はCOMMONﾌﾞﾛｯｸ%s2を左側へ拡張しました。FORTRAN 77標準では、禁止されている方法です。
:MSGATTR. 0
 The name %s1 has been equivalenced to a name in the COMMON block %s2.
 This relationship has caused the storage of the COMMON block to be
 extended to the left.
 FORTRAN 77 does not allow a COMMON block to be extended in this way.
:MSGSYM. EC_2NAM_EC
:MSGTXT. %s1 and %s2 in COMMON are equivalenced to each other
:MSGJTXT. 同じCOMMONﾌﾞﾛｯｸの内にある%s1と%s2は互いにEQUIVALENCEできません。
:MSGATTR. 0
 The names %s1 and %s2 appear in different COMMON blocks and each
 occupies its own piece of storage and therefore cannot be
 equivalenced.
:eMSGGRP. EC
:cmt -------------------------------------------------------------------
:MSGGRP. EN
:MSGGRPSTR. EN-
:MSGGRPTXT. END Statement
:cmt -------------------------------------------------------------------
:MSGSYM. EN_NO_END
:MSGTXT. missing END statement
:MSGJTXT. END文がありません。
:MSGATTR. 0
 The END statement for a PROGRAM, SUBROUTINE, FUNCTION or BLOCK DATA subprogram
 was not found before the next subprogram or the end of the source input
 stream.
:eMSGGRP. EN
:cmt -------------------------------------------------------------------
:MSGGRP. EQ
:MSGGRPSTR. EQ-
:MSGGRPTXT. Equal Sign
:cmt -------------------------------------------------------------------
:MSGSYM. EQ_BAD_TARGET
:MSGTXT. target of assignment is illegal
:MSGJTXT. 代入のﾀｰｹﾞｯﾄが間違っています。
:MSGATTR. 1
 The target of an assignment statement, an input/output status specifier
 in an input/output statement, or an inquiry specifier in an INQUIRE
 statement, is illegal.
 The target in any of the above cases must be a variable name, array element,
 or a substring name.
:MSGSYM. EQ_CANNOT_ASSIGN
:MSGTXT. cannot assign value to %s1
:MSGJTXT. 代入文の左側の%s1ﾀｲﾌﾟに値を割り当てることはできません。
:MSGATTR. 2
 An attempt has been made to assign a value to a symbol with class %s1.
 For example, an array name cannot be the target of an assignment
 statement.
 This error may also be issued when an illegal target is used for the
 input/output status specifier in an input/output statement or an inquiry
 specifier in an INQUIRE statement.
:MSGSYM. EQ_ILL_EQ_SIGN
:MSGTXT. illegal use of equal sign
:MSGJTXT. =の使い方がまちがっています(代入文ではありません)。
:MSGATTR. 1
 An equal sign has been found in the statement but the statement is
 not an assignment statement.
:MSGSYM. EQ_MULT_ASSGN
:MSGTXT. multiple assignment is not FORTRAN 77 standard
:MSGJTXT. 複数の代入用 = の使用はFORTRAN 77標準ではありません。
:MSGATTR. 0
 More than one equal sign has been found in the assignment statement.
:MSGSYM. EQ_NO_EQUALS
:MSGTXT. expecting equals sign
:MSGJTXT. 等号(=)が必要です。
:MSGATTR. 1
 The equal sign is missing or misplaced.
 The PARAMETER statement uses an equal sign to equate a symbolic name
 to the value of a constant expression.
 The I/O statements use an equal sign to equate the appropriate values
 to the various specifiers.
 The DO statement uses an equal sign to assign the initial value to
 the DO variable.
:eMSGGRP. EQ
:cmt -------------------------------------------------------------------
:MSGGRP. EV
:MSGGRPSTR. EV-
:MSGGRPTXT. Equivalenced Variables
:cmt -------------------------------------------------------------------
:MSGSYM. EV_DIFF_REL_POS
:MSGTXT. %s1 has been equivalenced to 2 different relative positions
:MSGJTXT. %s1を2つの異なる場所にEQUIVALENCEしてはいけません。
:MSGATTR. 0
 The storage unit referenced by %s1 has been equivalenced to two
 different storage units starting in two different places.
 One name cannot be associated to two different values at the same
 time.
:MSGSYM. EV_EQUIV_LIST
:MSGTXT. EQUIVALENCE list must contain at least 2 names
:MSGJTXT. EQUIVALENCEには少なくとも2つのｼﾝﾎﾞﾙ名が対で必要です。
:MSGATTR. 0
 The list of names to make a storage unit equivalent to several names
 must contain at least two names.
:MSGSYM. EV_SSCR_INVALID
:MSGTXT. %s1 incorrectly subscripted in %s2 statement
:MSGJTXT. %s1が%s2文の中で間違って添字を付けられています。
:MSGATTR. 0
 The name %s1 has been incorrectly subscripted in a %s2 statement.
:MSGSYM. EV_SSTR_INVALID
:MSGTXT. incorrect substring of %s1 in %s2 statement
:MSGJTXT. %s1は%s2文の中で間違った部分列となっています。
:MSGATTR. 0
 An attempt has been made to incorrectly substring %s1 in a %s2
 statement.
 For example, if a CHARACTER variable was declared to be of length 4
 then (2:5) would be an invalid substring expression.
:MSGSYM. EV_MIXED_EQUIV
:MSGTXT. equivalencing CHARACTER and non-CHARACTER data is not FORTRAN 77 standard
:MSGJTXT. FORTRAN 77の標準では文字と他のﾀｲﾌﾟのﾃﾞｰﾀをEQUIVALENCEできません。
:MSGATTR. 0
 Equivalencing numeric and character data is an extension to the
 FORTRAN 77 language.
:MSGSYM. EV_ONLY_IF_CHAR
:MSGTXT. attempt to substring %s1 in EQUIVALENCE statement but type is %s2
:MSGJTXT. EQUIVALENCEで部分列を示す%s1は%s2ですが文字型の必要があります。
:MSGATTR. 0
 An attempt has been made to substring the symbolic name %s1 in an
 EQUIVALENCE statement but the type of the name is %s2 and should be
 of type CHARACTER.
:eMSGGRP. EV
:cmt -------------------------------------------------------------------
:MSGGRP. EX
:MSGGRPSTR. EX-
:MSGGRPTXT. Exponentiation
:cmt -------------------------------------------------------------------
:MSGSYM. EX_Z_2_NOT_POS
:MSGTXT. zero**J where J <= 0 is not allowed
:MSGJTXT. J <= O の時 ZERO**J の計算はできません。
:MSGATTR. 2
 Zero cannot be raised to a power less than or equal to zero.
:MSGSYM. EX_NOT_INT_ARG
:MSGTXT. X**Y where X < 0.0, Y is not of type INTEGER, is not allowed
:MSGJTXT. X < 0.0 の時,Yが整数でなければ X**Y の計算はできません。
:MSGATTR. 0
 When X is less than zero, Y may only be of type INTEGER.
:MSGSYM. EX_CZ_2_NOT_REAL
:MSGTXT. (0,0)**Y where Y is not real is not allowed
:MSGJTXT. Yが実数でなければ (0,0)**Y の計算はできません。
:MSGATTR. 0
 In complex exponentiation, when the base is zero, the exponent may only be a
 real number or a complex number whose imaginary part is zero.
:eMSGGRP. EX
:cmt -------------------------------------------------------------------
:MSGGRP. EY
:MSGGRPSTR. EY-
:MSGGRPTXT. ENTRY Statement
:cmt -------------------------------------------------------------------
:MSGSYM. EY_TYPE_MISMATCH
:MSGTXT. type of entry %s1 does not match type of function %s2
:MSGJTXT. ENTRY %s1の型が,関数%s2の型と一致しません。
:MSGATTR. 0
 If the type of a function is CHARACTER or a user-defined STRUCTURE, then the
 type of all entry names must match the type of the function name.
:MSGSYM. EY_NOT_IN_CS
:MSGTXT. ENTRY statement not allowed within structured control blocks
:MSGJTXT. SELECT CASEやREMOTE BLOCKのような構造化ﾌﾞﾛｯｸ･ｽﾃｰﾄﾒﾝﾄの中にENTRYは使えません。
:MSGATTR. 0
 FORTRAN 77 does not allow an ENTRY statement to appear between the
 start and end of a control structure.
:MSGSYM. EY_SIZE_MISMATCH
:MSGTXT. size of entry %s1 does not match size of function %s2
:MSGJTXT. ENTRY %s1と関数名%s2は同じ長さでなければなりません。
:MSGATTR. 0
 The name %s1 found in an ENTRY statement must be declared to be the
 same size as that of the function name.
 If the name of the function or the name of any entry point has a
 length specification of (*), then all such entries must have a length
 specification of (*) otherwise they must all have a length
 specification of the same integer value.
:eMSGGRP. EY
:cmt -------------------------------------------------------------------
:MSGGRP. FM
:MSGGRPSTR. FM-
:MSGGRPTXT. Format
:cmt -------------------------------------------------------------------
:MSGSYM. FM_ASSUME_COMMA
:MSGTXT. missing delimiter in format string, comma assumed
:MSGJTXT. FORMATの編集記述子を区分するものがありません。ｺﾝﾏを区分するために挿入しました。
:MSGATTR. 2
 The omission of a comma between the descriptors listed in a format
 string is an extension to the FORTRAN 77 language.
 Care should be taken when omitting the comma since the assumed
 separation may not occur in the intended place.
:MSGSYM. FM_CONST
:MSGTXT. missing or invalid constant
:MSGJTXT. 編集記述子の1部である定数が無いか、形式が正しくありません。
:MSGATTR. 2
 An unsigned integer constant was expected with the indicated edit
 descriptor but was not correctly placed or was missing.
:MSGSYM. FM_D_EXT
:MSGTXT. Ew.dDe format code is not FORTRAN 77 standard
:MSGJTXT. Ew.dDe形式の編集記述子はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 The edit descriptor Ew.dDe is an extension to the FORTRAN 77
 language.
:MSGSYM. FM_DECIMAL
:MSGTXT. missing decimal point
:MSGJTXT. 小数点が必要です。
:MSGATTR. 2
 The indicated edit descriptor must have a decimal point and an
 integer to indicate the number of decimal positions.
 These edit descriptors include: F, E, D and G.
:MSGSYM. FM_DELIM
:MSGTXT. missing or invalid edit descriptor in format string
:MSGJTXT. FORMATの編集記述子が無いか、書き方が正しくありません。
:MSGATTR. 2
 In the format string, two delimiters were found in succession with no
 valid descriptor in between.
:MSGSYM. FM_FMTCHAR
:MSGTXT. unrecognizable edit descriptor in format string
:MSGJTXT. FORMATの中に間違った編集記述子があります。
:MSGATTR. 2
 An edit descriptor has been found in the format string that is not a
 valid code.
 Valid codes are: apostrophe ('), I, F, E, D, G, L, A, Z, H, T, TL,
 TR, X, /, :, S, SP, SS, P, BN, B, $, and \.
:MSGSYM. FM_INV_REP
:MSGTXT. invalid repeat specification
:MSGJTXT. 繰り返し指定が正しくありません。
:MSGATTR. 2
 The indicated repeatable edit descriptor is invalid.
 The forms of repeatable edit descriptors are:
 Iw, Iw.m, Fw.d, Ew.d, Ew.dEe, Dw.d, Gw.d, Gw.dEe, Lw, A, Aw, Ew.dDe,
 and Zw where w and e are positive unsigned integer constants, and d
 and m are unsigned integer constants.
:MSGSYM. FM_M_EXT
:MSGTXT. $ or \ format code is not FORTRAN 77 standard
:MSGJTXT. $や\のﾌｫｰﾏｯﾄｺｰﾄﾞはFORTRAN 77の標準ではありません。
:MSGATTR. 0
 The non-repeatable edit descriptors $ and \ are extensions to the
 FORTRAN 77 language.
:MSGSYM. FM_MODIFIER
:MSGTXT. invalid field modifier
:MSGJTXT. 変数などに対する編集記述子が正しくありません。
:MSGATTR. 2
 The indicated edit descriptor for a field is incorrect.
 Consult the Language Reference for the correct form of the edit descriptor.
:MSGSYM. FM_NO_EOS
:MSGTXT. expecting end of FORMAT statement but found more text
:MSGJTXT. FORMATの中に終りを示す)がありますがさらに右に記述が続いています。
:MSGATTR. 2
 The right parenthesis was encountered in the FORMAT statement to
 terminate the statement and more text was found on the line.
:MSGSYM. FM_NO_REP
:MSGTXT. repeat specification not allowed for this format code
:MSGJTXT. この編集記述子には繰返し指定は付けられません。
:MSGATTR. 2
 A repeat specification was found in front of a format code that is a
 nonrepeatable edit descriptor.
 These include: apostrophe, H, T, TL, TR, X, /, :, S, SP, SS, P, BN,
 BZ, $,and \.
:MSGSYM. FM_NO_STMTNO
:MSGTXT. no statement number on FORMAT statement
:MSGJTXT. FORMAT文に文ﾗﾍﾞﾙが必要です。
:MSGATTR. 0
 The FORMAT statement must have a statement label.
 This statement number is used by I/O statements to reference the
 FORMAT statement.
:MSGSYM. FM_QUOTE
:MSGTXT. no closing quote on apostrophe edit descriptor
:MSGJTXT. 引用符('')編集記述子を閉じるためのもう1個の引用符がありません。
:MSGATTR. 2
 The closing quote of an apostrophe edit descriptor was not found.
:MSGSYM. FM_SPEC_256
:MSGTXT. field count greater than 256 is invalid
:MSGJTXT. 編集記述子でﾌｨｰﾙﾄﾞの長さは256を越えてはいけません。
:MSGATTR. 2
 The repeat specification of the indicated edit descriptor is greater
 than the maximum allowed of 256.
:MSGSYM. FM_WIDTH
:MSGTXT. invalid field width specification
:MSGJTXT. 幅(長さ)の指定が正しくありません。
:MSGATTR. 2
 The width specifier on the indicated edit descriptor is invalid.
:MSGSYM. FM_Z_EXT
:MSGTXT. Z format code is not FORTRAN 77 standard
:MSGJTXT. Z変数記述子はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 The Z (hexadecimal format) repeatable edit descriptor is an extension
 to the FORTRAN 77 language.
:MSGSYM. FM_TOO_LARGE
:MSGTXT. FORMAT statement exceeds allotted storage size
:MSGJTXT. 1個のFORMAT文が大きすぎます。分割してください。
:MSGATTR. 0
 The maximum allowable size of a FORMAT statement has exceeded.
 The statement must be split into two or more FORMAT statements.
:MSGSYM. FM_NOT_INP
:MSGTXT. format specification not allowed on input
:MSGJTXT. 入力用としてFORMATの中の編集記述子は使えません。
:MSGATTR. 0
 A format specification, in the FORMAT statement, is not allowed to be
 used as an input specification.
 Valid specifications include: T, TL,TR, X, /, :, P, BN, BZ, I, F, E,
 D, G, L, A, and Z.
:MSGSYM. FM_REP
:MSGTXT. FORMAT missing repeatable edit descriptor
:MSGJTXT. I/O用としてFORMATの中の書式仕様に繰り返し指定が必要です。
:MSGATTR. 0
 An attempt has been made to read or write a piece of data without a
 valid repeatable edit descriptor.
 All data requires a repeatable edit descriptor in the format.
 The forms of repeatable edit descriptors are:
 Iw, Iw.m, Fw.d, Ew.d, Ew.dEe, Dw.d, Gw.d, Gw.dEe, Lw, A, Aw, Ew.dDe,
 and Zw where w and e are positive unsigned integer constants, and d
 and m are unsigned integer constants.
:MSGSYM. FM_ASSUME_CONST
:MSGTXT. missing constant before X edit descriptor, 1 assumed
:MSGJTXT. X編集記述子の前に定数がありません。1 を挿入しました。
:MSGATTR. 2
 The omission of the constant before an X edit descriptor in a format
 specification is an extension to the FORTRAN 77 language.
:MSGSYM. FM_Q_EXT
:MSGTXT. Ew.dQe format code is not FORTRAN 77 standard
:MSGJTXT. Ew.dQe形式の編集記述子はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 The edit descriptor Ew.dQe is an extension to the FORTRAN 77
 language.
:MSGSYM. FM_Q_FORMAT
:MSGTXT. Qw.d format code is not FORTRAN 77 standard
:MSGJTXT. Qw.d形式の編集記述子はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 The edit descriptor Qw.d is an extension to the FORTRAN 77
 language.
:eMSGGRP. FM
:cmt -------------------------------------------------------------------
:MSGGRP. GO
:MSGGRPSTR. GO-
:MSGGRPTXT. GOTO and ASSIGN Statements
:cmt -------------------------------------------------------------------
:MSGSYM. GO_ASSIGNED_BAD
:MSGTXT. %s1 statement label may not appear in ASSIGN statement but did in line %d2
:MSGJTXT. %s1文ﾗﾍﾞﾙはASSIGN文に使えません。%d2行をﾁｪｯｸしてください。
:MSGATTR. 0
 The statement label in the ASSIGN statement in line %d2 references a
 %s1 statement.
 The statement label in the ASSIGN statement must appear in the same
 program unit and must be that of an executable statement or a FORMAT
 statement.
:MSGSYM. GO_CANNOT_ASSIGN
:MSGTXT. ASSIGN of statement number %i1 in line %d2 not allowed
:MSGJTXT. ASSIGNの示す文番号%i1は実行できない行を指しています。%d2行をﾁｪｯｸしてください。
:MSGATTR. 2
 The statement label %d1 in the ASSIGN statement is used in the line
 %d2 which references a non-executable statement.
 A statement label must appear in the same program unit as the ASSIGN
 statement and must be that of an executable statement or a FORMAT
 statement.
:MSGSYM. GO_NO_TO
:MSGTXT. expecting TO
:MSGJTXT. ASSIGNのTOが無いか位置が間違っています。
:MSGATTR. 2
 The keyword TO is missing or misplaced in the ASSIGN statement.
:eMSGGRP. GO
:cmt -------------------------------------------------------------------
:MSGGRP. HO
:MSGGRPSTR. HO-
:MSGGRPTXT. Hollerith Constants
:cmt -------------------------------------------------------------------
:MSGSYM. HO_CONST
:MSGTXT. hollerith constant is not FORTRAN 77 standard
:MSGJTXT. ﾎﾚﾘｽ定数はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 Hollerith constants are an extension to the FORTRAN 77 language.
:MSGSYM. HO_CUT_OFF
:MSGTXT. not enough characters for hollerith constant
:MSGJTXT. 編集記述子のHかhの前の数(hollerith定数)に対してそれに続く文字が足りません。
:MSGATTR. 0
 The number of characters following the H or h is not equal to the
 constant preceding the H or h.
 A hollerith constant consists of a positive unsigned integer constant
 n followed by the letter H or h followed by a string of exactly n
 characters.
:eMSGGRP. HO
:cmt -------------------------------------------------------------------
:MSGGRP. IF
:MSGGRPSTR. IF-
:MSGGRPTXT. IF Statements
:cmt -------------------------------------------------------------------
:MSGSYM. IF_ELSE_LAST
:MSGTXT. ELSE block must be the last block in block IF
:MSGJTXT. ELSEﾌﾞﾛｯｸの後にまたELSE IFﾌﾞﾛｯｸが続いてはいけません。これがIFブロックの最後のﾌﾞﾛｯｸでなけれなりません。
:MSGATTR. 0
 Another ELSE IF block has been found after the ELSE block.
 The ELSE block must be the last block in an IF block.
 The form of the block IF is as follows:
 .millust begin
 IF (logical expression) THEN [:block-label]
          {statement}
 { ELSE IF
          {statement} }
 [ELSE
          {statement}  ]
 ENDIF
 .millust end
:MSGSYM. IF_NO_THEN
:MSGTXT. expecting THEN
:MSGJTXT. IFにはTHENが必要です。
:MSGATTR. 1
 The keyword THEN is missing or misplaced in the block IF statement.
 The form of the block IF is as follows:
 .millust begin
 IF (logical expression) THEN [:block-label]
          {statement}
 { ELSE IF
          {statement} }
 [ELSE
          {statement}  ]
 ENDIF
 .millust end
:eMSGGRP. IF
:cmt -------------------------------------------------------------------
:MSGGRP. IL
:MSGGRPSTR. IL-
:MSGGRPTXT. I/O Lists
:cmt -------------------------------------------------------------------
:MSGSYM. IL_BAD_FMT_SPEC
:MSGTXT. missing or invalid format/FMT specification
:MSGJTXT. FORMAT/FMT仕様がないか、あるいは正しくありません。
:MSGATTR. 2
 A valid format specification is required on all READ and WRITE
 statements.
 The format specification is specified by:
 .millust begin
 [FMT=] <format identifier>
 .millust end
 .pc
 .sy <format identifier>
 is one of the following: statement label, integer variable-name,
 character array-name, character expression, or *.
:MSGSYM. IL_BAD_INTL
:MSGTXT. the UNIT may not be an internal file for this statement
:MSGJTXT. この文でUNITに内部ﾌｧｲﾙを指定してはいけません(READ,WRITEのみです)。
:MSGATTR. 2
 An internal file may only be referenced in a READ or WRITE statement.
 An internal file may not be referenced in a BACKSPACE, CLOSE,
 ENDFILE, INQUIRE, OPEN, or REWIND statement.
:MSGSYM. IL_BAD_LIST
:MSGTXT. %s1 statement cannot have %s2 specification
:MSGJTXT. %s1文に%s2仕様は指定できません。
:MSGATTR. 2
 The I/O statement %s1 may not have the control information %s2
 specified.
:MSGSYM. IL_BAD_SIZE
:MSGTXT. variable must have a size of 4
:MSGJTXT. この変数の長さは4でなければなりません。
:MSGATTR. 2
 The variable used as a specifier in an I/O statement must be of size
 4 but another size was specified.
 These include the EXIST, OPENED, RECL, IOSTAT, NEXTREC, and NUMBER.
 The name used in the ASSIGN statement must also be of size 4 but a
 different size was specified.
:MSGSYM. IL_CTRL_LIST
:MSGTXT. missing or unrecognizable control list item %s1
:MSGJTXT. I/Oを制御する %s1項目に不足があるか,間違ったものがあります。
:MSGATTR. 2
 A control list item %s1 was encountered in an I/O statement and is not
 a valid control list item for that statement, or a control list item
 was expected and was not found.
:MSGSYM. IL_DUP_LIST
:MSGTXT. attempt to specify control list item %s1 more than once
:MSGJTXT. このI/Oを制御するための項目%s1は重複して指定できません。
:MSGATTR. 2
 The control list item %s1 in the indicated I/O statement, has been
 named more than once.
:MSGSYM. IL_EMPTY_IMP_DO
:MSGTXT. implied DO loop has no input/output list
:MSGJTXT. DO形並びのﾙｰﾌﾟが、I/Oリストの変数や式に対応していません。
:MSGATTR. 1
 The implied DO loop specified in the I/O statement does not
 correspond with a variable or expression in the input/output list.
:MSGSYM. IL_ILST
:MSGTXT. list-directed input/output with internal files is not FORTRAN 77 standard
:MSGJTXT. 内部ﾌｧｲﾙへの並びによる入出力処理はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 List-directed input/output with internal files is an extension to the
 FORTRAN 77 language.
:MSGSYM. IL_NO_ASTERISK
:MSGTXT. FORTRAN 77 standard requires an asterisk for list-directed formatting
:MSGJTXT. FORTRAN 77標準の場合,並びによる書式には(*)ｱｽﾀﾘｽｸが必要です。
:MSGATTR. 2
 An optional asterisk for list-directed formatting is an extension to
 the FORTRAN 77 language.
 The standard FORTRAN 77 language specifies that an asterisk is required.
:MSGSYM. IL_NO_UNIT_ID
:MSGTXT. missing or improper unit identification
:MSGJTXT. UNITが無いか,そのﾌｧｲﾙ識別子(ﾌｧｲﾙ番号)が正しくありません。
:MSGATTR. 0
 The control specifier, UNIT, in the I/O statement is either missing
 or identifies an improper unit.
 The unit specifier specifies an external unit or internal file.
 The external unit identifier is a non-negative integer expression or
 an asterisk.
 The internal file identifier is character variable, character array,
 character array element, or character substring.
:MSGSYM. IL_NO_FILE_OR_UNIT
:MSGTXT. missing unit identification or file specification
:MSGJTXT. UNITかFILEの指定がないか、間違っています。
:MSGATTR. 0
 An identifier to specifically identify the required file is missing.
 The UNIT specifier is used to identify the external unit or internal
 file.
 The FILE specifier in the INQUIRE and OPEN statements is used to
 identify the file name.
:MSGSYM. IL_STAR_NOT_ALLOWED
:MSGTXT. asterisk unit identifier not allowed in %s1 statement
:MSGJTXT. %s1文ではｱｽﾀﾘｽｸ(*)でなく,ﾌｧｲﾙ識別子(ﾌｧｲﾙ番号)が必要です。
:MSGATTR. 1
 The BACKSPACE, CLOSE, ENDFILE, INQUIRE, OPEN, and REWIND statements
 require the external unit identifier be an unsigned positive integer
 from 0 to 999.
:MSGSYM. IL_UNIT_AND_FILE
:MSGTXT. cannot have both UNIT and FILE specifier
:MSGJTXT. UNITとFILEの両方は同時に指定できません。
:MSGATTR. 0
 There are two valid forms of the INQUIRE statement; INQUIRE by FILE
 and INQUIRE by UNIT.
 Both of these specifiers cannot be specified in the same statement.
:MSGSYM. IL_AINTL
:MSGTXT. internal files require sequential access
:MSGJTXT. 内部ﾌｧｲﾙは順次ｱｸｾｽ処理でなければなりません。
:MSGATTR. 0
 An attempt has been made to randomly access an internal file.
 Internal files may only be accessed sequentially.
:MSGSYM. IL_END_REC
:MSGTXT. END specifier with REC specifier is not FORTRAN 77 standard
:MSGJTXT. RECを伴うENDはFORTRAN 77の標準ではありません。
:MSGATTR. 0
 The FORTRAN 77 standard specifies that an end-of-file condition can only
 occur with a file connected for sequential access or an internal file.
 The REC specifier indicates that the file is connected for direct access.
 This extension allows the programmer to detect an end-of-file condition when
 reading the records sequentially from a file connected for direct access.
:MSGSYM. IL_SPECIFIER_NOT_STANDARD
:MSGTXT. %s1 specifier in i/o list is not FORTRAN 77 standard
:MSGJTXT. 入出力並び中の%s1指定子はFORTRAN 77標準ではありません。
:MSGATTR. 0
 The specified i/o list item is provided as an extension to the
 FORTRAN 77 language.
:MSGSYM. IL_NO_IOLIST
:MSGTXT. i/o list is not allowed with NAMELIST-directed format
:MSGJTXT. NAMELIST指向の書式では,入出力並びは許されません。
:MSGATTR. 0
 An i/o list is not allowed when the format specification is a NAMELIST.
:MSGSYM. IL_NON_CHARACTER
:MSGTXT. non-character array as format specifier is not FORTRAN 77 standard
:MSGJTXT. 書式指定に文字以外の配列を使用するのは,FORTRAN 77の標準ではありません。
:MSGATTR. 2
 A format specifier must be of type character unless it is an array name.
 Allowing a non-character array name is an extension to the FORTRAN 77 standard.
:eMSGGRP. IL
:cmt -------------------------------------------------------------------
:MSGGRP. IM
:MSGGRPSTR. IM-
:MSGGRPTXT. IMPLICIT Statements
:cmt -------------------------------------------------------------------
:MSGSYM. IM_ILLEGAL_RANGE
:MSGTXT. illegal range of characters
:MSGJTXT. 範囲を指定する文字の順序が正しくありません。
:MSGATTR. 2
 In the IMPLICIT statement, the first letter in the range of
 characters must be smaller in the collating sequence than
 the second letter in the range.
:MSGSYM. IM_PREV_IMPLICIT
:MSGTXT. letter can only be implicitly declared once
:MSGJTXT. IMPLICITで同じ文字を重複して、指定してはいけません。
:MSGATTR. 2
 The indicated letter has been named more than once in this or a
 previous IMPLICIT statement.
 A letter may only be named once.
:MSGSYM. IM_UNRECOG_TYPE
:MSGTXT. unrecognizable type
:MSGJTXT. IMPLICITの指定された型が違います。
:MSGATTR. 2
 The type declared in the IMPLICIT statement is not one of INTEGER,
 REAL, DOUBLE PRECISION, COMPLEX, LOGICAL or CHARACTER.
:MSGSYM. IM_CHAR_STAR_STAR
:MSGTXT. (*) length specifier in an IMPLICIT statement is not FORTRAN 77 standard
:MSGJTXT. IMPLICIT文の中で(*)の長さの指定はFORTRAN 77の標準ではありません。
:MSGATTR. 1
 A character length specified of (*) in an IMPLICIT statement is an
 extension to the FORTRAN 77 language.
:MSGSYM. IM_NONE_USED
:MSGTXT. IMPLICIT NONE allowed once or not allowed with other IMPLICIT statements
:MSGJTXT. IMPLICIT NONEを指定すると他のどんなIMPLICITも書いてはいけません。
:MSGATTR. 0
 The IMPLICIT NONE statement must be the only IMPLICIT statement in
 the program unit in which it appears.
 Only one IMPLICIT NONE statement is allowed in a program unit.
:eMSGGRP. IM
:cmt -------------------------------------------------------------------
:MSGGRP. IO
:MSGGRPSTR. IO-
:MSGGRPTXT. Input/Output
:cmt -------------------------------------------------------------------
:MSGSYM. IO_ABACK
:MSGTXT. BACKSPACE statement requires sequential access mode
:MSGJTXT. 順次ｱｸｾｽでなければBACKSPACEは指定できません。
:MSGATTR. 0
 The file connected to the unit specified in the BACKSPACE statement
 has not been opened for sequential access.
:MSGSYM. IO_ACTIVE_ALREADY
:MSGTXT. input/output is already active
:MSGJTXT. I/Oの実行中に別なI/Oは実行できません。
:MSGATTR. 0
 An attempt has been made to read or write a record when there is an
 already active read or write in progress.
 The execution of a READ or WRITE statement has caused transfer to a
 function that contains a READ or WRITE statement.
:MSGSYM. IO_AEND
:MSGTXT. ENDFILE statement requires sequential access mode
:MSGJTXT. ENDFILEは順次ｱｸｾｽ処理でなければ指定できません。
:MSGATTR. 0
 The specified external unit identifier must be connected for
 sequential access but was connected for direct access.
:MSGSYM. IO_AF1
:MSGTXT. formatted connection requires formatted input/output statements
:MSGJTXT. OPENでFORM='FORMATTED'の時はREAD/WRITEはFORMATTED形式でなければなりません。
:MSGATTR. 0
 The FORM specifier in the OPEN statement specifies FORMATTED and the
 subsequent READ and/or WRITE statement does not use formatted I/O.
 If the FORM specifier has been omitted and access is SEQUENTIAL then
 FORMATTED is assumed.
 If the access is DIRECT then UNFORMATTED is assumed.
:MSGSYM. IO_AF2
:MSGTXT. unformatted connection requires unformatted input/output statements
:MSGJTXT. OPENでFORM='UNFORMATTED'の時はREAD/WRITEはUNFORMATTED形式でなければなりません。
:MSGATTR. 0
 The FORM specifier in the OPEN statement specifies UNFORMATTED and
 the subsequent READ and/or WRITE statement uses formatted I/O.
 If the FORM specifier has been omitted and access is SEQUENTIAL then
 FORMATTED is assumed.
 If the access is DIRECT then UNFORMATTED is assumed.
:MSGSYM. IO_AREW
:MSGTXT. REWIND statement requires sequential access
:MSGJTXT. REWIND文は順次ｱｸｾｽ処理でなければ指定できません。
:MSGATTR. 0
 The external unit identifier is not connected to a sequential file.
 The REWIND statement positions to the first record in the file.
:MSGSYM. IO_BAD_CHAR
:MSGTXT. bad character in input field
:MSGJTXT. 入力ﾃﾞｰﾀに書式に合わない文字が入っています。
:MSGATTR. 0
 The data received from the record in a file does not match the type
 of the input list item.
:MSGSYM. IO_BLNK_FMT
:MSGTXT. BLANK specifier requires FORM specifier to be 'FORMATTED'
:MSGJTXT. BLANKはFORM='FORMATTED'でなければ指定できません。
:MSGATTR. 0
 In the OPEN statement, the BLANK specifier may only be used when the
 FORM specifier has the value of FORMATTED.
 The BLANK specifier indicates whether blanks are treated as zeroes or
 ignored.
:MSGSYM. IO_FILE_PROBLEM
:MSGTXT. system file error - %s1
:MSGJTXT. ｼｽﾃﾑﾌｧｲﾙ･I/Oｴﾗｰです。-%s1
:MSGATTR. 0
 A system error has occurred while attempting to access a file.
 The I/O system error message is displayed.
:MSGSYM. IO_FMT_MISMATCH
:MSGTXT. format specification does not match data type
:MSGJTXT. 書式の中でﾃﾞ-ﾀ仕様とﾃﾞｰﾀ型がちがっています。
:MSGATTR. 0
 A format specification in the FMT specifier or FORMAT statement
 specifies data of one type and the variable list specifies data of a
 different type.
:MSGSYM. IO_FREE_MISMATCH
:MSGTXT. input item does not match the data type of list variable
:MSGJTXT. 入力の中で実際のﾃﾞ-ﾀと変数の型が合っていません。
:MSGATTR. 0
 In the READ statement, the data type of a variable listed is not of
 the same data type in the data file.
 For example, non-digit character data being read into an integer
 item.
:MSGSYM. IO_IFULL
:MSGTXT. internal file is full
:MSGJTXT. 内部ﾌｧｲﾙが一杯です。
:MSGATTR. 0
 The internal file is full of data.
 If a file is a variable then the file may only contain one record.
 If the file is a character array then there can be one record for
 each array element.
:MSGSYM. IO_IRECL
:MSGTXT. RECL specifier is invalid
:MSGJTXT. RECL(ﾚｺ-ﾄﾞの長さ)は正の整数/整数式でなければなりません。
:MSGATTR. 0
 In the OPEN statement, the record length specifier must be a positive
 integer expression.
:MSGSYM. IO_ISTAT
:MSGTXT. invalid STATUS specifier in CLOSE statement
:MSGJTXT. CLOSEのSTATUSが正しくありません(OPENとCLOSEのSTATUSが不一致です)。
:MSGATTR. 0
 The STATUS specifier can only have a value of KEEP or DELETE.
 If the STATUS in the OPEN statement is SCRATCH then the KEEP status
 on the CLOSE statement cannot be used.
:MSGSYM. IO_NOT_CONNECTED
:MSGTXT. unit specified is not connected
:MSGJTXT. UNITのﾌｧｲﾙ識別子(ﾌｧｲﾙ番号)が正しくﾌｧｲﾙに関係つけられていません。
:MSGATTR. 0
 The unit number specified in the I/O statement has not been
 previously connected.
:MSGSYM. IO_PAST_EOF
:MSGTXT. attempt to perform data transfer past end of file
:MSGJTXT. END FILEの後に更にﾃﾞｰﾀの読み書きをしてはいけません。
:MSGATTR. 0
 An attempt has been made to read or write data after the end of file
 has been read or written.
:MSGSYM. IO_RACCM
:MSGTXT. invalid RECL specifier/ACCESS specifier combination
:MSGJTXT. ACCESSがDIRECTの時はRECLが必要です。
:MSGATTR. 0
 In the OPEN statement, if the ACCESS specifier is DIRECT then the
 RECL specifier must be given.
:MSGSYM. IO_REC1_ACCM
:MSGTXT. REC specifier required in direct access input/output statements
:MSGJTXT. 直接ｱｸｾｽ･I/O文の中にはRECの指定が必要です。
:MSGATTR. 0
 In the OPEN statement, the ACCESS  specified was DIRECT.
 All subsequent input/output statements for that file must use the REC
 specifier to indicate which record to access.
:MSGSYM. IO_REC2_ACCM
:MSGTXT. REC specifier not allowed in sequential access input/output statements
:MSGJTXT. RECは順次ｱｸｾｽ･I/O文の中に指定してはいけません。
:MSGATTR. 0
 In the OPEN statement, the ACCESS specified was SEQUENTIAL.
 The REC specifier may not be used in subsequent I/O statements for
 that file.
 The REC specifier is used to indicate which record to access when
 access is DIRECT.
:MSGSYM. IO_SUBSEQUENT_OPEN
:MSGTXT. %s1 specifier may not change in a subsequent OPEN statement
:MSGJTXT. 同一ﾌｧｲﾙに対してOPEN後,別のOPENで%s1の指定を変更してはいけません。
:MSGATTR. 0
 The %s1 specifier may not be changed on a subsequent OPEN statement
 for the same file, in the same program.
 Only the BLANK specifier may be changed.
:MSGSYM. IO_SFILE
:MSGTXT. invalid STATUS specifier for given file
:MSGJTXT. STATUSの指定と実際のﾌｧｲﾙの状態が合っていません。
:MSGATTR. 0
 In the OPEN statement, the STATUS specifier does not match with the
 actual file status: OLD means the file must exist, NEW means the file
 must not exist.
 If the STATUS specifier is omitted, UNKNOWN is assumed.
:MSGSYM. IO_SNAME
:MSGTXT. invalid STATUS specifier/FILE specifier combination
:MSGJTXT. STATUSとFILEの指定内容が食い違っています。
:MSGATTR. 0
 In the OPEN statement, if the STATUS is SCRATCH, the FILE specifier
 cannot be used.
 If the STATUS is NEW or OLD, the FILE specifier must be given.
:MSGSYM. IO_UNFMT_RECL
:MSGTXT. record size exceeded during unformatted input/output
:MSGJTXT. UNFORMATTED I/Oでレコードの大きさが最大長を超えました。
:MSGATTR. 0
 This error is issued when the size of an i/o list item exceeds the
 maximum record size of the file.
 The record size can be specified using the RECL= specified in the OPEN
 statement.
:MSGSYM. IO_UNIT_EXIST
:MSGTXT. unit specified does not exist
:MSGJTXT. UNITのﾌｧｲﾙ識別子にﾌｧｲﾙが接続されていません。
:MSGATTR. 0
 The external unit identifier specified in the input/output statement
 has not yet been connected.
 Use preconnection or the OPEN statement to connect a file to the
 external unit identifier.
:MSGSYM. IO_IREC
:MSGTXT. REC specifier is invalid
:MSGJTXT. RECは符号なしの正の整数でなければなりません。
:MSGATTR. 0
 The REC specifier must be an unsigned positive integer.
:MSGSYM. IO_IUNIT
:MSGTXT. UNIT specifier is invalid
:MSGJTXT. UNITの識別子(ﾌｧｲﾙ番号)が正しくありません。
:MSGATTR. 0
 The UNIT specifier must be an unsigned integer between 0 and 999
 inclusive.
:MSGSYM. IO_BUFF_LEN
:MSGTXT. formatted record or format edit descriptor is too large for record size
:MSGJTXT. 書式つきﾚｺｰﾄﾞまたは書式で指定した編集記述子の長さがﾚｺｰﾄﾞｻｲｽﾞより大きすぎます。
:MSGATTR. 0
 This error is issued when the amount of formatted data in a READ, WRITE or
 PRINT statement exceeds the maximum record size of the file.
 The record size can be specified using the RECL= specified in the OPEN
 statement.
:MSGSYM. IO_BAD_SPEC
:MSGTXT. illegal '%s1=' specifier
:MSGJTXT. %s1= の指定が正しくありません。
:MSGATTR. 0
 In the OPEN or CLOSE statement the value associated with the %s1
 specifier is not a valid value.
 In the OPEN statement,
 STATUS may only be one of OLD, NEW, SCRATCH or UNKNOWN;
 ACCESS may only be one of SEQUENTIAL, APPEND or DIRECT;
 FORM may only be one of FORMATTED or UNFORMATTED;
 CARRIAGECONTROL may only be one of YES or NO;
 RECORDTYPE may only be one of FIXED, TEXT or VARIABLE;
 ACTION may only be one of READ, WRITE or READ/WRITE; and
 BLANK may only be one of NULL, or ZERO.
 In the CLOSE statement the STATUS may only be one of KEEP or DELETE.
:MSGSYM. IO_CC_FORM
:MSGTXT. invalid CARRIAGECONTROL specifier/FORM specifier combination
:MSGJTXT. CARRIAGECONTROLとFORM指定子の組合せが無効です。
:MSGATTR. 0
 The CARRIAGECONTROL specifier is only allowed with formatted i/o statements.
:MSGSYM. IO_BAD_ACTION
:MSGTXT. i/o operation not consistent with file attributes
:MSGJTXT. 指定されたﾌｧｲﾙ属性では,この入出力操作は許されません。
:MSGATTR. 0
 An attempt was made to read from a file that was opened with
 ACTION=WRITE or write to a file that was opened with ACTION=READ.
 This message is also issued if you attempt to write to a read-only file or
 read from a write-only file.
:MSGSYM. IO_NML_NO_SUCH_NAME
:MSGTXT. symbol %s1 not found in NAMELIST
:MSGJTXT. NAMELIST中でｼﾝﾎﾞﾙ%s1が見つかりません。
:MSGATTR. 0
 During NAMELIST-directed input, a symbol was specified that does not belong
 to the NAMELIST group specified in the i/o statement.
:MSGSYM. IO_NML_BAD_SYNTAX
:MSGTXT. syntax error during NAMELIST-directed input
:MSGJTXT. NAMELIST指向の入力で,ｼﾝﾀｯｸｽｴﾗｰがあります。
:MSGATTR. 0
 Bad input was encountered during NAMELIST-directed input.
 Data must be in a special form during NAMELIST-directed input.
:MSGSYM. IO_NML_BAD_SUBSCRIPT
:MSGTXT. subscripting error during NAMELIST-directed i/o
:MSGJTXT. NAMELIST指向の入出力で,添字にｴﾗｰがあります。
:MSGATTR. 0
 An array was incorrectly subscripted during NAMELIST-directed input.
:MSGSYM. IO_NML_BAD_SUBSTRING
:MSGTXT. substring error during NAMELIST-directed i/o
:MSGJTXT. NAMELIST指向の入出力で,部分文字列にｴﾗｰがあります。
:MSGATTR. 0
 An character array element or variable was incorrectly substring during
 NAMELIST-directed input.
:MSGSYM. IO_IBLOCKSIZE
:MSGTXT. BLOCKSIZE specifier is invalid
:MSGJTXT. BLOCKSIZE が正しくありません。
:MSGATTR. 0
 In the OPEN statement, the block size specifier must be a positive
 integer expression.
:MSGSYM. IO_NO_RECORDS
:MSGTXT. invalid operation for files with no record structure
:MSGJTXT. ﾚｺｰﾄﾞ構造をもたないﾌｧｲﾙに対して操作が誤っています。
:MSGATTR. 0
 An attempt has been made to perform an i/o operation on a file that requires
 a record structure.
 For example, it is illegal to use a BACKSPACE statement for a file that
 has no record structure.
:MSGSYM. IO_IOVERFLOW
:MSGTXT. integer overflow converting character data to integer
:MSGJTXT. 文字ﾃﾞｰﾀを整数に変換するとき整数が桁あふれしました。
:MSGATTR. 0
 An overflow has occurred while converting the character data read to its
 internal representation as an integer.
:MSGSYM. IO_FRANGE_EXCEEDED
:MSGTXT. range exceeded converting character data to floating-point
:MSGJTXT. 文字ﾃﾞｰﾀを浮動小数点数に変換したとき範囲を越えました。
:MSGATTR. 0
 An overflow or underflow has occurred while converting the character data
 read to its internal representation as a floating-point number.
:eMSGGRP. IO
:cmt -------------------------------------------------------------------
:MSGGRP. KO
:MSGGRPSTR. KO-
:MSGGRPTXT. Program Termination
:cmt -------------------------------------------------------------------
:MSGSYM. KO_FDIV_ZERO
:MSGTXT. floating-point divide by zero
:MSGJTXT. 浮動小数点数はｾﾞﾛで割ることはできません。
:MSGATTR. 0
 An attempt has been made to divide a number by zero in a
 floating-point expression.
:MSGSYM. KO_FOVERFLOW
:MSGTXT. floating-point overflow
:MSGJTXT. 浮動小数点数が桁あふれしました。
:MSGATTR. 0
 The floating-point expression result has exceeded the maximum
 floating-point number.
:MSGSYM. KO_FUNDERFLOW
:MSGTXT. floating-point underflow
:MSGJTXT. 浮動小数点数が指数下位桁あふれをしました。
:MSGATTR. 0
 The floating-point expression result has exceeded the minimum
 floating-point number.
:MSGSYM. KO_IDIV_ZERO
:MSGTXT. integer divide by zero
:MSGJTXT. 整数はｾﾞﾛで割れません。
:MSGATTR. 0
 An attempt has been made to divide a number by zero in an integer
 expression.
:MSGSYM. KO_INTERRUPT
:MSGTXT. program interrupted from keyboard
:MSGJTXT. 実行を中断しました。
:MSGATTR. 0
 The user has interrupted the compilation or execution of a program
 through use of the keyboard.
:MSGSYM. KO_IOVERFLOW
:MSGTXT. integer overflow
:MSGJTXT. 整数が桁あふれしました。
:MSGATTR. 1
 The integer expression result has exceeded the maximum integer
 number.
:MSGSYM. KO_PAGES_OUT
:MSGTXT. maximum pages of output exceeded
:MSGJTXT. 印刷ﾍﾟｰｼﾞ数がｺﾝﾊﾟｲﾗ･ｵﾌﾟｼｮﾝPAGESで指定した最高値をこえました。
:MSGATTR. 0
 The specified maximum number of output pages has been exceeded.
 The maximum number of output pages can be increased
 by using the "pages=n" option in the command line
 or specifying C$PAGES=n in the source file.
:MSGSYM. KO_STMT_CNT
:MSGTXT. statement count has been exceeded
:MSGJTXT. ﾌﾟﾛｸﾞﾗﾑの行数がｺﾝﾊﾟｲﾗ･ｵﾌﾟｼｮﾝSTATEMENTSで指定した最高値を超えました。
:MSGATTR. 0
 The maximum number of source statements has been executed.
 The maximum number of source statements that can be executed can be
 increased by using the "statements=n" option in the command line or
 specifying C$STATEMENTS=n in the source file.
:MSGSYM. KO_TIME_OUT
:MSGTXT. time limit exceeded
:MSGJTXT. 時間切れです(実行を中止します)。
:MSGATTR. 0
 The maximum amount of time for program execution has been exceeded.
 The maximum amount of time can be increased
 by using the "time=t" option in the command line or specifying C$TIME=t
 in the source file.
:eMSGGRP. KO
:cmt -------------------------------------------------------------------
:MSGGRP. LI
:MSGGRPSTR. LI-
:MSGGRPTXT. Library Routines
:cmt -------------------------------------------------------------------
:MSGSYM. LI_ARG_GT_ZERO
:MSGTXT. argument must be greater than zero
:MSGJTXT. 引数は1個以上でなければなりません。
:MSGATTR. 0
 The argument to the intrinsic function must be greater than zero
 (i.e., a positive number).
:MSGSYM. LI_ARG_LE_ONE
:MSGTXT. absolute value of argument to arcsine, arccosine must not exceed one
:MSGJTXT. ASIN,ACOSの引数の絶対値は1を超えてはいけません。
:MSGATTR. 0
 The absolute value of the argument to the intrinsic function ASIN or
 ACOS cannot be greater than or equal to the value 1.0.
:MSGSYM. LI_ARG_NEG
:MSGTXT. argument must not be negative
:MSGJTXT. 組込み関数に対する引数はﾏｲﾅｽではいけません。
:MSGATTR. 0
 The argument to the intrinsic function must be greater than or equal
 to zero.
:MSGSYM. LI_ARG_ZERO
:MSGTXT. argument(s) must not be zero
:MSGJTXT. 組込み関数に対する引数はｾﾞﾛではいけません。
:MSGATTR. 0
 The argument(s) to the intrinsic function must not be zero.
:MSGSYM. LI_CHAR_BOUND
:MSGTXT. argument of CHAR must be in the range zero to 255
:MSGJTXT. CHARに対する引数の値は0から255の間でなければなりません。
:MSGATTR. 0
 The argument to the intrinsic function CHAR must be in the range 0 to
 255 inclusive.
 CHAR returns the character represented by an 8-bit pattern.
:MSGSYM. LI_2_CMPLX
:MSGTXT. %s1 intrinsic function cannot be passed 2 complex arguments
:MSGJTXT. %s1組込み関数の第2引数に複素数を指定できません。
:MSGATTR. 2
 The second argument to the intrinsic function CMPLX and DCMPLX cannot
 be a complex number.
:MSGSYM. LI_CMPLX_TYPES
:MSGTXT. argument types must be the same for the %s1 intrinsic function
:MSGJTXT. %s1組込み関数の第2引数の型は第1引数の型と同じでなければなりません。
:MSGATTR. 2
 The second argument to the intrinsic function CMPLX or DCMPLX must be
 of the same type as the first argument.
 The second argument may only be used when the first argument is of
 type INTEGER, REAL or DOUBLE PRECISION.
:MSGSYM. LI_EXP_CNV_TYPE
:MSGTXT. expecting numeric argument, but %s1 argument was found
:MSGJTXT. 数値型の引数が必要ですが%s1型が指定されています。
:MSGATTR. 2
 The argument to the intrinsic function, INT, REAL, DBLE, CMPLX, or
 DCMPLX was of type %s1 and a numeric argument was expected.
:MSGSYM. LI_ICHAR_ONE
:MSGTXT. length of ICHAR argument greater than one
:MSGJTXT. ICHAR引数は文字型で長さは1でなければなりません。
:MSGATTR. 0
 The length of the argument to the intrinsic function ICHAR must be of
 type CHARACTER and length of 1.
 ICHAR converts a character to its integer representation.
:MSGSYM. LI_ILL_PARM_CODE
:MSGTXT. cannot pass %s1 as argument to intrinsic function
:MSGJTXT. 組込み関数の引数として%s1を指定してはいけません。
:MSGATTR. 2
 The item %s1 cannot be used as an argument to an intrinsic function.
 Only constants, simple variables, array elements, and substring array
 elements may be used as arguments.
:MSGSYM. LI_NO_PARM
:MSGTXT. intrinsic function requires argument(s)
:MSGJTXT. 組込み関数には引数が必要です。
:MSGATTR. 2
 An attempt has been made to invoke an intrinsic function and no
 actual arguments were listed.
:MSGSYM. LI_NO_SPECIFIC
:MSGTXT. %s1 argument type is invalid for this generic function
:MSGJTXT. この総称関数に対して%s1の引数型は指定できません。
:MSGATTR. 2
 The type of the argument used in the generic intrinsic function is
 not correct.
:MSGSYM. LI_NOT_PARM
:MSGTXT. this intrinsic function cannot be passed as an argument
:MSGJTXT. 組込み関数名は実引数として指定できません。
:MSGATTR. 2
 Only the specific name of the intrinsic function can be used as an
 actual argument.
 The generic name may not be used.
 When the generic and intrinsic names are the same, use the INTRINSIC
 statement.
:MSGSYM. LI_PT_MISMATCH
:MSGTXT. expecting %s1 argument, but %s2 argument was found
:MSGJTXT. %s1型の引数が必要ですが%s2が指定されています。
:MSGATTR. 2
 An argument of type %s2 was passed to a function but an argument of
 type %s1 was expected.
:MSGSYM. LI_WRONG_TYPE
:MSGTXT. intrinsic function was assigned wrong type
:MSGJTXT. 定義された組込み関数の型と呼び出しの型は一致しなければなりません。
:MSGATTR. 2
 The declared type of an intrinsic function does not agree with the
 actual type.
:MSGSYM. LI_IF_NOT_STANDARD
:MSGTXT. intrinsic function %s1 is not FORTRAN 77 standard
:MSGJTXT. 組込み関数%s1はFORTRAN 77の標準ではありません。
:MSGATTR. 2
 The specified intrinsic function is provided as an extension to the
 FORTRAN 77 language.
:MSGSYM. LI_ARG_ALLOCATED
:MSGTXT. argument to ALLOCATED intrinsic function must be an allocatable array or character*(*) variable
:MSGJTXT. ALLOCATED組込み関数の引数は、割り付け可能な配列またはcharacter*(*)型の変数でなければなりません。
:MSGATTR. 2
 The argument to the intrinsic function ALLOCATED must be an allocatable
 array or character*(*) variable.
:MSGSYM. LI_ARG_ISIZEOF
:MSGTXT. invalid argument to ISIZEOF intrinsic function
:MSGJTXT. 組込み関数ISIZEOFの引数が間違っています。
:MSGATTR. 2
 The argument to the intrinsic function ISIZEOF must be a user-defined
 structure name, a symbol name, or a constant.
:eMSGGRP. LI
:cmt -------------------------------------------------------------------
:MSGGRP. MD
:MSGGRPSTR. MD-
:MSGGRPTXT. Mixed Mode
:cmt -------------------------------------------------------------------
:MSGSYM. MD_BAD_REL_OPN
:MSGTXT. relational operator has a logical operand
:MSGJTXT. 関係演算子(.GT.,.LT.など)が論理演算子(.OR.,.AND.)として使われてはいけません。
:MSGATTR. 1
 The operands of a relational expression must either be both
 arithmetic or both character expressions.
 The operand indicated is a logical expression.
:MSGSYM. MD_DBLE_WITH_CMPLX
:MSGTXT. mixing DOUBLE PRECISION and COMPLEX types is not FORTRAN 77 standard
:MSGJTXT. 倍精度と複素数型を式に同時に書くことはFORTRAN 77の標準ではありません。
:MSGATTR. 1
 The mixing of items of type DOUBLE PRECISION and COMPLEX in an
 expression is an extension to the FORTRAN 77 language.
:MSGSYM. MD_ILL_OPR
:MSGTXT. operator not expecting %s1 operands
:MSGJTXT. この演算子は%s1型の要素を扱えません。
:MSGATTR. 1
 Operands of type %s1 cannot be used with the indicated operator.
 The operators **, /, *, +, and &minus. may only have numeric type data.
 The operator // may only have character type data.
:MSGSYM. MD_MIXED
:MSGTXT. operator not expecting %s1 and %s2 operands
:MSGJTXT. この演算子は%s1型と%s2型の要素を扱えません。
:MSGATTR. 1
 Operands of conflicting type have been encountered.
 For example, in a relational expression, it is not possible to compare a
 character expression to an arithmetic expression.
 Also, the type of the left hand operand of the field selection operator must
 be a user-defined structure.
:MSGSYM. MD_RELOP_OPND_COMPLEX
:MSGTXT. complex quantities can only be compared using .EQ. or .NE.
:MSGJTXT. 複素数の比較には.EQ.と.NE.しか使えません。
:MSGATTR. 1
 Complex operands cannot be compared using less than (.LT.), less than
 or equal (.LE.), greater than (.GT.), or greater than or equal (.GE.)
 operators.
:MSGSYM. MD_UNARY_OP
:MSGTXT. unary operator not expecting %s1 type
:MSGJTXT. 単項演算子(1個で作用する+,-,.NOT.など)は%s1型に使えません。
:MSGATTR. 2
 The unary operators, + and &minus., may only be used with numeric types.
 The unary operator .NOT. may be used only with a logical or integer operand.
 The indicated operand was of type %s1 which is not one of the valid types.
:MSGSYM. MD_LOGOPR_INTOPN
:MSGTXT. logical operator with integer operands is not FORTRAN 77 standard
:MSGJTXT. 整数要素に対する論理演算子はFORTRAN 77の標準ではありません。
:MSGATTR. 1
 Integer operands are permitted with the logical
 operators .AND., .OR., .EQV., .NEQV., .NOT. and .XOR. as an extension to the
 FORTRAN 77 language.
:MSGSYM. MD_LOGOPR_EXTN
:MSGTXT. logical operator %s1 is not FORTRAN 77 standard
:MSGJTXT. 論理演算子%s1はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 The specified logical operator is an extension to the FORTRAN 77 standard.
:eMSGGRP. MD
:cmt -------------------------------------------------------------------
:MSGGRP. MO
:MSGGRPSTR. MO-
:MSGGRPTXT. Memory Overflow
:cmt -------------------------------------------------------------------
:MSGSYM. MO_COMPILER_LIMIT
:MSGTXT. %s1 exceeds compiler limit of %u2 bytes
:MSGJTXT. %s1の大きさがｺﾝﾊﾟｲﾗ限度である%u2ﾊﾞｲﾄ(文字)を超えてはいけません。
:MSGATTR. 0
 An internal compiler limit has been reached.
 %s1 describes the limit and %d2 specifies the limit.
:MSGSYM. MO_DYNAMIC_OUT
:MSGTXT. out of memory
:MSGJTXT. メモリ不足です。
:MSGATTR. 0
 All available memory has been used up.
 During the compilation phase, memory is primarily used for the symbol table.
 During execution, memory is used for file descriptors and buffers,
 and dynamically allocatable arrays and character*(*) variables.
:MSGSYM. MO_LIST_TOO_BIG
:MSGTXT. dynamic memory exhausted due to length of this statement - statement ignored
:MSGJTXT. この文が長いために、動的ﾒﾓﾘが不足しました。ｰこの文を無視します。
:MSGATTR. 0
 There was not enough memory to encode the specified statement.
 This message is usually issued when the compiler is low on memory or if the
 statement is a very large statement that spans many continuation lines.
 This error does not terminate the compiler since it may have been caused by
 a very large statement.
 The compiler attempts to compile the remaining statements.
:MSGSYM. MO_STORAGE_NOT_ALLOCATED
:MSGTXT. attempt to deallocate an unallocated array or character*(*) variable
:MSGJTXT. ﾒﾓﾘを割り付けた配列あるいはcharacter*(*)型の変数以外のメモリは解放できません。
:MSGATTR. 0
 An attempt has been made to deallocate an array that has not been previously
 allocated.
 An array or character*(*) variable must be allocated using an ALLOCATE
 statement.
:MSGSYM. MO_STORAGE_ALLOCATED
:MSGTXT. attempt to allocate an already allocated array or character*(*) variable
:MSGJTXT. 既にﾒﾓﾘを割り付けた配列あるいはcharacter*(*)型の変数にはﾒﾓﾘを割り付けられません。
:MSGATTR. 0
 An attempt has been made to allocate an array or character*(*) variable that
 has been previously allocated in an ALLOCATE statement.
:MSGSYM. MO_OBJECT_OUT
:MSGTXT. object memory exhausted
:MSGJTXT. ﾌﾟﾛｸﾞﾗﾑのｵﾌﾞｼﾞｪｸﾄ･ｺｰﾄﾞが大きすぎて入りません。
:MSGATTR. 0
 The amount of object code generated for the program has exceeded the amount
 of memory allocated to store the object code.
 The "/codesize" option can be used to increase the amount of memory
 allocated for object code.
:eMSGGRP. MO
:cmt -------------------------------------------------------------------
:MSGGRP. PC
:MSGGRPSTR. PC-
:MSGGRPTXT. Parentheses
:cmt -------------------------------------------------------------------
:MSGSYM. PC_NO_CLOSEPAREN
:MSGTXT. missing or misplaced closing parenthesis
:MSGJTXT. 右かっこ')'がありません。
:MSGATTR. 1
 An opening parenthesis '(' was found but no matching closing
 parenthesis ')' was found before the end of the statement.
:MSGSYM. PC_NO_OPENPAREN
:MSGTXT. missing or misplaced opening parenthesis
:MSGJTXT. 左かっこ'('がありません。
:MSGATTR. 1
 A closing parenthesis ')' was found before the matching opening
 parenthesis '('.
:MSGSYM. PC_SURP_PAREN
:MSGTXT. unexpected parenthesis
:MSGJTXT. 必要のないかっこがあります。
:MSGATTR. 1
 A parenthesis was found in a statement where parentheses are not
 expected.
:MSGSYM. PC_UNMATCH_PAREN
:MSGTXT. unmatched parentheses
:MSGJTXT. 左右のかっこの組合せが合っていません。
:MSGATTR. 1
 The parentheses in the expression are not balanced.
:eMSGGRP. PC
:cmt -------------------------------------------------------------------
:MSGGRP. PR
:MSGGRPSTR. PR-
:MSGGRPTXT. PRAGMA Compiler Directive
:cmt -------------------------------------------------------------------
:MSGSYM. PR_SYMBOL_NAME
:MSGTXT. expecting symbolic name
:MSGJTXT. ｼﾝﾎﾞﾙ名を指定してください。
:MSGATTR. 0
 Every auxiliary pragma must refer to a symbol.
 This error is issued when the symbolic name is illegal or missing.
 Valid symbolic names are formed from the following characters: a dollar
 sign, an underscore, digits and any letter of the alphabet.
 The first character of a symbolic name must be alphabetic, a dollar sign, or
 an underscore.
:MSGSYM. PR_BAD_PARM_SIZE
:MSGTXT. illegal size specified for VALUE attribute
:MSGJTXT. VALUE属性に対するｻｲｽﾞ指定が間違っています。
:MSGATTR. 0
 The VALUE argument attribute of an auxiliary pragma contains in illegal
 length specification.
 Valid length specifications are 1, 2, 4 and 8.
:MSGSYM. PR_BAD_PARM_ATTR
:MSGTXT. illegal argument attribute
:MSGJTXT. 引数属性が間違っています。
:MSGATTR. 0
 An illegal argument attribute was specified.
 Valid argument attributes are VALUE, REFERENCE, or DATA_REFERENCE.
:MSGSYM. PR_BAD_CONTINUATION
:MSGTXT. continuation line must contain a comment character in column 1
:MSGJTXT. 継続行は1ｶﾗﾑ目にｺﾒﾝﾄ文字が必要です。
:MSGATTR. 0
 When continuing a line of an auxiliary pragma directive, the continued line
 must end with a back-slash ('\') character and the continuation line must
 begin with a comment character ('c', 'C' or '*') in column 1.
:MSGSYM. PR_BAD_SYNTAX
:MSGTXT. expecting '%s1' near '%s2'
:MSGJTXT. '%s1'が必要な所にありません。場所は'%s2'の近くです。
:MSGATTR. 0
 A syntax error was found while processing a PRAGMA directive.
 %s1 identifies the expected information and %s2 identifies where in the
 pragma the error occurred.
:MSGSYM. PR_BYTE_SEQ_LIMIT
:MSGTXT. in-line byte sequence limit exceeded
:MSGJTXT. ｲﾝﾗｲﾝ･ﾊﾞｲﾄｼｰｹﾝｽの制限を超えました。
:MSGATTR. 0
 The limit on the number of bytes of code that can be generated in-line using
 a an auxiliary pragma has been exceeded.
 The limit is 127 bytes.
:MSGSYM. PR_BAD_BYTE_SEQ
:MSGTXT. illegal hexadecimal data in byte sequence
:MSGJTXT. ﾊﾞｲﾄｼｰｹﾝｽ中に不正な16進ﾃﾞｰﾀがあります。
:MSGATTR. 0
 An illegal hexadecimal constant was encountered while processing a in-line
 byte sequence of an auxiliary pragma.
 Valid hexadecimal constants in an in-line byte sequence must begin with the
 letter Z or z and followed by a string of hexadecimal digits.
:MSGSYM. PR_SYMBOL_UNRESOLVED
:MSGTXT. symbol '%s1' in in-line assembly code cannot be resolved
:MSGJTXT. ｲﾝﾗｲﾝ･ｱｾﾝﾌﾞﾘ･ｺｰﾄﾞ内のｼﾝﾎﾞﾙ '%s1'は参照未解決です。
:MSGATTR. 0
 The symbol %s1, referenced in an assembly language instruction in an
 auxiliary pragma, could not be resolved.
:eMSGGRP. PR
:cmt -------------------------------------------------------------------
:MSGGRP. RE
:MSGGRPSTR. RE-
:MSGGRPTXT. RETURN Statement
:cmt -------------------------------------------------------------------
:MSGSYM. RE_ALT_IN_SUBROUTINE
:MSGTXT. alternate return specifier only allowed in subroutine
:MSGJTXT. 代替戻り指定子は、ｻﾌﾞﾙｰﾁﾝだけに許されます。
:MSGATTR. 2
 An alternate return specifier, in the RETURN statement, may only be
 specified when returning from a subroutine.
:MSGSYM. RE_IN_PROGRAM
:MSGTXT. RETURN statement in main program is not FORTRAN 77 standard
:MSGJTXT. ﾒｲﾝﾌﾟﾛｸﾞﾗﾑ中のRETURN文は,FORTRAN 77標準ではありません。
:MSGATTR. 0
 A RETURN statement in the main program is allowed as an extension to the
 FORTRAN 77 standard.
:eMSGGRP. RE
:cmt -------------------------------------------------------------------
:MSGGRP. SA
:MSGGRPSTR. SA-
:MSGGRPTXT. SAVE Statement
:cmt -------------------------------------------------------------------
:MSGSYM. SA_COMBLK_EMPTY
:MSGTXT. COMMON block %s1 saved but not properly defined
:MSGJTXT. COMMONﾌﾞﾛｯｸ%s1をSAVEされていますが、そのCOMMONﾌﾞﾛｯｸ名が定義されていません。
:MSGATTR. 0
 The named COMMON block %s1 was listed in a SAVE statement but there is
 no named COMMON block defined by that name.
:MSGSYM. SA_COMMON_NOT_SAVED
:MSGTXT. COMMON block %s1 must be saved in every subprogram in which it appears
:MSGJTXT. COMMONﾌﾞﾛｯｸ%s1は、そのCOMMONﾌﾞﾛｯｸがある全ての副ﾌﾟﾛｸﾞﾗﾑでSAVEしなければなりません。
:MSGATTR. 0
 The named COMMON block %s1 appears in a SAVE statement in another
 subprogram and is not in a SAVE statement in this subprogram.
 If a named COMMON block is specified in a SAVE statement in a
 subprogram, it must be specified in a SAVE statement in every
 subprogram in which that COMMON block appears.
:MSGSYM. SA_SAVED
:MSGTXT. name already appeared in a previous SAVE statement
:MSGJTXT. 同じﾌﾟﾛｸﾞﾗﾑの中で2度以上同じ名前でSAVEしてはいけません。
:MSGATTR. 2
 The indicated name has already been referenced in another SAVE
 statement in this subprogram.
:eMSGGRP. SA
:cmt -------------------------------------------------------------------
:MSGGRP. SF
:MSGGRPSTR. SF-
:MSGGRPTXT. Statement Functions
:cmt -------------------------------------------------------------------
:MSGSYM. SF_DUPLICATE_DUMMY_PARM
:MSGTXT. statement function definition contains duplicate dummy arguments
:MSGJTXT. 文関数の定義する仮引数が重複してはいけません。
:MSGATTR. 2
 A dummy argument is repeated in the argument list of the statement function.
:MSGSYM. SF_ILL_CHAR_LEN
:MSGTXT. character length of statement function name must not be (*)
:MSGJTXT. 文関数の文字数は(*)であってはいけません。 整数にしてください。
:MSGATTR. 2
 If the type of a character function is character, its length specification
 must not be (*); it must be a constant integer expression.
:MSGSYM. SF_ILL_DUMMY_PARM
:MSGTXT. statement function definition contains illegal dummy argument
:MSGJTXT. 文関数の文字型仮引数は(*)の長さを持っていてはいけません。
:MSGATTR. 2
 A dummy argument of type CHARACTER must have a length specification
 of an integer constant expression that is not (*).
:MSGSYM. SF_ILL_PARM_PASSED
:MSGTXT. cannot pass %s1 %s2 to statement function
:MSGJTXT. 間違った(*)長さで文字結合している%s1 %s2は文関数の実引数に指定できません。
:MSGATTR. 2
 The actual arguments to a statement function can be any expression
 except character expressions involving the concatenation of an
 operand whose length specification is (*) unless the operand is a
 symbolic constant.
:MSGSYM. SF_PARM_TYPE_MISMATCH
:MSGTXT. %s1 actual argument was passed to %s2 dummy argument
:MSGJTXT. 実引数の型%s1と仮引数の型%s2が合いません。
:MSGATTR. 2
 The indicated actual argument is of type %s1 which is not the same
 type as that of the dummy argument of type %s2.
:MSGSYM. SF_WRONG_NUM_PARMS
:MSGTXT. incorrect number of arguments passed to statement function %s1
:MSGJTXT. 文関数%s1に渡す引数の数が間違っています。
:MSGATTR. 2
 The number of arguments passed to statement function %s1 does not
 agree with the number of dummy arguments specified in its definition.
:MSGSYM. SF_ILL_TYPE
:MSGTXT. type of statement function name must not be a user-defined structure
:MSGJTXT. 文関数名の型にﾕｰｻﾞｰ定義の構造体を指定してはいけません。
:MSGATTR. 2
 The type of a statement function cannot be a user-defined structure.
 Valid types for statement functions are: LOGICAL*1, LOGICAL, INTEGER*1,
 INTEGER*2, INTEGER, REAL, DOUBLE PRECISION, COMPLEX, DOUBLE COMPLEX, and
 CHARACTER.
 If the statement function is of type CHARACTER, its length specification
 must not be (*); it must be an integer constant expression.
:eMSGGRP. SF
:cmt -------------------------------------------------------------------
:MSGGRP. SM
:MSGGRPSTR. SM-
:MSGGRPTXT. Source Management
:cmt -------------------------------------------------------------------
:MSGSYM. SM_IO_READ_ERR
:MSGTXT. system file error reading %s1 - %s2
:MSGJTXT. ｼｽﾃﾑ･ﾌｧｲﾙ読み込みｴﾗｰ: %s1 - %s2
:MSGATTR. 0
 An I/O error, described by %s2, has occurred while reading the
 FORTRAN source file %s1.
:MSGSYM. SM_OPENING_FILE
:MSGTXT. error opening file %s1 - %s2
:MSGJTXT. ﾌｧｲﾙｵｰﾌﾟﾝｴﾗｰ: %s1 - %s2
:MSGATTR. 0
 The FORTRAN source file %s1 could not be opened.
 The error is described by %s2.
:MSGSYM. SM_IO_WRITE_ERR
:MSGTXT. system file error writing %s1 - %s2
:MSGJTXT. ｼｽﾃﾑ･ﾌｧｲﾙ書出しｴﾗｰ: %s1 - %s2
:MSGATTR. 0
 An I/O error, described by %s2, has occurred while writing to the
 file %s1.
:MSGSYM. SM_SPAWN_ERR
:MSGTXT. error spawning %s1 - %s2
:MSGJTXT. spawnｴﾗｰ : %s1 - %s2
:MSGATTR. 0
 An error, described by %s2, occurred while trying to spawn the external
 program named %s1.
:MSGSYM. SM_LINK_ERR
:MSGTXT. error while linking
:MSGJTXT. ﾘﾝｸ時ｴﾗｰ
:MSGATTR. 0
 An error occurred while trying to create the executable file.  See the
 WLINK documentation for a description of the error.
:MSGSYM. SM_OUT_OF_VM_FILES
:MSGTXT. error opening %s1 - too many temporary files exist
:MSGJTXT. ﾌｧｲﾙｵｰﾌﾟﾝｴﾗｰ %s1 - ﾃﾝﾎﾟﾗﾘﾌｧｲﾙが多すぎます。
:MSGATTR. 0
 The compiler was not able to open a temporary file for intermediate storage
 during code generation.
 Temporary files are created in the directory specified by the TMP
 environment variable.
 If the TMP environment variable is not set, the temporary file is created
 in the current directory.
 This error is issued if an non-existent directory is specified in the TMP
 environment variable, or more than 26 concurrent compiles are taking place
 in a multi-tasking environment and the directory in which the temporary
 files are created is the same for all compilation processes.
:MSGSYM. SM_BROWSE_ERROR
:MSGTXT. generation of browsing information failed
:MSGJTXT. ﾌﾞﾗｳｽﾞ情報の生成に失敗しました。
:MSGATTR. 0
 An error occurred during the generation of browsing information.
 For example, a disk full condition encountered during the creation of the
 browser module file will cause this message to be issued.
 Browsing information is generated when the /db switch is specified.
:eMSGGRP. SM
:cmt -------------------------------------------------------------------
:MSGGRP. SP
:MSGGRPSTR. SP-
:MSGGRPTXT. Structured Programming Features
:cmt -------------------------------------------------------------------
:MSGSYM. SP_ATEND_AND_ENDEQUALS
:MSGTXT. cannot have both ATEND and the END= specifier
:MSGJTXT. READにAT ENDとEND=を同時に指定できません。
:MSGATTR. 0
 It is not valid to use the AT END control statement and the END=
 option on the READ statement.
 Only one method can be used to control the end-of-file condition.
:MSGSYM. SP_READ_NO_ATEND
:MSGTXT. ATEND must immediately follow a READ statement
:MSGJTXT. AT ENDはREADのすぐ後に来なければなりません。
:MSGATTR. 0
 The indicated AT END control statement or block does not immediately
 follow a READ statement.
 The AT END control statement or block is executed when an end-of-file
 condition is encountered during the read.
:MSGSYM. SP_BAD_LABEL
:MSGTXT. block label must be a symbolic name
:MSGJTXT. ﾌﾞﾛｯｸ･ﾗﾍﾞﾙはｼﾝﾎﾞﾙ名でなくてはなりません。
:MSGATTR. 2
 The indicated block label must be a symbolic name.
 A symbolic name must start with a letter and contain no more than 32
 letters and digits.
 A letter is an upper or lower case letter of the alphabet, a dollar sign ($),
 or an underscore (_).
 A digit is a character in the range '0' to '9'.
:MSGSYM. SP_BAD_QUIT
:MSGTXT. could not find a structure to %s1 from
:MSGJTXT. %s1への構造体を検出できません。
:MSGATTR. 0
 This message is issued in the following cases.
 .autonote
 .note
 There is no control structure to QUIT from.
 The QUIT statement will transfer control to the statement following the
 currently active control structure or return from a REMOTE BLOCK if no
 other control structures are active within the REMOTE BLOCK.
 .note
 There is no control structure to EXIT from.
 The EXIT statement is used to exit a loop-processing structure such as
 DO, DO WHILE, WHILE and LOOP, to return from a REMOTE BLOCK regardless of the
 number of active control structures within the REMOTE BLOCK, or to transfer
 control from a GUESS or ADMIT block to the statement following the ENDGUESS
 statement.
 .note
 There is no active looping control structure from which a CYCLE statement
 can be used.
 A CYCLE statement can only be used within a
 DO, DO WHILE, WHILE and LOOP control structure.
 .endnote
:MSGSYM. SP_BLK_IN_STRUCTURE
:MSGTXT. REMOTE BLOCK is not allowed in the range of any control structure
:MSGJTXT. 制御構造内部にREMOTE BLOCKを含めることはできません。
:MSGATTR. 0
 An attempt has been made to define a REMOTE BLOCK inside a control
 structure.
 Control structures include IF, LOOP, WHILE, DO, SELECT and GUESS.
 When a REMOTE BLOCK definition is encountered during execution,
 control is transferred to the statement following the corresponding
 END BLOCK statement.
:MSGSYM. SP_SELECT_THEN_CASE
:MSGTXT. the SELECT statement must be followed immediately by a CASE statement
:MSGJTXT. SELECTのすぐ後にCASEが来なければなりません。
:MSGATTR. 0
 The statement immediately after the SELECT statement must be a CASE
 statement.
 The SELECT statement allows one of a number of blocks of code (case
 blocks) to be selected for execution by means of an integer
 expression in the SELECT statement.
:MSGSYM. SP_CASE_OVERLAP
:MSGTXT. cases are overlapping
:MSGJTXT. CASEの指定子が重複しています。
:MSGATTR. 0
 The case lists specified in the CASE statements in the SELECT control
 structure are in conflict.
 Each case list must specify a unique integer constant expression or
 range.
:MSGSYM. SP_EMPTY_SELECT
:MSGTXT. select structure requires at least one CASE statement
:MSGJTXT. SELECTには少なくとも1つのCASE文が必要です。
:MSGATTR. 0
 In the SELECT control structure, there must be at least one CASE
 statement.
:MSGSYM. SP_FROM_OUTSIDE
:MSGTXT. cannot branch to %i1 from outside control structure in line %d2
:MSGJTXT. 制御構造の外部から%i1に%d2行目のように入ることはできません。
:MSGATTR. 0
 The statement in line %d2 passes control to the statement %d1 in a
 control structure.
 Control may only be passed out of a control structure or to another
 place in that control structure.
 Control structures include DO, GUESS, IF, LOOP, SELECT, and WHILE.
:MSGSYM. SP_INTO_STRUCTURE
:MSGTXT. cannot branch to %i1 inside structure on line %d2
:MSGJTXT. 制御構造内部の%i1(%d2行目)に飛ぶことはできません。
:MSGATTR. 0
 The statement attempts to pass control to statement %d1 in line %d2
 which is in a control structure.
 Control may only be passed out of a control structure or to another
 place in that control structure.
 Control structures include DO, GUESS, IF, LOOP, SELECT, and WHILE.
:MSGSYM. SP_NEVER_CASE
:MSGTXT. low end of range exceeds the high end
:MSGJTXT. 範囲の初めを示す番号が終りの番号より大きくてはいけません。
:MSGATTR. 0
 The first number, the low end of the range, is greater than the
 second number, the high end of the range.
:MSGSYM. SP_OTHERWISE_LAST
:MSGTXT. default case block must follow all case blocks
:MSGJTXT. ﾃﾞﾌｫﾙﾄのCASEﾌﾞﾛｯｸは全てのCASEﾌﾞﾛｯｸの後に来なくてはなりません。
:MSGATTR. 0
 The default case block in the SELECT control structure must be the last
 case block.
 A case block may not follow the default case block.
:MSGSYM. SP_OUT_OF_BLOCK
:MSGTXT. attempt to branch out of a REMOTE BLOCK
:MSGJTXT. REMOTE BLOCKから外に飛び出ることは許されません(END BLOCKで終らせてください)。
:MSGATTR. 0
 An attempt has been made to transfer execution control out of a
 REMOTE BLOCK.
 A REMOTE BLOCK may only be terminated with the END BLOCK statement.
 Execution of a REMOTE BLOCK is similar in concept to execution of a
 subroutine.
:MSGSYM. SP_RB_UNDEFINED
:MSGTXT. attempt to EXECUTE undefined REMOTE BLOCK %s1
:MSGJTXT. EXECUTEされるREMOTEﾌﾞﾛｯｸ %s1は同じﾌﾟﾛｸﾞﾗﾑの中になければなりません。
:MSGATTR. 0
 The REMOTE BLOCK %s1 referenced in the EXECUTE statement does not
 exist in the current program unit.
 A REMOTE BLOCK is local to the program unit in which it is defined
 and may not be referenced from another program unit.
:MSGSYM. SP_RECURSE
:MSGTXT. attempted to use REMOTE BLOCK recursively
:MSGJTXT. REMOTE BLOCK は再帰的に実行することはできません。
:MSGATTR. 0
 An attempt was made to execute a REMOTE BLOCK which was already
 active.
:MSGSYM. SP_RET_IN_REMOTE
:MSGTXT. cannot RETURN from subprogram within a REMOTE BLOCK
:MSGJTXT. 副ﾌﾟﾛｸﾞﾗﾑのREMOTE BLOCKの中でRETURNを実行できません。
:MSGATTR. 0
 An illegal attempt has been made to execute a RETURN statement within
 a REMOTE BLOCK in a subprogram.
:MSGSYM. SP_STRUCTURED_EXT
:MSGTXT. %s1 statement is not FORTRAN 77 standard
:MSGJTXT. %s1文はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 The statement %s1 is an extension to the FORTRAN 77 language.
:MSGSYM. SP_UNFINISHED
:MSGTXT. %s1 block is unfinished
:MSGJTXT. %s1ﾌﾞﾛｯｸを終了させる文がありません。
:MSGATTR. 0
 The block starting with the statement %s1 does not have the ending
 block statement.
 For example: ATENDDO-ENDATEND, DO-ENDDO, GUESS-ENDGUESS, IF-ENDIF,
 LOOP-ENDLOOP, SELECT-ENDSELECT, STRUCTURE-ENDSTRUCTURE and WHILE-ENDWHILE.
:MSGSYM. SP_UNMATCHED
:MSGTXT. %s1 statement does not match with %s2 statement
:MSGJTXT. 文%s1は文%s2と組合せられません。
:MSGATTR. 0
 The statement %s1, which ends a control structure, cannot be used with
 statement %s2 to form a control structure.
 Valid control structures are: LOOP - ENDLOOP, LOOP - UNTIL, WHILE -
 ENDWHILE, and WHILE - UNTIL.
:MSGSYM. SP_INCOMPLETE
:MSGTXT. incomplete control structure found at %s1 statement
:MSGJTXT. %s1に対応して制御構造の初めを示す文がありません。
:MSGATTR. 0
 The ending control structure statement %s1 was found and there was no
 preceding matching beginning statement.
 Valid control structures include: ATENDDO - ENDATEND, GUESS -
 ENDGUESS, IF - ENDIF, LOOP - ENDLOOP, REMOTE BLOCK - ENDBLOCK, and
 SELECT - ENDSELECT.
:MSGSYM. SP_NOT_IN_STRUCTURE
:MSGTXT. %s1 statement is not allowed in %s2 definition
:MSGJTXT. %s1文は%s2定義内では指定できません。
:MSGATTR. 0
 Statement %s1 is not allowed between a %s2 statement and the
 corresponding END %s2 statement.
 For example, an EXTERNAL statement is not allowed between a STRUCTURE and
 END STRUCTURE statement, a UNION and END UNION statement, or a MAP and
 END MAP statement.
:MSGSYM. SP_NO_SUCH_FIELD
:MSGTXT. no such field name found in structure %s1
:MSGJTXT. 構造体%s1の中に,このﾌｨｰﾙﾄﾞ名が見つかりません。
:MSGATTR. 2
 A structure reference contained a field name that does not belong to the
 specified structure.
:MSGSYM. SP_DUPLICATE_FIELD
:MSGTXT. multiple definition of field name %s1
:MSGJTXT. ﾌｨｰﾙﾄﾞ名%s1は重複して定義されています。
:MSGATTR. 2
 The field name %s1 has already been defined in a structure.
:MSGSYM. SP_UNDEF_STRUCT
:MSGTXT. structure %s1 has not been defined
:MSGJTXT. 構造体%s1は定義されていません。
:MSGATTR. 0
 An attempt has been made to declare a symbol of user-defined type %s1.
 No structure definition for %s1 has occurred.
:MSGSYM. SP_STRUCT_DEFINED
:MSGTXT. structure %s1 has already been defined
:MSGJTXT. 構造体%s1は既に定義されています。
:MSGATTR. 2
 The specified structure has already been defined as a structure.
:MSGSYM. SP_STRUCT_NEEDS_FIELD
:MSGTXT. structure %s1 must contain at least one field
:MSGJTXT. 構造体%s1には,少なくとも1つのﾌｨｰﾙﾄﾞが必要です。
:MSGATTR. 0
 Structures must contain at least one field definition.
:MSGSYM. SP_STRUCT_RECURSION
:MSGTXT. recursion detected in definition of structure %s1
:MSGJTXT. 構造体%s1の定義の中に,再帰定義があります。
:MSGATTR. 0
 Structure %s1 has been defined recursively.
 For example, it is illegal for structure X to contain a field that is itself
 a structure named X.
:MSGSYM. SP_STRUCT_HAS_UNION
:MSGTXT. illegal use of structure %s1 containing union
:MSGJTXT. 共用体を含む構造体%s1の使い方が間違っています。
:MSGATTR. 2
 Structures containing unions cannot be used in formatted I/O statements or
 data initialized.
:MSGSYM. SP_ALLOC_NOT_IN_STRUCT
:MSGTXT. allocatable arrays cannot be fields within structures
:MSGJTXT. 割り当て可能な配列は,構造体のﾌｨｰﾙﾄﾞにすることはできません。
:MSGATTR. 0
 An allocatable array cannot appear as a field name within a structure
 definition.
:MSGSYM. SP_INTEGER_CONDITION
:MSGTXT. an integer conditional expression is not FORTRAN 77 standard
:MSGJTXT. 整数条件式はFORTRAN 77の標準ではありません。
:MSGATTR. 2
 A conditional expression is the expression that is evaluated and checked to
 determine a path of execution.
 A conditional expression can be found in an IF or WHILE statement.
 FORTRAN 77 requires that the conditional expression be a logical expression.
 As an extension, an integer expression is also allowed.
 When an integer expression is used, it is converted to a logical expression
 by comparing the value of the integer expression to zero.
:MSGSYM. SP_STATEMENT_REQUIRED
:MSGTXT. %s1 statement must be used within %s2 definition
:MSGJTXT. %s1文は%s2の定義の中で使用しなければなりません。
:MSGATTR. 2
 The statement identified by %s1 must appear within a definition identified
 by %s2.
:eMSGGRP. SP
:cmt -------------------------------------------------------------------
:MSGGRP. SR
:MSGGRPSTR. SR-
:MSGGRPTXT. Subprograms
:cmt -------------------------------------------------------------------
:MSGSYM. SR_EXTRNED_TWICE
:MSGTXT. name can only appear in an EXTERNAL statement once
:MSGJTXT. 関数/ｻﾌﾞﾙｰﾁﾝ名はEXTERNALの中に2度以上指定できません。
:MSGATTR. 2
 A function/subroutine name appears more than once in an EXTERNAL
 statement.
:MSGSYM. SR_ILL_CHARFUNC
:MSGTXT. character function %s1 may not be called since size was declared as (*)
:MSGJTXT. ｻｲｽﾞが(*)の文字型関数%s1は呼出せません(*は外部関数,仮引数,PARAMETER文字定数に対してのみです)。
:MSGATTR. 2
 In the declaration of the character function name, the length was
 defined to be (*).
 The (*) length specification is only allowed for external functions,
 dummy arguments or symbolic character constants.
:MSGSYM. SR_ILL_PARM
:MSGTXT. %s1 can only be used an an argument to a subroutine
:MSGJTXT. %s1はｻﾌﾞﾙｰﾁﾝに対する引数としてのみ使用できます。
:MSGATTR. 2
 The specified class of an argument must only be passed to a subroutine.
 For example, an alternate return specifier is illegal as a subscript or
 an argument to a function.
:MSGSYM. SR_INTRNSC_EXTRN
:MSGTXT. name cannot appear in both an INTRINSIC and EXTERNAL statement
:MSGJTXT. 同じ名前をINTRINSICとEXTERNALの両方に指定することはできません。
:MSGATTR. 2
 The same name appears in an INTRINSIC statement and in an EXTERNAL
 statement.
:MSGSYM. SR_NO_SUBRTN_NAME
:MSGTXT. expecting a subroutine name
:MSGJTXT. SUBROUTINEでｻﾌﾞﾙｰﾁﾝ名を定義して下さい。
:MSGATTR. 2
 The subroutine named in the CALL statement does not define a
 subroutine.
 A subroutine is declared in a SUBROUTINE statement.
:MSGSYM. SR_ILL_IN_PROG
:MSGTXT. %s1 statement not allowed in main program
:MSGJTXT. %s1文はﾒｲﾝ･ﾌﾟﾛｸﾞﾗﾑの中に書いてはいけません。
:MSGATTR. 0
 The main program can contain any statements except a FUNCTION,
 SUBROUTINE, BLOCK DATA, or ENTRY statement.
 A SAVE statement is allowed but has no effect in the main program.
 A RETURN statement in the main program is an extension to the
 FORTRAN 77 language.
:MSGSYM. SR_NOT_INTRNSC
:MSGTXT. not an intrinsic FUNCTION name
:MSGJTXT. INTRINSICの名前は組込み関数名でなければなりません。
:MSGATTR. 2
 A name in the INTRINSIC statement is not an intrinsic function name.
 Refer to the Language Reference for a complete list of the intrinsic
 functions.
:MSGSYM. SR_PREV_INTRNSC
:MSGTXT. name can only appear in an INTRINSIC statement once
:MSGJTXT. 同じ組込み関数名を重複してINTRINSICの中に指定してはいけません。
:MSGATTR. 2
 An intrinsic function name appears more than once in the intrinsic
 function list.
:MSGSYM. SR_TRIED_RECURSION
:MSGTXT. subprogram recursion detected
:MSGJTXT. 副ﾌﾟﾛｸﾞﾗﾑの再帰呼出しが検出されました。
:MSGATTR. 2
 An attempt has been made to recursively invoke a subprogram, that is,
 to invoke an already active subprogram.
:MSGSYM. SR_TWO_PROGRAMS
:MSGTXT. two main program units in the same file
:MSGJTXT. 1個のﾌﾟﾛｸﾞﾗﾑ内にﾒｲﾝ･ﾌﾟﾛｸﾞﾗﾑの初めを示す命令が2個以上あってはなりません。
:MSGATTR. 0
 There are two places in the program that signify the start of a main
 program.
 The PROGRAM statement or the first statement that is not enclosed by
 a PROGRAM, FUNCTION, SUBROUTINE or BLOCK DATA statement specifies the
 main program start.
:MSGSYM. SR_TWO_UNNAMED
:MSGTXT. only one unnamed %s1 is allowed in an executable program
:MSGJTXT. 名前の無い%s1はﾌﾟﾛｸﾞﾗﾑの中に1個しか指定できません。
:MSGATTR. 0
 There may only be one unnamed BLOCK DATA subprogram or main program
 in an executable program.
:MSGSYM. SR_FUNC_AS_SUB
:MSGTXT. function referenced as a subroutine
:MSGJTXT. 関数はｻﾌﾞﾙｰﾁﾝとして使えません。
:MSGATTR. 2
 An attempt has been made to invoke a function using the CALL
 statement.
:MSGSYM. SR_ALREADY_ACTIVE
:MSGTXT. attempt to invoke active function/subroutine
:MSGJTXT. 関数/ｻﾌﾞﾙｰﾁﾝは自分の中から自分を、または呼び出した親を呼び出すことはできません。
:MSGATTR. 0
 An attempt has been made to invoke the current function/subroutine or
 a function/subroutine that was used to invoke current
 function/subroutine.
 The traceback produced when the error occurred lists all currently
 active functions/subroutines.
:MSGSYM. SR_ARG_USED_NOT_PASSED
:MSGTXT. dummy argument %s1 is not in dummy argument list of entered subprogram
:MSGJTXT. 仮引数%s1はENTRYと副ﾌﾟﾛｸﾞﾗﾑの両方に引数として定義してください。
:MSGATTR. 0
 The named dummy argument found in the ENTRY statement does not appear
 in the subroutine's dummy argument list in the subprogram statement.
:MSGSYM. SR_BAD_TYPE
:MSGTXT. function referenced as %s1 but defined to be %s2
:MSGJTXT. 呼び出された関数は%s1型ですがFUNCTION/ENTRYで%s2となっていて一致しません。
:MSGATTR. 0
 An attempt has been made to invoke a function of the type %s1 but the
 function was defined as %s2 in the FUNCTION or ENTRY statement.
 The function name's type must be correctly declared in the main
 program.
:MSGSYM. SR_CHAR_SIZE_MISMATCH
:MSGTXT. function referenced as CHARACTER*%u1 but defined to be CHARACTER*%u2
:MSGJTXT. CHARACTER*%u1として呼出された関数名はCHARACTER*%u2として定義されていて長さが合いません。
:MSGATTR. 0
 The character length of the function in the calling subprogram is %d1 but
 the length used to define the function is %d2.
 These two lengths must match.
:MSGSYM. SR_NO_PROG
:MSGTXT. missing main program
:MSGJTXT. PROGRAM(省略可能)で始まりENDで終わるﾒｲﾝ･ﾌﾟﾛｸﾞﾗﾑが必要です。
:MSGATTR. 0
 The program file is either empty or contains only subroutines and
 functions.
 Each program require a main program.
 A main program starts with an optional PROGRAM statement and ends
 with an END statement.
:MSGSYM. SR_SUB_AS_FUNC
:MSGTXT. subroutine referenced as a function
:MSGJTXT. ｻﾌﾞﾙｰﾁﾝは関数として使えません。
:MSGATTR. 0
 An attempt has been made to invoke a name as a function and has been
 defined as a subroutine in a SUBROUTINE or ENTRY statement.
:MSGSYM. SR_BLKDAT_CALLED
:MSGTXT. attempt to invoke a block data subprogram
:MSGJTXT. BLOCK DATA副ﾌﾟﾛｸﾞﾗﾑは変数の初期値化のためのもので呼び出すことはできません。
:MSGATTR. 0
 An attempt has been made to invoke a block data subprogram.
 Block data subprograms are used to initialize variables before
 program execution commences.
:MSGSYM. SR_STRUCT_MISMATCH
:MSGTXT. structure type of function %s1 does not match expected structure type
:MSGJTXT. 関数%s1の構造体の型が一致しません。
:MSGATTR. 0
 The function returns a structure that is not equivalent to the structure
 expected.
 Two structures are equivalent if the types and orders of each field
 are the same.
 Unions are considered equivalent if their sizes are the same.
 Field names, and the structure name itself, do not have to be the same.
:eMSGGRP. SR
:cmt -------------------------------------------------------------------
:MSGGRP. SS
:MSGGRPSTR. SS-
:MSGGRPTXT. Subscripts and Substrings
:cmt -------------------------------------------------------------------
:MSGSYM. SS_FUNCTION_VALUE
:MSGTXT. substringing of function or statement function return value is not FORTRAN 77 standard
:MSGJTXT. 文字型関数や文関数で取り出した文字列の部分列化は,FORTRAN 77の標準ではありません。
:MSGATTR. 2
 The character value returned from a CHARACTER function or statement
 function cannot be substring.
 Only character variable names and array element names may be
 substring.
:MSGSYM. SS_ONLY_IF_CHAR
:MSGTXT. substringing valid only for character variables and array elements
:MSGJTXT. 文字型変数や配列要素以外は部分列にできません。
:MSGATTR. 2
 An attempt has been made to substring a name that is not defined to
 be of type CHARACTER and is neither a variable nor an array element.
:MSGSYM. SS_SSCR_RANGE
:MSGTXT. subscript expression out of range; %s1 does not exist
:MSGJTXT. 添字式が配列の範囲外を間違って指しています(%s1は存在しません)。
:MSGATTR. 0
 An attempt has been made to reference an element in an array that is
 out of bounds of the declared array size.
 The array element %s1 does not exist.
:MSGSYM. SS_SSTR_RANGE
:MSGTXT. substring expression (%i1:%i2) is out of range
:MSGJTXT. 部分列の式(%i1:%i2)が範囲外を間違って指しています。
:MSGATTR. 0
 An expression in the substring is larger than the string length or
 less than the value 1.
 The substring expression must be one in which
 .millust begin
 1 <= %d1 <= %d2 <= len
 .millust end
:eMSGGRP. SS
:cmt -------------------------------------------------------------------
:MSGGRP. ST
:MSGGRPSTR. ST-
:MSGGRPTXT. Statements and Statement Numbers
:cmt -------------------------------------------------------------------
:MSGSYM. ST_ALREADY
:MSGTXT. statement number %i1 has already been defined in line %d2
:MSGJTXT. 重複して文番号%i1を%d2行目で定義してはいけません。
:MSGATTR. 0
 The two statements, in line %d2 and the current line, in the current
 program unit have the same statement label number, namely %d1.
:MSGSYM. ST_ASF_LATE
:MSGTXT. statement function definition appears after first executable statement
:MSGJTXT. 文関数の定義はﾌﾟﾛｸﾞﾗﾑの中で最初の実行可能な文の前になければなりません。
:MSGATTR. 0
 There is a statement function definition after the first executable
 statement in the program unit.
 Statement function definitions must follow specification statements
 and precede executable statements.
 Check that the statement function name is not an undeclared array
 name.
:MSGSYM. ST_BAD_BRANCHED
:MSGTXT. %s1 statement must not be branched to but was in line %d2
:MSGJTXT. %s1文は%d2行目からGOTOを受けることができない命令です。
:MSGATTR. 0
 Line %d2 passed execution control down to the statement %s1.
 The specification statements, ADMIT, AT END, BLOCK DATA, CASE, ELSE,
 ELSE IF, END AT END, END BLOCK, END DO, END LOOP, END SELECT, END
 WHILE, ENTRY, FORMAT, FUNCTION, OTHERWISE, PROGRAM, QUIT, REMOTE
 BLOCK, SAVE, SUBROUTINE, and UNTIL statements may not have control of
 execution transferred to it.
:MSGSYM. ST_CANNOT_BRANCH
:MSGTXT. branch to statement %i1 in line %d2 not allowed
:MSGJTXT. %i1文(%d2行目)に飛ぶことはできません。
:MSGATTR. 2
 An attempt has been made to pass execution control up to the
 statement labelled %d1 in line %d2.
 The specification statements, ADMIT, AT END, BLOCK DATA, CASE, ELSE,
 ELSE IF, END AT END, END BLOCK, END DO, END LOOP, END SELECT, END
 WHILE, ENTRY, FORMAT, FUNCTION, OTHERWISE, PROGRAM, QUIT, REMOTE
 BLOCK, SAVE, SUBROUTINE, and UNTIL statements may not have control of
 execution transferred to it.
:MSGSYM. ST_DATA_ALREADY
:MSGTXT. specification statement must appear before %s1 is initialized
:MSGJTXT. 変数等の定義は%s1が初期化される前に指定しなければなりません。
:MSGATTR. 2
 The variable %s1 has been initialized in a specification statement.
 A COMMON or EQUIVALENCE statement then references the variable.
 The COMMON or EQUIVALENCE statement must appear before the item can
 be initialized.
 Use the DATA statement to initialize data in this case.
:MSGSYM. ST_EXPECT_FORMAT
:MSGTXT. statement %i1 was referenced as a FORMAT statement in line %d2
:MSGJTXT. %i1文(%d2行目)が間違ってFORMATとして指定されています。
:MSGATTR. 0
 The statement in line %d2 references statement label %d1 as a FORMAT
 statement.
 The statement at that label is not a FORMAT statement.
:MSGSYM. ST_IMPLICIT_LATE
:MSGTXT. IMPLICIT statement appears too late
:MSGJTXT. IMPLICITは(PARAMETER以外の)変数の定義をする文の前に指定してください。
:MSGATTR. 0
 The current IMPLICIT statement is out of order.
 The IMPLICIT statement may be interspersed with the PARAMETER
 statement but must appear before other specification statements.
:MSGSYM. ST_NO_EXEC
:MSGTXT. this statement will never be executed due to the preceding branch
:MSGJTXT. この文はこの前に飛越し命令があるので実行されることがありません。
:MSGATTR. 0
 Because execution control will always be passed around the indicated
 statement, the statement will never be executed.
:MSGSYM. ST_NO_STMTNO
:MSGTXT. expecting statement number
:MSGJTXT. 文番号を入れて下さい。
:MSGATTR. 2
 The keyword GOTO or ASSIGN has been detected and the next part of the
 statement was not a statement number as was expected.
:MSGSYM. ST_NOT_FORMAT
:MSGTXT. statement number %i1 was not used as a FORMAT statement in line %d2
:MSGJTXT. 文番号%i1(%d2行目)はFORMAT文ではありませんが、FORMAT文であるかのように使用されています。
:MSGATTR. 2
 The statement at line %d2 with statement number %d1 is not a FORMAT
 statement but the current statement uses statement number %d1
 as if it labelled a FORMAT statement.
:MSGSYM. ST_SPEC_LATE
:MSGTXT. specification statement appears too late
:MSGJTXT. 変数等の定義はDATA、文関数、実行可能命令の前に指定しなければなりません。
:MSGATTR. 0
 The indicated specification statement appears after
 a statement function definition or an executable statement.
 All specification statements must appear before these types of
 statements.
:MSGSYM. ST_NOT_ALLOWED
:MSGTXT. %s1 statement not allowed after %s2 statement
:MSGJTXT. %s1文は%s2文の後に指定してはいけません。
:MSGATTR. 2
 The statement %s1 cannot be the object of a %s2 statement.
 %s2 represents a logical IF or WHILE statement.
 These statements include: specification statements, ADMIT, AT END,
 CASE, DO, ELSE, ELSE IF END, END AT END, END BLOCK, END DO, END
 GUESS, ENDIF, END LOOP, END SELECT, END WHILE, ENTRY, FORMAT,
 FUNCTION, GUESS, logical IF, block IF, LOOP, OTHERWISE, PROGRAM,
 REMOTE BLOCK, SAVE, SELECT, SUBROUTINE, UNTIL, and WHILE.
:MSGSYM. ST_NUM_TOO_BIG
:MSGTXT. statement number must be 99999 or less
:MSGJTXT. 文番号は99999かそれ以下でなければなりません。
:MSGATTR. 2
 The statement label number specified in the indicated statement has
 more than 5 digits.
:MSGSYM. ST_NUM_ZERO
:MSGTXT. statement number cannot be zero
:MSGJTXT. 文番号は0であってはいけません。
:MSGATTR. 1
 The statement label number specified in the indicated statement is
 zero.
 Statement label numbers must be greater than 0 and less than or equal
 to 99999.
:MSGSYM. ST_TO_SELF
:MSGTXT. this statement could branch to itself
:MSGJTXT. 自分自身に飛んではいけません。
:MSGATTR. 2
 The indicated statement refers to a statement label number which
 appears on the statement itself and therefore could branch to itself,
 creating an endless loop.
:MSGSYM. ST_UNDEFINED
:MSGTXT. missing statement number %i1 - used in line %d2
:MSGJTXT. 文番号%i1はﾌﾟﾛｸﾞﾗﾑの中に無いので%d2行目の指定はｴﾗｰです。
:MSGATTR. 0
 A statement with the statement label number %d1 does not exist in the
 current program unit.
 The statement label number is referenced in line %d2 of the program
 unit.
:MSGSYM. ST_UNKNOWN_STMT
:MSGTXT. undecodeable statement or misspelled word %s1
:MSGJTXT. 代入文として間違っているか、文字のつづりが間違った%s1があります。
:MSGATTR. 2
 The statement cannot be identified as an assignment statement or any
 other type of FORTRAN statement.
 The first word of a FORTRAN statement must be a statement keyword or
 the statement must be an assignment statement.
:MSGSYM. ST_UNREFERENCED
:MSGTXT. statement %i1 will never be executed due to the preceding branch
:MSGJTXT. 文%i1はこの前に飛越し命令があるので実行されることはありません。
:MSGATTR. 0
 The statement with the statement label number of %d1 will never be
 executed because the preceding statement will always pass execution
 control around the statement and no other reference is made to the
 statement label.
:MSGSYM. ST_WANT_NAME
:MSGTXT. expecting keyword or symbolic name
:MSGJTXT. ｷｰﾜｰﾄﾞかｼﾝﾎﾞﾙ名を指定してください。
:MSGATTR. 1
 The first character of a statement is not an alphabetic.
 The first word of a statement must be a statement keyword or a
 symbolic name.
 Symbolic names must start with a letter (upper case or lower case), a
 dollar sign ($) or an underscore (_).
:MSGSYM. ST_LONG_NUM
:MSGTXT. number in %s1 statement is longer than 5 digits
:MSGJTXT. %s1の数字は5桁より大きくてはいけません。
:MSGATTR. 1
 The number in the PAUSE or STOP statement is longer than 5 digits.
:MSGSYM. ST_DATA_TOO_EARLY
:MSGTXT. position of DATA statement is not FORTRAN 77 standard
:MSGJTXT. DATA 文の位置はFORTRAN 77の標準ではありません。
:MSGATTR. 0
 The FORTRAN 77 standard requires DATA statements to appear after all
 specification statements.
 As an extension to the standard, Watcom FORTRAN 77 allows DATA statements
 to appear before specification statements.
 Note that in the latter case, the type of the symbol must be established
 before data initialization occurs.
:MSGSYM. ST_FORMAT_LABEL
:MSGTXT. no FORMAT statement with given label
:MSGJTXT. 指定したﾗﾍﾞﾙの位置にFORMAT文がありません。
:MSGATTR. 0
 The current statement refers to the label of a FORMAT statement but
 the label appears on some other statement that is not a FORMAT
 statement.
:MSGSYM. ST_NUMBER
:MSGTXT. statement number not in list or not the label of an executable statement
:MSGJTXT. 文番号がかっこの中にない時や実行可能な文に付いていない時はｴﾗｰです。
:MSGATTR. 0
 The specified statement number in the indicated statement is not in
 the list of statement numbers or it is not the statement label number
 of an executable statement.
:MSGSYM. ST_BRANCH_INTO
:MSGTXT. attempt to branch into a control structure
:MSGJTXT. この制御構造の中に飛び込むことはできません。
:MSGATTR. 0
 An attempt has been made to pass execution control into a control
 structure.
 A statement uses a computed statement label number to transfer
 control.
 This value references a statement inside a control structure.
:eMSGGRP. ST
:cmt -------------------------------------------------------------------
:MSGGRP. SV
:MSGGRPSTR. SV-
:MSGGRPTXT. Subscripted Variables
:cmt -------------------------------------------------------------------
:MSGSYM. SV_ARR_DECL
:MSGTXT. variable %s1 in array declarator must be in COMMON or a dummy argument
:MSGJTXT. 配列の宣言子である変数%s1はCOMMONの中か仮引数として指定しなければなりません。
:MSGATTR. 0
 The variable %s1 was used as an array declarator in a subroutine or
 function but the variable was not in a COMMON block nor was it a
 dummy argument in the FUNCTION, SUBROUTINE or ENTRY statement.
:MSGSYM. SV_ARR_PARM
:MSGTXT. adjustable/assumed size array %s1 must be a dummy argument
:MSGJTXT. 調整可能な、あるいは仮ｻｲｽﾞの配列%s1は仮引数でなければなりません。
:MSGATTR. 0
 The array %s1 used in the current subroutine or function must be a
 dummy argument.
 When the array declarator is adjustable or assumed-size, the array
 name must be a dummy argument.
:MSGSYM. SV_BAD_SSCR
:MSGTXT. invalid subscript expression
:MSGJTXT. 添字式が間違っています。
:MSGATTR. 2
 The indicated subscript expression is not a valid integer expression
 or the high bound of the array is less than the low bound of the
 array when declaring the size of the array.
:MSGSYM. SV_INV_SSCR
:MSGTXT. invalid number of subscripts
:MSGJTXT. 添字の個数が正しくありません。
:MSGATTR. 2
 The number of subscripts used to describe an array element does not
 match the number of subscripts in the array declaration.
 The maximum number of subscripts allowed is 7.
:MSGSYM. SV_NO_LIST
:MSGTXT. using %s1 name incorrectly without list
:MSGJTXT. %s1は添字なしに使えません。
:MSGATTR. 2
 An attempt has been made to assign a value to the declared array %s1.
 Values may only be assigned to elements in the array.
 An array element is the array name followed by integer expressions
 enclosed in parentheses and separated by commas.
:MSGSYM. SV_TRIED_SSTR
:MSGTXT. cannot substring array name %s1
:MSGJTXT. 配列名%s1は添字がないと部分列にできません。
:MSGATTR. 1
 An attempt has been made to substring the array %s1.
 Only an array element may be substring.
:MSGSYM. SV_PVD
:MSGTXT. %s1 treated as an assumed size array
:MSGJTXT. %s1は擬寸法仮配列とみなします(最後の次元が1の時などです)。
:MSGATTR. 0
 A dummy array argument has been declared with 1 in the last
 dimension.
 The array is treated as if an '*' had been specified in place of the
 1.
 This is done to support a feature called "pseudo-variable
 dimensioning" which was supported by some FORTRAN IV compilers and is
 identical in concept to FORTRAN 77 assumed-size arrays.
:MSGSYM. SV_CANT_USE_ASSUMED
:MSGTXT. assumed size array %s1 cannot be used as an i/o list item or a format/unit identifier
:MSGJTXT. 擬寸法仮配列(最後の次元が*の時)%s1はI/O書式やFORMAT/UNITの中に使えません。
:MSGATTR. 2
 Assumed size arrays (arrays whose last dimension is '*') must not
 appear as an i/o list item (i.e. in a PRINT statement), a format
 identifier or an internal file specifier.
:MSGSYM. SV_DIMENSION_LIMIT
:MSGTXT. limit of 65535 elements per dimension has been exceeded
:MSGJTXT. 次元の要素限度は65535を超えてはいけません。
:MSGATTR. 2
 On the IBM PC, for 16-bit real mode applications, the number of elements in a
 dimension must not exceed 65535.
:eMSGGRP. SV
:cmt -------------------------------------------------------------------
:MSGGRP. SX
:MSGGRPSTR. SX-
:MSGGRPTXT. Syntax Errors
:cmt -------------------------------------------------------------------
:MSGSYM. SX_UNEXPECTED_OPN
:MSGTXT. unexpected number or name %s1
:MSGJTXT. 数字か名前%s1がまちがった場所にあります。
:MSGATTR. 2
 The number or name %s1 is in an unexpected place in the statement.
:MSGSYM. SX_BAD_OPR_SEQ
:MSGTXT. bad sequence of operators
:MSGJTXT. 演算子の順番がまちがっています。
:MSGATTR. 2
 The indicated arithmetic operator is out of order.
 An arithmetic operator is one of the following:
 **, *, /, +, and &minus..
 All arithmetic operators must be followed by at least a primary.
 A primary is an array element, constant, (expression), function name,
 or variable name.
:MSGSYM. SX_INV_OPR
:MSGTXT. invalid operator
:MSGJTXT. 演算子が正しくありません。
:MSGATTR. 1
 The indicated operator between the two arithmetic primaries is not a
 valid operator.
 Valid arithmetic operators include: **, *, /, +, and &minus..
 A primary is an array element, constant, (expression), function name,
 or variable name.
:MSGSYM. SX_JUNK_AFTER_RBR
:MSGTXT. expecting end of statement after right parenthesis
:MSGJTXT. 右かっこの後にｽﾃｰﾄﾒﾝﾄをつづけてはいけません。
:MSGATTR. 0
 The end of the statement is indicated by the closing right
 parenthesis but more characters were found on the line.
 Multiple statements per line are not allowed in FORTRAN 77.
:MSGSYM. SX_NO_ASTERISK
:MSGTXT. expecting an asterisk
:MSGJTXT. ｱｽﾀﾘｽｸ(*)が必要です。
:MSGATTR. 1
 The next character of the statement should be an asterisk but another
 character was found instead.
:MSGSYM. SX_NO_COLON
:MSGTXT. expecting colon
:MSGJTXT. ｺﾛﾝ(:)が必要です。
:MSGATTR. 1
 A colon (:) was expecting but not found.
 For example, the colon separating the low and high bounds of a
 character substring was not found.
:MSGSYM. SX_NO_EOS_OR_COLON
:MSGTXT. expecting colon or end of statement
:MSGJTXT. 制御文の終りに関係のないものがあります( : かﾌﾞﾛｯｸ･ﾗﾍﾞﾙが必要です)。
:MSGATTR. 2
 On a control statement, a word was found at the end of the statement
 that cannot be related to the statement.
 The last word on several of the control statements may be a block
 label.
 All block labels must be preceded by a colon (:).
:MSGSYM. SX_MISSING_COMMA
:MSGTXT. missing comma
:MSGJTXT. ｺﾝﾏがありません。
:MSGATTR. 1
 A comma was expected and is missing.
 There must be a comma after the statement keyword AT END when a
 statement follows.
 A comma must occur between the two statement labels in the GO TO
 statement.
 A comma must occur between the expressions in the DO statement.
 A comma must occur between the names listed in the DATA statement
 and specification statements.
 A comma must occur between the specifiers in I/O statements.
:MSGSYM. SX_EOS_EXPECTED
:MSGTXT. expecting end of statement
:MSGJTXT. 文の終りに関係のないものがあります(1行に1文しか書けません)。
:MSGATTR. 1
 The end of the statement was expected but more words were found on
 the line and cannot be associated to the statement.
 FORTRAN 77 only allows for one statement per line.
:MSGSYM. SX_NO_INTEGER_VAR
:MSGTXT. expecting integer variable
:MSGJTXT. 整数型変数でなければなりません。
:MSGATTR. 2
 The name indicated in the statement must be of type INTEGER but is
 not.
:MSGSYM. SX_NO_NAME
:MSGTXT. expecting %s1 name
:MSGJTXT. 名前%s1がここになければなりません。
:MSGATTR. 2
 A name with the characteristic %s1 was expected at the indicated place
 in the statement but is missing.
:MSGSYM. SX_EXPECT_INT
:MSGTXT. expecting an integer
:MSGJTXT. 整数でなければなりません。
:MSGATTR. 2
 The length specifier, as in the IMPLICIT statement, must be an
 integer constant or an integer constant expression.
 The repeat specifier of the value to be assigned to the variables, as
 in the DATA statement, must be an integer constant or an integer
 constant expression.
:MSGSYM. SX_NO_NUMBER_VAR
:MSGTXT. expecting INTEGER, REAL, or DOUBLE PRECISION variable
:MSGJTXT. 整数か実数か倍精度型の変数でなければなりません。
:MSGATTR. 2
 The indicated DO variable is not one of the types INTEGER, REAL, or
 DOUBLE PRECISION.
:MSGSYM. SX_NO_OPR
:MSGTXT. missing operator
:MSGJTXT. 演算子がありません。
:MSGATTR. 1
 Two primaries were found in an expression and an operator was not
 found in between.
 A primary is an array element, constant, (expression), function name,
 or variable name.
:MSGSYM. SX_NO_SLASH
:MSGTXT. expecting a slash
:MSGJTXT. ｽﾗｯｼｭ(/)が必要です。
:MSGATTR. 1
 A slash is expected in the indicated place in the statement.
 Slashes must be balanced as parentheses.
 Slashes are used to enclose the initial data values in specification
 statements or to enclose names of COMMON blocks.
:MSGSYM. SX_WRONG_TYPE
:MSGTXT. expecting %s1 expression
:MSGJTXT. ﾀｲﾌﾟ%s1の式でなければなりません。
:MSGATTR. 2
 An expression of type %s1 is required.
:MSGSYM. SX_NOT_CONST_EXPR
:MSGTXT. expecting a constant expression
:MSGJTXT. 定数式でなければなりません。
:MSGATTR. 2
 A constant expression is required.
:MSGSYM. SX_NOT_SIMPLE_NUMBER
:MSGTXT. expecting INTEGER, REAL, or DOUBLE PRECISION expression
:MSGJTXT. 整数か実数か倍精度の数式でなければなりません。
:MSGATTR. 2
 The indicated expression is not one of type INTEGER, REAL, or DOUBLE
 PRECISION.
 Each expression following the DO variable must be an expression of
 one of these types.
:MSGSYM. SX_NUM_OR_LIT
:MSGTXT. expecting INTEGER or CHARACTER constant
:MSGJTXT. 整数型か文字型定数でなければなりません。
:MSGATTR. 2
 In the PAUSE and STOP statement, the name following the keyword must
 be a constant of type INTEGER or of type CHARACTER.
 This constant will be printed on the console when the statement is
 executed.
:MSGSYM. SX_SURP_OPR
:MSGTXT. unexpected operator
:MSGJTXT. 余分な演算子があります。
:MSGATTR. 1
 An operand was expected but none was found.
 For example, in an I/O statement, the comma is used to separate I/O
 list items.
 Two consecutive commas without an I/O list item between them would
 result in this error.
:MSGSYM. SX_QUOTE
:MSGTXT. no closing quote on literal string
:MSGJTXT. 文字列を閉じる右の(')引用符がありません。
:MSGATTR. 2
 The closing quote of a literal string was not found before the end of
 the statement.
:MSGSYM. SX_CONST
:MSGTXT. missing or invalid constant
:MSGJTXT. 定数が無いか、形式が正しくありません。
:MSGATTR. 2
 In a DATA statement, the constant required to initialize a variable
 was not found or incorrectly specified.
:MSGSYM. SX_EXPECT_CHAR_CONST
:MSGTXT. expecting character constant
:MSGJTXT. 文字型定数でなければなりません。
:MSGATTR. 2
 A character constant is required.
:eMSGGRP. SX
:cmt -------------------------------------------------------------------
:MSGGRP. TY
:MSGGRPSTR. TY-
:MSGGRPTXT. Type Statements
:cmt -------------------------------------------------------------------
:MSGSYM. TY_CHAR_BEFORE_PAREN
:MSGTXT. length specification before array declarator is not FORTRAN 77 standard
:MSGJTXT. 配列宣言子の前の長さ指定はFORTRAN 77の標準ではありません。
:MSGATTR. 1
 An array declarator specified immediately after the length
 specification of the array is an extension to the FORTRAN 77
 language.
:MSGSYM. TY_ILL_TYP_SIZE
:MSGTXT. %i1 is an illegal length for %s2 type
:MSGJTXT. %i1の長さ指定は%s2型に対して正しくありません。
:MSGATTR. 2
 The length specifier %d1 is not valid for the type %s2.
 For type LOGICAL, valid lengths are 1 and 4.
 For the type INTEGER, valid lengths are 1, 2, and 4.
 For the type REAL, valid lengths are 4 and 8.
 For the type COMPLEX, valid lengths are 8 and 16.
 On the IBM PC, the length specifier for items of type CHARACTER must
 be greater than 0 and not exceed 65535.
:MSGSYM. TY_LEN_SPEC
:MSGTXT. length specifier in %s1 statement is not FORTRAN 77 standard
:MSGJTXT. %s1文の長さ指定はFORTRAN 77の標準ではありません。
:MSGATTR. 1
 A length specifier in certain type specification statements is an
 extension to the FORTRAN 77 language.
 These include: LOGICAL*1, LOGICAL*4, INTEGER*1, INTEGER*2, INTEGER*4,
 REAL*4, REAL*8, COMPLEX*8, and COMPLEX*16.
:MSGSYM. TY_NOT_DBL_PREC
:MSGTXT. length specification not allowed with type %s1
:MSGJTXT. %s1ﾀｲﾌﾟのｽﾃｰﾄﾒﾝﾄに長さ指定はいりません。
:MSGATTR. 2
 A length specification is not allowed in a DOUBLE PRECISION or
 DOUBLE COMPLEX statement.
:MSGSYM. TY_TYP_PREV_DEF
:MSGTXT. type of %s1 has already been established as %s2
:MSGJTXT. %s1は%s2として定義されていますので、別な名前に変えてください。
:MSGATTR. 2
 The indicated name %s1 has already been declared to have a different
 type, namely %s2.
 The name %s1 cannot be used in this specification statement.
:MSGSYM. TY_UNDECLARED
:MSGTXT. type of %s1 has not been declared
:MSGJTXT. IMPLICIT NONEの時は%s1の型は定義してください。
:MSGATTR. 2
 The indicated name %s1 has not been declared.
 This message is only issued when the IMPLICIT NONE specification
 statement is used.
:MSGSYM. TY_ILL_USE
:MSGTXT. %s1 of type %s2 is illegal in %s3 statement
:MSGJTXT. %s2型である%s1は%s3文中で間違って使われています。
:MSGATTR. 2
 The symbol %s1 with type %s2 cannot be used in statement %s3.
 For example, a symbol of type STRUCTURE cannot be used in a PARAMETER
 statement.
:eMSGGRP. TY
:cmt -------------------------------------------------------------------
:MSGGRP. VA
:MSGGRPSTR. VA-
:MSGGRPTXT. Variable Names
:cmt -------------------------------------------------------------------
:MSGSYM. VA_ILL_USE
:MSGTXT. illegal use of %s1 name %s2 in %s3 statement
:MSGJTXT. %s1と定義された名前%s2は%s3文では使えません。
:MSGATTR. 2
 The name %s2 has been defined as %s1 and cannot be used as a name in
 the statement %s3.
:MSGSYM. VA_NAME_LEN_EXT
:MSGTXT. symbolic name %s1 is longer than 6 characters
:MSGJTXT. ｼﾝﾎﾞﾙ名%s1に7文字以上使えるのは標準ではありません。
:MSGATTR. 0
 Symbolic names greater than 6 characters is an extension to the
 FORTRAN 77 language.
 The maximum length is 32 characters.
:MSGSYM. VA_PREV_DEF_NAM
:MSGTXT. %s1 has already been defined as a %s2
:MSGJTXT. %s1はすでに%s2として定義されています。
:MSGATTR. 0
 The name %s1 has been previously defined as a %s2 in another
 statement and cannot be redefined as specified in the indicated
 statement.
:MSGSYM. VA_UNDEFINED
:MSGTXT. %s1 %s2 has not been defined
:MSGJTXT. %s1として%s2は使われていますが、そのように定義してください。
:MSGATTR. 0
 The name %s2 has been referenced to be a %s1 but has not been defined
 as such in the program unit.
:MSGSYM. VA_UNREFERENCED
:MSGTXT. %s1 is an unreferenced symbol
:MSGJTXT. %s1は定義されましたが使われていません。
:MSGATTR. 0
 The name %s1 has been defined but not referenced.
:MSGSYM. VA_SAME_NAMELIST
:MSGTXT. %s1 already belongs to this NAMELIST group
:MSGJTXT. %s1は既にこのNAMELISTｸﾞﾙｰﾌﾟに属しています。
:MSGATTR. 2
 The name %s1 can only appear in a NAMELIST group once.
 However, a name can belong to multiple NAMELIST groups.
:MSGSYM. VA_USED_NOT_DEFINED
:MSGTXT. %s1 has been used but not defined
:MSGJTXT. %s1が使われましたが、定義されていません。
:MSGATTR. 0
 %s1 has not been defined before using it in a way that requires its
 definition.
 Note that symbols that are equivalenced, belong to a common block, are
 dummy arguments, or passed as an argument to a subprogram, will not be
 checked to ensure that they have been defined before requiring a value.
:MSGSYM. VA_ALLOCATABLE_STORAGE
:MSGTXT. dynamically allocating %s1 is not FORTRAN 77 standard
:MSGJTXT. 動的に割り当てている%s1はFORTRAN 77の標準ではありません。
:MSGATTR. 2
 Allocatable storage are extensions to the FORTRAN 77 standard.
:MSGSYM. VA_BAD_SYM_IN_NAMELIST
:MSGTXT. %s1 in NAMELIST %s2 is illegal
:MSGJTXT. NAMELIST %s2の中の%s1は不正です。
:MSGATTR. 0
 Symbol %s1 appearing in NAMELIST %s2 is illegal.
 Symbols appearing in a NAMELIST cannot be dummy arguments, allocatable, or of
 a user-defined type.
:eMSGGRP. VA
:cmt -------------------------------------------------------------------
:MSGGRP. MS
:MSGGRPSTR. MS-
:MSGGRPTXT. Additional Messages
:cmt -------------------------------------------------------------------
:MSGSYM. MS_STRUCT_DEFN
:MSGTXT. structure definition
:MSGJTXT. 構造体定義
:MSGATTR. 0
:MSGSYM. MS_GROUP_NAME
:MSGTXT. group name
:MSGJTXT. ｸﾞﾙｰﾌﾟ名
:MSGATTR. 0
:MSGSYM. MS_STMT_FUNC
:MSGTXT. statement function
:MSGJTXT. 文関数
:MSGATTR. 0
:MSGSYM. MS_STMT_FUNC_DEFN
:MSGTXT. statement function definition
:MSGJTXT. 文関数定義
:MSGATTR. 0
:MSGSYM. MS_SIMP_VAR
:MSGTXT. simple variable
:MSGJTXT. 単純変数
:MSGATTR. 0
:MSGSYM. MS_COM_VAR
:MSGTXT. variable in COMMON
:MSGJTXT. COMMON内変数
:MSGATTR. 0
:MSGSYM. MS_EQUIV_VAR
:MSGTXT. equivalenced variable
:MSGJTXT. 等価変数
:MSGATTR. 0
:MSGSYM. MS_SP_ARG
:MSGTXT. subprogram argument
:MSGJTXT. 副ﾌﾟﾛｸﾞﾗﾑ引数
:MSGATTR. 0
:MSGSYM. MS_SF_ARG
:MSGTXT. statement function argument
:MSGJTXT. 文関数引数
:MSGATTR. 0
:MSGSYM. MS_ARRAY
:MSGTXT. array
:MSGJTXT. 配列
:MSGATTR. 0
:MSGSYM. MS_ALLOC_ARRAY
:MSGTXT. allocated array
:MSGJTXT. 割り当てられた配列
:MSGATTR. 0
:MSGSYM. MS_VAR_ARR
:MSGTXT. variable or array
:MSGJTXT. 変数または配列
:MSGATTR. 0
:MSGSYM. MS_INTR_FUNC
:MSGTXT. intrinsic function
:MSGJTXT. 組み込み関数
:MSGATTR. 0
:MSGSYM. MS_EXT_PROC
:MSGTXT. external subprogram
:MSGJTXT. 外部副ﾌﾟﾛｸﾞﾗﾑ
:MSGATTR. 0
:MSGSYM. MS_ASSIGN
:MSGTXT. assignment
:MSGJTXT. 代入
:MSGATTR. 0
:MSGSYM. MS_ARITH_IF
:MSGTXT. arithmetic if
:MSGJTXT. 算術 if
:MSGATTR. 0
:MSGSYM. MS_AT_EOSTMT
:MSGTXT. at end of statement,
:MSGJTXT. 文の終わり,
:MSGATTR. 0
:MSGSYM. MS_DEF_SPROG
:MSGTXT. Defining subprogram: %s1; first ISN is %u2
:MSGJTXT. 副ﾌﾟﾛｸﾞﾗﾑの定義: %s1; 始めのISNは %u2
:MSGATTR. 0
:MSGSYM. MS_LIB_MEMBER
:MSGTXT. library member %s1,
:MSGJTXT. ﾗｲﾌﾞﾗﾘﾒﾝﾊﾞｰ %s1,
:MSGATTR. 0
:MSGSYM. MS_COLUMN
:MSGTXT. column %d1,
:MSGJTXT. ｶﾗﾑ %d1,
:MSGATTR. 0
:MSGSYM. MS_IN
:MSGTXT. in %s1,
:MSGJTXT. in %s1,
:MSGATTR. 0
:MSGSYM. MS_STATISTICS
:MSGTXT. %d1 statements, %i2 bytes, %d3 extensions, %d4 warnings, %d5 errors
:MSGJTXT. %d1 ｽﾃｰﾄﾒﾝﾄ, %i2 ﾊﾞｲﾄ, %d3 拡張ﾒｯｾｰｼﾞ, %d4 警告ｴﾗｰ, %d5 ｴﾗｰ
:MSGATTR. 0
:MSGSYM. MS_CODE_SIZE
:MSGTXT. Code size (in bytes):
:MSGJTXT. ｺｰﾄﾞｻｲｽﾞ (ﾊﾞｲﾄ):
:MSGATTR. 0
:MSGSYM. MS_NUMBER_OF_ERRORS
:MSGTXT. Number of errors:
:MSGJTXT. ｴﾗｰ数:
:MSGATTR. 0
:MSGSYM. MS_COMPILE_TIME
:MSGTXT. Compile time (in seconds):
:MSGJTXT. ｺﾝﾊﾟｲﾙ時間 (秒):
:MSGATTR. 0
:MSGSYM. MS_NUMBER_OF_WARNINGS
:MSGTXT. Number of warnings:
:MSGJTXT. 警告ｴﾗｰ数:
:MSGATTR. 0
:MSGSYM. MS_NUMBER_OF_STATEMENTS
:MSGTXT. Number of statements compiled:
:MSGJTXT. ｺﾝﾊﾟｲﾙされた文の数:
:MSGATTR. 0
:MSGSYM. MS_NUMBER_OF_EXTENSIONS
:MSGTXT. Number of extensions:
:MSGJTXT. 拡張仕様の使用数:
:MSGATTR. 0
:MSGSYM. MS_TRACE_INFO
:MSGTXT. - Executing line %u1 in file %s2
:MSGJTXT. - 実行ﾗｲﾝ %u1 ﾌｧｲﾙ %s2
:MSGATTR. 0
:MSGSYM. MS_CALLED_FROM
:MSGTXT. - Called from line %u1 in file %s2
:MSGJTXT. - %s2 ﾌｧｲﾙ, %u1 行からの呼び出し
:MSGATTR. 0
:eMSGGRP. MS
