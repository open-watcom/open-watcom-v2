AR Subprogram Arguments
AR_BAD_COUNT [   c2] invalid number of arguments to intrinsic function %s1
AR_BAD_COUNT [j  c2] 組込み関数%s1で定義された引数の数とそれを呼出すために指定した引数の数が一致していません。
 The number of actual arguments specified in the argument list for the
 intrinsic function %s1 does not agree with the dummy argument list.
 Consult the Language Reference for information on intrinsic functions and
 their arguments.
AR_DUPLICATE_PARM [   c0] dummy argument %s1 appears more than once
AR_DUPLICATE_PARM [j  c0] 同じ仮引数%s1が2度以上使われてはいけません。
 The same dummy argument %s1 is named more than once in the dummy
 argument list.
AR_ARG_CHAR_MISMATCH [ w r0] argument number %d1 (%s2) in call to %s3 must be CHARACTER*%u4 or bigger but size passed was %d5
AR_ARG_CHAR_MISMATCH [jw r0] %s3をCALLする%d1番目の引数(%s2)はCHARACTER*%u4かそれ以上必要です。しかし%d5となっています。
 Argument number %d1 in the call to subprogram %s3 was declared in the calling
 subprogram to be CHARACTER*%d4.
 In subprogram %s3, the argument was declared to be CHARACTER*%d3.
 The size of the argument, in the calling program, must be greater
 than or equal to the size in the subroutine/function.
AR_COD_MISMATCH [ w r0] argument number %d1 (%s2) in call to %s3 - actual argument was %s4 but dummy argument was %s5
AR_COD_MISMATCH [jw r0] %s3をCALLする%d1番目の引数(%s2)は実引数が%s4でした。しかし仮引数は%s5でした。
 Argument number %d1 in the call to subprogram %s3 was a %s4 in the calling
 subprogram but a %s5 in the called subprogram.
 This message is issued when, for example, an function name is passed to
 an array name.
AR_CODI_MISMATCH [ w r0] argument number %d1 - %s2 was passed to an intrinsic function
AR_CODI_MISMATCH [jw r0] %d1番目の引数 - %s2が組込み関数に渡されました。
 Argument number %d1 to an intrinsic function was declared as a %s2 and cannot
 be specified as an argument to an intrinsic function.
 For example, it is illegal to pass an array to the SIN intrinsic function.
 Constants, simple variables, array elements, and substrung array
 elements may be used as arguments.
AR_FUN_EXPECT [ w r0] argument number %d1 (%s2) in call to %s3 - a subroutine was passed but a function was expected
AR_FUN_EXPECT [jw r0] %s3へのCALLの%d1番目の引数(%s2) - ｻﾌﾞﾙｰﾁﾝが渡されましたが,関数でなければなりません。
 Argument number %d1 in the call to subprogram %s3 is a subroutine but
 was used as a function in subprogram %s3.
AR_PARM_COUNT [ w r0] number of arguments expected in call to %s1 was %d2, but found %d3
AR_PARM_COUNT [jw r0] %s1を呼び出す時の引数は%d2個でなければなりません。%d3個ありました。
 The number of arguments in the call to subprogram %s1 was %d3 and does not
 match the definition of subprogram %s1 which was defined with %d2 arguments.
AR_SUB_EXPECT [ w r0] argument number %d1 (%s2) in call to %s3 - a function was passed but a subroutine was expected
AR_SUB_EXPECT [jw r0] %s3をCALLする%d1番目の引数(%s2) - 関数が渡されましたが,ｻﾌﾞﾙｰﾁﾝでなければなりません。
 Argument number %d1 in the call to subprogram %s3 is a function but
 was used as a subroutine in subprogram %s3.
AR_TYP_MISMATCH [ w r0] argument number %d1 (%s2) in call to %s3 - %s4 argument was passed but %s5 argument was expected
AR_TYP_MISMATCH [jw r0] %3をCALLする%d1番目の引数(%s2) - %s4引数が渡されましたが,%s5引数でなければなりません。
 The type of argument number %d1 in the call to subprogram %s3 was %s4.
 The type of argument number %d1 was declared as %s5 in subprogram %s3.
 The number and types of actual and dummy arguments must match.
AR_STRUCT_MISMATCH [ w r0] argument number %d1 (%s2) in call to %s3 - type of structure passed does not match type of structure expected
AR_STRUCT_MISMATCH [jw r0] %s3をCALLする%d1番目の引数(%s2) - 渡された構造体の型は、期待される構造体の型と合っていません。
 The structure type of argument number %d1 in the call to subprogram
 %s3 does not match the expected structure type.
 Two structures are equivalent if the type of the corresponding field names
 are the same.
 Unions are considered equivalent if their sizes are the same.
 Field names, and the structure name itself, do not have to be the same.
AR_PASS_BY_MISMATCH [ w r0] argument number %d1 (%s2) in call to %s3 - argument passed by value but expected by reference
AR_PASS_BY_MISMATCH [jw r0] %s3をCALLする%d1番目の引数(%s2) - 値渡しの引数ですが,ｱﾄﾞﾚｽ渡しでなければなりません。
 Argument number %d1 in the call to subprogram %s3 was passed by value.
 The called subprogram expected the address of the argument instead of its
 value.
AR_ENTRY_TOO_LATE [   c2] dummy argument %s1 must not appear before definition of ENTRY %s2
AR_ENTRY_TOO_LATE [j  c2] 仮引数%s1は,ENTRY %s2の定義の前になければなりません。
 The dummy argument %s1 has appeared in an executable statement before
 its appearance in the definition of %s2 in an ENTRY statement.
 This is illegal.
BD Block Data Subprograms
BD_BLKDAT_NOT_COMMON [   c0] %s1 was initialized in a block data subprogram but is not in COMMON
BD_BLKDAT_NOT_COMMON [j  c0] %s1はBLOCK DATA副ﾌﾟﾛｸﾞﾗﾑで初期値されましたが, COMMONﾌﾞﾛｯｸにありません。
 The variable or array element, %s1, was initialized in a BLOCK DATA
 subprogram but was not specified in a named COMMON block.
BD_IN_BLOCK_DATA [   c0] %s1 statement is not permitted in a BLOCK DATA subprogram
BD_IN_BLOCK_DATA [j  c0] %s1文はBLOCK DATA副ﾌﾟﾛｸﾞﾗﾑの中には書けません。
 The statement, %s1, is not allowed in a BLOCK DATA subprogram.
 The only statements which are allowed to appear are:
 IMPLICIT, PARAMETER, DIMENSION, COMMON, SAVE, EQUIVALENCE, DATA, END,
 and type statements.
CC Source Format and Contents
CC_BAD_CHAR [   c1] invalid character encountered in source input
CC_BAD_CHAR [j  c1] ﾌﾟﾛｸﾞﾗﾑの中に使ってはいけない文字があります。
 The indicated statement contains an invalid character.
 Valid characters are: letters, digits, $, *, ., +, &minus., /, :, =, (, ),
 !, %,  ', and ,(comma).
 Any character may be used inside a character or hollerith string.
CC_NOT_DIGITS [   c0] invalid character in statement number columns
CC_NOT_DIGITS [j  c0] 文番号桁に不適切な文字があります。
 A column in columns 1 to 5 of the indicated statement contains a
 non-digit character.
 Columns 1 to 5 contain the statement number label.
 It is made up of digits from 0 to 9 and is greater than 0 and less
 than or equal to 99999.
CC_NOT_INITIAL [   c1] character in continuation column, but no statement to continue
CC_NOT_INITIAL [j  c1] 継続桁に文字がありますが,継続する行がありません。
 The character in column 6 indicates that this line is a continuation
 of the previous statement but there is no previous statement to
 continue.
CC_SET_EXTEND [   c0] character encountered is not FORTRAN 77 standard
CC_SET_EXTEND [j  c0] FORTRAN 77の標準ｾｯﾄにはない文字があります。
 A non-standard character was encountered in the source input stream.
 This is most likely caused by the use of lower case letters.
CC_STMTNO_ON_CONT [   c0] columns 1-5 in a continuation line must be blank
CC_STMTNO_ON_CONT [j  c0] 継続行のｶﾗﾑ1から5はﾌﾞﾗﾝｸでなければなりません。
 When column 6 is marked as a continuation statement to the previous
 line, columns 1 to 5 must be left blank.
CC_TOO_MANY_CONT [   c0] more than 19 continuation lines is not FORTRAN 77 standard
CC_TOO_MANY_CONT [j  c0] 19行を超える継続行はFORTRAN 77標準ではありません。
 More than 19 continuation lines is an extension to the FORTRAN 77 language.
CC_EOL_COMMENT [   c0] end-of-line comment is not FORTRAN 77 standard
CC_EOL_COMMENT [j  c0] 終端行注訳はFORTRAN 77標準ではありません。
 End-of-line comments are an extension to the FORTRAN 77 language.
 End-of-line comments start with the exclamation mark (!) character.
CC_D_IN_COLUMN_1 [   c0] D in column 1 is not FORTRAN 77 standard
CC_D_IN_COLUMN_1 [j  c0] 1ｶﾗﾑ目のDはFORTRAN 77標準ではありません。
 A "D" in column 1 signifies a debug statement that is compiled
 when the "__debug__" macro symbol is defined.
 If the "__debug__" macro symbol is not defined, the statement is ignored.
 The "c$define" compiler directive or the "define" compiler option can be
 used to define the "__debug__" macro symbol.
CC_CONT_OVERFLOW [   c0] too many continuation lines
CC_CONT_OVERFLOW [j  c0] 継続行が多すぎます。
 The limit on the number of continuation lines has been reached.
 This limit depends on the size of each continuation line.
 A minimum of 61 continuation lines is permitted.
 If the "xline" option is used, a minimum of 31 continuation lines is
 permitted.
CM COMMON Blocks
CM_ALREADY_IN_COM [   c2] %s1 already in COMMON
CM_ALREADY_IN_COM [j  c2] %s1は他のCOMMONﾌﾞﾛｯｸにもあります。
 The variable or array name, %s1, has already been specified in this or
 another COMMON block.
CM_COMMON [   c0] initializing %s1 in COMMON outside of block data subprogram is not FORTRAN 77 standard
CM_COMMON [j  c0] BLOCKDATA副ﾌﾟﾛｸﾞﾗﾑの外側のCOMMONﾌﾞﾛｯｸで%s1を初期化することは,FORTRAN 77標準ではありません。
 The symbol %s1, in a named COMMON block, has been initialized outside of a
 block data subprogram.
 This is an extension to the FORTRAN 77 language.
CM_MIXED_COMMON [   c0] character and non-character data in COMMON is not FORTRAN 77 standard
CM_MIXED_COMMON [j  c0] FORTRAN 77標準ｾｯﾄではCOMMONﾌﾞﾛｯｸの中に文字や数値を同時に含めることはできせん。
 The FORTRAN 77 standard specifies that a COMMON block cannot contain
 both numeric and character data.
 Allowing COMMON blocks to contain both numeric and character data is an
 extension to the FORTRAN 77 standard.
CM_NAMED_DIFF_SIZE [   c0] COMMON block %s1 has been defined with a different size
CM_NAMED_DIFF_SIZE [j  c0] COMMONﾌﾞﾛｯｸ%s1は異なる大きさで定義されていました。
 The COMMON block %s1 has been defined with a different size in another
 subprogram.
 A named COMMON block must define the same amount of storage units
 where ever named.
CM_BLKDATA_ALREADY [   c0] named COMMON block %s1 appears in more than one BLOCK DATA subprogram
CM_BLKDATA_ALREADY [j  c0] 名前の付いたCOMMONﾌﾞﾛｯｸ%s1が,2個以上のBLOCK DATA副ﾌﾟﾛｸﾞﾗﾑ にあります。
 The named COMMON block, %s1, may not appear in more than one BLOCK
 DATA subprogram.
CM_BLANK_DIFF_SIZE [   c0] blank COMMON block has been defined with a different size
CM_BLANK_DIFF_SIZE [j  c0] 名前のないCOMMONﾌﾞﾛｯｸが異なる大きさで定義されていました。
 The blank COMMON block has been defined with a different size in
 another subprogram.
 This is legal but a warning message is issued.
CN Constants
CN_DOUBLE_COMPLEX [   c1] DOUBLE PRECISION COMPLEX constants are not FORTRAN 77 standard
CN_DOUBLE_COMPLEX [j  c1] FORTRAN 77標準ｾｯﾄでは倍精度の複素数型の定数は使えません。
 Double precision complex numbers are an extension to the FORTRAN 77
 language.
 The indicated number is a complex number and at least one of the
 parts, real or imaginary, is a double precision constant.
 Both real and imaginary parts will be double precision.
CN_FLOAT [   c2] invalid floating-point constant %s1
CN_FLOAT [j  c2] %s1は正しい浮動小数点定数ではありません。
 The floating-point constant %s1 is invalid.
 Refer to the chapter entitled "Names, Data Types and Constants"
 in the Language Reference.
CN_ZERO_LEN [   c0] zero length character constants are not allowed
CN_ZERO_LEN [j  c0] 長さがｾﾞﾛの文字定数は使えません。
 FORTRAN 77 does not allow character constants of length 0 (i.e., an
 empty string).
CN_BAD_HEX_OCT [   c2] invalid hexadecimal/octal constant
CN_BAD_HEX_OCT [j  c2] 16進数／8進数の定数が無効です。
 An invalid hexadecimal or octal constant was specified.
 Hexadecimal constants can only contain digits or the letters 'a' through
 'f' and 'A' through 'F'.
 Octal constants can only contain the digits '0' through '7'.
CN_HEX_OCT [   c2] hexadecimal/octal constant is not FORTRAN 77 standard
CN_HEX_OCT [j  c2] 16進数／8進数の定数はFORTRAN 77の標準ではありません。
 Hexadecimal and octal constants are extensions to the FORTRAN 77 standard.
CO Compiler Options
CO_ALREADY_OPEN [   c0] %s1 is already being included
CO_ALREADY_OPEN [j  c0] %s1は既に読み込まれています。
 An attempt has been made to include a file that is currently being
 included in the program.
CO_BAD_NO [   c0] '%s1' option cannot take a NO prefix
CO_BAD_NO [j  c0] '%s1'ｵﾌﾟｼｮﾝの前にNOは付けられません。
 The compiler option %s1, cannot have the NO prefix specified.
 The NO prefix is used to negate an option.
 Certain options, including all options that require a value cannot have a
 NO prefix.
CO_NEED_EQUALS [   c0] expecting an equals sign following the %s1 option
CO_NEED_EQUALS [j  c0] %s1ｵﾌﾟｼｮﾝの後に＝が必要です。
 The compiler option %s1, requires an equal sign to be between the
 option keyword and its associated value.
CO_WANT_NUMBER [   c0] the '%s1' option requires a number
CO_WANT_NUMBER [j  c0] '%s1'ｵﾌﾟｼｮﾝには数字の値が必要です。
 The compiler option %s1 and an equal sign has been detected but the
 required associated value is missing.
CO_NOT_RECOG [   c0] option '%s1' not recognized - ignored
CO_NOT_RECOG [j  c0] '%s1'ｵﾌﾟｼｮﾝは使えません。無視しました。
 The option %s1 is not a recognized compiler option and has been ignored.
 Consult the User's Guide for a complete list of compiler options.
CO_NOT_IN_SOURCE [   c0] '%s1' option not allowed in source input stream
CO_NOT_IN_SOURCE [j  c0] '%s1'ｵﾌﾟｼｮﾝはﾌﾟﾛｸﾞﾗﾑの中に書けません。ｺﾏﾝﾄﾞ行で指示してください。
 The option %s1 can only be specified on the command line.
 Consult the User's Guide for a description of which options are allowed
 in the source input stream.
CO_MACRO_NESTING_EXCEEDED [   c0] nesting level exceeded for compiler directives
CO_MACRO_NESTING_EXCEEDED [j  c0] ｺﾝﾊﾟｲﾗｰ疑似命令のﾈｽﾄﾚﾍﾞﾙが深すぎます。
 Use of the C$IFDEF or C$IFNDEF compiler directives has caused the maximum
 nesting level to be exceeded.
 The maximum nesting level is 16.
CO_MACRO_STRUCTURE_MISMATCH [   c0] mismatching compiler directives
CO_MACRO_STRUCTURE_MISMATCH [j  c0] ｺﾝﾊﾟｲﾗｰ疑似命令が対応していません。
 This error message is issued if, for example, a C$ENDIF directive is used and
 no matching C$IFDEF or C$IFNDEF precedes it.
 Incorrect nesting of C$IFDEF, C$IFNDEF, C$ELSE and C$ENDIF directives will
 also cause this message to be issued.
CO_DATA_IN_BAD_FILE [ w c0] DATA option not allowed
CO_DATA_IN_BAD_FILE [jw c0] 読み込むﾌﾟﾛｸﾞﾗﾑの中にDATAｺﾝﾊﾟｲﾗｰｵﾌﾟｼｮﾝを含めてはいけません。
 A source file has been included into the current program through the
 use of the INCLUDE compiler option.
 This included source file cannot contain the DATA compiler option.
CO_LIMIT_TOO_BIG [ w c0] maximum limit exceeded in the '%s1' option - option ignored
CO_LIMIT_TOO_BIG [jw c0] '%s1'の最大限界値をｵｰﾊﾞ-しました。ｵﾌﾟｼｮﾝは無視されました。
 The user has specified a value on an option which exceeds the maximum allowed
 value.
CO_DATA_WITH_OBJ [ w c0] DATA option not allowed with OBJECT option
CO_DATA_WITH_OBJ [jw c0] DATAｵﾌﾟｼｮﾝとOBJECTｵﾌﾟｼｮﾝは同時に指定できません。
 The DATA compiler option can not appear a file that is compiled with the
 OBJECT option.
CP Compiler Errors
CP_TERMINATE [   r0] program abnormally terminated
CP_TERMINATE [j  r0] ﾌﾟﾛｸﾞﾗﾑが異常終了しました。
 This message is issued during the execution of the program.
 If you are running WATFOR-77, this message indicates that an internal error
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
CP_BAD_PARM_REGISTER [ o c0] argument %d1 incompatible with register
CP_BAD_PARM_REGISTER [jo c0] 引数%d1がﾚｼﾞｽﾀと一致しません。
 The register specified in an auxiliary pragma for argument number %d1
 is invalid.
CP_BAD_RETURN_REGISTER [ o c0] subprogram %s1 has invalid return register
CP_BAD_RETURN_REGISTER [jo c0] 副ﾌﾟﾛｸﾞﾗﾑ%s1に間違ったﾘﾀｰﾝﾚｼﾞｽﾀがあります。
 The register specified in an auxiliary pragma for the return value of
 function %s1 is invalid.
 This error is issued when, for example, an auxiliary pragma is used to
 specify EAX as the return register for a double precision function.
CP_LOW_ON_MEMORY [ o c0] low on memory - unable to fully optimize %s1
CP_LOW_ON_MEMORY [jo c0] ﾒﾓﾘｰ不足です。- %s1を最適化できませんでした。
 There is not enough memory for the code generator to fully optimize
 subprogram %s1.
CP_BACK_END_ERROR [ o c0] internal compiler error %d1
CP_BACK_END_ERROR [jo c0] ｺﾝﾊﾟｲﾗ内部ｴﾗｰです。 %d1
 This error is an internal code generation error.
 Please report the specified internal compiler error number and any other
 helpful information about the program being compiled to Watcom so that
 the problem can be fixed.
CP_BAD_SAVE [ o c0] illegal register modified by %s1
CP_BAD_SAVE [jo c0] %s1で変更したレジスタは正しくありません。
 An illegal register was said to be modified by %s1 in the auxiliary pragma
 for %s1.
 In a 32-bit flat memory model, the base pointer register EBP and segment
 registers CS, DS, ES, and SS cannot be modified.
 In small data models, the base pointer register (32-bit EBP or 16-bit BP)
 and segment registers CS, DS, and SS cannot be modified.
 In large data models, the base pointer register (32-bit EBP or 16-bit BP)
 and segment registers CS, and SS cannot be modified.
CP_ERROR [ o c0] %s1
CP_ERROR [jo c0] %s1
 The message specified by %s1 indicates an error during the code generation
 phase.
 The most probable cause is an invalid instruction in the in-line assembly
 code specified in an auxiliary pragma.
CP_FATAL_ERROR [ o c0] fatal: %s1
CP_FATAL_ERROR [jo c0] 致命的エラー: %s1
 The specified error indicates that the code generator has been abnormally
 terminated.
 This message will be issued if any internal limit is reached or a keyboard
 interrupt sequence is pressed during the code generation phase.
CP_MEMORY_NOT_FREED [   c0] dynamic memory not freed
CP_MEMORY_NOT_FREED [j  c0] 動的ﾒﾓﾘｰが解放されませんでした。
 This message indicates an internal compiler error.
 Please report this error and any other helpful information about the program
 being compiled to Watcom so that the problem can be fixed.
CP_FREEING_UNOWNED_MEMORY [   c0] freeing unowned dynamic memory
CP_FREEING_UNOWNED_MEMORY [j  c0] 確保していない動的ﾒﾓﾘｰが解放されました。
 This message indicates an internal compiler error.
 Please report this error and any other helpful information about the program
 being compiled to Watcom so that the problem can be fixed.
CP_AUTO_EQUIV_TOO_LARGE [ o c0] The automatic equivalence containing %s1 exceeds 32K limit
CP_AUTO_EQUIV_TOO_LARGE [jo c0] %s1を含む自動等価変数が32Kの限界を超えました。
 In 16-bit environments, the size of an equivalence on the stack must not 
 exceed 32767 bytes.
CP_AUTO_RET_TOO_LARGE [ o c0] The return value of %s1 exceeds 32K limit
CP_AUTO_RET_TOO_LARGE [jo c0] %s1の戻り値が32Kの限界を超えました。
 In 16-bit environments, the size of the return value of a function must not
 exceed 32767 bytes.
CP_AUTO_VAR_TOO_LARGE [ o c0] The automatic variable %s1 exceeds 32K limit
CP_AUTO_VAR_TOO_LARGE [jo c0] 自動変数%s1が32Kの限界を超えました。
 In 16-bit environments, the size of any variable on the stack must not
 exceed 32767 bytes.
CV Character Variables
CV_BAD_LEN [   c0] CHARACTER variable %s1 with length (*) not allowed in this expression
CV_BAD_LEN [j  c0] この式の中に長さ(*)を指定した文字変数-%s1は使えません。
 The length of the result of evaluating the expression is
 indeterminate.
 One of the operands has an indeterminate length and the result is
 being assigned to a temporary.
