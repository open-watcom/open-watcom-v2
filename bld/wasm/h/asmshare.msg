pick( SIZE_DOESNT_MATCH_WITH_PREVIOUS_DEFINITION,
                                "Size doesn't match with previous definition" ,
                                "前の定義とサイズが一致していません" )
pick( INVALID_INSTRUCTION_WITH_CURRENT_CPU_SETTING,
                                "Invalid instruction with current CPU setting" ,
                                "現在のCPUの設定に対して無効な命令です" )
pick( LOCK_PREFIX_IS_NOT_ALLOWED_ON_THIS_INSTRUCTION,
                                "LOCK prefix is not allowed on this instruction" ,
                                "この命令に対して LOCK プレフィックスは使用できません" )
pick( REP_PREFIX_IS_NOT_ALLOWED_ON_THIS_INSTRUCTION,
                                "REP prefix is not allowed on this instruction" ,
                                "この命令に対して REP プレフィックスは使用できません" )
pick( INVALID_MEMORY_POINTER,
                                "Invalid memory pointer" ,
                                "無効なメモリポインタです" )
pick( CANNOT_USE_386_ADDRESSING_MODE_WITH_CURRENT_CPU_SETTING,
                                "Cannot use 386 addressing mode with current CPU setting" ,
                                "現在のCPUの設定では386アドレスを使用できません" )
pick( TOO_MANY_BASE_REGISTERS,
                                "Too many base registers" ,
                                "ベースレジスタが多すぎます" )
pick( INVALID_INDEX_REGISTER,
                                "Invalid index register" ,
                                "インデックスレジタが無効です" )
pick( SCALE_FACTOR_MUST_BE_1_2_4_OR_8,
                                "Scale factor must be 1, 2, 4 or 8" ,
                                "スケールファクタは1,2,4,8のいずれかでなければなりません" )
pick( INVALID_ADDRESSING_MODE_WITH_CURRENT_CPU_SETTING,
                                "invalid addressing mode with current CPU setting" ,
                                "現在のCPUの設定に対して無効なアドレスモードの指定です" )
pick( ESP_CANNOT_BE_USED_AS_INDEX,
                                "ESP cannot be used as index" ,
                                "ESP はインデックスとしては使用できません" )
pick( TOO_MANY_BASE_INDEX_REGISTERS,
                                "Too many base/index registers" ,
                                "ベース／インデックス レジスタが多すぎます" )
pick( MEMORY_OFFSET_CANNOT_REFERENCE_TO_MORE_THAN_ONE_LABEL,
                                "Memory offset cannot reference to more than one label" ,
                                "メモリオフセットは２個以上のラベルへの参照を行えません" )
pick( OFFSET_MUST_BE_RELOCATABLE,
                                "Offset must be relocatable" ,
                                "オフセットはリロケｰタブルでなければなりません" )
pick( MEMORY_OFFSET_EXPECTED,
                                "Memory offset expected" ,
                                "メモリオフセットでなければなりません" )
pick( INVALID_INDIRECT_MEMORY_OPERAND,
                                "Invalid indirect memory operand" ,
                                "無効な間接メモリオペランドです" )
pick( CANNOT_MIX_16_AND_32_BIT_REGISTERS,
                                "Cannot mix 16 and 32-bit registers" ,
                                "16ビットレジスタと32ビットレジスタは混在できません" )
pick( CPU_TYPE_ALREADY_SET,
                                "CPU type already set" ,
                                "CPUのタイプは既に設定されています" )
pick( UNKNOWN_DIRECTIVE,
                                "Unknown directive" ,
                                "無効なディレクティブです" )
pick( EXPECTING_COMMA,
                                "Expecting comma" ,
                                "コンマが必要です" )
pick( EXPECTING_NUMBER,
                                "Expecting number" ,
                                "数字が必要です" )
pick( INVALID_LABEL_DEFINITION,
                                "Invalid label definition" ,
                                "無効なラベル定義です" )
