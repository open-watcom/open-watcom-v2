/**** Common messages ****/
pick( AS_MSG_ERROR,     "Error: ",
                "(J)Error: " )
pick( AS_MSG_WARNING,       "Warning: ",
                "(J)Warning: " )
pick( GET_STRING,       "%s", "%s" )
pick( CONST_OUT_OF_RANGE,   "constant out of range - truncated",
                "(J)constant out of range - truncated" )
pick( INTERNAL_ETBURN_FAILED,   "internal - unable to burn down expression tree",
                "(J)internal - unable to burn down expression tree" )
pick( AT_REG_USED_WITHOUT_REQ,  "used $at without \".set noat\"",
                "(J)used $at without \".set noat\"" )
pick( INS_USES_AT_REG,      "instruction requires $at with \".set noat\" in effect",
                "(J)instruction requires $at with \".set noat\" in effect" )
pick( MACRO_INSTRUCTION,    "this is a macro instruction",
                "(J)this is a macro instruction" )
pick( IMPROPERLY_FORMED_DIROP,  "improperly formed directive operand",
                "(J)improperly formed directive operand" )
pick( IMPROPERLY_FORMED_INSOP,  "improperly formed instruction operand",
                "(J)improperly formed instruction operand" )
pick( IMPROPER_DIROP,       "improper pseudo operand #%d",
                "(J)improper pseudo operand #%d" )
pick( OP_OUT_OF_RANGE,      "operand #%d out of range",
                "(J)operand #%d out of range" )
pick( DIROP_ERR_MISSING,    "missing or improper pseudo operand #%d",
                "(J)missing or improper pseudo operand #%d" )
pick( DIROP_ERR_TOO_MANY,   "too many pseudo operand(s)",
                "(J)too many pseudo operand(s)" )
pick( IMPROPER_SET_DIRECTIVE,   "improper .set option: \"%s\"",
                "(J)improper .set option: \"%s\"" )
pick( SYM_ALREADY_DEFINED,  "symbol '%s' is previously defined",
                "(J)symbol '%s' is previously defined" )
pick( DIRECTIVE_NOT_SUPPORTED,  "directive not supported: '%s'",
                "(J)directive not supported: '%s'" )
pick( INTERNAL_UNKNOWN_DT_PARM, "internal - unknown dirFunc parameter: %d",
                "(J)internal - unknown dirFunc parameter: %d" )
pick( ILLEGAL_UNARY_EXPR,   "illegal unary expression",
                "(J)illegal unary expression" )
pick( UNEXPECTED_UNARY_CLASS,   "unexpected class of unary node",
                "(J)unexpected class of unary node" )
pick( ILLEGAL_BINARY_EXPR,  "illegal binary expression",
                "(J)illegal binary expression" )
pick( UNEXPECTED_BINARY_CLASS,  "unexpected class of binary node",
                "(J)unexpected class of binary node" )
pick( UNMATCHED_HIGH_RELOC, "unmatched 'h^%s' reloc found",
                "(J)unmatched 'h^%s' reloc found" )
pick( UNRESOLVED_BACK_NUMREF,   "unresolved backward numeric label reference",
                "(J)unresolved backward numeric label reference" )
pick( NON_EXISTANT_FORWARD_REF, "non-existant forward reference(s) to numeric label #%d",
                "(J)non-existant forward reference(s) to numeric label #%d" )
pick( INVALID_RELOC_MODIFIER,   "invalid relocatable modifier",
                "(J)invalid relocatable modifier" )
pick( IMPROPER_OPERAND,     "improper operand #%d",
                "(J)improper operand #%d" )
pick( RELOC_NOT_ALLOWED,    "relocatable constant not allowed in operand #%d",
                "(J)relocatable constant not allowed in operand #%d" )
pick( OPERAND_INCORRECT,    "operand #%d is incorrect",
                "(J)operand #%d is incorrect" )
pick( OPERAND_UNEXPECTED,   "operand #%d is unexpected",
                "(J)operand #%d is unexpected" )
pick( NOT_ENOUGH_INSOP,     "not enough operands for instruction",
                "(J)not enough operands for instruction" )
pick( MAX_NUMOP_EXCEEDED,   "exceeded the maximum number of operands",
                "(J)exceeded the maximum number of operands" )
pick( SYNTAX_ERROR,     "syntax error",
                "(J)syntax error" )
pick( PROBLEM_AT_EOL,       "%s at EOL",
                "(J)%s at EOL" )
pick( PROBLEM_AT_EOS,       "%s at the end of statement",
                "(J)%s at the end of statement" )
pick( PROBLEM_AT_WHERE,     "%s at '%s'",
                "(J)%s at '%s'" )
#if defined( AS_ALPHA )
pick( INVALID_PALCODE,      "invalid PALcode",
                "(J)invalid PALcode" )
#elif defined( AS_PPC )
pick( BAD_REG_ENCODING,     "operand #%d has bad %s encoding",
                "(J)operand #%d has bad %s encoding" )
#endif