CV_CHARSTAR_ILLEGAL [   c0] character variable %s1 with length (*) must be a subprogram argument
CV_CHARSTAR_ILLEGAL [j  c0] 長さに(*)を指定した文字変数%s1は副ﾌﾟﾛｸﾞﾗﾑの仮引数以外に使えません。
 The character variable %s1 with a length specification (*) can only be
 used to declare dummy arguments in the subprogram.
 The length of a dummy argument assumes the length of the
 corresponding actual argument.
CV_OVERLAPING_ASSGT [ w r0] left and right hand sides overlap in a character assignment statement
CV_OVERLAPING_ASSGT [jw r0] 文字の代入文において左右の文字部分列が重なり合って、互いに内容をこわしてしまいます。
 The expression on the right hand side defines a substring of a
 character variable and tries to assign it to an overlapping part of
 the same character variable.
DA Data Initialization
DA_BAD_DO_VAR [   c2] implied DO variable %s1 must be an integer variable
DA_BAD_DO_VAR [j  c2] DO形並びの変数%s1はINTEGER変数でなければなりません。
 The implied DO variable %s1 must be declared as a variable of type
 INTEGER or must have an implicit INTEGER type.
DA_BAD_RPT_SPEC [   c2] repeat specification must be a positive integer
DA_BAD_RPT_SPEC [j  c2] 繰返しの指定はﾌﾟﾗｽの整数でなければなりません。
 The repeat specification in the constant list of the DATA statement
 must be an unsigned positive integer.
DA_BAD_VAR_IN_EXPR [   c2] %s1 appears in an expression but is not an implied DO variable
DA_BAD_VAR_IN_EXPR [j  c2] DATAの式の中に使われている%s1はDO形並びの変数でなければなりません。
 The variable %s1 is used to express the array elements in the DATA
 statement but the variable is not used as an implied DO variable.
DA_BLANK_INIT [   c0] %s1 in blank COMMON block cannot be initialized
DA_BLANK_INIT [j  c0] 名前の無いCOMMONﾌﾞﾛｯｸにある%s1にはDATAで初期値を与えられません。
 A blank or unnamed COMMON block is a COMMON statement with the block
 name omitted.
 The entries in blank COMMON blocks cannot be initialized using DATA
 statements.
DA_HEX_CONST [   c2] data initialization with hexadecimal constant is not FORTRAN 77 standard
DA_HEX_CONST [j  c2] FORTRAN 77標準では16進数で初期値は与えられません。
 Data initialization with hexadecimal constants is an extension to the
 FORTRAN 77 language.
DA_ILL_NAME [   c0] cannot initialize %s1 %s2
DA_ILL_NAME [j  c0] DATAで%s1 %s2には初期値を与えられません。
 Symbol %s2 was used as a %s1.
 It is illegal for such a symbol to be initialized in a DATA statement.
 The DATA statement can only be used to initialize variables, arrays,
 array elements, and substrings.
DA_IN_TYPE_STMT [   c2] data initialization in %s1 statement is not FORTRAN 77 standard
DA_IN_TYPE_STMT [j  c2] FORTRAN 77標準では%s1ｽﾃｰﾄﾒﾝﾄで初期値を与えられません。
 Data initialization in type specification statements is an extension
 to the FORTRAN 77 language.
 These include: CHARACTER, COMPLEX, DOUBLE PRECISION, INTEGER,
 LOGICAL, and REAL.
DA_NOT_ENOUGH [ o c0] not enough constants for list of variables
DA_NOT_ENOUGH [jo c0] DATAの中の変数に対応する定数の数が足りません。
 There are not enough constants specified to initialize all of the
 names listed in the DATA statement.
DA_NOT_ENOUGH [ w r0] not enough constants for list of variables
DA_NOT_ENOUGH [jw r0] DATAの中の変数に対応する定数の数が足りません。
 There are not enough constants specified to initialize all of the
 names listed in the DATA statement.
DA_TOO_MUCH [ o c0] too many constants for list of variables
DA_TOO_MUCH [jo c0] 変数に対して定数の数が多過ぎます。
 There are too many constants specified to initialize the names listed
 in the DATA statement.
DA_TOO_MUCH [ w r0] too many constants for list of variables
DA_TOO_MUCH [jw r0] 変数に対して定数の数が多過ぎます。
 There are too many constants specified to initialize the names listed
 in the DATA statement.
DA_TYPE_MISMATCH [ o c0] cannot initialize %s1 variable %s2 with %s3 constant
DA_TYPE_MISMATCH [jo c0] %s1変数に%s2定数を初期値として入れられません。
 The constant of type %s3 cannot be used to initialize the variable %s2 of
 type %s1.
DA_TYPE_MISMATCH [ w r0] cannot initialize %s1 variable with %s2 constant
DA_TYPE_MISMATCH [jw r0] %s1変数に%s2定数を初期値として入れられません。
 The constant of type %s2 cannot be used to initialize a variable of
 type %s1.
DA_DATA_TWICE [ w r0] entity can only be initialized once during data initialization
DA_DATA_TWICE [jw r0] DATAの中で1個の変数に1度以上、初期値を与えてはいけません。
 An attempt has been made to initialize an entity more than once in
 DATA statements.
DM Dimensioned Variables
DM_SYM_PARM [   c2] using %s1 incorrectly in dimension expression
DM_SYM_PARM [j  c2] 次元の定義式には%s1型は使えません。整数式のみです。
 The name used as a dimension declarator has been previously declared
 as type %s1 and cannot be used as a dimension declarator. A dimension
 declarator must be an integer expression.
DM_RANGE_ERR_ON_DIMN [ w r0] array or array element (possibly substrung) associated with %s1 too small
DM_RANGE_ERR_ON_DIMN [jw r0] 配列または配列要素(文字列の一部など)と思われる%s1のｻｲｽﾞが小さすぎます。
 The dummy argument, array %s1, is defined to be larger than the size
 of the actual argument.
DO DO-loops
DO_BACKWARDS_DO [   c2] statement number %i1 already defined in line %d2 - DO loop is backwards
DO_BACKWARDS_DO [j  c2] DOの終りを示す文番号%i1は既に%d2行目で定義されています。終わり文番号は、DO文より後ろです。
 The statement number to indicate the end of the DO control structure
 has been used previously in the program unit and cannot be used to
 terminate the DO loop.
 The terminal statement named in the DO statement must follow the DO
 statement.
DO_ENDING_BAD [   c0] %s1 statement not allowed at termination of DO range
DO_ENDING_BAD [j  c0] %s1文はDO範囲の終りとして使えません。
 A non-executable statement cannot be used as the terminal statement
 of a DO loop.
 These statements include: all declarative statements,
 ADMIT, AT END, BLOCK DATA, CASE, DO, ELSE, ELSE IF, END, END AT END,
 END BLOCK, END GUESS, END IF, END LOOP, END SELECT, END WHILE, ENTRY,
 FORMAT, FUNCTION, assigned GO TO, unconditional GO TO, GUESS,
 arithmetic and block IF, LOOP, OTHERWISE, PROGRAM, RETURN, SAVE,
 SELECT, STOP, SUBROUTINE, UNTIL, and WHILE.
DO_NESTING_BAD [   c0] improper nesting of DO loop
DO_NESTING_BAD [j  c0] 外側のDOﾙｰﾌﾟが終わる前に内側のDOﾙｰﾌﾟは正しく終わらなければなりません。
 A nested DO loop has not been properly terminated before the
 termination of the outer DO loop.
DO_BAD_ENDDO [   c0] ENDDO cannot terminate DO loop with statement label
DO_BAD_ENDDO [j  c0] ENDDOではﾗﾍﾞﾙ付きのDOﾙｰﾌﾟを終えることはできません。
 The ENDDO statement can only terminate a DO loop in which no
 statement label was specified in the defining DO statement.
DO_DO_EXT [   c0] this DO loop form is not FORTRAN 77 standard
DO_DO_EXT [j  c0] このDOﾙｰﾌﾟの形式はFORTRAN 77の標準ではありません。
 As an extension to FORTRAN 77, the following forms of the DO loop are
 also supported.
 .autonote
 .note
 A DO loop with no statement label specified in the defining DO
 statement.
 .note
 The DO WHILE form of the DO statement.
 .endnote
DO_NO_COMMA_OR_VAR [   c1] expecting comma or DO variable
DO_NO_COMMA_OR_VAR [j  c1] DOと終了を示す番号行の次はｺﾝﾏかDO変数がなければなりません。
 The item following the DO keyword and the terminal statement-label
 (if present) must be either a comma or a DO variable.
 A DO variable is an integer, real or double precision variable name.
 The DO statement syntax is as follows:
 .millust begin
 DO <tsl> <,> DO-var = ex, ex <, ex>
 .millust end
DO_PARM_REDEFINED [   c2] DO variable cannot be redefined while DO loop is active
DO_PARM_REDEFINED [j  c2] DOﾙｰﾌﾟの中でDO変数の値を実行中に変えてはいけません。
 The DO variable named in the DO statement cannot have its value
 altered by a statement in the DO loop structure.
DO_0INC [ w r0] incrementation parameter for DO-loop cannot be zero
DO_0INC [jw r0] DOﾙｰﾌﾟの増分にｾﾞﾛを指定してはいけません。
 The third expression in the DO statement cannot be zero.
 This expression indicates the increment to the DO variable each
 iteration of the DO loop.
 If the increment expression is not specified a value of 1 is assumed.
EC Equivalence and/or Common
EC_PAST_BEG [   c0] equivalencing %s1 has caused extension of COMMON block %s2 to the left
EC_PAST_BEG [j  c0] EQUIVALENCEされた%s1はCOMMONﾌﾞﾛｯｸ%s2を左側へ拡張しました。FORTRAN 77標準では、禁止されている方法です。
 The name %s1 has been equivalenced to a name in the COMMON block %s2.
 This relationship has caused the storage of the COMMON block to be
 extended to the left.
 FORTRAN 77 does not allow a COMMON block to be extended in this way.
EC_2NAM_EC [   c0] %s1 and %s2 in COMMON are equivalenced to each other
EC_2NAM_EC [j  c0] 同じCOMMONﾌﾞﾛｯｸの内にある%s1と%s2は互いにEQUIVALENCEできません。
 The names %s1 and %s2 appear in different COMMON blocks and each
 occupies its own piece of storage and therefore cannot be
 equivalenced.
EN END Statement
EN_NO_END [   c0] missing END statement
EN_NO_END [j  c0] END文がありません。
 The END statement for a PROGRAM, SUBROUTINE, FUNCTION or BLOCK DATA subprogram
 was not found before the next subprogram or the end of the source input
 stream.
EQ Equal Sign
EQ_BAD_TARGET [   c1] target of assignment is illegal
EQ_BAD_TARGET [j  c1] 代入のﾀｰｹﾞｯﾄが間違っています。
 The target of an assignment statement, an input/output status specifier
 in an input/output statement, or an inquiry specifier in an INQUIRE
 statement, is illegal.
 The target in any of the above cases must be a variable name, array element,
 or a substring name.
EQ_CANNOT_ASSIGN [   c2] cannot assign value to %s1
EQ_CANNOT_ASSIGN [j  c2] 代入文の左側の%s1ﾀｲﾌﾟに値を割り当てることはできません。
 An attempt has been made to assign a value to a symbol with class %s1.
 For example, an array name cannot be the target of an assignment
 statement.
 This error may also be issued when an illegal target is used for the
 input/output status specifier in an input/output statement or an inquiry
 specifier in an INQUIRE statement.
EQ_ILL_EQ_SIGN [   c1] illegal use of equal sign
EQ_ILL_EQ_SIGN [j  c1] =の使い方がまちがっています(代入文ではありません)。
 An equal sign has been found in the statement but the statement is
 not an assignment statement.
EQ_MULT_ASSGN [   c0] multiple assignment is not FORTRAN 77 standard
EQ_MULT_ASSGN [j  c0] 複数の代入用 = の使用はFORTRAN 77標準ではありません。
 More than one equal sign has been found in the assignment statement.
EQ_NO_EQUALS [   c1] expecting equals sign
EQ_NO_EQUALS [j  c1] 等号(=)が必要です。
 The equal sign is missing or misplaced.
 The PARAMETER statement uses an equal sign to equate a symbolic name
 to the value of a constant expression.
 The I/O statements use an equal sign to equate the appropriate values
 to the various specifiers.
 The DO statement uses an equal sign to assign the initial value to
 the DO variable.
EV Equivalenced Variables
EV_DIFF_REL_POS [   c0] %s1 has been equivalenced to 2 different relative positions
EV_DIFF_REL_POS [j  c0] %s1を2つの異なる場所にEQUIVALENCEしてはいけません。
 The storage unit referenced by %s1 has been equivalenced to two
 different storage units starting in two different places.
 One name cannot be associated to two different values at the same
 time.
EV_EQUIV_LIST [   c0] EQUIVALENCE list must contain at least 2 names
EV_EQUIV_LIST [j  c0] EQUIVALENCEには少なくとも2つのｼﾝﾎﾞﾙ名が対で必要です。
 The list of names to make a storage unit equivalent to several names
 must contain at least two names.
EV_SSCR_INVALID [   c0] %s1 incorrectly subscripted in %s2 statement
EV_SSCR_INVALID [j  c0] %s1が%s2文の中で間違って添字を付けられています。
 The name %s1 has been incorrectly subscripted in a %s2 statement.
EV_SSTR_INVALID [   c0] incorrect substring of %s1 in %s2 statement
EV_SSTR_INVALID [j  c0] %s1は%s2文の中で間違った部分列となっています。
 An attempt has been made to incorrectly substring %s1 in a %s2
 statement.
 For example, if a CHARACTER variable was declared to be of length 4
 then (2:5) would be an invalid substring expression.
EV_MIXED_EQUIV [   c0] equivalencing CHARACTER and non-CHARACTER data is not FORTRAN 77 standard
EV_MIXED_EQUIV [j  c0] FORTRAN 77の標準では文字と他のﾀｲﾌﾟのﾃﾞｰﾀをEQUIVALENCEできません。
 Equivalencing numeric and character data is an extension to the
 FORTRAN 77 language.
EV_ONLY_IF_CHAR [   c0] attempt to substring %s1 in EQUIVALENCE statement but type is %s2
EV_ONLY_IF_CHAR [j  c0] EQUIVALENCEで部分列を示す%s1は%s2ですが文字型の必要があります。
 An attempt has been made to substring the symbolic name %s1 in an
 EQUIVALENCE statement but the type of the name is %s2 and should be
 of type CHARACTER.
EX Exponentiation
EX_Z_2_NOT_POS [    2] zero**J where J <= 0 is not allowed
EX_Z_2_NOT_POS [j   2] J <= O の時 ZERO**J の計算はできません。
 Zero cannot be raised to a power less than or equal to zero.
EX_NOT_INT_ARG [   r0] X**Y where X < 0.0, Y is not of type INTEGER, is not allowed
EX_NOT_INT_ARG [j  r0] X < 0.0 の時,Yが整数でなければ X**Y の計算はできません。
 When X is less than zero, Y may only be of type INTEGER.
EX_CZ_2_NOT_REAL [   r0] (0,0)**Y where Y is not real is not allowed
EX_CZ_2_NOT_REAL [j  r0] Yが実数でなければ (0,0)**Y の計算はできません。
 In complex exponentiation, when the base is zero, the exponent may only be a
 real number or a complex number whose imaginary part is zero.
EY ENTRY Statement
EY_TYPE_MISMATCH [   c0] type of entry %s1 does not match type of function %s2
EY_TYPE_MISMATCH [j  c0] ENTRY %s1の型が,関数%s2の型と一致しません。
 If the type of a function is CHARACTER or a user-defined STRUCTURE, then the
 type of all entry names must match the type of the function name.
EY_NOT_IN_CS [   c0] ENTRY statement not allowed within structured control blocks
EY_NOT_IN_CS [j  c0] SELECT CASEやREMOTE BLOCKのような構造化ﾌﾞﾛｯｸ･ｽﾃｰﾄﾒﾝﾄの中にENTRYは使えません。
 FORTRAN 77 does not allow an ENTRY statement to appear between the
 start and end of a control structure.
EY_SIZE_MISMATCH [   c0] size of entry %s1 does not match size of function %s2
EY_SIZE_MISMATCH [j  c0] ENTRY %s1と関数名%s2は同じ長さでなければなりません。
 The name %s1 found in an ENTRY statement must be declared to be the
 same size as that of the function name.
 If the name of the function or the name of any entry point has a
 length specification of (*), then all such entries must have a length
 specification of (*) otherwise they must all have a length
 specification of the same integer value.
FM Format
FM_ASSUME_COMMA [    2] missing delimiter in format string, comma assumed
FM_ASSUME_COMMA [j   2] FORMATの編集記述子を区分するものがありません。ｺﾝﾏを区分するために挿入しました。
 The omission of a comma between the descriptors listed in a format
 string is an extension to the FORTRAN 77 language.
 Care should be taken when omitting the comma since the assumed
 separation may not occur in the intended place.
FM_CONST [    2] missing or invalid constant
FM_CONST [j   2] 編集記述子の1部である定数が無いか、形式が正しくありません。
 An unsigned integer constant was expected with the indicated edit
 descriptor but was not correctly placed or was missing.
FM_D_EXT [    0] Ew.dDe format code is not FORTRAN 77 standard
FM_D_EXT [j   0] Ew.dDe形式の編集記述子はFORTRAN 77の標準ではありません。
 The edit descriptor Ew.dDe is an extension to the FORTRAN 77
 language.
FM_DECIMAL [    2] missing decimal point
FM_DECIMAL [j   2] 小数点が必要です。
 The indicated edit descriptor must have a decimal point and an
 integer to indicate the number of decimal positions.
 These edit descriptors include: F, E, D and G.
FM_DELIM [    2] missing or invalid edit descriptor in format string
FM_DELIM [j   2] FORMATの編集記述子が無いか、書き方が正しくありません。
 In the format string, two delimiters were found in succession with no
 valid descriptor in between.
FM_FMTCHAR [    2] unrecognizable edit descriptor in format string
FM_FMTCHAR [j   2] FORMATの中に間違った編集記述子があります。
 An edit descriptor has been found in the format string that is not a
 valid code.
 Valid codes are: apostrophe ('), I, F, E, D, G, L, A, Z, H, T, TL,
 TR, X, /, :, S, SP, SS, P, BN, B, $, and \.
FM_INV_REP [    2] invalid repeat specification
FM_INV_REP [j   2] 繰り返し指定が正しくありません。
 The indicated repeatable edit descriptor is invalid.
 The forms of repeatable edit descriptors are:
 Iw, Iw.m, Fw.d, Ew.d, Ew.dEe, Dw.d, Gw.d, Gw.dEe, Lw, A, Aw, Ew.dDe,
 and Zw where w and e are positive unsigned integer constants, and d
 and m are unsigned integer constants.
FM_M_EXT [    0] $ or \ format code is not FORTRAN 77 standard
FM_M_EXT [j   0] $や\のﾌｫｰﾏｯﾄｺｰﾄﾞはFORTRAN 77の標準ではありません。
 The non-repeatable edit descriptors $ and \ are extensions to the
 FORTRAN 77 language.
FM_MODIFIER [    2] invalid field modifier
FM_MODIFIER [j   2] 変数などに対する編集記述子が正しくありません。
 The indicated edit descriptor for a field is incorrect.
 Consult the Language Reference for the correct form of the edit descriptor.
FM_NO_EOS [   c2] expecting end of FORMAT statement but found more text
FM_NO_EOS [j  c2] FORMATの中に終りを示す)がありますがさらに右に記述が続いています。
 The right parenthesis was encountered in the FORMAT statement to
 terminate the statement and more text was found on the line.
FM_NO_REP [    2] repeat specification not allowed for this format code
FM_NO_REP [j   2] この編集記述子には繰返し指定は付けられません。
 A repeat specification was found in front of a format code that is a
 nonrepeatable edit descriptor.
 These include: apostrophe, H, T, TL, TR, X, /, :, S, SP, SS, P, BN,
 BZ, $,and \.
FM_NO_STMTNO [   c0] no statement number on FORMAT statement
FM_NO_STMTNO [j  c0] FORMAT文に文ﾗﾍﾞﾙが必要です。
 The FORMAT statement must have a statement label.
 This statement number is used by I/O statements to reference the
 FORMAT statement.
FM_QUOTE [    2] no closing quote on apostrophe edit descriptor
FM_QUOTE [j   2] 引用符('')編集記述子を閉じるためのもう1個の引用符がありません。
 The closing quote of an apostrophe edit descriptor was not found.
FM_SPEC_256 [    2] field count greater than 256 is invalid
FM_SPEC_256 [j   2] 編集記述子でﾌｨｰﾙﾄﾞの長さは256を越えてはいけません。
 The repeat specification of the indicated edit descriptor is greater
 than the maximum allowed of 256.
FM_WIDTH [    2] invalid field width specification
FM_WIDTH [j   2] 幅(長さ)の指定が正しくありません。
 The width specifier on the indicated edit descriptor is invalid.
FM_Z_EXT [    0] Z format code is not FORTRAN 77 standard
FM_Z_EXT [j   0] Z変数記述子はFORTRAN 77の標準ではありません。
 The Z (hexadecimal format) repeatable edit descriptor is an extension
 to the FORTRAN 77 language.
FM_TOO_LARGE [   r0] FORMAT statement exceeds allotted storage size
FM_TOO_LARGE [j  r0] 1個のFORMAT文が大きすぎます。分割してください。
 The maximum allowable size of a FORMAT statement has exceeded.
 The statement must be split into two or more FORMAT statements.
FM_NOT_INP [   r0] format specification not allowed on input
FM_NOT_INP [j  r0] 入力用としてFORMATの中の編集記述子は使えません。
 A format specification, in the FORMAT statement, is not allowed to be
 used as an input specification.
 Valid specifications include: T, TL,TR, X, /, :, P, BN, BZ, I, F, E,
 D, G, L, A, and Z.
FM_REP [   r0] FORMAT missing repeatable edit descriptor
FM_REP [j  r0] I/O用としてFORMATの中の書式仕様に繰り返し指定が必要です。
 An attempt has been made to read or write a piece of data without a
 valid repeatable edit descriptor.
 All data requires a repeatable edit descriptor in the format.
 The forms of repeatable edit descriptors are:
 Iw, Iw.m, Fw.d, Ew.d, Ew.dEe, Dw.d, Gw.d, Gw.dEe, Lw, A, Aw, Ew.dDe,
 and Zw where w and e are positive unsigned integer constants, and d
 and m are unsigned integer constants.
FM_ASSUME_CONST [    2] missing constant before X edit descriptor, 1 assumed
FM_ASSUME_CONST [j   2] X編集記述子の前に定数がありません。1 を挿入しました。
 The omission of the constant before an X edit descriptor in a format
 specification is an extension to the FORTRAN 77 language.