pick( INVALID_USE_OF_SHORT_NEAR_FAR_OPERATOR,
                                "Invalid use of SHORT, NEAR, FAR operator" ,
                                "SHORT, NEAR, FARオペレータの使用方法が無効です" )
pick( NO_MEMORY,
                                "No memory" ,
                                "メモリがありません" )
pick( CANNOT_USE_386_SEGMEMT_REGISTER_WITH_CURRENT_CPU_SETTING,
                                "Cannot use 386 segment register with current CPU setting" ,
                                "現在のCPUの設定では386セグメントレジスタは使用できません" )
pick( POP_CS_IS_NOT_ALLOWED,
                                "POP CS is not allowed" ,
                                "POP CSはできません" )
pick( CANNOT_USE_386_REGISTER_WITH_CURRENT_CPU_SETTING,
                                "Cannot use 386 register with current CPU setting" ,
                                "現在のCPUの設定では386レジスタは使用できません" )
pick( ONLY_MOV_CAN_USE_SPECIAL_REGISTER,
                                "Only MOV can use special register" ,
                                "特別レジスタは MOV 命令でのみ使用できます" )
pick( CANNOT_USE_TR3_TR4_TR5_IN_CURRENT_CPU_SETTING,
                                "Cannot use TR3, TR4, TR5 in current CPU setting" ,
                                "現在のCPUの設定では TR3, TR4, TR5 は使用できません" )
pick( CANNOT_USE_SHORT_WITH_CALL,
                                "Cannot use SHORT with CALL" ,
                                "CALL命令と共に SHORT を使うことはできません" )
pick( ONLY_SHORT_DISPLACEMENT_IS_ALLOWED,
                                "Only SHORT displacement is allowed" ,
                                "SHORTディスプレースメントのみ可能です" )
pick( SYNTAX_ERROR,
                                "Syntax error" ,
                                "構文エラー" )
pick( PREFIX_MUST_BE_FOLLOWED_BY_AN_INSTRUCTION,
                                "Prefix must be followed by an instruction" ,
                                "プレフィックスの後には命令がなければなりません" )
pick( NO_SIZE_GIVEN_BEFORE_PTR_OPERATOR,
                                "No size given before 'PTR' operator" ,
                                "PTRオペレータの前にサイズ指定がありません" )
pick( INVALID_IMUL_FORMAT,
                                "Invalid IMUL format" ,
                                "IMUL命令の使用形式が不適切です" )
pick( INVALID_SHLD_SHRD_FORMAT,
                                "Invalid SHLD/SHRD format" ,
                                "SHLD/SHRD命令の使用形式が不適切です" )
pick( TOO_MANY_COMMAS,
                                "Too many commas" ,
                                "コンマ(,)が多すぎます" )
pick( SYNTAX_ERROR_UNEXPECTED_COLON,
                                "Syntax error: Unexpected colon" ,
                                "構文エラー: 不適切なコロン(:)があります" )
pick( OPERANDS_MUST_BE_THE_SAME_SIZE,
                                "Operands must be the same size" ,
                                "オペランドは同じサイズでなければなりません" )
pick( INVALID_INSTRUCTION_OPERANDS,
                                "Invalid instruction operands" ,
                                "無効な命令オペランドです" )
pick( IMMEDIATE_CONSTANT_TOO_LARGE,
                                "Immediate constant too large" ,
                                "即値定数が大きすぎます" )
pick( CANNOT_USE_SHORT_OR_NEAR,
                                "Can not use short or near modifiers with this instruction" ,
                                "この命令と一緒にshortまたはnearを使用することはできません" )
pick( JUMP_OUT_OF_RANGE,
                                "Jump out of range" ,
                                "範囲外へのジャンプです" )
pick( DISPLACEMENT_OUT_OF_RANGE,
                                "Displacement cannot be larger than 32k" ,
                                "ディスプレースメントが32Kを越えています" )