#ifdef _STANDALONE_
/**** Standalone assembler messages ****/
pick( GLOBL_DECL_OUT_OF_ORDER,  "out-of-order global declaration; symbol still static",
                "(J)out-of-order global declaration; symbol still static" )
pick( NO_FILENAME_SPECIFIED,    "no file name specified",
                "(J)no file name specified" )
pick( UNABLE_TO_OPEN,       "Unable to open '%s'",
                "(J)Unable to open '%s'" )
pick( UNABLE_TO_CREATE,     "Unable to create '%s'",
                "(J)Unable to create '%s'" )
pick( INVALID_OPTION,       "invalid option %s",
                "(J)invalid option %s" )
pick( ABORT_PARSE,      "too many errors - aborted",
                "(J)too many errors - aborted" )
pick( AS_ERROR_REPORT,      "%s: %d lines, %d warnings, %d errors\n",
                "(J)%s: %d lines, %d warnings, %d errors\n" )
pick( INTERNAL_UNKNOWN_SECTYPE, "internal - unknown section type",
                "(J)internal - unknown section type" )
pick( INVALID_SECT_ATTR,    "invalid section attribute '%c'",
                "(J)invalid section attribute '%c'" )
pick( SEC_ATTR_CONFLICT,    "section attributes conflict with previous definition",
                "(J)section attributes conflict with previous definition" )
pick( INVALID_BSS_DIRECTIVE,    "directive '%s' invalid within an uninitialized data section",
                "(J)directive '%s' invalid within an uninitialized data section" )
pick( INVALID_BSS_STATEMENT,    "statement invalid within an uninitialized data section",
                "(J)statement invalid within an uninitialized data section" )
pick( INVALID_RELOC_SYM,    "'%s' is not a valid relocatable symbol",
                "(J)'%s' is not a valid relocatable symbol" )
pick( IMPROPER_LINE_DIRECTIVE,  "improper #line preprocessing directive",
                "(J)improper #line preprocessing directive" )
#else
/**** Inline assembler messages ****/
pick( SYMBOL_NOT_DECLARED,  "symbol '%s' has not been declared",
                "(J)symbol '%s' has not been declared" )
pick( CANNOT_JUMP_TO_STACKVAR,  "cannot transfer control to a stack variable",
                "(J)cannot transfer control to a stack variable" )
pick( BAD_BASE_REG_FOR_STACKVAR,"base register unexpected for the stack variable",
                "(J)base register unexpected for the stack variable" )
pick( OFFSET_OUT_OF_RANGE,  "offset out of range",
                "(J)offset out of range" )
pick( ABS_REF_NOT_ALLOWED,  "absolute reference to '%s' not allowed",
                "(J)absolute reference to '%s' not allowed" )
#endif

#ifdef _STANDALONE_
/**** Usage messages - MUST GO LAST!!! ****/
#if defined( AS_ALPHA )
pick( USAGE_1,          "Usage: wasaxp {options} {asm_files}",
                "(J)Usage: wasaxp {options} {asm_files}" )
#elif defined( AS_PPC )
pick( USAGE_1,          "Usage: wasppc {options} {asm_files}",
                "(J)Usage: wasppc {options} {asm_files}" )
#elif defined( AS_MIPS )
pick( USAGE_1,          "Usage: wasmips {options} {asm_files}",
                "(J)Usage: wasmips {options} {asm_files}" )
#else
#error unknown AS_CPU
#endif
pick( USAGE_2,          "Options:          (/option is also supported)",
                "(J)Options:          (/option is also supported)" )
pick( USAGE_3,          "-d<name>[=text]   define text macro",
                "(J)-d<name>[=text]   define text macro" )
pick( USAGE_4,          "-e<number>        set error limit number",
                "(J)-e<number>        set error limit number" )
pick( USAGE_5,          "-fo=<filename>    set output filename (applies to the first asm_file)",
                "(J)-fo=<filename>    set output filename (applies to the first asm_file)" )
pick( USAGE_6,          "-h or -?          print this message",
                "(J)-h or -?          print this message" )
pick( USAGE_7,          "-i=<path>         set include path",
                "(J)-i=<path>         set include path" )
pick( USAGE_8,          "-o{e,c}           object file format (ELF,COFF)",
                "(J)-o{e,c}           object file format (ELF,COFF)" )
pick( USAGE_9,          "-q or -zq         operate quietly",
                "(J)-q or -zq         operate quietly" )
pick( USAGE_10,         "-we               treat all warnings as errors",
                "(J)-we               treat all warnings as errors" )
#ifdef AS_DEBUG_DUMP
pick( USAGE_11,         "DEBUG options:",
                "DEBUG options:" )
pick( USAGE_12,         "-v{i,l,p,s,t}     view (instruction,lex buffer,parse,symbols,ins table)",
                "-v{i,l,p,s,t}     view (instruction,lex buffer,parse,symbols,ins table)" )
#endif
pick( USAGE_LAST,        ".", "." )
#endif  // _STANDALONE_