FM_Q_EXT [    0] Ew.dQe format code is not FORTRAN 77 standard
FM_Q_EXT [j   0] Ew.dQe形式の編集記述子はFORTRAN 77の標準ではありません。
 The edit descriptor Ew.dQe is an extension to the FORTRAN 77
 language.
FM_Q_FORMAT [    0] Qw.d format code is not FORTRAN 77 standard
FM_Q_FORMAT [j   0] Qw.d形式の編集記述子はFORTRAN 77の標準ではありません。
 The edit descriptor Qw.d is an extension to the FORTRAN 77
 language.
GO GOTO and ASSIGN Statements
GO_ASSIGNED_BAD [   c0] %s1 statement label may not appear in ASSIGN statement but did in line %d2
GO_ASSIGNED_BAD [j  c0] %s1文ﾗﾍﾞﾙはASSIGN文に使えません。%d2行をﾁｪｯｸしてください。
 The statement label in the ASSIGN statement in line %d2 references a
 %s1 statement.
 The statement label in the ASSIGN statement must appear in the same
 program unit and must be that of an executable statement or a FORMAT
 statement.
GO_CANNOT_ASSIGN [   c2] ASSIGN of statement number %i1 in line %d2 not allowed
GO_CANNOT_ASSIGN [j  c2] ASSIGNの示す文番号%i1は実行できない行を指しています。%d2行をﾁｪｯｸしてください。
 The statement label %d1 in the ASSIGN statement is used in the line
 %d2 which references a non-executable statement.
 A statement label must appear in the same program unit as the ASSIGN
 statement and must be that of an executable statement or a FORMAT
 statement.
GO_NO_TO [   c2] expecting TO
GO_NO_TO [j  c2] ASSIGNのTOが無いか位置が間違っています。
 The keyword TO is missing or misplaced in the ASSIGN statement.
HO Hollerith Constants
HO_CONST [   c0] hollerith constant is not FORTRAN 77 standard
HO_CONST [j  c0] ﾎﾚﾘｽ定数はFORTRAN 77の標準ではありません。
 Hollerith constants are an extension to the FORTRAN 77 language.
HO_CUT_OFF [   c0] not enough characters for hollerith constant
HO_CUT_OFF [j  c0] 編集記述子のHかhの前の数(hollerith定数)に対してそれに続く文字が足りません。
 The number of characters following the H or h is not equal to the
 constant preceding the H or h.
 A hollerith constant consists of a positive unsigned integer constant
 n followed by the letter H or h followed by a string of exactly n
 characters.
IF IF Statements
IF_ELSE_LAST [   c0] ELSE block must be the last block in block IF
IF_ELSE_LAST [j  c0] ELSEﾌﾞﾛｯｸの後にまたELSE IFﾌﾞﾛｯｸが続いてはいけません。これがIFブロックの最後のﾌﾞﾛｯｸでなけれなりません。
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
IF_NO_THEN [   c1] expecting THEN
IF_NO_THEN [j  c1] IFにはTHENが必要です。
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
IL I/O Lists
IL_BAD_FMT_SPEC [   c2] missing or invalid format/FMT specification
IL_BAD_FMT_SPEC [j  c2] FORMAT/FMT仕様がないか、あるいは正しくありません。
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
IL_BAD_INTL [   c2] the UNIT may not be an internal file for this statement
IL_BAD_INTL [j  c2] この文でUNITに内部ﾌｧｲﾙを指定してはいけません(READ,WRITEのみです)。
 An internal file may only be referenced in a READ or WRITE statement.
 An internal file may not be referenced in a BACKSPACE, CLOSE,
 ENDFILE, INQUIRE, OPEN, or REWIND statement.
IL_BAD_LIST [   c2] %s1 statement cannot have %s2 specification
IL_BAD_LIST [j  c2] %s1文に%s2仕様は指定できません。
 The I/O statement %s1 may not have the control information %s2
 specified.
IL_BAD_SIZE [   c2] variable must have a size of 4
IL_BAD_SIZE [j  c2] この変数の長さは4でなければなりません。
 The variable used as a specifier in an I/O statement must be of size
 4 but another size was specified.
 These include the EXIST, OPENED, RECL, IOSTAT, NEXTREC, and NUMBER.
 The name used in the ASSIGN statement must also be of size 4 but a
 different size was specified.
IL_CTRL_LIST [   c2] missing or unrecognizable control list item %s1
IL_CTRL_LIST [j  c2] I/Oを制御する %s1項目に不足があるか,間違ったものがあります。
 A control list item %s1 was encountered in an I/O statement and is not
 a valid control list item for that statement, or a control list item
 was expected and was not found.
IL_DUP_LIST [   c2] attempt to specify control list item %s1 more than once
IL_DUP_LIST [j  c2] このI/Oを制御するための項目%s1は重複して指定できません。
 The control list item %s1 in the indicated I/O statement, has been
 named more than once.
IL_EMPTY_IMP_DO [   c1] implied DO loop has no input/output list
IL_EMPTY_IMP_DO [j  c1] DO形並びのﾙｰﾌﾟが、I/Oリストの変数や式に対応していません。
 The implied DO loop specified in the I/O statement does not
 correspond with a variable or expression in the input/output list.
IL_ILST [   c0] list-directed input/output with internal files is not FORTRAN 77 standard
IL_ILST [j  c0] 内部ﾌｧｲﾙへの並びによる入出力処理はFORTRAN 77の標準ではありません。
 List-directed input/output with internal files is an extension to the
 FORTRAN 77 language.
IL_NO_ASTERISK [   c2] FORTRAN 77 standard requires an asterisk for list-directed formatting
IL_NO_ASTERISK [j  c2] FORTRAN 77標準の場合,並びによる書式には(*)ｱｽﾀﾘｽｸが必要です。
 An optional asterisk for list-directed formatting is an extension to
 the FORTRAN 77 language.
 The standard FORTRAN 77 language specifies that an asterisk is required.
IL_NO_UNIT_ID [   c0] missing or improper unit identification
IL_NO_UNIT_ID [j  c0] UNITが無いか,そのﾌｧｲﾙ識別子(ﾌｧｲﾙ番号)が正しくありません。
 The control specifier, UNIT, in the I/O statement is either missing
 or identifies an improper unit.
 The unit specifier specifies an external unit or internal file.
 The external unit identifier is a non-negative integer expression or
 an asterisk.
 The internal file identifier is character variable, character array,
 character array element, or character substring.
IL_NO_FILE_OR_UNIT [   c0] missing unit identification or file specification
IL_NO_FILE_OR_UNIT [j  c0] UNITかFILEの指定がないか、間違っています。
 An identifier to specifically identify the required file is missing.
 The UNIT specifier is used to identify the external unit or internal
 file.
 The FILE specifier in the INQUIRE and OPEN statements is used to
 identify the file name.
IL_STAR_NOT_ALLOWED [   c1] asterisk unit identifier not allowed in %s1 statement
IL_STAR_NOT_ALLOWED [j  c1] %s1文ではｱｽﾀﾘｽｸ(*)でなく,ﾌｧｲﾙ識別子(ﾌｧｲﾙ番号)が必要です。
 The BACKSPACE, CLOSE, ENDFILE, INQUIRE, OPEN, and REWIND statements
 require the external unit identifier be an unsigned positive integer
 from 0 to 999.
IL_UNIT_AND_FILE [   c0] cannot have both UNIT and FILE specifier
IL_UNIT_AND_FILE [j  c0] UNITとFILEの両方は同時に指定できません。
 There are two valid forms of the INQUIRE statement; INQUIRE by FILE
 and INQUIRE by UNIT.
 Both of these specifiers cannot be specified in the same statement.
IL_AINTL [   c0] internal files require sequential access
IL_AINTL [j  c0] 内部ﾌｧｲﾙは順次ｱｸｾｽ処理でなければなりません。
 An attempt has been made to randomly access an internal file.
 Internal files may only be accessed sequentially.
IL_END_REC [   c0] END specifier with REC specifier is not FORTRAN 77 standard
IL_END_REC [j  c0] RECを伴うENDはFORTRAN 77の標準ではありません。
 The FORTRAN 77 standard specifies that an end-of-file condition can only
 occur with a file connected for sequential access or an internal file.
 The REC specifier indicates that the file is connected for direct access.
 This extension allows the programmer to detect an end-of-file condition when
 reading the records sequentially from a file connected for direct access.
IL_SPECIFIER_NOT_STANDARD [   c0] %s1 specifier in i/o list is not FORTRAN 77 standard
IL_SPECIFIER_NOT_STANDARD [j  c0] 入出力並び中の%s1指定子はFORTRAN 77標準ではありません。
 The specified i/o list item is provided as an extension to the
 FORTRAN 77 language.
IL_NO_IOLIST [   c0] i/o list is not allowed with NAMELIST-directed format
IL_NO_IOLIST [j  c0] NAMELIST指向の書式では,入出力並びは許されません。
 An i/o list is not allowed when the format specification is a NAMELIST.
IL_NON_CHARACTER [   c2] non-character array as format specifier is not FORTRAN 77 standard
IL_NON_CHARACTER [j  c2] 書式指定に文字以外の配列を使用するのは,FORTRAN 77の標準ではありません。
 A format specifier must be of type character unless it is an array name.
 Allowing a non-character array name is an extension to the FORTRAN 77 standard.
IM IMPLICIT Statements
IM_ILLEGAL_RANGE [   c2] illegal range of characters
IM_ILLEGAL_RANGE [j  c2] 範囲を指定する文字の順序が正しくありません。
 In the IMPLICIT statement, the first letter in the range of
 characters must be smaller in the collating sequence than
 the second letter in the range.
IM_PREV_IMPLICIT [   c2] letter can only be implicitly declared once
IM_PREV_IMPLICIT [j  c2] IMPLICITで同じ文字を重複して、指定してはいけません。
 The indicated letter has been named more than once in this or a
 previous IMPLICIT statement.
 A letter may only be named once.
IM_UNRECOG_TYPE [   c2] unrecognizable type
IM_UNRECOG_TYPE [j  c2] IMPLICITの指定された型が違います。
 The type declared in the IMPLICIT statement is not one of INTEGER,
 REAL, DOUBLE PRECISION, COMPLEX, LOGICAL or CHARACTER.
IM_CHAR_STAR_STAR [   c1] (*) length specifier in an IMPLICIT statement is not FORTRAN 77 standard
IM_CHAR_STAR_STAR [j  c1] IMPLICIT文の中で(*)の長さの指定はFORTRAN 77の標準ではありません。
 A character length specified of (*) in an IMPLICIT statement is an
 extension to the FORTRAN 77 language.
IM_NONE_USED [   c0] IMPLICIT NONE allowed once or not allowed with other IMPLICIT statements
IM_NONE_USED [j  c0] IMPLICIT NONEを指定すると他のどんなIMPLICITも書いてはいけません。
 The IMPLICIT NONE statement must be the only IMPLICIT statement in
 the program unit in which it appears.
 Only one IMPLICIT NONE statement is allowed in a program unit.
IO Input/Output
IO_ABACK [   r0] BACKSPACE statement requires sequential access mode
IO_ABACK [j  r0] 順次ｱｸｾｽでなければBACKSPACEは指定できません。
 The file connected to the unit specified in the BACKSPACE statement
 has not been opened for sequential access.
IO_ACTIVE_ALREADY [   r0] input/output is already active
IO_ACTIVE_ALREADY [j  r0] I/Oの実行中に別なI/Oは実行できません。
 An attempt has been made to read or write a record when there is an
 already active read or write in progress.
 The execution of a READ or WRITE statement has caused transfer to a
 function that contains a READ or WRITE statement.
IO_AEND [   r0] ENDFILE statement requires sequential access mode
IO_AEND [j  r0] ENDFILEは順次ｱｸｾｽ処理でなければ指定できません。
 The specified external unit identifier must be connected for
 sequential access but was connected for direct access.
IO_AF1 [   r0] formatted connection requires formatted input/output statements
IO_AF1 [j  r0] OPENでFORM='FORMATTED'の時はREAD/WRITEはFORMATTED形式でなければなりません。
 The FORM specifier in the OPEN statement specifies FORMATTED and the
 subsequent READ and/or WRITE statement does not use formatted I/O.
 If the FORM specifier has been omitted and access is SEQUENTIAL then
 FORMATTED is assumed.
 If the access is DIRECT then UNFORMATTED is assumed.
IO_AF2 [   r0] unformatted connection requires unformatted input/output statements
IO_AF2 [j  r0] OPENでFORM='UNFORMATTED'の時はREAD/WRITEはUNFORMATTED形式でなければなりません。
 The FORM specifier in the OPEN statement specifies UNFORMATTED and
 the subsequent READ and/or WRITE statement uses formatted I/O.
 If the FORM specifier has been omitted and access is SEQUENTIAL then
 FORMATTED is assumed.
 If the access is DIRECT then UNFORMATTED is assumed.
IO_AREW [   r0] REWIND statement requires sequential access
IO_AREW [j  r0] REWIND文は順次ｱｸｾｽ処理でなければ指定できません。
 The external unit identifier is not connected to a sequential file.
 The REWIND statement positions to the first record in the file.
IO_BAD_CHAR [   r0] bad character in input field
IO_BAD_CHAR [j  r0] 入力ﾃﾞｰﾀに書式に合わない文字が入っています。
 The data received from the record in a file does not match the type
 of the input list item.
IO_BLNK_FMT [   r0] BLANK specifier requires FORM specifier to be 'FORMATTED'
IO_BLNK_FMT [j  r0] BLANKはFORM='FORMATTED'でなければ指定できません。
 In the OPEN statement, the BLANK specifier may only be used when the
 FORM specifier has the value of FORMATTED.
 The BLANK specifier indicates whether blanks are treated as zeroes or
 ignored.
IO_FILE_PROBLEM [   r0] system file error - %s1
IO_FILE_PROBLEM [j  r0] ｼｽﾃﾑﾌｧｲﾙ･I/Oｴﾗｰです。-%s1
 A system error has occurred while attempting to access a file.
 The I/O system error message is displayed.
IO_FMT_MISMATCH [   r0] format specification does not match data type
IO_FMT_MISMATCH [j  r0] 書式の中でﾃﾞ-ﾀ仕様とﾃﾞｰﾀ型がちがっています。
 A format specification in the FMT specifier or FORMAT statement
 specifies data of one type and the variable list specifies data of a
 different type.
IO_FREE_MISMATCH [   r0] input item does not match the data type of list variable
IO_FREE_MISMATCH [j  r0] 入力の中で実際のﾃﾞ-ﾀと変数の型が合っていません。
 In the READ statement, the data type of a variable listed is not of
 the same data type in the data file.
 For example, non-digit character data being read into an integer
 item.
IO_IFULL [   r0] internal file is full
IO_IFULL [j  r0] 内部ﾌｧｲﾙが一杯です。
 The internal file is full of data.
 If a file is a variable then the file may only contain one record.
 If the file is a character array then there can be one record for
 each array element.
IO_IRECL [   r0] RECL specifier is invalid
IO_IRECL [j  r0] RECL(ﾚｺ-ﾄﾞの長さ)は正の整数/整数式でなければなりません。
 In the OPEN statement, the record length specifier must be a positive
 integer expression.
IO_ISTAT [   r0] invalid STATUS specifier in CLOSE statement
IO_ISTAT [j  r0] CLOSEのSTATUSが正しくありません(OPENとCLOSEのSTATUSが不一致です)。
 The STATUS specifier can only have a value of KEEP or DELETE.
 If the STATUS in the OPEN statement is SCRATCH then the KEEP status
 on the CLOSE statement cannot be used.
IO_NOT_CONNECTED [   r0] unit specified is not connected
IO_NOT_CONNECTED [j  r0] UNITのﾌｧｲﾙ識別子(ﾌｧｲﾙ番号)が正しくﾌｧｲﾙに関係つけられていません。
 The unit number specified in the I/O statement has not been
 previously connected.
IO_PAST_EOF [   r0] attempt to perform data transfer past end of file
IO_PAST_EOF [j  r0] END FILEの後に更にﾃﾞｰﾀの読み書きをしてはいけません。
 An attempt has been made to read or write data after the end of file
 has been read or written.
IO_RACCM [   r0] invalid RECL specifier/ACCESS specifier combination
IO_RACCM [j  r0] ACCESSがDIRECTの時はRECLが必要です。
 In the OPEN statement, if the ACCESS specifier is DIRECT then the
 RECL specifier must be given.
IO_REC1_ACCM [   r0] REC specifier required in direct access input/output statements
IO_REC1_ACCM [j  r0] 直接ｱｸｾｽ･I/O文の中にはRECの指定が必要です。
 In the OPEN statement, the ACCESS  specified was DIRECT.
 All subsequent input/output statements for that file must use the REC
 specifier to indicate which record to access.
IO_REC2_ACCM [   r0] REC specifier not allowed in sequential access input/output statements
IO_REC2_ACCM [j  r0] RECは順次ｱｸｾｽ･I/O文の中に指定してはいけません。
 In the OPEN statement, the ACCESS specified was SEQUENTIAL.
 The REC specifier may not be used in subsequent I/O statements for
 that file.
 The REC specifier is used to indicate which record to access when
 access is DIRECT.
IO_SUBSEQUENT_OPEN [   r0] %s1 specifier may not change in a subsequent OPEN statement
IO_SUBSEQUENT_OPEN [j  r0] 同一ﾌｧｲﾙに対してOPEN後,別のOPENで%s1の指定を変更してはいけません。
 The %s1 specifier may not be changed on a subsequent OPEN statement
 for the same file, in the same program.
 Only the BLANK specifier may be changed.
IO_SFILE [   r0] invalid STATUS specifier for given file
IO_SFILE [j  r0] STATUSの指定と実際のﾌｧｲﾙの状態が合っていません。
 In the OPEN statement, the STATUS specifier does not match with the
 actual file status: OLD means the file must exist, NEW means the file
 must not exist.
 If the STATUS specifier is omitted, UNKNOWN is assumed.
IO_SNAME [   r0] invalid STATUS specifier/FILE specifier combination
IO_SNAME [j  r0] STATUSとFILEの指定内容が食い違っています。
 In the OPEN statement, if the STATUS is SCRATCH, the FILE specifier
 cannot be used.
 If the STATUS is NEW or OLD, the FILE specifier must be given.
IO_UNFMT_RECL [   r0] record size exceeded during unformatted input/output
IO_UNFMT_RECL [j  r0] UNFORMATTED I/Oでレコードの大きさが最大長を超えました。
 This error is issued when the size of an i/o list item exceeds the
 maximum record size of the file.
 The record size can be specified using the RECL= specified in the OPEN
 statement.
IO_UNIT_EXIST [   r0] unit specified does not exist
IO_UNIT_EXIST [j  r0] UNITのﾌｧｲﾙ識別子にﾌｧｲﾙが接続されていません。
 The external unit identifier specified in the input/output statement
 has not yet been connected.
 Use preconnection or the OPEN statement to connect a file to the
 external unit identifier.
IO_IREC [   r0] REC specifier is invalid
IO_IREC [j  r0] RECは符号なしの正の整数でなければなりません。
 The REC specifier must be an unsigned positive integer.
IO_IUNIT [   r0] UNIT specifier is invalid
IO_IUNIT [j  r0] UNITの識別子(ﾌｧｲﾙ番号)が正しくありません。
 The UNIT specifier must be an unsigned integer between 0 and 999
 inclusive.
IO_BUFF_LEN [   r0] formatted record or format edit descriptor is too large for record size
IO_BUFF_LEN [j  r0] 書式つきﾚｺｰﾄﾞまたは書式で指定した編集記述子の長さがﾚｺｰﾄﾞｻｲｽﾞより大きすぎます。
 This error is issued when the amount of formatted data in a READ, WRITE or
 PRINT statement exceeds the maximum record size of the file.
 The record size can be specified using the RECL= specified in the OPEN
 statement.
IO_BAD_SPEC [   r0] illegal '%s1=' specifier
IO_BAD_SPEC [j  r0] %s1= の指定が正しくありません。
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
IO_CC_FORM [   r0] invalid CARRIAGECONTROL specifier/FORM specifier combination
IO_CC_FORM [j  r0] CARRIAGECONTROLとFORM指定子の組合せが無効です。
 The CARRIAGECONTROL specifier is only allowed with formatted i/o statements.
IO_BAD_ACTION [   r0] i/o operation not consistent with file attributes
IO_BAD_ACTION [j  r0] 指定されたﾌｧｲﾙ属性では,この入出力操作は許されません。
 An attempt was made to read from a file that was opened with
 ACTION=WRITE or write to a file that was opened with ACTION=READ.
 This message is also issued if you attempt to write to a read-only file or
 read from a write-only file.
IO_NML_NO_SUCH_NAME [   r0] symbol %s1 not found in NAMELIST
IO_NML_NO_SUCH_NAME [j  r0] NAMELIST中でｼﾝﾎﾞﾙ%s1が見つかりません。
 During NAMELIST-directed input, a symbol was specified that does not belong
 to the NAMELIST group specified in the i/o statement.
IO_NML_BAD_SYNTAX [   r0] syntax error during NAMELIST-directed input
IO_NML_BAD_SYNTAX [j  r0] NAMELIST指向の入力で,ｼﾝﾀｯｸｽｴﾗｰがあります。
 Bad input was encountered during NAMELIST-directed input.
 Data must be in a special form during NAMELIST-directed input.
IO_NML_BAD_SUBSCRIPT [   r0] subscripting error during NAMELIST-directed i/o
IO_NML_BAD_SUBSCRIPT [j  r0] NAMELIST指向の入出力で,添字にｴﾗｰがあります。
 An array was incorrectly subscripted during NAMELIST-directed input.
IO_NML_BAD_SUBSTRING [   r0] substring error during NAMELIST-directed i/o
IO_NML_BAD_SUBSTRING [j  r0] NAMELIST指向の入出力で,部分文字列にｴﾗｰがあります。
 An character array element or variable was incorrectly substrung during
 NAMELIST-directed input.
IO_IBLOCKSIZE [   r0] BLOCKSIZE specifier is invalid
IO_IBLOCKSIZE [j  r0] BLOCKSIZE が正しくありません。
 In the OPEN statement, the block size specifier must be a positive
 integer expression.
IO_NO_RECORDS [   r0] invalid operation for files with no record structure
IO_NO_RECORDS [j  r0] ﾚｺｰﾄﾞ構造をもたないﾌｧｲﾙに対して操作が誤っています。
 An attempt has been made to perform an i/o operation on a file that requires
 a record structure.
 For example, it is illegal to use a BACKSPACE statement for a file that
 has no record structure.