pick( INITIALIZER_OUT_OF_RANGE,
                                "Initializer value too large" ,
                                "設定されている初期値が大きすぎます" )
pick( SYMBOL_ALREADY_DEFINED,
                                "Symbol already defined" ,
                                "このシンボルは既に定義されています" )
pick( IMMEDIATE_DATA_TOO_BIG,
                                "Immediate data too large" ,
                                "即値データが大きすぎます" )
pick( IMMEDIATE_DATA_OUT_OF_RANGE,
                                "Immediate data out of range" ,
                                "即値データの値が範囲外です" )
pick( NO_JUMP_TO_AUTO,
                                "Can not transfer control to stack symbol" ,
                                "スタックシンボルへ制御を移すことはできません" )
pick( OFFSET_TOO_SMALL,
                                "Offset cannot be smaller than WORD size" ,
                                "オフセットはワード(2バイト)より小さくできません" )
pick( CANNOT_OFFSET_AUTO,
                                "Can not take offset of stack symbol" ,
                                "スタックシンボルのオフセットを取ることはできません" )
pick( CANNOT_SEG_AUTO,
                                "Can not take segment of stack symbol" ,
                                "スタック・シンボルのセグメントは取れません" )
pick( SEGMENT_TOO_BIG,
                                "Segment too large" ,
                                "セグメントが大きすぎます" )
pick( OFFSET_TOO_BIG,
                                "Offset cannot be larger than 32k" ,
                                "オフセットは32Kより大きくできません" )
pick( OP_2_TOO_BIG,
                                "Operand 2 too big" ,
                                "オペランド2が大きすぎます" )
pick( OP_1_TOO_SMALL,
                                "Operand 1 too small" ,
                                "オペランド1が小さすぎます" )
pick( TOO_MANY_OPERATOR,
                                "Too many arithmetic operators" ,
                                "算術計算の演算子が多すぎます" )
pick( TOO_MANY_OPEN_SQUARE_BRACKETS,
                                "Too many open square brackets" ,
                                "左カギ括弧([)が多すぎます" )
pick( TOO_MANY_CLOSE_SQUARE_BRACKETS,
                                "Too many close square brackets" ,
                                "右カギ括弧(])が多すぎます" )
pick( TOO_MANY_OPEN_BRACKETS,
                                "Too many open brackets" ,
                                "左括弧'('が多すぎます" )
pick( TOO_MANY_CLOSE_BRACKETS,
                                "Too many close brackets" ,
                                "右括弧')'が多すぎます" )
pick( INVALID_NUMBER_DIGIT,
                                "Invalid number digit" ,
                                "不適切な数値です" )
pick( ASM_CODE_TOO_LONG,
                                "Assembler Code is too long" ,
                                "アセンブラコードが長すぎます" )
pick( BRACKETS_NOT_BALANCED,
                                "Brackets are not balanced" ,
                                "左右の括弧の数が合いません" )
pick( OPERATOR_EXPECTED,
                                "Operator is expected" ,
                                "演算子がなければなりません" )
pick( OPERAND_EXPECTED,
                                "Operand is expected" ,
                                "オペランドでなければなりません" )
pick( TOO_MANY_TOKENS,
                                "Too many tokens in a line" ,
                                "1行にあるトークンが多すぎます" )
pick( BRACKET_EXPECTED,
                                "Bracket is expected" ,
                                "括弧がなければなりません" )
pick( ILLEGAL_USE_OF_REGISTER,
                                "Illegal use of register" ,
                                "レジスタの使用方法が間違っています" )
pick( ILLEGAL_USE_OF_LABEL,
                                "Illegal use of label" ,
                                "ラベルの使用方法が間違っています" )
pick( INVALID_OPERAND_IN_ADDITION,
                                "Invalid operand in addition" ,
                                "加算命令のオペランドが無効です" )