IO_IOVERFLOW [   r0] integer overflow converting character data to integer
IO_IOVERFLOW [j  r0] 文字ﾃﾞｰﾀを整数に変換するとき整数が桁あふれしました。
 An overflow has occurred while converting the character data read to its
 internal representation as an integer.
IO_FRANGE_EXCEEDED [   r0] range exceeded converting character data to floating-point
IO_FRANGE_EXCEEDED [j  r0] 文字ﾃﾞｰﾀを浮動小数点数に変換したとき範囲を越えました。
 An overflow or underflow has occurred while converting the character data
 read to its internal representation as a floating-point number.
KO Program Termination
KO_FDIV_ZERO [    0] floating-point divide by zero
KO_FDIV_ZERO [j   0] 浮動小数点数はｾﾞﾛで割ることはできません。
 An attempt has been made to divide a number by zero in a
 floating-point expression.
KO_FOVERFLOW [    0] floating-point overflow
KO_FOVERFLOW [j   0] 浮動小数点数が桁あふれしました。
 The floating-point expression result has exceeded the maximum
 floating-point number.
KO_FUNDERFLOW [    0] floating-point underflow
KO_FUNDERFLOW [j   0] 浮動小数点数が指数下位桁あふれをしました。
 The floating-point expression result has exceeded the minimum
 floating-point number.
KO_IDIV_ZERO [    0] integer divide by zero
KO_IDIV_ZERO [j   0] 整数はｾﾞﾛで割れません。
 An attempt has been made to divide a number by zero in an integer
 expression.
KO_INTERRUPT [    0] program interrupted from keyboard
KO_INTERRUPT [j   0] 実行を中断しました。
 The user has interrupted the compilation or execution of a program
 through use of the keyboard.
KO_IOVERFLOW [    1] integer overflow
KO_IOVERFLOW [j   1] 整数が桁あふれしました。
 The integer expression result has exceeded the maximum integer
 number.
KO_PAGES_OUT [ w r0] maximum pages of output exceeded
KO_PAGES_OUT [jw r0] 印刷ﾍﾟｰｼﾞ数がｺﾝﾊﾟｲﾗ･ｵﾌﾟｼｮﾝPAGESで指定した最高値をこえました。
 The specified maximum number of output pages has been exceeded.
 The maximum number of output pages can be increased
 by using the "pages=n" option in the command line
 or specifying C$PAGES=n in the source file.
KO_STMT_CNT [ w r0] statement count has been exceeded
KO_STMT_CNT [jw r0] ﾌﾟﾛｸﾞﾗﾑの行数がｺﾝﾊﾟｲﾗ･ｵﾌﾟｼｮﾝSTATEMENTSで指定した最高値を超えました。
 The maximum number of source statements has been executed.
 The maximum number of source statements that can be executed can be
 increased by using the "statements=n" option in the command line or
 specifying C$STATEMENTS=n in the source file.
KO_TIME_OUT [ w r0] time limit exceeded
KO_TIME_OUT [jw r0] 時間切れです(実行を中止します)。
 The maximum amount of time for program execution has been exceeded.
 The maximum amount of time can be increased
 by using the "time=t" option in the command line or specifying C$TIME=t
 in the source file.
LI Library Routines
LI_ARG_GT_ZERO [   r0] argument must be greater than zero
LI_ARG_GT_ZERO [j  r0] 引数は1個以上でなければなりません。
 The argument to the intrinsic function must be greater than zero
 (i.e., a positive number).
LI_ARG_LE_ONE [   r0] absolute value of argument to arcsine, arccosine must not exceed one
LI_ARG_LE_ONE [j  r0] ASIN,ACOSの引数の絶対値は1を超えてはいけません。
 The absolute value of the argument to the intrinsic function ASIN or
 ACOS cannot be greater than or equal to the value 1.0.
LI_ARG_NEG [   r0] argument must not be negative
LI_ARG_NEG [j  r0] 組込み関数に対する引数はﾏｲﾅｽではいけません。
 The argument to the intrinsic function must be greater than or equal
 to zero.
LI_ARG_ZERO [   r0] argument(s) must not be zero
LI_ARG_ZERO [j  r0] 組込み関数に対する引数はｾﾞﾛではいけません。
 The argument(s) to the intrinsic function must not be zero.
LI_CHAR_BOUND [    0] argument of CHAR must be in the range zero to 255
LI_CHAR_BOUND [j   0] CHARに対する引数の値は0から255の間でなければなりません。
 The argument to the intrinsic function CHAR must be in the range 0 to
 255 inclusive.
 CHAR returns the character represented by an 8-bit pattern.
LI_2_CMPLX [   c2] %s1 intrinsic function cannot be passed 2 complex arguments
LI_2_CMPLX [j  c2] %s1組込み関数の第2引数に複素数を指定できません。
 The second argument to the intrinsic function CMPLX and DCMPLX cannot
 be a complex number.
LI_CMPLX_TYPES [   c2] argument types must be the same for the %s1 intrinsic function
LI_CMPLX_TYPES [j  c2] %s1組込み関数の第2引数の型は第1引数の型と同じでなければなりません。
 The second argument to the intrinsic function CMPLX or DCMPLX must be
 of the same type as the first argument.
 The second argument may only be used when the first argument is of
 type INTEGER, REAL or DOUBLE PRECISION.
LI_EXP_CNV_TYPE [   c2] expecting numeric argument, but %s1 argument was found
LI_EXP_CNV_TYPE [j  c2] 数値型の引数が必要ですが%s1型が指定されています。
 The argument to the intrinsic function, INT, REAL, DBLE, CMPLX, or
 DCMPLX was of type %s1 and a numeric argument was expected.
LI_ICHAR_ONE [   r0] length of ICHAR argument greater than one
LI_ICHAR_ONE [j  r0] ICHAR引数は文字型で長さは1でなければなりません。
 The length of the argument to the intrinsic function ICHAR must be of
 type CHARACTER and length of 1.
 ICHAR converts a character to its integer representation.
LI_ILL_PARM_CODE [   c2] cannot pass %s1 as argument to intrinsic function
LI_ILL_PARM_CODE [j  c2] 組込み関数の引数として%s1を指定してはいけません。
 The item %s1 cannot be used as an argument to an intrinsic function.
 Only constants, simple variables, array elements, and substrung array
 elements may be used as arguments.
LI_NO_PARM [   c2] intrinsic function requires argument(s)
LI_NO_PARM [j  c2] 組込み関数には引数が必要です。
 An attempt has been made to invoke an intrinsic function and no
 actual arguments were listed.
LI_NO_SPECIFIC [   c2] %s1 argument type is invalid for this generic function
LI_NO_SPECIFIC [j  c2] この総称関数に対して%s1の引数型は指定できません。
 The type of the argument used in the generic intrinsic function is
 not correct.
LI_NOT_PARM [   c2] this intrinsic function cannot be passed as an argument
LI_NOT_PARM [j  c2] 組込み関数名は実引数として指定できません。
 Only the specific name of the intrinsic function can be used as an
 actual argument.
 The generic name may not be used.
 When the generic and intrinsic names are the same, use the INTRINSIC
 statement.
LI_PT_MISMATCH [   c2] expecting %s1 argument, but %s2 argument was found
LI_PT_MISMATCH [j  c2] %s1型の引数が必要ですが%s2が指定されています。
 An argument of type %s2 was passed to a function but an argument of
 type %s1 was expected.
LI_WRONG_TYPE [   c2] intrinsic function was assigned wrong type
LI_WRONG_TYPE [j  c2] 定義された組込み関数の型と呼び出しの型は一致しなければなりません。
 The declared type of an intrinsic function does not agree with the
 actual type.
LI_IF_NOT_STANDARD [   c2] intrinsic function %s1 is not FORTRAN 77 standard
LI_IF_NOT_STANDARD [j  c2] 組込み関数%s1はFORTRAN 77の標準ではありません。
 The specified intrinsic function is provided as an extension to the
 FORTRAN 77 language.
LI_ARG_ALLOCATED [   c2] argument to ALLOCATED intrinsic function must be an allocatable array or character*(*) variable
LI_ARG_ALLOCATED [j  c2] ALLOCATED組込み関数の引数は、割り付け可能な配列またはcharacter*(*)型の変数でなければなりません。
 The argument to the intrinsic function ALLOCATED must be an allocatable
 array or character*(*) variable.
LI_ARG_ISIZEOF [   c2] invalid argument to ISIZEOF intrinsic function
LI_ARG_ISIZEOF [j  c2] 組込み関数ISIZEOFの引数が間違っています。
 The argument to the intrinsic function ISIZEOF must be a user-defined
 structure name, a symbol name, or a constant.
MD Mixed Mode
MD_BAD_REL_OPN [   c1] relational operator has a logical operand
MD_BAD_REL_OPN [j  c1] 関係演算子(.GT.,.LT.など)が論理演算子(.OR.,.AND.)として使われてはいけません。
 The operands of a relational expression must either be both
 arithmetic or both character expressions.
 The operand indicated is a logical expression.
MD_DBLE_WITH_CMPLX [   c1] mixing DOUBLE PRECISION and COMPLEX types is not FORTRAN 77 standard
MD_DBLE_WITH_CMPLX [j  c1] 倍精度と複素数型を式に同時に書くことはFORTRAN 77の標準ではありません。
 The mixing of items of type DOUBLE PRECISION and COMPLEX in an
 expression is an extension to the FORTRAN 77 language.
MD_ILL_OPR [   c1] operator not expecting %s1 operands
MD_ILL_OPR [j  c1] この演算子は%s1型の要素を扱えません。
 Operands of type %s1 cannot be used with the indicated operator.
 The operators **, /, *, +, and &minus. may only have numeric type data.
 The operator // may only have character type data.
MD_MIXED [   c1] operator not expecting %s1 and %s2 operands
MD_MIXED [j  c1] この演算子は%s1型と%s2型の要素を扱えません。
 Operands of conflicting type have been encountered.
 For example, in a relational expression, it is not possible to compare a
 character expression to an arithmetic expression.
 Also, the type of the left hand operand of the field selection operator must
 be a user-defined structure.
MD_RELOP_OPND_COMPLEX [   c1] complex quantities can only be compared using .EQ. or .NE.
MD_RELOP_OPND_COMPLEX [j  c1] 複素数の比較には.EQ.と.NE.しか使えません。
 Complex operands cannot be compared using less than (.LT.), less than
 or equal (.LE.), greater than (.GT.), or greater than or equal (.GE.)
 operators.
MD_UNARY_OP [   c2] unary operator not expecting %s1 type
MD_UNARY_OP [j  c2] 単項演算子(1個で作用する+,-,.NOT.など)は%s1型に使えません。
 The unary operators, + and &minus., may only be used with numeric types.
 The unary operator .NOT. may be used only with a logical or integer operand.
 The indicated operand was of type %s1 which is not one of the valid types.
MD_LOGOPR_INTOPN [   c1] logical operator with integer operands is not FORTRAN 77 standard
MD_LOGOPR_INTOPN [j  c1] 整数要素に対する論理演算子はFORTRAN 77の標準ではありません。
 Integer operands are permitted with the logical
 operators .AND., .OR., .EQV., .NEQV., .NOT. and .XOR. as an extension to the
 FORTRAN 77 language.
MD_LOGOPR_EXTN [   c0] logical operator %s1 is not FORTRAN 77 standard
MD_LOGOPR_EXTN [j  c0] 論理演算子%s1はFORTRAN 77の標準ではありません。
 The specified logical operator is an extension to the FORTRAN 77 standard.
MO Memory Overflow
MO_COMPILER_LIMIT [   c0] %s1 exceeds compiler limit of %u2 bytes
MO_COMPILER_LIMIT [j  c0] %s1の大きさがｺﾝﾊﾟｲﾗ限度である%u2ﾊﾞｲﾄ(文字)を超えてはいけません。
 An internal compiler limit has been reached.
 %s1 describes the limit and %d2 specifies the limit.
MO_DYNAMIC_OUT [    0] out of memory
MO_DYNAMIC_OUT [j   0] メモリ不足です。
 All available memory has been used up.
 During the compilation phase, memory is primarily used for the symbol table.
 During execution, memory is used for file descriptors and buffers,
 and dynamically allocatable arrays and character*(*) variables.
MO_LIST_TOO_BIG [   c0] dynamic memory exhausted due to length of this statement - statement ignored
MO_LIST_TOO_BIG [j  c0] この文が長いために、動的ﾒﾓﾘが不足しました。ｰこの文を無視します。
 There was not enough memory to encode the specified statement.
 This message is usually issued when the compiler is low on memory or if the
 statement is a very large statement that spans many continuation lines.
 This error does not terminate the compiler since it may have been caused by
 a very large statement.
 The compiler attempts to compile the remaining statements.
MO_STORAGE_NOT_ALLOCATED [   r0] attempt to deallocate an unallocated array or character*(*) variable
MO_STORAGE_NOT_ALLOCATED [j  r0] ﾒﾓﾘを割り付けた配列あるいはcharacter*(*)型の変数以外のメモリは解放できません。
 An attempt has been made to deallocate an array that has not been previously
 allocated.
 An array or character*(*) variable must be allocated using an ALLOCATE
 statement.
MO_STORAGE_ALLOCATED [   r0] attempt to allocate an already allocated array or character*(*) variable
MO_STORAGE_ALLOCATED [j  r0] 既にﾒﾓﾘを割り付けた配列あるいはcharacter*(*)型の変数にはﾒﾓﾘを割り付けられません。
 An attempt has been made to allocate an array or character*(*) variable that
 has been previously allocated in an ALLOCATE statement.
MO_OBJECT_OUT [ w c0] object memory exhausted
MO_OBJECT_OUT [jw c0] ﾌﾟﾛｸﾞﾗﾑのｵﾌﾞｼﾞｪｸﾄ･ｺｰﾄﾞが大きすぎて入りません。
 The amount of object code generated for the program has exceeded the amount
 of memory allocated to store the object code.
 The "/codesize" option can be used to increase the amount of memory
 allocated for object code.
PC Parentheses
PC_NO_CLOSEPAREN [   c1] missing or misplaced closing parenthesis
PC_NO_CLOSEPAREN [j  c1] 右かっこ')'がありません。
 An opening parenthesis '(' was found but no matching closing
 parenthesis ')' was found before the end of the statement.
PC_NO_OPENPAREN [    1] missing or misplaced opening parenthesis
PC_NO_OPENPAREN [j   1] 左かっこ'('がありません。
 A closing parenthesis ')' was found before the matching opening
 parenthesis '('.
PC_SURP_PAREN [   c1] unexpected parenthesis
PC_SURP_PAREN [j  c1] 必要のないかっこがあります。
 A parenthesis was found in a statement where parentheses are not
 expected.
PC_UNMATCH_PAREN [    1] unmatched parentheses
PC_UNMATCH_PAREN [j   1] 左右のかっこの組合せが合っていません。
 The parentheses in the expression are not balanced.
PR PRAGMA Compiler Directive
PR_SYMBOL_NAME [   c0] expecting symbolic name
PR_SYMBOL_NAME [j  c0] ｼﾝﾎﾞﾙ名を指定してください。
 Every auxiliary pragma must refer to a symbol.
 This error is issued when the symbolic name is illegal or missing.
 Valid symbolic names are formed from the following characters: a dollar
 sign, an underscore, digits and any letter of the alphabet.
 The first character of a symbolic name must be alphabetic, a dollar sign, or
 an underscore.
PR_BAD_PARM_SIZE [   c0] illegal size specified for VALUE attribute
PR_BAD_PARM_SIZE [j  c0] VALUE属性に対するｻｲｽﾞ指定が間違っています。
 The VALUE argument attribute of an auxiliary pragma contains in illegal
 length specification.
 Valid length specifications are 1, 2, 4 and 8.
PR_BAD_PARM_ATTR [   c0] illegal argument attribute
PR_BAD_PARM_ATTR [j  c0] 引数属性が間違っています。
 An illegal argument attribute was specified.
 Valid argument attributes are VALUE, REFERENCE, or DATA_REFERENCE.