pick( INVALID_OPERAND_IN_SUBTRACTION,
                                "Invalid operand in subtraction" ,
                                "減算命令のオペランドが無効です" )
pick( ONE_OPERAND_MUST_BE_CONSTANT,
                                "One operand must be constant" ,
                                "一方のオペランドは定数でなければなりません" )
pick( CONSTANT_EXPECTED,
                                "Constant operand is expected" ,
                                "定数オペランドでなければなりません" )
pick( ADDITION_CONSTANT_EXPECTED,
                                "A constant operand is expected in addition" ,
                                "加算命令には定数オペランドがなければなりません" )
pick( SUBTRACTION_CONSTANT_EXPECTED,
                                "A constant operand is expected in subtraction" ,
                                "減算命令には定数オペランドがなければなりません" )
pick( MULTIPLICATION_CONSTANT_EXPECTED,
                                "A constant operand is expected in multiplication" ,
                                "乗算命令には定数オペランドがなければなりません" )
pick( DIVISION_CONSTANT_EXPECTED,
                                "A constant operand is expected in division" ,
                                "除算命令には定数オペランドがなければなりません" )
pick( POSITIVE_SIGN_CONSTANT_EXPECTED,
                                "A constant operand is expected after a positive sign" ,
                                "正の符号(+)の後には定数オペランドがなければなりません" )
pick( NEGATIVE_SIGN_CONSTANT_EXPECTED,
                                "A constant operand is expected after a negative sign" ,
                                "負の符号(-)の後には定数オペランドがなければなりません" )
pick( LABEL_NOT_DEFINED,
                                "Label is not defined" ,
                                "ラベルは定義されていません" )
pick( MORE_THAN_ONE_OVERRIDE,
                                "More than one override" ,
                                "2個以上のオーバーライドがあります" )
pick( LABEL_IS_EXPECTED,
                                "Label is expected" ,
                                "ラベルがなければなりません" )
pick( ONLY_SEG_OR_GROUP_ALLOWED,
                                "Only segment or group label is allowed" ,
                                "セグメントあるいはグループのラベルのみが許されます" )
pick( REG_OR_LABEL_EXPECTED_IN_OVERRIDE,
                                "Only register or label is expected in override" ,
                                "オーバーライドされるのはレジスタまたはラベルのみです" )
pick( UNEXPECTED_END_OF_FILE,
                                "Unexpected end of file" ,
                                "途中でファイルが終わりました" )
pick( LABEL_TOO_LONG,
                                "Label is too long" ,
                                "ラベルが長すぎます" )
pick( NOT_IMPLEMENTED,
                                "This feature has not been implemented yet" ,
                                "この機能はまだ使用できません" )
pick( INTERNAL_ERROR_1,
                                "Internal Error #1" ,
                                "内部エラー #1" )
pick( CANNOT_OFFSET_GRP,
                                "Can not take offset of group" ,
                                "グループのオフセットは取れません" )
pick( CANNOT_OFFSET_SEG,
                                "Can not take offset of segment" ,
                                "セグメントのオフセットは取れません" )
pick( INVALID_CHARACTER,
                                "Invalid character found" ,
                                "使用できない文字があります" )
pick( INVALID_SIZE,
                                "Invalid operand size for instruction" ,
                                "命令に対してオペランドの大きさが不適切です" )
pick( NOT_SUPPORTED,
                                "This instruction is not supported" ,
                                "この命令はサポートされていません" )
pick( ASSUMING_BYTE,
                                "size not specified -- BYTE PTR is assumed" ,
                                "大きさが指定されていません -- BYTE PTRを仮定します" )
pick( ASSUMING_WORD,
                                "size not specified -- WORD PTR is assumed" ,
                                "大きさが指定されていません -- WORD PTRを仮定します" )
pick( ASSUMING_DWORD,
                                "size not specified -- DWORD PTR is assumed" ,
                                "大きさが指定されていません -- DWORD PTRを仮定します" )