PR_BAD_CONTINUATION [   c0] continuation line must contain a comment character in column 1
PR_BAD_CONTINUATION [j  c0] 継続行は1ｶﾗﾑ目にｺﾒﾝﾄ文字が必要です。
 When continuing a line of an auxiliary pragma directive, the continued line
 must end with a back-slash ('\') character and the continuation line must
 begin with a comment character ('c', 'C' or '*') in column 1.
PR_BAD_SYNTAX [   c0] expecting '%s1' near '%s2'
PR_BAD_SYNTAX [j  c0] '%s1'が必要な所にありません。場所は'%s2'の近くです。
 A syntax error was found while processing a PRAGMA directive.
 %s1 identifies the expected information and %s2 identifies where in the
 pragma the error occurred.
PR_BYTE_SEQ_LIMIT [ o c0] in-line byte sequence limit exceeded
PR_BYTE_SEQ_LIMIT [jo c0] ｲﾝﾗｲﾝ･ﾊﾞｲﾄｼｰｹﾝｽの制限を超えました。
 The limit on the number of bytes of code that can be generated in-line using
 a an auxiliary pragma has been exceeded.
 The limit is 127 bytes.
PR_BAD_BYTE_SEQ [ o c0] illegal hexadecimal data in byte sequence
PR_BAD_BYTE_SEQ [jo c0] ﾊﾞｲﾄｼｰｹﾝｽ中に不正な16進ﾃﾞｰﾀがあります。
 An illegal hexadecimal constant was encountered while processing a in-line
 byte sequence of an auxiliary pragma.
 Valid hexadecimal constants in an in-line byte sequence must begin with the
 letter Z or z and followed by a string of hexadecimal digits.
PR_SYMBOL_UNRESOLVED [ o c0] symbol '%s1' in in-line assembly code cannot be resolved
PR_SYMBOL_UNRESOLVED [jo c0] ｲﾝﾗｲﾝ･ｱｾﾝﾌﾞﾘ･ｺｰﾄﾞ内のｼﾝﾎﾞﾙ '%s1'は参照未解決です。
 The symbol %s1, referenced in an assembly language instruction in an
 auxiliary pragma, could not be resolved.
RE RETURN Statement
RE_ALT_IN_SUBROUTINE [   c2] alternate return specifier only allowed in subroutine
RE_ALT_IN_SUBROUTINE [j  c2] 代替戻り指定子は、ｻﾌﾞﾙｰﾁﾝだけに許されます。
 An alternate return specifier, in the RETURN statement, may only be
 specified when returning from a subroutine.
RE_IN_PROGRAM [   c0] RETURN statement in main program is not FORTRAN 77 standard
RE_IN_PROGRAM [j  c0] ﾒｲﾝﾌﾟﾛｸﾞﾗﾑ中のRETURN文は,FORTRAN 77標準ではありません。
 A RETURN statement in the main program is allowed as an extension to the
 FORTRAN 77 standard.
SA SAVE Statement
SA_COMBLK_EMPTY [   c0] COMMON block %s1 saved but not properly defined
SA_COMBLK_EMPTY [j  c0] COMMONﾌﾞﾛｯｸ%s1をSAVEされていますが、そのCOMMONﾌﾞﾛｯｸ名が定義されていません。
 The named COMMON block %s1 was listed in a SAVE statement but there is
 no named COMMON block defined by that name.
SA_COMMON_NOT_SAVED [   c0] COMMON block %s1 must be saved in every subprogram in which it appears
SA_COMMON_NOT_SAVED [j  c0] COMMONﾌﾞﾛｯｸ%s1は、そのCOMMONﾌﾞﾛｯｸがある全ての副ﾌﾟﾛｸﾞﾗﾑでSAVEしなければなりません。
 The named COMMON block %s1 appears in a SAVE statement in another
 subprogram and is not in a SAVE statement in this subprogram.
 If a named COMMON block is specified in a SAVE statement in a
 subprogram, it must be specified in a SAVE statement in every
 subprogram in which that COMMON block appears.
SA_SAVED [   c2] name already appeared in a previous SAVE statement
SA_SAVED [j  c2] 同じﾌﾟﾛｸﾞﾗﾑの中で2度以上同じ名前でSAVEしてはいけません。
 The indicated name has already been referenced in another SAVE
 statement in this subprogram.
SF Statement Functions
SF_DUPLICATE_DUMMY_PARM [   c2] statement function definition contains duplicate dummy arguments
SF_DUPLICATE_DUMMY_PARM [j  c2] 文関数の定義する仮引数が重複してはいけません。
 A dummy argument is repeated in the argument list of the statement function.
SF_ILL_CHAR_LEN [   c2] character length of statement function name must not be (*)
SF_ILL_CHAR_LEN [j  c2] 文関数の文字数は(*)であってはいけません。 整数にしてください。
 If the type of a character function is character, its length specification
 must not be (*); it must be a constant integer expression.
SF_ILL_DUMMY_PARM [   c2] statement function definition contains illegal dummy argument
SF_ILL_DUMMY_PARM [j  c2] 文関数の文字型仮引数は(*)の長さを持っていてはいけません。
 A dummy argument of type CHARACTER must have a length specification
 of an integer constant expression that is not (*).
SF_ILL_PARM_PASSED [   c2] cannot pass %s1 %s2 to statement function
SF_ILL_PARM_PASSED [j  c2] 間違った(*)長さで文字結合している%s1 %s2は文関数の実引数に指定できません。
 The actual arguments to a statement function can be any expression
 except character expressions involving the concatenation of an
 operand whose length specification is (*) unless the operand is a
 symbolic constant.
SF_PARM_TYPE_MISMATCH [   c2] %s1 actual argument was passed to %s2 dummy argument
SF_PARM_TYPE_MISMATCH [j  c2] 実引数の型%s1と仮引数の型%s2が合いません。
 The indicated actual argument is of type %s1 which is not the same
 type as that of the dummy argument of type %s2.
SF_WRONG_NUM_PARMS [   c2] incorrect number of arguments passed to statement function %s1
SF_WRONG_NUM_PARMS [j  c2] 文関数%s1に渡す引数の数が間違っています。
 The number of arguments passed to statement function %s1 does not
 agree with the number of dummy arguments specified in its definition.
SF_ILL_TYPE [   c2] type of statement function name must not be a user-defined structure
SF_ILL_TYPE [j  c2] 文関数名の型にﾕｰｻﾞｰ定義の構造体を指定してはいけません。
 The type of a statement function cannot be a user-defined structure.
 Valid types for statement functions are: LOGICAL*1, LOGICAL, INTEGER*1,
 INTEGER*2, INTEGER, REAL, DOUBLE PRECISION, COMPLEX, DOUBLE COMPLEX, and
 CHARACTER.
 If the statement function is of type CHARACTER, its length specification
 must not be (*); it must be an integer constant expression.
SM Source Management
SM_IO_READ_ERR [   c0] system file error reading %s1 - %s2
SM_IO_READ_ERR [j  c0] ｼｽﾃﾑ･ﾌｧｲﾙ読み込みｴﾗｰ: %s1 - %s2
 An I/O error, described by %s2, has occurred while reading the
 FORTRAN source file %s1.
SM_OPENING_FILE [   c0] error opening file %s1 - %s2
SM_OPENING_FILE [j  c0] ﾌｧｲﾙｵｰﾌﾟﾝｴﾗｰ: %s1 - %s2
 The FORTRAN source file %s1 could not be opened.
 The error is described by %s2.
SM_IO_WRITE_ERR [   c0] system file error writing %s1 - %s2
SM_IO_WRITE_ERR [j  c0] ｼｽﾃﾑ･ﾌｧｲﾙ書出しｴﾗｰ: %s1 - %s2
 An I/O error, described by %s2, has occurred while writing to the
 file %s1.
SM_SPAWN_ERR [ w c0] error spawning %s1 - %s2
SM_SPAWN_ERR [jw c0] spawnｴﾗｰ : %s1 - %s2
 An error, described by %s2, occurred while trying to spawn the external
 program named %s1.
SM_LINK_ERR [ w c0] error while linking
SM_LINK_ERR [jw c0] ﾘﾝｸ時ｴﾗｰ
 An error occurred while trying to create the executable file.  See the
 WLINK documentation for a description of the error.
SM_OUT_OF_VM_FILES [ o c0] error opening %s1 - too many temporary files exist
SM_OUT_OF_VM_FILES [jo c0] ﾌｧｲﾙｵｰﾌﾟﾝｴﾗｰ %s1 - ﾃﾝﾎﾟﾗﾘﾌｧｲﾙが多すぎます。
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
SM_BROWSE_ERROR [   c0] generation of browsing information failed
SM_BROWSE_ERROR [j  c0] ﾌﾞﾗｳｽﾞ情報の生成に失敗しました。
 An error occurred during the generation of browsing information.
 For example, a disk full condition encountered during the creation of the
 browser module file will cause this message to be issued.
 Browsing information is generated when the /db switch is specified.
SP Structured Programming Features
SP_ATEND_AND_ENDEQUALS [   c0] cannot have both ATEND and the END= specifier
SP_ATEND_AND_ENDEQUALS [j  c0] READにAT ENDとEND=を同時に指定できません。
 It is not valid to use the AT END control statement and the END=
 option on the READ statement.
 Only one method can be used to control the end-of-file condition.
SP_READ_NO_ATEND [   c0] ATEND must immediately follow a READ statement
SP_READ_NO_ATEND [j  c0] AT ENDはREADのすぐ後に来なければなりません。
 The indicated AT END control statement or block does not immediately
 follow a READ statement.
 The AT END control statement or block is executed when an end-of-file
 condition is encountered during the read.
SP_BAD_LABEL [   c2] block label must be a symbolic name
SP_BAD_LABEL [j  c2] ﾌﾞﾛｯｸ･ﾗﾍﾞﾙはｼﾝﾎﾞﾙ名でなくてはなりません。
 The indicated block label must be a symbolic name.
 A symbolic name must start with a letter and contain no more than 32
 letters and digits.
 A letter is an upper or lower case letter of the alphabet, a dollar sign ($),
 or an underscore (_).
 A digit is a character in the range '0' to '9'.
SP_BAD_QUIT [   c0] could not find a structure to %s1 from
SP_BAD_QUIT [j  c0] %s1への構造体を検出できません。
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
SP_BLK_IN_STRUCTURE [   c0] REMOTE BLOCK is not allowed in the range of any control structure
SP_BLK_IN_STRUCTURE [j  c0] 制御構造内部にREMOTE BLOCKを含めることはできません。
 An attempt has been made to define a REMOTE BLOCK inside a control
 structure.
 Control structures include IF, LOOP, WHILE, DO, SELECT and GUESS.
 When a REMOTE BLOCK definition is encountered during execution,
 control is transferred to the statement following the corresponding
 END BLOCK statement.
SP_SELECT_THEN_CASE [   c0] the SELECT statement must be followed immediately by a CASE statement
SP_SELECT_THEN_CASE [j  c0] SELECTのすぐ後にCASEが来なければなりません。
 The statement immediately after the SELECT statement must be a CASE
 statement.
 The SELECT statement allows one of a number of blocks of code (case
 blocks) to be selected for execution by means of an integer
 expression in the SELECT statement.
SP_CASE_OVERLAP [   c0] cases are overlapping
SP_CASE_OVERLAP [j  c0] CASEの指定子が重複しています。
 The case lists specified in the CASE statements in the SELECT control
 structure are in conflict.
 Each case list must specify a unique integer constant expression or
 range.
SP_EMPTY_SELECT [   c0] select structure requires at least one CASE statement
SP_EMPTY_SELECT [j  c0] SELECTには少なくとも1つのCASE文が必要です。
 In the SELECT control structure, there must be at least one CASE
 statement.
SP_FROM_OUTSIDE [   c0] cannot branch to %i1 from outside control structure in line %d2
SP_FROM_OUTSIDE [j  c0] 制御構造の外部から%i1に%d2行目のように入ることはできません。
 The statement in line %d2 passes control to the statement %d1 in a
 control structure.
 Control may only be passed out of a control structure or to another
 place in that control structure.
 Control structures include DO, GUESS, IF, LOOP, SELECT, and WHILE.
SP_INTO_STRUCTURE [   c0] cannot branch to %i1 inside structure on line %d2
SP_INTO_STRUCTURE [j  c0] 制御構造内部の%i1(%d2行目)に飛ぶことはできません。
 The statement attempts to pass control to statement %d1 in line %d2
 which is in a control structure.
 Control may only be passed out of a control structure or to another
 place in that control structure.
 Control structures include DO, GUESS, IF, LOOP, SELECT, and WHILE.
SP_NEVER_CASE [   c0] low end of range exceeds the high end
SP_NEVER_CASE [j  c0] 範囲の初めを示す番号が終りの番号より大きくてはいけません。
 The first number, the low end of the range, is greater than the
 second number, the high end of the range.
SP_OTHERWISE_LAST [   c0] default case block must follow all case blocks
SP_OTHERWISE_LAST [j  c0] ﾃﾞﾌｫﾙﾄのCASEﾌﾞﾛｯｸは全てのCASEﾌﾞﾛｯｸの後に来なくてはなりません。
 The default case block in the SELECT control structure must be the last
 case block.
 A case block may not follow the default case block.
SP_OUT_OF_BLOCK [   c0] attempt to branch out of a REMOTE BLOCK
SP_OUT_OF_BLOCK [j  c0] REMOTE BLOCKから外に飛び出ることは許されません(END BLOCKで終らせてください)。
 An attempt has been made to transfer execution control out of a
 REMOTE BLOCK.
 A REMOTE BLOCK may only be terminated with the END BLOCK statement.
 Execution of a REMOTE BLOCK is similar in concept to execution of a
 subroutine.
SP_RB_UNDEFINED [   c0] attempt to EXECUTE undefined REMOTE BLOCK %s1
SP_RB_UNDEFINED [j  c0] EXECUTEされるREMOTEﾌﾞﾛｯｸ %s1は同じﾌﾟﾛｸﾞﾗﾑの中になければなりません。
 The REMOTE BLOCK %s1 referenced in the EXECUTE statement does not
 exist in the current program unit.
 A REMOTE BLOCK is local to the program unit in which it is defined
 and may not be referenced from another program unit.
SP_RECURSE [   c0] attempted to use REMOTE BLOCK recursively
SP_RECURSE [j  c0] REMOTE BLOCK は再帰的に実行することはできません。
 An attempt was made to execute a REMOTE BLOCK which was already
 active.
SP_RET_IN_REMOTE [   c0] cannot RETURN from subprogram within a REMOTE BLOCK
SP_RET_IN_REMOTE [j  c0] 副ﾌﾟﾛｸﾞﾗﾑのREMOTE BLOCKの中でRETURNを実行できません。
 An illegal attempt has been made to execute a RETURN statement within
 a REMOTE BLOCK in a subprogram.
SP_STRUCTURED_EXT [   c0] %s1 statement is not FORTRAN 77 standard
SP_STRUCTURED_EXT [j  c0] %s1文はFORTRAN 77の標準ではありません。
 The statement %s1 is an extension to the FORTRAN 77 language.
SP_UNFINISHED [   c0] %s1 block is unfinished
SP_UNFINISHED [j  c0] %s1ﾌﾞﾛｯｸを終了させる文がありません。
 The block starting with the statement %s1 does not have the ending
 block statement.
 For example: ATENDDO-ENDATEND, DO-ENDDO, GUESS-ENDGUESS, IF-ENDIF,
 LOOP-ENDLOOP, SELECT-ENDSELECT, STRUCTURE-ENDSTRUCTURE and WHILE-ENDWHILE.
SP_UNMATCHED [   c0] %s1 statement does not match with %s2 statement
SP_UNMATCHED [j  c0] 文%s1は文%s2と組合せられません。
 The statement %s1, which ends a control structure, cannot be used with
 statement %s2 to form a control structure.
 Valid control structures are: LOOP - ENDLOOP, LOOP - UNTIL, WHILE -
 ENDWHILE, and WHILE - UNTIL.
SP_INCOMPLETE [   c0] incomplete control structure found at %s1 statement
SP_INCOMPLETE [j  c0] %s1に対応して制御構造の初めを示す文がありません。
 The ending control structure statement %s1 was found and there was no
 preceding matching beginning statement.
 Valid control structures include: ATENDDO - ENDATEND, GUESS -
 ENDGUESS, IF - ENDIF, LOOP - ENDLOOP, REMOTE BLOCK - ENDBLOCK, and
 SELECT - ENDSELECT.
SP_NOT_IN_STRUCTURE [   c0] %s1 statement is not allowed in %s2 definition
SP_NOT_IN_STRUCTURE [j  c0] %s1文は%s2定義内では指定できません。
 Statement %s1 is not allowed between a %s2 statement and the
 corresponding END %s2 statement.
 For example, an EXTERNAL statement is not allowed between a STRUCTURE and
 END STRUCTURE statement, a UNION and END UNION statement, or a MAP and
 END MAP statement.
SP_NO_SUCH_FIELD [   c2] no such field name found in structure %s1
SP_NO_SUCH_FIELD [j  c2] 構造体%s1の中に,このﾌｨｰﾙﾄﾞ名が見つかりません。
 A structure reference contained a field name that does not belong to the
 specified structure.
SP_DUPLICATE_FIELD [   c2] multiple definition of field name %s1
SP_DUPLICATE_FIELD [j  c2] ﾌｨｰﾙﾄﾞ名%s1は重複して定義されています。
 The field name %s1 has already been defined in a structure.
SP_UNDEF_STRUCT [   c0] structure %s1 has not been defined
SP_UNDEF_STRUCT [j  c0] 構造体%s1は定義されていません。
 An attempt has been made to declare a symbol of user-defined type %s1.
 No structure definition for %s1 has occurred.
SP_STRUCT_DEFINED [   c2] structure %s1 has already been defined
SP_STRUCT_DEFINED [j  c2] 構造体%s1は既に定義されています。
 The specified structure has already been defined as a structure.
SP_STRUCT_NEEDS_FIELD [   c0] structure %s1 must contain at least one field
SP_STRUCT_NEEDS_FIELD [j  c0] 構造体%s1には,少なくとも1つのﾌｨｰﾙﾄﾞが必要です。
 Structures must contain at least one field definition.
SP_STRUCT_RECURSION [   c0] recursion detected in definition of structure %s1
SP_STRUCT_RECURSION [j  c0] 構造体%s1の定義の中に,再帰定義があります。
 Structure %s1 has been defined recursively.
 For example, it is illegal for structure X to contain a field that is itself
 a structure named X.
SP_STRUCT_HAS_UNION [   c2] illegal use of structure %s1 containing union
SP_STRUCT_HAS_UNION [j  c2] 共用体を含む構造体%s1の使い方が間違っています。
 Structures containing unions cannot be used in formatted I/O statements or
 data initialized.
SP_ALLOC_NOT_IN_STRUCT [   c0] allocatable arrays cannot be fields within structures
SP_ALLOC_NOT_IN_STRUCT [j  c0] 割り当て可能な配列は,構造体のﾌｨｰﾙﾄﾞにすることはできません。
 An allocatable array cannot appear as a field name within a structure
 definition.
SP_INTEGER_CONDITION [   c2] an integer conditional expression is not FORTRAN 77 standard
SP_INTEGER_CONDITION [j  c2] 整数条件式はFORTRAN 77の標準ではありません。
 A conditional expression is the expression that is evaluated and checked to
 determine a path of execution.
 A conditional expression can be found in an IF or WHILE statement.
 FORTRAN 77 requires that the conditional expression be a logical expression.
 As an extension, an integer expression is also allowed.
 When an integer expression is used, it is converted to a logical expression
 by comparing the value of the integer expression to zero.
SP_STATEMENT_REQUIRED [   c2] %s1 statement must be used within %s2 definition
SP_STATEMENT_REQUIRED [j  c2] %s1文は%s2の定義の中で使用しなければなりません。
 The statement identified by %s1 must appear within a definition identified
 by %s2.
SR Subprograms
SR_EXTRNED_TWICE [   c2] name can only appear in an EXTERNAL statement once
SR_EXTRNED_TWICE [j  c2] 関数/ｻﾌﾞﾙｰﾁﾝ名はEXTERNALの中に2度以上指定できません。
 A function/subroutine name appears more than once in an EXTERNAL
 statement.
SR_ILL_CHARFUNC [   c2] character function %s1 may not be called since size was declared as (*)
SR_ILL_CHARFUNC [j  c2] ｻｲｽﾞが(*)の文字型関数%s1は呼出せません(*は外部関数,仮引数,PARAMETER文字定数に対してのみです)。
 In the declaration of the character function name, the length was
 defined to be (*).
 The (*) length specification is only allowed for external functions,
 dummy arguments or symbolic character constants.
SR_ILL_PARM [   c2] %s1 can only be used an an argument to a subroutine
SR_ILL_PARM [j  c2] %s1はｻﾌﾞﾙｰﾁﾝに対する引数としてのみ使用できます。
 The specified class of an argument must only be passed to a subroutine.
 For example, an alternate return specifier is illegal as a subscript or
 an argument to a function.
SR_INTRNSC_EXTRN [   c2] name cannot appear in both an INTRINSIC and EXTERNAL statement
SR_INTRNSC_EXTRN [j  c2] 同じ名前をINTRINSICとEXTERNALの両方に指定することはできません。
 The same name appears in an INTRINSIC statement and in an EXTERNAL
 statement.
SR_NO_SUBRTN_NAME [   c2] expecting a subroutine name
SR_NO_SUBRTN_NAME [j  c2] SUBROUTINEでｻﾌﾞﾙｰﾁﾝ名を定義して下さい。
 The subroutine named in the CALL statement does not define a
 subroutine.
 A subroutine is declared in a SUBROUTINE statement.
SR_ILL_IN_PROG [   c0] %s1 statement not allowed in main program
SR_ILL_IN_PROG [j  c0] %s1文はﾒｲﾝ･ﾌﾟﾛｸﾞﾗﾑの中に書いてはいけません。
 The main program can contain any statements except a FUNCTION,
 SUBROUTINE, BLOCK DATA, or ENTRY statement.
 A SAVE statement is allowed but has no effect in the main program.
 A RETURN statement in the main program is an extension to the
 FORTRAN 77 language.
SR_NOT_INTRNSC [   c2] not an intrinsic FUNCTION name
SR_NOT_INTRNSC [j  c2] INTRINSICの名前は組込み関数名でなければなりません。
 A name in the INTRINSIC statement is not an intrinsic function name.
 Refer to the Language Reference for a complete list of the intrinsic
 functions.
SR_PREV_INTRNSC [   c2] name can only appear in an INTRINSIC statement once
SR_PREV_INTRNSC [j  c2] 同じ組込み関数名を重複してINTRINSICの中に指定してはいけません。
 An intrinsic function name appears more than once in the intrinsic
 function list.
SR_TRIED_RECURSION [   c2] subprogram recursion detected
SR_TRIED_RECURSION [j  c2] 副ﾌﾟﾛｸﾞﾗﾑの再帰呼出しが検出されました。
 An attempt has been made to recursively invoke a subprogram, that is,
 to invoke an already active subprogram.
SR_TWO_PROGRAMS [   c0] two main program units in the same file
SR_TWO_PROGRAMS [j  c0] 1個のﾌﾟﾛｸﾞﾗﾑ内にﾒｲﾝ･ﾌﾟﾛｸﾞﾗﾑの初めを示す命令が2個以上あってはなりません。
 There are two places in the program that signify the start of a main
 program.
 The PROGRAM statement or the first statement that is not enclosed by
 a PROGRAM, FUNCTION, SUBROUTINE or BLOCK DATA statement specifies the
 main program start.
SR_TWO_UNNAMED [   c0] only one unnamed %s1 is allowed in an executable program
SR_TWO_UNNAMED [j  c0] 名前の無い%s1はﾌﾟﾛｸﾞﾗﾑの中に1個しか指定できません。
 There may only be one unnamed BLOCK DATA subprogram or main program
 in an executable program.
SR_FUNC_AS_SUB [   c2] function referenced as a subroutine
SR_FUNC_AS_SUB [j  c2] 関数はｻﾌﾞﾙｰﾁﾝとして使えません。
 An attempt has been made to invoke a function using the CALL
 statement.
SR_ALREADY_ACTIVE [ w r0] attempt to invoke active function/subroutine
SR_ALREADY_ACTIVE [jw r0] 関数/ｻﾌﾞﾙｰﾁﾝは自分の中から自分を、または呼び出した親を呼び出すことはできません。
 An attempt has been made to invoke the current function/subroutine or
 a function/subroutine that was used to invoke current
 function/subroutine.
 The traceback produced when the error occurred lists all currently
 active functions/subroutines.
SR_ARG_USED_NOT_PASSED [ w r0] dummy argument %s1 is not in dummy argument list of entered subprogram
SR_ARG_USED_NOT_PASSED [jw r0] 仮引数%s1はENTRYと副ﾌﾟﾛｸﾞﾗﾑの両方に引数として定義してください。
 The named dummy argument found in the ENTRY statement does not appear
 in the subroutine's dummy argument list in the subprogram statement.
SR_BAD_TYPE [ w r0] function referenced as %s1 but defined to be %s2
SR_BAD_TYPE [jw r0] 呼び出された関数は%s1型ですがFUNCTION/ENTRYで%s2となっていて一致しません。
 An attempt has been made to invoke a function of the type %s1 but the
 function was defined as %s2 in the FUNCTION or ENTRY statement.
 The function name's type must be correctly declared in the main
 program.
SR_CHAR_SIZE_MISMATCH [ w r0] function referenced as CHARACTER*%u1 but defined to be CHARACTER*%u2
SR_CHAR_SIZE_MISMATCH [jw r0] CHARACTER*%u1として呼出された関数名はCHARACTER*%u2として定義されていて長さが合いません。
 The character length of the function in the calling subprogram is %d1 but
 the length used to define the function is %d2.
 These two lengths must match.
SR_NO_PROG [ w c0] missing main program
SR_NO_PROG [jw c0] PROGRAM(省略可能)で始まりENDで終わるﾒｲﾝ･ﾌﾟﾛｸﾞﾗﾑが必要です。
 The program file is either empty or contains only subroutines and
 functions.
 Each program require a main program.
 A main program starts with an optional PROGRAM statement and ends
 with an END statement.
SR_SUB_AS_FUNC [ w r0] subroutine referenced as a function
SR_SUB_AS_FUNC [jw r0] ｻﾌﾞﾙｰﾁﾝは関数として使えません。
 An attempt has been made to invoke a name as a function and has been
 defined as a subroutine in a SUBROUTINE or ENTRY statement.
SR_BLKDAT_CALLED [ w r0] attempt to invoke a block data subprogram
SR_BLKDAT_CALLED [jw r0] BLOCK DATA副ﾌﾟﾛｸﾞﾗﾑは変数の初期値化のためのもので呼び出すことはできません。
 An attempt has been made to invoke a block data subprogram.
 Block data subprograms are used to initialize variables before
 program execution commences.
SR_STRUCT_MISMATCH [ w r0] structure type of function %s1 does not match expected structure type
SR_STRUCT_MISMATCH [jw r0] 関数%s1の構造体の型が一致しません。
 The function returns a structure that is not equivalent to the structure
 expected.
 Two structures are equivalent if the types and orders of each field
 are the same.
 Unions are considered equivalent if their sizes are the same.
 Field names, and the structure name itself, do not have to be the same.
SS Subscripts and Substrings
SS_FUNCTION_VALUE [   c2] substringing of function or statement function return value is not FORTRAN 77 standard
SS_FUNCTION_VALUE [j  c2] 文字型関数や文関数で取り出した文字列の部分列化は,FORTRAN 77の標準ではありません。
 The character value returned from a CHARACTER function or statement
 function cannot be substrung.
 Only character variable names and array element names may be
 substrung.
SS_ONLY_IF_CHAR [   c2] substringing valid only for character variables and array elements
SS_ONLY_IF_CHAR [j  c2] 文字型変数や配列要素以外は部分列にできません。
 An attempt has been made to substring a name that is not defined to
 be of type CHARACTER and is neither a variable nor an array element.
SS_SSCR_RANGE [   r0] subscript expression out of range; %s1 does not exist
SS_SSCR_RANGE [j  r0] 添字式が配列の範囲外を間違って指しています(%s1は存在しません)。
 An attempt has been made to reference an element in an array that is
 out of bounds of the declared array size.
 The array element %s1 does not exist.
SS_SSTR_RANGE [   r0] substring expression (%i1:%i2) is out of range
SS_SSTR_RANGE [j  r0] 部分列の式(%i1:%i2)が範囲外を間違って指しています。
 An expression in the substring is larger than the string length or
 less than the value 1.
 The substring expression must be one in which
 .millust begin
 1 <= %d1 <= %d2 <= len
 .millust end
ST Statements and Statement Numbers
ST_ALREADY [   c0] statement number %i1 has already been defined in line %d2
ST_ALREADY [j  c0] 重複して文番号%i1を%d2行目で定義してはいけません。
 The two statements, in line %d2 and the current line, in the current
 program unit have the same statement label number, namely %d1.
ST_ASF_LATE [   c0] statement function definition appears after first executable statement
ST_ASF_LATE [j  c0] 文関数の定義はﾌﾟﾛｸﾞﾗﾑの中で最初の実行可能な文の前になければなりません。
 There is a statement function definition after the first executable
 statement in the program unit.
 Statement function definitions must follow specification statements
 and precede executable statements.
 Check that the statement function name is not an undeclared array
 name.
ST_BAD_BRANCHED [   c0] %s1 statement must not be branched to but was in line %d2
ST_BAD_BRANCHED [j  c0] %s1文は%d2行目からGOTOを受けることができない命令です。
 Line %d2 passed execution control down to the statement %s1.
 The specification statements, ADMIT, AT END, BLOCK DATA, CASE, ELSE,
 ELSE IF, END AT END, END BLOCK, END DO, END LOOP, END SELECT, END
 WHILE, ENTRY, FORMAT, FUNCTION, OTHERWISE, PROGRAM, QUIT, REMOTE
 BLOCK, SAVE, SUBROUTINE, and UNTIL statements may not have control of
 execution transferred to it.
ST_CANNOT_BRANCH [   c2] branch to statement %i1 in line %d2 not allowed
ST_CANNOT_BRANCH [j  c2] %i1文(%d2行目)に飛ぶことはできません。
 An attempt has been made to pass execution control up to the
 statement labelled %d1 in line %d2.
 The specification statements, ADMIT, AT END, BLOCK DATA, CASE, ELSE,
 ELSE IF, END AT END, END BLOCK, END DO, END LOOP, END SELECT, END
 WHILE, ENTRY, FORMAT, FUNCTION, OTHERWISE, PROGRAM, QUIT, REMOTE
 BLOCK, SAVE, SUBROUTINE, and UNTIL statements may not have control of
 execution transferred to it.
ST_DATA_ALREADY [   c2] specification statement must appear before %s1 is initialized
ST_DATA_ALREADY [j  c2] 変数等の定義は%s1が初期化される前に指定しなければなりません。
 The variable %s1 has been initialized in a specification statement.
 A COMMON or EQUIVALENCE statement then references the variable.
 The COMMON or EQUIVALENCE statement must appear before the item can
 be initialized.
 Use the DATA statement to initialize data in this case.
ST_EXPECT_FORMAT [   c0] statement %i1 was referenced as a FORMAT statement in line %d2
ST_EXPECT_FORMAT [j  c0] %i1文(%d2行目)が間違ってFORMATとして指定されています。
 The statement in line %d2 references statement label %d1 as a FORMAT
 statement.
 The statement at that label is not a FORMAT statement.
ST_IMPLICIT_LATE [   c0] IMPLICIT statement appears too late
ST_IMPLICIT_LATE [j  c0] IMPLICITは(PARAMETER以外の)変数の定義をする文の前に指定してください。
 The current IMPLICIT statement is out of order.
 The IMPLICIT statement may be interspersed with the PARAMETER
 statement but must appear before other specification statements.
ST_NO_EXEC [   c0] this statement will never be executed due to the preceding branch
ST_NO_EXEC [j  c0] この文はこの前に飛越し命令があるので実行されることがありません。
 Because execution control will always be passed around the indicated
 statement, the statement will never be executed.
ST_NO_STMTNO [   c2] expecting statement number
ST_NO_STMTNO [j  c2] 文番号を入れて下さい。
 The keyword GOTO or ASSIGN has been detected and the next part of the
 statement was not a statement number as was expected.
ST_NOT_FORMAT [   c2] statement number %i1 was not used as a FORMAT statement in line %d2
ST_NOT_FORMAT [j  c2] 文番号%i1(%d2行目)はFORMAT文ではありませんが、FORMAT文であるかのように使用されています。
 The statement at line %d2 with statement number %d1 is not a FORMAT
 statement but the current statement uses statement number %d1
 as if it labelled a FORMAT statement.
ST_SPEC_LATE [   c0] specification statement appears too late
ST_SPEC_LATE [j  c0] 変数等の定義はDATA、文関数、実行可能命令の前に指定しなければなりません。
 The indicated specification statement appears after
 a statement function definition or an executable statement.
 All specification statements must appear before these types of
 statements.
ST_NOT_ALLOWED [   c2] %s1 statement not allowed after %s2 statement
ST_NOT_ALLOWED [j  c2] %s1文は%s2文の後に指定してはいけません。
 The statement %s1 cannot be the object of a %s2 statement.
 %s2 represents a logical IF or WHILE statement.
 These statements include: specification statements, ADMIT, AT END,
 CASE, DO, ELSE, ELSE IF END, END AT END, END BLOCK, END DO, END
 GUESS, ENDIF, END LOOP, END SELECT, END WHILE, ENTRY, FORMAT,
 FUNCTION, GUESS, logical IF, block IF, LOOP, OTHERWISE, PROGRAM,
 REMOTE BLOCK, SAVE, SELECT, SUBROUTINE, UNTIL, and WHILE.
ST_NUM_TOO_BIG [   c2] statement number must be 99999 or less
ST_NUM_TOO_BIG [j  c2] 文番号は99999かそれ以下でなければなりません。
 The statement label number specified in the indicated statement has
 more than 5 digits.
ST_NUM_ZERO [   c1] statement number cannot be zero
ST_NUM_ZERO [j  c1] 文番号は0であってはいけません。
 The statement label number specified in the indicated statement is
 zero.
 Statement label numbers must be greater than 0 and less than or equal
 to 99999.
ST_TO_SELF [   c2] this statement could branch to itself
ST_TO_SELF [j  c2] 自分自身に飛んではいけません。
 The indicated statement refers to a statement label number which
 appears on the statement itself and therefore could branch to itself,
 creating an endless loop.
ST_UNDEFINED [   c0] missing statement number %i1 - used in line %d2
ST_UNDEFINED [j  c0] 文番号%i1はﾌﾟﾛｸﾞﾗﾑの中に無いので%d2行目の指定はｴﾗｰです。
 A statement with the statement label number %d1 does not exist in the
 current program unit.
 The statement label number is referenced in line %d2 of the program
 unit.
ST_UNKNOWN_STMT [   c2] undecodeable statement or misspelled word %s1
ST_UNKNOWN_STMT [j  c2] 代入文として間違っているか、文字のつづりが間違った%s1があります。
 The statement cannot be identified as an assignment statement or any
 other type of FORTRAN statement.
 The first word of a FORTRAN statement must be a statement keyword or
 the statement must be an assignment statement.
ST_UNREFERENCED [   c0] statement %i1 will never be executed due to the preceding branch
ST_UNREFERENCED [j  c0] 文%i1はこの前に飛越し命令があるので実行されることはありません。
 The statement with the statement label number of %d1 will never be
 executed because the preceding statement will always pass execution
 control around the statement and no other reference is made to the
 statement label.
ST_WANT_NAME [   c1] expecting keyword or symbolic name
ST_WANT_NAME [j  c1] ｷｰﾜｰﾄﾞかｼﾝﾎﾞﾙ名を指定してください。
 The first character of a statement is not an alphabetic.
 The first word of a statement must be a statement keyword or a
 symbolic name.
 Symbolic names must start with a letter (upper case or lower case), a
 dollar sign ($) or an underscore (_).
ST_LONG_NUM [   c1] number in %s1 statement is longer than 5 digits
ST_LONG_NUM [j  c1] %s1の数字は5桁より大きくてはいけません。
 The number in the PAUSE or STOP statement is longer than 5 digits.
ST_DATA_TOO_EARLY [   c0] position of DATA statement is not FORTRAN 77 standard
ST_DATA_TOO_EARLY [j  c0] DATA 文の位置はFORTRAN 77の標準ではありません。
 The FORTRAN 77 standard requires DATA statements to appear after all
 specification statements.
 As an extension to the standard, Watcom FORTRAN 77 allows DATA statements
 to appear before specification statements.
 Note that in the latter case, the type of the symbol must be established
 before data initialization occurs.
ST_FORMAT_LABEL [ w r0] no FORMAT statement with given label
ST_FORMAT_LABEL [jw r0] 指定したﾗﾍﾞﾙの位置にFORMAT文がありません。
 The current statement refers to the label of a FORMAT statement but
 the label appears on some other statement that is not a FORMAT
 statement.
ST_NUMBER [ w r0] statement number not in list or not the label of an executable statement
ST_NUMBER [jw r0] 文番号がかっこの中にない時や実行可能な文に付いていない時はｴﾗｰです。
 The specified statement number in the indicated statement is not in
 the list of statement numbers or it is not the statement label number
 of an executable statement.
ST_BRANCH_INTO [ w r0] attempt to branch into a control structure
ST_BRANCH_INTO [jw r0] この制御構造の中に飛び込むことはできません。
 An attempt has been made to pass execution control into a control
 structure.
 A statement uses a computed statement label number to transfer
 control.
 This value references a statement inside a control structure.
SV Subscripted Variables
SV_ARR_DECL [   c0] variable %s1 in array declarator must be in COMMON or a dummy argument
SV_ARR_DECL [j  c0] 配列の宣言子である変数%s1はCOMMONの中か仮引数として指定しなければなりません。
 The variable %s1 was used as an array declarator in a subroutine or
 function but the variable was not in a COMMON block nor was it a
 dummy argument in the FUNCTION, SUBROUTINE or ENTRY statement.
SV_ARR_PARM [   c0] adjustable/assumed size array %s1 must be a dummy argument
SV_ARR_PARM [j  c0] 調整可能な、あるいは仮ｻｲｽﾞの配列%s1は仮引数でなければなりません。
 The array %s1 used in the current subroutine or function must be a
 dummy argument.
 When the array declarator is adjustable or assumed-size, the array
 name must be a dummy argument.
SV_BAD_SSCR [    2] invalid subscript expression
SV_BAD_SSCR [j   2] 添字式が間違っています。
 The indicated subscript expression is not a valid integer expression
 or the high bound of the array is less than the low bound of the
 array when declaring the size of the array.
SV_INV_SSCR [   c2] invalid number of subscripts
SV_INV_SSCR [j  c2] 添字の個数が正しくありません。
 The number of subscripts used to describe an array element does not
 match the number of subscripts in the array declaration.
 The maximum number of subscripts allowed is 7.
SV_NO_LIST [   c2] using %s1 name incorrectly without list
SV_NO_LIST [j  c2] %s1は添字なしに使えません。
 An attempt has been made to assign a value to the declared array %s1.
 Values may only be assigned to elements in the array.
 An array element is the array name followed by integer expressions
 enclosed in parentheses and separated by commas.
SV_TRIED_SSTR [   c1] cannot substring array name %s1
SV_TRIED_SSTR [j  c1] 配列名%s1は添字がないと部分列にできません。
 An attempt has been made to substring the array %s1.
 Only an array element may be substrung.
SV_PVD [   c0] %s1 treated as an assumed size array
SV_PVD [j  c0] %s1は擬寸法仮配列とみなします(最後の次元が1の時などです)。
 A dummy array argument has been declared with 1 in the last
 dimension.
 The array is treated as if an '*' had been specified in place of the
 1.
 This is done to support a feature called "pseudo-variable
 dimensioning" which was supported by some FORTRAN IV compilers and is
 identical in concept to FORTRAN 77 assumed-size arrays.
SV_CANT_USE_ASSUMED [   c2] assumed size array %s1 cannot be used as an i/o list item or a format/unit identifier
SV_CANT_USE_ASSUMED [j  c2] 擬寸法仮配列(最後の次元が*の時)%s1はI/O書式やFORMAT/UNITの中に使えません。
 Assumed size arrays (arrays whose last dimension is '*') must not
 appear as an i/o list item (i.e. in a PRINT statement), a format
 identifier or an internal file specifier.
SV_DIMENSION_LIMIT [    2] limit of 65535 elements per dimension has been exceeded
SV_DIMENSION_LIMIT [j   2] 次元の要素限度は65535を超えてはいけません。
 On the IBM PC, for 16-bit real mode applications, the number of elements in a
 dimension must not exceed 65535.
SX Syntax Errors
SX_UNEXPECTED_OPN [   c2] unexpected number or name %s1
SX_UNEXPECTED_OPN [j  c2] 数字か名前%s1がまちがった場所にあります。
 The number or name %s1 is in an unexpected place in the statement.
SX_BAD_OPR_SEQ [   c2] bad sequence of operators
SX_BAD_OPR_SEQ [j  c2] 演算子の順番がまちがっています。
 The indicated arithmetic operator is out of order.
 An arithmetic operator is one of the following:
 **, *, /, +, and &minus..
 All arithmetic operators must be followed by at least a primary.
 A primary is an array element, constant, (expression), function name,
 or variable name.
SX_INV_OPR [   c1] invalid operator
SX_INV_OPR [j  c1] 演算子が正しくありません。
 The indicated operator between the two arithmetic primaries is not a
 valid operator.
 Valid arithmetic operators include: **, *, /, +, and &minus..
 A primary is an array element, constant, (expression), function name,
 or variable name.
SX_JUNK_AFTER_RBR [   c0] expecting end of statement after right parenthesis
SX_JUNK_AFTER_RBR [j  c0] 右かっこの後にｽﾃｰﾄﾒﾝﾄをつづけてはいけません。
 The end of the statement is indicated by the closing right
 parenthesis but more characters were found on the line.
 Multiple statements per line are not allowed in FORTRAN 77.
SX_NO_ASTERISK [   c1] expecting an asterisk
SX_NO_ASTERISK [j  c1] ｱｽﾀﾘｽｸ(*)が必要です。
 The next character of the statement should be an asterisk but another
 character was found instead.
SX_NO_COLON [   c1] expecting colon
SX_NO_COLON [j  c1] ｺﾛﾝ(:)が必要です。
 A colon (:) was expecting but not found.
 For example, the colon separating the low and high bounds of a
 character substring was not found.
SX_NO_EOS_OR_COLON [   c2] expecting colon or end of statement
SX_NO_EOS_OR_COLON [j  c2] 制御文の終りに関係のないものがあります( : かﾌﾞﾛｯｸ･ﾗﾍﾞﾙが必要です)。
 On a control statement, a word was found at the end of the statement
 that cannot be related to the statement.
 The last word on several of the control statements may be a block
 label.
 All block labels must be preceded by a colon (:).
SX_MISSING_COMMA [   c1] missing comma
SX_MISSING_COMMA [j  c1] ｺﾝﾏがありません。
 A comma was expected and is missing.
 There must be a comma after the statement keyword AT END when a
 statement follows.
 A comma must occur between the two statement labels in the GO TO
 statement.
 A comma must occur between the expressions in the DO statement.
 A comma must occur between the names listed in the DATA statement
 and specification statements.
 A comma must occur between the specifiers in I/O statements.
SX_EOS_EXPECTED [   c1] expecting end of statement
SX_EOS_EXPECTED [j  c1] 文の終りに関係のないものがあります(1行に1文しか書けません)。
 The end of the statement was expected but more words were found on
 the line and cannot be associated to the statement.
 FORTRAN 77 only allows for one statement per line.
SX_NO_INTEGER_VAR [   c2] expecting integer variable
SX_NO_INTEGER_VAR [j  c2] 整数型変数でなければなりません。
 The name indicated in the statement must be of type INTEGER but is
 not.
SX_NO_NAME [   c2] expecting %s1 name
SX_NO_NAME [j  c2] 名前%s1がここになければなりません。
 A name with the characteristic %s1 was expected at the indicated place
 in the statement but is missing.
SX_EXPECT_INT [   c2] expecting an integer
SX_EXPECT_INT [j  c2] 整数でなければなりません。
 The length specifier, as in the IMPLICIT statement, must be an
 integer constant or an integer constant expression.
 The repeat specifier of the value to be assigned to the variables, as
 in the DATA statement, must be an integer constant or an integer
 constant expression.
SX_NO_NUMBER_VAR [   c2] expecting INTEGER, REAL, or DOUBLE PRECISION variable
SX_NO_NUMBER_VAR [j  c2] 整数か実数か倍精度型の変数でなければなりません。
 The indicated DO variable is not one of the types INTEGER, REAL, or
 DOUBLE PRECISION.
SX_NO_OPR [   c1] missing operator
SX_NO_OPR [j  c1] 演算子がありません。
 Two primaries were found in an expression and an operator was not
 found in between.
 A primary is an array element, constant, (expression), function name,
 or variable name.
SX_NO_SLASH [   c1] expecting a slash
SX_NO_SLASH [j  c1] ｽﾗｯｼｭ(/)が必要です。
 A slash is expected in the indicated place in the statement.
 Slashes must be balanced as parentheses.
 Slashes are used to enclose the initial data values in specification
 statements or to enclose names of COMMON blocks.
SX_WRONG_TYPE [   c2] expecting %s1 expression
SX_WRONG_TYPE [j  c2] ﾀｲﾌﾟ%s1の式でなければなりません。
 An expression of type %s1 is required.
SX_NOT_CONST_EXPR [   c2] expecting a constant expression
SX_NOT_CONST_EXPR [j  c2] 定数式でなければなりません。
 A constant expression is required.
SX_NOT_SIMPLE_NUMBER [   c2] expecting INTEGER, REAL, or DOUBLE PRECISION expression
SX_NOT_SIMPLE_NUMBER [j  c2] 整数か実数か倍精度の数式でなければなりません。
 The indicated expression is not one of type INTEGER, REAL, or DOUBLE
 PRECISION.
 Each expression following the DO variable must be an expression of
 one of these types.
SX_NUM_OR_LIT [   c2] expecting INTEGER or CHARACTER constant
SX_NUM_OR_LIT [j  c2] 整数型か文字型定数でなければなりません。
 In the PAUSE and STOP statement, the name following the keyword must
 be a constant of type INTEGER or of type CHARACTER.
 This constant will be printed on the console when the statement is
 executed.
SX_SURP_OPR [   c1] unexpected operator
SX_SURP_OPR [j  c1] 余分な演算子があります。
 An operand was expected but none was found.
 For example, in an I/O statement, the comma is used to separate I/O
 list items.
 Two consecutive commas without an I/O list item between them would
 result in this error.
SX_QUOTE [   c2] no closing quote on literal string
SX_QUOTE [j  c2] 文字列を閉じる右の(')引用符がありません。
 The closing quote of a literal string was not found before the end of
 the statement.
SX_CONST [   c2] missing or invalid constant
SX_CONST [j  c2] 定数が無いか、形式が正しくありません。
 In a DATA statement, the constant required to initialize a variable
 was not found or incorrectly specified.
SX_EXPECT_CHAR_CONST [   c2] expecting character constant
SX_EXPECT_CHAR_CONST [j  c2] 文字型定数でなければなりません。
 A character constant is required.
TY Type Statements
TY_CHAR_BEFORE_PAREN [   c1] length specification before array declarator is not FORTRAN 77 standard
TY_CHAR_BEFORE_PAREN [j  c1] 配列宣言子の前の長さ指定はFORTRAN 77の標準ではありません。
 An array declarator specified immediately after the length
 specification of the array is an extension to the FORTRAN 77
 language.
TY_ILL_TYP_SIZE [   c2] %i1 is an illegal length for %s2 type
TY_ILL_TYP_SIZE [j  c2] %i1の長さ指定は%s2型に対して正しくありません。
 The length specifier %d1 is not valid for the type %s2.
 For type LOGICAL, valid lengths are 1 and 4.
 For the type INTEGER, valid lengths are 1, 2, and 4.
 For the type REAL, valid lengths are 4 and 8.
 For the type COMPLEX, valid lengths are 8 and 16.
 On the IBM PC, the length specifier for items of type CHARACTER must
 be greater than 0 and not exceed 65535.
TY_LEN_SPEC [   c1] length specifier in %s1 statement is not FORTRAN 77 standard
TY_LEN_SPEC [j  c1] %s1文の長さ指定はFORTRAN 77の標準ではありません。
 A length specifier in certain type specification statements is an
 extension to the FORTRAN 77 language.
 These include: LOGICAL*1, LOGICAL*4, INTEGER*1, INTEGER*2, INTEGER*4,
 REAL*4, REAL*8, COMPLEX*8, and COMPLEX*16.
TY_NOT_DBL_PREC [   c2] length specification not allowed with type %s1
TY_NOT_DBL_PREC [j  c2] %s1ﾀｲﾌﾟのｽﾃｰﾄﾒﾝﾄに長さ指定はいりません。
 A length specification is not allowed in a DOUBLE PRECISION or
 DOUBLE COMPLEX statement.
TY_TYP_PREV_DEF [   c2] type of %s1 has already been established as %s2
TY_TYP_PREV_DEF [j  c2] %s1は%s2として定義されていますので、別な名前に変えてください。
 The indicated name %s1 has already been declared to have a different
 type, namely %s2.
 The name %s1 cannot be used in this specification statement.
TY_UNDECLARED [   c2] type of %s1 has not been declared
TY_UNDECLARED [j  c2] IMPLICIT NONEの時は%s1の型は定義してください。
 The indicated name %s1 has not been declared.
 This message is only issued when the IMPLICIT NONE specification
 statement is used.
TY_ILL_USE [   c2] %s1 of type %s2 is illegal in %s3 statement
TY_ILL_USE [j  c2] %s2型である%s1は%s3文中で間違って使われています。
 The symbol %s1 with type %s2 cannot be used in statement %s3.
 For example, a symbol of type STRUCTURE cannot be used in a PARAMETER
 statement.
UV Undefined Variables
UV_CH_ASGN_UNDEFINED [ w r0] substring (%u1:%u2) of expression on right side of equal sign is undefined
UV_CH_ASGN_UNDEFINED [jw r0] =の右側の式の部分列(%u1:%u2)に値が入っていません。
 The substring (%d1:%d2) has been used on the right hand side of an
 expression and its value is undefined.
 A value must be assigned to the variable name or array element name
 before it can be used in an expression.
UV_CH_FN_UNDEFINED [ w r0] substring (%u1:%u2) of return value of character function %s3 is undefined
UV_CH_FN_UNDEFINED [jw r0] 戻った結果の部分列(%u1:%u2)[文字関数%s3から]の中に値が入っていなければなりません。
 The character function %s3 has been referenced and the function name
 has not been assigned a value in the function.
 The substring (%d1:%d2) does not have a value.
UV_CH_REL_UNDEFINED [ w r0] substring (%u1:%u2) of an operand in a relational expression is undefined
UV_CH_REL_UNDEFINED [jw r0] 関係式の部分列(%u1:%u2)に値が入っていません。
 The substring (%d1:%d2) has been used in a relational expression and
 has not previously been assigned a value.
UV_IF_CHAR_ARG_UNDEFINED [ w r0] substring (%u1:%u2) of an argument to an intrinsic function is undefined
UV_IF_CHAR_ARG_UNDEFINED [jw r0] 組込み関数の引数である部分列(%u1:%u2)に値が入っていません。
 The substring (%d1:%d2), which is an actual argument to an intrinsic
 function, does not have a value assigned to it.
UV_UNDEFD [ w r0] the value of %s1 is undefined
UV_UNDEFD [jw r0] %s1に値が入っていません。
 The variable name %s1 has been used in a statement and a value has not
 yet been assigned to it.
 All variables must first be initialized through the use of statements
 or data initialization in specification statements.
UV_ARG_UNDEFINED [ w r0] argument number %d1 passed to intrinsic function is undefined
UV_ARG_UNDEFINED [jw r0] 組込み関数の第%d1番目の引数に値が入っていません。
 The actual argument number %d1 does not have a value assigned to it.
VA Variable Names
VA_ILL_USE [   c2] illegal use of %s1 name %s2 in %s3 statement
VA_ILL_USE [j  c2] %s1と定義された名前%s2は%s3文では使えません。
 The name %s2 has been defined as %s1 and cannot be used as a name in
 the statement %s3.
VA_NAME_LEN_EXT [   c0] symbolic name %s1 is longer than 6 characters
VA_NAME_LEN_EXT [j  c0] ｼﾝﾎﾞﾙ名%s1に7文字以上使えるのは標準ではありません。
 Symbolic names greater than 6 characters is an extension to the
 FORTRAN 77 language.
 The maximum length is 32 characters.
VA_PREV_DEF_NAM [   c0] %s1 has already been defined as a %s2
VA_PREV_DEF_NAM [j  c0] %s1はすでに%s2として定義されています。
 The name %s1 has been previously defined as a %s2 in another
 statement and cannot be redefined as specified in the indicated
 statement.
VA_UNDEFINED [   c0] %s1 %s2 has not been defined
VA_UNDEFINED [j  c0] %s1として%s2は使われていますが、そのように定義してください。
 The name %s2 has been referenced to be a %s1 but has not been defined
 as such in the program unit.
VA_UNREFERENCED [   c0] %s1 is an unreferenced symbol
VA_UNREFERENCED [j  c0] %s1は定義されましたが使われていません。
 The name %s1 has been defined but not referenced.
VA_SAME_NAMELIST [   c2] %s1 already belongs to this NAMELIST group
VA_SAME_NAMELIST [j  c2] %s1は既にこのNAMELISTｸﾞﾙｰﾌﾟに属しています。
 The name %s1 can only appear in a NAMELIST group once.
 However, a name can belong to multiple NAMELIST groups.
VA_USED_NOT_DEFINED [   c0] %s1 has been used but not defined
VA_USED_NOT_DEFINED [j  c0] %s1が使われましたが、定義されていません。
 %s1 has not been defined before using it in a way that requires its
 definition.
 Note that symbols that are equivalenced, belong to a common block, are
 dummy arguments, or passed as an argument to a subprogram, will not be
 checked to ensure that they have been defined before requiring a value.
VA_ALLOCATABLE_STORAGE [   c2] dynamically allocating %s1 is not FORTRAN 77 standard
VA_ALLOCATABLE_STORAGE [j  c2] 動的に割り当てている%s1はFORTRAN 77の標準ではありません。
 Allocatable storage are extensions to the FORTRAN 77 standard.
VA_BAD_SYM_IN_NAMELIST [   c0] %s1 in NAMELIST %s2 is illegal
VA_BAD_SYM_IN_NAMELIST [j  c0] NAMELIST %s2の中の%s1は不正です。
 Symbol %s1 appearing in NAMELIST %s2 is illegal.
 Symbols appearing in a NAMELIST cannot be dummy arguments, allocatable, or of
 a user-defined type.
MS Additional Messages
MS_USAGE_LINE [ o c0] Usage: %s1 {- or /<option>} <file-spec> {- or /<option>}
MS_USAGE_LINE [jo c0] 使用方法: %s1 {- or /<option>} <file-spec> {- or /<option>}
MS_USAGE_LINE [ w c0] Usage: %s1 {- or /<option>} <file-spec> {- or /<option>} {<prog-parm>}
MS_USAGE_LINE [jw c0] 使用方法: %s1 {- or /<option>} <file-spec> {- or /<option>} {<prog-parm>}
MS_OPT_LIST [ . c0] generate a listing file
MS_OPT_LIST [j. c0] ﾘｽﾄﾌｧｲﾙの生成
MS_OPT_PRINT [ . c0] listing file to printer
MS_OPT_PRINT [j. c0] ﾘｽﾄﾌｧｲﾙをﾌﾟﾘﾝﾀｰに出力
MS_OPT_TYPE [ . c0] listing file to terminal
MS_OPT_TYPE [j. c0] ﾘｽﾄﾌｧｲﾙを画面に出力
MS_OPT_DISK [ . c0] listing file to disk
MS_OPT_DISK [j. c0] ﾘｽﾄﾌｧｲﾙをﾃﾞｨｽｸに出力
MS_OPT_ERRFILE [ . c0] generate an error file
MS_OPT_ERRFILE [j. c0] ｴﾗｰﾌｧｲﾙの生成
MS_OPT_EXT [ . c0] issue extension messages
MS_OPT_EXT [j. c0] 拡張ﾒｯｾｰｼﾞの出力
MS_OPT_REFERENCE [ . c0] issue unreferenced warning
MS_OPT_REFERENCE [j. c0] 未参照警告ｴﾗｰの出力
MS_OPT_WARN [ . c0] issue warning messages
MS_OPT_WARN [j. c0] 警告ｴﾗｰﾒｯｾｰｼﾞの出力
MS_OPT_SHORT [ . c0] INTEGER/LOGICAL size 2/1
MS_OPT_SHORT [j. c0] INTEGER/LOGICAL の大きさは 2/1
MS_OPT_INCLIST [ . c0] list INCLUDE files
MS_OPT_INCLIST [j. c0] INCLUDE ﾌｧｲﾙのﾘｽﾄ
MS_OPT_TERM [ . c0] display diagnostic messages
MS_OPT_TERM [j. c0] 診断ﾒｯｾｰｼﾞの表示
MS_OPT_EXPLICIT [ . c0] explicit typing required
MS_OPT_EXPLICIT [j. c0] 明示的な型が必要
MS_OPT_XLINE [ . c0] extend line length
MS_OPT_XLINE [j. c0] 行の長さを拡張
MS_OPT_DEBUG [ w c0] invoke run-time debugger
MS_OPT_DEBUG [jw c0] ﾗﾝﾀｲﾑﾃﾞﾊﾞｯｶﾞの起動
MS_OPT_DEBUG [ c c0] /trace /bounds
MS_OPT_DEBUG [jc c0] /trace /bounds
MS_OPT_DEFINE [ . c0] define macro
MS_OPT_DEFINE [j. c0] ﾏｸﾛ定義
MS_OPT_EXTEND_FORMAT [ . c0] relax FORMAT type checking
MS_OPT_EXTEND_FORMAT [j. c0] FORMAT ﾀｲﾌﾟﾁｪｯｸの緩和
MS_OPT_WILD [ . c0] relax wild branch checking
MS_OPT_WILD [j. c0] ﾜｲﾙﾄﾞﾌﾞﾗﾝﾁﾁｪｯｸの緩和
MS_OPT_OBJECT [ w c0] generate object code
MS_OPT_OBJECT [jw c0] ｵﾌﾞｼﾞｪｸﾄｺｰﾄﾞの生成
MS_OPT_EDIT [ w c0] use editor interface
MS_OPT_EDIT [jw c0] ｴﾃﾞｨﾀｰｲﾝﾀｰﾌｪｰｽの使用
MS_OPT_LINK [ w c0] generate an executable
MS_OPT_LINK [jw c0] 実行ﾌｧｲﾙの生成
MS_OPT_RUN [ w c0] run the executable file
MS_OPT_RUN [jw c0] 実行ﾌｧｲﾙの起動
MS_OPT_XTYPE [ w c0] unit 6 output to terminal
MS_OPT_XTYPE [jw c0] 装置6 は端末に接続
MS_OPT_LOG_IO [ w c0] log unit *
MS_OPT_LOG_IO [jw c0] log unit *
MS_OPT_CHECK [ w c0] undefined variable checking
MS_OPT_CHECK [jw c0] 未定義変数のﾁｪｯｸ
MS_OPT_ARRCHECK [ w c0] array type checking
MS_OPT_ARRCHECK [jw c0] 配列ﾀｲﾌﾟのﾁｪｯｸ
MS_OPT_ST_LIM [ w c0] statement limit
MS_OPT_ST_LIM [jw c0] 文ﾘﾐｯﾄ
MS_OPT_PG_LIM [ w c0] page limit
MS_OPT_PG_LIM [jw c0] ﾍﾟｰｼﾞﾘﾐｯﾄ
MS_OPT_TM_LIM [ w c0] time limit
MS_OPT_TM_LIM [jw c0] ﾀｲﾑﾘﾐｯﾄ
MS_OPT_MM_LIM [ w c0] code size limit
MS_OPT_MM_LIM [jw c0] ｺｰﾄﾞｻｲｽﾞﾘﾐｯﾄ
MS_OPT_PAGE_SIZE [ w c0] number of lines per page
MS_OPT_PAGE_SIZE [jw c0] 1ﾍﾟｰｼﾞあたりの行数
MS_OPT_MATH_INT_OVER [ w c0] report integer overflows
MS_OPT_MATH_INT_OVER [jw c0] 整数ｵｰﾊﾞｰﾌﾛｰの報告
MS_OPT_MATH_FLT_OVER [ w c0] report float-pt. overflows
MS_OPT_MATH_FLT_OVER [jw c0] 浮動小数点ｵｰﾊﾞｰﾌﾛｰの報告
MS_OPT_MATH_FLT_UNDER [ w c0] report float-pt. underflows
MS_OPT_MATH_FLT_UNDER [jw c0] 浮動小数点ｱﾝﾀﾞｰﾌﾛｰの報告
MS_OPT_SYNTAX [ c c0] syntax check only
MS_OPT_SYNTAX [jc c0] 文法ﾁｪｯｸのみ
MS_OPT_DFLT_LIB [ c c0] generate default libraries
MS_OPT_DFLT_LIB [jc c0] ﾃﾞﾌｫﾙﾄﾗｲﾌﾞﾗﾘの生成
MS_CPUOPT_FPC [ c8c0] floating-point calls
MS_CPUOPT_FPC [jc8c0] 浮動小数点呼び出し
MS_CPUOPT_FPI [ c8c0] emulate 80x87 instructions
MS_CPUOPT_FPI [jc8c0] 80x87 命令のｴﾐｭﾚｰﾄ
MS_CPUOPT_FPI87 [ c8c0] in-line 80x87 instructions
MS_CPUOPT_FPI87 [jc8c0] 80x87 ｲﾝﾗｲﾝ命令
MS_CPUOPT_FP287 [ c8c0] 80287 instructions
MS_CPUOPT_FP287 [jc8c0] 80287 命令
MS_CPUOPT_FP387 [ c8c0] 80387 instructions
MS_CPUOPT_FP387 [jc8c0] 80387 命令
MS_CPUOPT_FP5 [ c8c0] /fp3 optimized for Pentium
MS_CPUOPT_FP5 [jc8c0] Pentium用最適化をする/fp3ｵﾌﾟｼｮﾝ
MS_CPUOPT_FPR [ c8c0] 80x87 reverse compatibility
MS_CPUOPT_FPR [jc8c0] 80x87 ﾘﾊﾞｰｽ互換
MS_CGOPT_DB_LINE [ c c0] line # debugging information
MS_CGOPT_DB_LINE [jc c0] line # ﾃﾞﾊﾞｯｸﾞ情報
MS_CGOPT_DB_LOCALS [ c c0] full debugging information
MS_CGOPT_DB_LOCALS [jc c0] 全てのﾃﾞﾊﾞｯｸﾞ情報
MS_OZOPT_O_BASE_PTR [ c3c0] base pointer optimizations
MS_OZOPT_O_BASE_PTR [jc3c0] ﾍﾞｰｽﾎﾟｲﾝﾀの最適化
MS_OZOPT_O_CALL_RET [ c c0] no call-ret optimizations
MS_OZOPT_O_CALL_RET [jc c0] call-retの最適化なし
MS_OZOPT_O_DISABLE [ c c0] disable optimizations
MS_OZOPT_O_DISABLE [jc c0] 最適化の禁止
MS_OZOPT_O_NEED_FRAME [ c8c0] no stack frame optimizations
MS_OZOPT_O_NEED_FRAME [jc8c0] ｽﾀｯｸﾌﾚｰﾑの最適化なし
MS_OZOPT_O_INLINE [ c c0] statement functions in-line
MS_OZOPT_O_INLINE [jc c0] ｲﾝﾗｲﾝ文関数
MS_OZOPT_O_LOOP [ c c0] loop optimizations
MS_OZOPT_O_LOOP [jc c0] ﾙｰﾌﾟの最適化
MS_OZOPT_O_MATH [ c c0] math optimizations
MS_OZOPT_O_MATH [jc c0] 数学的最適化
MS_OZOPT_O_NUMERIC [ c c0] numerical optimizations
MS_OZOPT_O_NUMERIC [jc c0] 数値の最適化
MS_OZOPT_O_VOLATILE [ c c0] precision optimizations
MS_OZOPT_O_VOLATILE [jc c0] 精度の最適化
MS_OZOPT_O_INSSCHED [ c c0] instruction scheduling
MS_OZOPT_O_INSSCHED [jc c0] 命令ｽｹｼﾞｭｰﾘﾝｸﾞ
MS_OZOPT_O_SPACE [ c c0] space optimizations
MS_OZOPT_O_SPACE [jc c0] ｻｲｽﾞの最適化
MS_OZOPT_O_TIME [ c c0] time optimizations
MS_OZOPT_O_TIME [jc c0] 時間の最適化
MS_OZOPT_O_X [ c3c0] /o[b,bp,i,k,l,m,r,t,do]
MS_OZOPT_O_X [jc3c0] /o[b,bp,i,k,l,m,r,t,do]
MS_OZOPT_O_X [ cic0] /o[bp,i,l,k,m,r,t,do]
MS_OZOPT_O_X [jcic0] /o[bp,i,l,k,m,r,t,do]
MS_OZOPT_O_X [ crc0] /o[bp,i,l,k,m,r,t,do]
MS_OZOPT_O_X [jcrc0] /o[bp,i,l,k,m,r,t,do]
MS_CGOPT_STACK_CHK [ c8c0] stack checking
MS_CGOPT_STACK_CHK [jc8c0] ｽﾀｯｸﾁｪｯｸ
MS_CGOPT_DATA_THRESH [ c c0] set data threshold
MS_CGOPT_DATA_THRESH [jc c0] ﾃﾞｰﾀｽﾚｯｼｮﾙﾄﾞの設定
MS_OPT_AUTOMATIC [ c c0] local variables on the stack
MS_OPT_AUTOMATIC [jc c0] ｽﾀｯｸ上のﾛｰｶﾙ変数
MS_OPT_DESCRIPTOR [ c c0] pass character descriptors
MS_OPT_DESCRIPTOR [jc c0] 文字指示子渡し
MS_OPT_SAVE [ c c0] SAVE local variables
MS_OPT_SAVE [jc c0] ﾛｰｶﾙ変数を待避(SAVE文と同じ)
MS_CGOPT_OBJ_NAME [ c c0] name object file
MS_CGOPT_OBJ_NAME [jc c0] ｵﾌﾞｼﾞｪｸﾄﾌｧｲﾙ名の指定
MS_CGOPT_CONST_CODE [ c8c0] constants in code segment
MS_CGOPT_CONST_CODE [jc8c0] ｺｰﾄﾞｾｸﾞﾒﾝﾄ内の定数
MS_CGOPT_M_MEDIUM [ cic0] medium memory model
MS_CGOPT_M_MEDIUM [jcic0] ﾐﾃﾞｨｱﾑﾒﾓﾘﾓﾃﾞﾙ
MS_CGOPT_M_LARGE [ cic0] large memory model
MS_CGOPT_M_LARGE [jcic0] ﾗｰｼﾞﾒﾓﾘﾓﾃﾞﾙ
MS_CGOPT_M_HUGE [ cic0] huge memory model
MS_CGOPT_M_HUGE [jcic0] ﾋｭｰｼﾞﾒﾓﾘﾓﾃﾞﾙ
MS_CPUOPT_8086 [ cic0] assume 8086 processor
MS_CPUOPT_8086 [jcic0] 8086 ﾌﾟﾛｾｯｻ命令を使用
MS_CPUOPT_80186 [ cic0] assume 80186 processor
MS_CPUOPT_80186 [jcic0] 80186 ﾌﾟﾛｾｯｻ命令を使用
MS_CPUOPT_80286 [ cic0] assume 80286 processor
MS_CPUOPT_80286 [jcic0] 80286 ﾌﾟﾛｾｯｻ命令を使用
MS_CGOPT_STK_ARGS [ c3c0] stack calling convention
MS_CGOPT_STK_ARGS [jc3c0] ｽﾀｯｸ呼び出し規約
MS_CGOPT_EZ_OMF [ c3c0] Easy OMF-386 object files
MS_CGOPT_EZ_OMF [jc3c0] Easy OMF-386 ｵﾌﾞｼﾞｪｸﾄﾌｧｲﾙ
MS_CGOPT_BW [ c c0] default windowed application
MS_CGOPT_BW [jc c0] ﾃﾞﾌｫﾙﾄｳｨﾝﾄﾞｳｱﾌﾟﾘｹｰｼｮﾝ
MS_CGOPT_BM [ c3c0] multithread application
MS_CGOPT_BM [jc3c0] ﾏﾙﾁｽﾚｯﾄﾞｱﾌﾟﾘｹｰｼｮﾝ
MS_CGOPT_BM [ crc0] multithread application
MS_CGOPT_BM [jcrc0] ﾏﾙﾁｽﾚｯﾄﾞｱﾌﾟﾘｹｰｼｮﾝ
MS_CGOPT_BD [ c3c0] dynamic link library
MS_CGOPT_BD [jc3c0] ﾀﾞｲﾅﾐｯｸﾘﾝｸﾗｲﾌﾞﾗﾘ
MS_CGOPT_BD [ crc0] dynamic link library
MS_CGOPT_BD [jcrc0] ﾀﾞｲﾅﾐｯｸﾘﾝｸﾗｲﾌﾞﾗﾘ
MS_CGOPT_STACK_GROW [ c3c0] automatic stack growing
MS_CGOPT_STACK_GROW [jc3c0] ｽﾀｯｸの自動伸張
MS_CGOPT_M_FLAT [ c3c0] flat memory model
MS_CGOPT_M_FLAT [jc3c0] ﾌﾗｯﾄﾒﾓﾘﾓﾃﾞﾙ
MS_CGOPT_M_SMALL [ c3c0] small memory model
MS_CGOPT_M_SMALL [jc3c0] ｽﾓｰﾙﾒﾓﾘﾓﾃﾞﾙ
MS_CGOPT_M_COMPACT [ c3c0] compact memory model
MS_CGOPT_M_COMPACT [jc3c0] ｺﾝﾊﾟｸﾄﾒﾓﾘﾓﾃﾞﾙ
MS_CGOPT_M_MEDIUM [ c3c0] medium memory model
MS_CGOPT_M_MEDIUM [jc3c0] ﾐﾃﾞｨｱﾑﾒﾓﾘﾓﾃﾞﾙ
MS_CGOPT_M_LARGE [ c3c0] large memory model
MS_CGOPT_M_LARGE [jc3c0] ﾗｰｼﾞﾒﾓﾘﾓﾃﾞﾙ
MS_CPUOPT_80386 [ c8c0] assume 80386 processor
MS_CPUOPT_80386 [jc8c0] 80386 ﾌﾟﾛｾｯｻ命令を使用
MS_CPUOPT_80486 [ c8c0] /3 optimized for 80486
MS_CPUOPT_80486 [jc8c0] 80486用最適化をする/3ｵﾌﾟｼｮﾝ
MS_CPUOPT_80586 [ c8c0] /3 optimized for Pentium
MS_CPUOPT_80586 [jc8c0] Pentium用最適化をする/3ｵﾌﾟｼｮﾝ
MS_CGOPT_SEG_REGS [ c8c0] save/restore segment regs
MS_CGOPT_SEG_REGS [jc8c0] ｾｸﾞﾒﾝﾄﾚｼﾞｽﾀの待避/復帰
MS_CGOPT_FS_FLOATS [ c8c0] FS not fixed
MS_CGOPT_FS_FLOATS [jc8c0] FSを固定しない
MS_CGOPT_GS_FLOATS [ c8c0] GS not fixed
MS_CGOPT_GS_FLOATS [jc8c0] GSを固定しない
MS_CGOPT_SS_FLOATS [ cic0] SS not default data segment
MS_CGOPT_SS_FLOATS [jcic0] SSはﾃﾞﾌｫﾙﾄのﾃﾞｰﾀｾｸﾞﾒﾝﾄでない
MS_CGOPT_WINDOWS [ c8c0] generate code for Windows
MS_CGOPT_WINDOWS [jc8c0] ｳｨﾝﾄﾞｳｽﾞ用ｺｰﾄﾞ生成
MS_STRUCT_DEFN [   c0] structure definition
MS_STRUCT_DEFN [j  c0] 構造体定義
MS_GROUP_NAME [   c0] group name
MS_GROUP_NAME [j  c0] ｸﾞﾙｰﾌﾟ名
MS_STMT_FUNC [   c0] statement function
MS_STMT_FUNC [j  c0] 文関数
MS_STMT_FUNC_DEFN [   c0] statement function definition
MS_STMT_FUNC_DEFN [j  c0] 文関数定義
MS_SIMP_VAR [   c0] simple variable
MS_SIMP_VAR [j  c0] 単純変数
MS_COM_VAR [   c0] variable in COMMON
MS_COM_VAR [j  c0] COMMON内変数
MS_EQUIV_VAR [   c0] equivalenced variable
MS_EQUIV_VAR [j  c0] 等価変数
MS_SP_ARG [   c0] subprogram argument
MS_SP_ARG [j  c0] 副ﾌﾟﾛｸﾞﾗﾑ引数
MS_SF_ARG [   c0] statement function argument
MS_SF_ARG [j  c0] 文関数引数
MS_ARRAY [   c0] array
MS_ARRAY [j  c0] 配列
MS_ALLOC_ARRAY [   c0] allocated array
MS_ALLOC_ARRAY [j  c0] 割り当てられた配列
MS_VAR_ARR [   c0] variable or array
MS_VAR_ARR [j  c0] 変数または配列
MS_INTR_FUNC [   c0] intrinsic function
MS_INTR_FUNC [j  c0] 組み込み関数
MS_EXT_PROC [ o c0] external subprogram
MS_EXT_PROC [jo c0] 外部副ﾌﾟﾛｸﾞﾗﾑ
MS_EXT_PROC [ w  0] external subprogram
MS_EXT_PROC [jw  0] 外部副ﾌﾟﾛｸﾞﾗﾑ
MS_ASSIGN [   c0] assignment
MS_ASSIGN [j  c0] 代入
MS_ARITH_IF [   c0] arithmetic if
MS_ARITH_IF [j  c0] 算術 if
MS_AT_EOSTMT [   c0] at end of statement,
MS_AT_EOSTMT [j  c0] 文の終わり,
MS_DEF_SPROG [   c0] Defining subprogram: %s1; first ISN is %u2
MS_DEF_SPROG [j  c0] 副ﾌﾟﾛｸﾞﾗﾑの定義: %s1; 始めのISNは %u2
MS_LIB_MEMBER [   c0] library member %s1,
MS_LIB_MEMBER [j  c0] ﾗｲﾌﾞﾗﾘﾒﾝﾊﾞｰ %s1,
MS_COLUMN [   c0] column %d1,
MS_COLUMN [j  c0] ｶﾗﾑ %d1,
MS_IN [   c0] in %s1,
MS_IN [j  c0] in %s1,
MS_NO_EDITOR [ w c0] Unable to start editor
MS_NO_EDITOR [jw c0] ｴﾃﾞｨﾀｰが起動できません
MS_WATFOR_EXIT [ w c0] To exit, press Ctrl/Z and then press Enter
MS_WATFOR_EXIT [jw c0] Ctrl/Zを押して改行すると終了します。
MS_CANT_EXEC [ w c0] unable to execute command
MS_CANT_EXEC [jw c0] ｺﾏﾝﾄﾞが実行できません。
MS_STATISTICS [   c0] %d1 statements, %i2 bytes, %d3 extensions, %d4 warnings, %d5 errors
MS_STATISTICS [j  c0] %d1 ｽﾃｰﾄﾒﾝﾄ, %i2 ﾊﾞｲﾄ, %d3 拡張ﾒｯｾｰｼﾞ, %d4 警告ｴﾗｰ, %d5 ｴﾗｰ
MS_CODE_SIZE [   c0] Code size (in bytes):
MS_CODE_SIZE [j  c0] ｺｰﾄﾞｻｲｽﾞ (ﾊﾞｲﾄ):
MS_NUMBER_OF_ERRORS [   c0] Number of errors:
MS_NUMBER_OF_ERRORS [j  c0] ｴﾗｰ数:
MS_COMPILE_TIME [   c0] Compile time (in seconds):
MS_COMPILE_TIME [j  c0] ｺﾝﾊﾟｲﾙ時間 (秒):
MS_NUMBER_OF_WARNINGS [   c0] Number of warnings:
MS_NUMBER_OF_WARNINGS [j  c0] 警告ｴﾗｰ数:
MS_NUMBER_OF_STATEMENTS [ o c0] Number of statements compiled:
MS_NUMBER_OF_STATEMENTS [jo c0] ｺﾝﾊﾟｲﾙされた文の数:
MS_NUMBER_OF_EXTENSIONS [   c0] Number of extensions:
MS_NUMBER_OF_EXTENSIONS [j  c0] 拡張仕様の使用数:
MS_TRACE_INFO [   r0] - Executing line %u1 in file %s2
MS_TRACE_INFO [j  r0] - 実行ﾗｲﾝ %u1 ﾌｧｲﾙ %s2
MS_CALLED_FROM [   r0] - Called from line %u1 in file %s2
MS_CALLED_FROM [j  r0] - %s2 ﾌｧｲﾙ, %u1 行からの呼び出し
MS_ERR_WRT_LST [ w r0] error writing to listing file - %s1
MS_ERR_WRT_LST [jw r0] ﾘｽﾄﾌｧｲﾙへの書き込みｴﾗｰ - %s1
MS_EXEC_IN [ w r0] - Executing in
MS_EXEC_IN [jw r0] - 実行中 
MS_CALLED_FROM_LG [ w r0] - Called from
MS_CALLED_FROM_LG [jw r0] - 呼び出し 
MS_TB_LIBRARY [ w r0] %s1, statement %u2 in library member %s3
MS_TB_LIBRARY [jw r0] %s1, ｽﾃｰﾄﾒﾝﾄ %u2 ﾗｲﾌﾞﾗﾘ番号 %s3
MS_TB_FILE [ w r0] %s1, statement %u2 in file %s3
MS_TB_FILE [jw r0] %s1, ｽﾃｰﾄﾒﾝﾄ %u2 ﾌｧｲﾙ %s3
MS_INTERRUPT_PROMPT [ w r0] Do you really want to stop (y/n)?
MS_INTERRUPT_PROMPT [jw r0] 中止しますか (y/n)?
MS_NO_8087 [ w r0] *ERR* math coprocessor is not present\r\n
MS_NO_8087 [jw r0] *ERR* 数値演算ｺﾌﾟﾛｾｯｻがありません。\r\n
MS_DB_BAD_ISN [ w r0] invalid ISN
MS_DB_BAD_ISN [jw r0] ISNが無効です。
MS_DB_ENTERED [ w r0] Debugger entered at %s1
MS_DB_ENTERED [jw r0] %s1でﾃﾞﾊﾞｯｶﾞに入りました。
MS_DB_BREAK [ w r0] Breakpoint set at %d1
MS_DB_BREAK [jw r0] %d1にﾌﾞﾚｰｸﾎﾟｲﾝﾄを設定します。
MS_DB_RET_FROM [ w r0] Returned from %s1
MS_DB_RET_FROM [jw r0] %s1から戻りました。
MS_DB_TRACE_INSTR [ w r0] Into[I key] Over[space bar] Next[N key] Exit[Enter key]
MS_DB_TRACE_INSTR [jw r0] ｽﾃｯﾌﾟｲﾝ[I key] ｽﾃｯﾌﾟｵｰﾊﾞｰ[space bar] 次行[N key] 終了[Enter key]
MS_DB_VAR_NOT_FOUND [ w r0] variable not found
MS_DB_VAR_NOT_FOUND [jw r0] 変数が見つかりません。
MS_DB_ARR_NOT_DIMMED [ w r0] array dimensions not yet established
MS_DB_ARR_NOT_DIMMED [jw r0] 配列の次元が設定されていません。
MS_DB_SYNTAX_ERR [ w r0] not understood
MS_DB_SYNTAX_ERR [jw r0] ｼﾝﾀｯｸｽｴﾗｰです。
MS_DB_SSTR_BAD [ w r0] invalid substring list
MS_DB_SSTR_BAD [jw r0] 部分文字列が無効です。
MS_DB_SSCR_BAD [ w r0] invalid subscript list
MS_DB_SSCR_BAD [jw r0] 添え字が無効です。
MS_DB_BAD_UNIT [ w r0] unit not found
MS_DB_BAD_UNIT [jw r0] 装置が見つかりません。
MS_DB_NO_LISTING_FILE [ w r0] unable to open listing file
MS_DB_NO_LISTING_FILE [jw r0] ﾘｽﾄﾌｧｲﾙがｵｰﾌﾟﾝできません。
MS_DB_HELP_1 [ w r0] Breakpoint[/c] [<isn>].....show/set/clear breakpoint
MS_DB_HELP_1 [jw r0] Breakpoint[/c] [<isn>].....show/set/clear ﾌﾞﾚｰｸﾎﾟｲﾝﾄ
MS_DB_HELP_2 [ w r0] Display [*|<isn>[:<isn>]]..display source
MS_DB_HELP_2 [jw r0] Display [*|<isn>[:<isn>]]..ｿｰｽｺｰﾄﾞの表示
MS_DB_HELP_3 [ w r0] Go [<isn>].................go [until <isn> reached]
MS_DB_HELP_3 [jw r0] Go [<isn>].................実行 [<isn> まで]
MS_DB_HELP_4 [ w r0] Help.......................display this help screen
MS_DB_HELP_4 [jw r0] Help.......................ﾍﾙﾌﾟ画面の表示
MS_DB_HELP_5 [ w r0] Logio [<unit>].............list logged units or toggle logging of unit
MS_DB_HELP_5 [jw r0] Logio [<unit>].............指定装置の入出力記録の表示または記録のON/OFF指定
MS_DB_HELP_6 [ w r0] Quit.......................quit debugger
MS_DB_HELP_6 [jw r0] Quit.......................ﾃﾞﾊﾞｯｶﾞの終了
MS_DB_HELP_7 [ w r0] Trace......................enter trace mode
MS_DB_HELP_7 [jw r0] Trace......................ﾄﾚｰｽﾓｰﾄﾞの入力
MS_DB_HELP_8 [ w r0] Unit [<unit>]..............list information for connected unit(s)
MS_DB_HELP_8 [jw r0] Unit [<unit>]..............接続装置に関する情報の表示
MS_DB_HELP_9 [ w r0] Where......................display current traceback information
MS_DB_HELP_9 [jw r0] Where......................現在の実行経過情報の表示
MS_DB_HELP_10 [ w r0] ! [<cmd>]..................system services
MS_DB_HELP_10 [jw r0] ! [<cmd>]..................ｼｽﾃﾑｻｰﾋﾞｽ
MS_DB_HELP_11 [ w r0] / <x> <y>..................assign <y> to <x>
MS_DB_HELP_11 [jw r0] / <x> <y>..................<y> を <x>に割り当てる
MS_DB_HELP_12 [ w r0] ? [(z|a)] [<x>[,<x>]...]...print variables using specified format
MS_DB_HELP_12 [jw r0] ? [(z|a)] [<x>[,<x>]...]...書式仕様に従った変数情報の表示
MS_CTG_FILE_MANAGEMENT [ . c0] *File Management Options*
MS_CTG_FILE_MANAGEMENT [j. c0] *ﾌｧｲﾙﾏﾈｰｼﾞﾒﾝﾄｵﾌﾟｼｮﾝ*
MS_CTG_DIAGNOSTIC [ . c0] *Diagnostic Options*
MS_CTG_DIAGNOSTIC [j. c0] *診断ｵﾌﾟｼｮﾝ*
MS_CTG_DEBUGGING [ . c0] *Debugging Options*
MS_CTG_DEBUGGING [j. c0] *ﾃﾞﾊﾞｯｸﾞｵﾌﾟｼｮﾝ*
MS_CTG_MISC [ . c0] *Miscellaneous Options*
MS_CTG_MISC [j. c0] *その他のｵﾌﾟｼｮﾝ*
MS_CTG_FPUOPTS [ c c0] *Floating-Point Options*
MS_CTG_FPUOPTS [jc c0] *浮動小数点ｵﾌﾟｼｮﾝ*
MS_CTG_OPTIMIZATIONS [ c c0] *Optimizations*
MS_CTG_OPTIMIZATIONS [jc c0] *最適化*
MS_CTG_MEMORY_MODELS [ c c0] *Memory Models*
MS_CTG_MEMORY_MODELS [jc c0] *ﾒﾓﾘﾓﾃﾞﾙ*
MS_CTG_CPU [ c c0] *CPU Targets*
MS_CTG_CPU [jc c0] *CPU ﾀｰｹﾞｯﾄ*
CL_CTG_COMPILE_AND_LINK [ c c0] *Compile and Link Options*
CL_CTG_COMPILE_AND_LINK [jc c0] *ｺﾝﾊﾟｲﾙ/ﾘﾝｸｵﾌﾟｼｮﾝ*
CL_USAGE_LINE [ li 0] Usage: wfl {- or /<option> | <file>}
CL_USAGE_LINE [jli 0] 使用方法: wfl {- or /<option> | <file>}
CL_USAGE_LINE [ l3 0] Usage: wfl386 {- or /<option> | <file>}
CL_USAGE_LINE [jl3 0] 使用方法: wfl386 {- or /<option> | <file>}
CL_COMPILE_ONLY [ l  0] compile only, no linking
CL_COMPILE_ONLY [jl  0] ｺﾝﾊﾟｲﾙのみ, ﾘﾝｸは行わない
CL_IGNORE_WFL [ l  0] ignore WFL variable
CL_IGNORE_WFL [jl  0] WFL環境変数を無視
CL_RUN_PROTECT [ l  0] run protected mode compiler
CL_RUN_PROTECT [jl  0] ﾌﾟﾛﾃｸﾄﾓｰﾄﾞｺﾝﾊﾟｲﾗｰの実行
CL_GENERATE_MAP [ l  0] generate map file
CL_GENERATE_MAP [jl  0] ﾏｯﾌﾟﾌｧｲﾙの生成
CL_NAME_EXECUTABLE [ l  0] name executable file
CL_NAME_EXECUTABLE [jl  0] 実行ファイル名
CL_KEEP_DIRECTIVE [ l  0] keep directive file
CL_KEEP_DIRECTIVE [jl  0] 擬似命令ﾌｧｲﾙの保持
CL_INCLUDE_DIRECTIVE [ l  0] include directive file
CL_INCLUDE_DIRECTIVE [jl  0] 擬似命令ﾌｧｲﾙのｲﾝｸﾙｰﾄﾞ
CL_LINK_FOR_OS2 [ li 0] link for OS/2 protect-mode
CL_LINK_FOR_OS2 [jli 0] OS/2 ﾌﾟﾛﾃｸﾄﾓｰﾄﾞ用ﾘﾝｸ
CL_LINK_FOR_DOS [ li 0] link for DOS real-mode
CL_LINK_FOR_DOS [jli 0] DOS ﾘｱﾙﾓｰﾄﾞ用ﾘﾝｸ
CL_LINK_FOR_SYSTEM [ l  0] link for specified system
CL_LINK_FOR_SYSTEM [jl  0] 指定ｼｽﾃﾑ用ﾘﾝｸ
CL_STACK_SIZE [ l  0] set stack size
CL_STACK_SIZE [jl  0] ｽﾀｯｸｻｲｽﾞの設定
CL_LINKER_DIRECTIVES [ l  0] additional linker directives
CL_LINKER_DIRECTIVES [jl  0] ﾘﾝｶｰ擬似命令の追加
CL_OUT_OF_MEMORY [ l  0] Out of memory
CL_OUT_OF_MEMORY [jl  0] ﾒﾓﾘ不足
CL_ERROR_OPENING_TMP_FILE [ l  0] Error: Unable to open temporary file
CL_ERROR_OPENING_TMP_FILE [jl  0] ｴﾗｰ  : ﾃﾝﾎﾟﾗﾘﾌｧｲﾙがｵｰﾌﾟﾝできない
CL_UNABLE_TO_OPEN [ l  0] Unable to open '%s1'
CL_UNABLE_TO_OPEN [jl  0] '%s1'がｵｰﾌﾟﾝできない
CL_UNABLE_TO_INVOKE_COMPILER [ l  0] Error: Unable to invoke compiler
CL_UNABLE_TO_INVOKE_COMPILER [jl  0] ｴﾗｰ  : ｺﾝﾊﾟｲﾗｰが起動できない
CL_BAD_COMPILE [ l  0] Error: Compiler returned a bad status compiling '%s1'
CL_BAD_COMPILE [jl  0] ｴﾗｰ  : '%s1'ｺﾝﾊﾟｲﾙ中にｺﾝﾊﾟｲﾗが不正なｽﾃｰﾀｽを返した
CL_UNABLE_TO_INVOKE_LINKER [ l  0] Error: Unable to invoke linker
CL_UNABLE_TO_INVOKE_LINKER [jl  0] ｴﾗｰ  : ﾘﾝｶｰが起動できない
CL_BAD_LINK [ l  0] Error: Linker returned a bad status
CL_BAD_LINK [jl  0] ｴﾗｰ  : ﾘﾝｶｰが不正なｽﾃｰﾀｽを返した
CL_UNABLE_TO_FIND [ l  0] Error: Unable to find '%s1'
CL_UNABLE_TO_FIND [jl  0] ｴﾗｰ  : '%s1'が見つからない
MS_OPT_TRACE [ c c0] generate run-time traceback
MS_OPT_TRACE [jc c0] ﾗﾝﾀｲﾑﾄﾚｰｽﾊﾞｯｸの生成
MS_OPT_BOUNDS [ c c0] generate bounds checking
MS_OPT_BOUNDS [jc c0] 境界ﾁｪｯｸの生成
MS_CTG_DBCS [ . c0] *Character Set Options*
MS_CTG_DBCS [j. c0] *文字ｾｯﾄｵﾌﾟｼｮﾝ*
MS_OPT_CHINESE [ . c0] Chinese character set
MS_OPT_CHINESE [j. c0] 中国語文字ｾｯﾄ
MS_OPT_JAPANESE [ . c0] Japanese character set
MS_OPT_JAPANESE [j. c0] 日本語文字ｾｯﾄ
MS_OPT_KOREAN [ . c0] Korean character set
MS_OPT_KOREAN [j. c0] 韓国語文字ｾｯﾄ
MS_OPT_DEPENDENCY [ c c0] generate file dependencies
MS_OPT_DEPENDENCY [jc c0] ﾌｧｲﾙ依存性情報の生成
MS_CTG_APPTYPE [ . c0] *Application Type*
MS_CTG_APPTYPE [j. c0] *ｱﾌﾟﾘｹｰｼｮﾝﾀｲﾌﾟ*
MS_OPT_RESOURCES [ . c0] messages in resource file
MS_OPT_RESOURCES [j. c0] ﾘｿｰｽﾌｧｲﾙ内のﾒｯｾｰｼﾞ
MS_OPT_UNIT_6_CC [ . c0] devices are carriage control
MS_OPT_UNIT_6_CC [j. c0] 装置はｷｬﾘｯｼﾞ制御されます
MS_OPT_QUIET [ . c0] operate quietly
MS_OPT_QUIET [j. c0] 起動ﾒｯｾｰｼﾞを表示しない
MS_EXECUTION_TIME [ w c0] Execution time (in seconds):
MS_EXECUTION_TIME [jw c0] 実行時間 (秒):
MS_LOCAL_DATA_SIZE [ w c0] Local data size (in bytes):
MS_LOCAL_DATA_SIZE [jw c0] ﾛｰｶﾙﾃﾞｰﾀの大きさ (ﾊﾞｲﾄ):
MS_GLOBAL_DATA_SIZE [ w c0] Global data size (in bytes):
MS_GLOBAL_DATA_SIZE [jw c0] ｸﾞﾛｰﾊﾞﾙﾃﾞｰﾀの大きさ (ﾊﾞｲﾄ):
MS_BYTES_FREE [ w c0] Object bytes free:
MS_BYTES_FREE [jw c0] ｵﾌﾞｼﾞｪｸﾄ･ﾊﾞｲﾄ･ﾌﾘｰ:
MS_STATEMENTS_EXECUTED [ w c0] Number of statements executed:
MS_STATEMENTS_EXECUTED [jw c0] 実行される文の数:
MS_OPT_BROWSE [ . c0] generate browse information
MS_OPT_BROWSE [j. c0] ﾌﾞﾗｳｽﾞ情報の生成
MS_OPT_EXTEND_REAL [ . c0] extend float-pt. precision
MS_OPT_EXTEND_REAL [j. c0] 拡張浮動小数点数精度
MS_OZOPT_O_UNROLL [ c c0] loop optimizations/unrolling
MS_OZOPT_O_UNROLL [jc c0] ﾙｰﾌﾟ最適化/ｱﾝﾛｰﾘﾝｸﾞ
MS_CGOPT_ALIGN [ c c0] align COMMON segments
MS_CGOPT_ALIGN [jc c0] COMMONｾｸﾞﾒﾝﾄの整合
MS_OPT_LF_WITH_FF [ . c0] LF with FF
MS_OPT_LF_WITH_FF [j. c0] FF付きLF
MS_CGOPT_DI_WATCOM [ c c0] WATCOM debugging information
MS_CGOPT_DI_WATCOM [jc c0] WATCOMﾃﾞﾊﾞｯｸﾞ情報
MS_CGOPT_DI_DWARF [ c c0] DWARF debugging information
MS_CGOPT_DI_DWARF [jc c0] DWARFﾃﾞﾊﾞｯｸﾞ情報
MS_CGOPT_DI_CV [ . c0] Codeview debugging info.
MS_CGOPT_DI_CV [j. c0] Codeviewﾃﾞﾊﾞｯｸﾞ情報
CL_UNABLE_TO_INVOKE_CVPACK [ l  0] Error: Unable to invoke CVPACK
CL_UNABLE_TO_INVOKE_CVPACK [jl  0] ｴﾗｰ: CVPACKを起動できません
CL_BAD_CVPACK [ l  0] Error: CVPACK returned a bad status
CL_BAD_CVPACK [jl  0] Error: CVPACKが不正なｽﾃｰﾀｽを返しました
MS_OZOPT_O_FASTDO [ . c0] DO-variables do not overflow
MS_OZOPT_O_FASTDO [j. c0] DO変数がｵｰﾊﾞｰﾌﾛｰしません
MS_OPT_INCPATH [ . c0] path for INCLUDE files
MS_OPT_INCPATH [j. c0] INCLUDEﾌｧｲﾙ用のﾊﾟｽ
MS_CPUOPT_FPD [ c8c0] enable Pentium FDIV check
MS_CPUOPT_FPD [jc8c0] Pentium FDIV命令のﾁｪｯｸを有効にします
MS_OZOPT_O_BRANCH_PREDICTION [ c c0] branch prediction
MS_OZOPT_O_BRANCH_PREDICTION [jc c0] 分岐予測
MS_CGOPT_MANGLE [ c c0] mangle COMMON segment names
MS_CGOPT_MANGLE [jc c0] COMMONｾｸﾞﾒﾝﾄ名をﾏﾝｸﾞﾙ化します
MS_OPT_PROMOTE [ c c0] promote intrinsic arguments
MS_OPT_PROMOTE [jc c0] 組込み引数を格上げします
MS_OPT_COMMA_SEP [ c c0] allow comma separator
MS_OPT_COMMA_SEP [jc c0] ｺﾝﾏ区切りを有効にします
MS_CPUOPT_FP6 [ c8c0] /fp3 optimized for Pentium Pro
MS_CPUOPT_FP6 [jc8c0] Pentium Pro用に最適化された/fp3
MS_CPUOPT_80686 [ c8c0] /3 optimized for Pentium Pro
MS_CPUOPT_80686 [jc8c0] Pentium Pro用に最適化された/3
MS_OZOPT_O_SUPER_OPTIMAL [ c c0] optimize at expense of compile-time
MS_OZOPT_O_SUPER_OPTIMAL [jc c0] ｺﾝﾊﾟｲﾙ時間が長引いても最適化を強化します
MS_OZOPT_O_FLOW_REG_SAVES [ c c0] move register saves into flow graph
MS_OZOPT_O_FLOW_REG_SAVES [jc c0] ﾚｼﾞｽﾀ保存をﾌﾛｰｸﾞﾗﾌに移動します
MS_OZOPT_O_LOOP_INVAR [ c c0] loop optimizations/assume loop invariant float-pt. variables are initialized
MS_OZOPT_O_LOOP_INVAR [jc c0] ﾙｰﾌﾟ最適化/ﾙｰﾌﾟ内で浮動小数点数が不変であると仮定します。変数は初期化されます
