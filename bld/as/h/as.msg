/**** Common messages ****/
PICK( AS_MSG_ERROR,		"Error: ",
				"(J)Error: " )
PICK( AS_MSG_WARNING,		"Warning: ",
				"(J)Warning: " )
PICK( GET_STRING,		"%s", "%s" )
PICK( CONST_OUT_OF_RANGE,	"constant out of range - truncated",
				"(J)constant out of range - truncated" )
PICK( INTERNAL_ETBURN_FAILED,	"internal - unable to burn down expression tree",
				"(J)internal - unable to burn down expression tree" )
PICK( AT_REG_USED_WITHOUT_REQ,	"used $at without \".set noat\"",
				"(J)used $at without \".set noat\"" )
PICK( INS_USES_AT_REG,		"instruction requires $at with \".set noat\" in effect",
				"(J)instruction requires $at with \".set noat\" in effect" )
PICK( MACRO_INSTRUCTION,	"this is a macro instruction",
				"(J)this is a macro instruction" )
PICK( IMPROPERLY_FORMED_DIROP,	"improperly formed directive operand",
				"(J)improperly formed directive operand" )
PICK( IMPROPERLY_FORMED_INSOP,	"improperly formed instruction operand",
				"(J)improperly formed instruction operand" )
PICK( IMPROPER_DIROP,		"improper pseudo operand #%d",
				"(J)improper pseudo operand #%d" )
PICK( OP_OUT_OF_RANGE,		"operand #%d out of range",
				"(J)operand #%d out of range" )
PICK( DIROP_ERR_MISSING,	"missing or improper pseudo operand #%d",
				"(J)missing or improper pseudo operand #%d" )
PICK( DIROP_ERR_TOO_MANY,	"too many pseudo operand(s)",
				"(J)too many pseudo operand(s)" )
PICK( IMPROPER_SET_DIRECTIVE,	"improper .set option: \"%s\"",
				"(J)improper .set option: \"%s\"" )
PICK( SYM_ALREADY_DEFINED,	"symbol '%s' is previously defined",
				"(J)symbol '%s' is previously defined" )
PICK( DIRECTIVE_NOT_SUPPORTED,	"directive not supported: '%s'",
				"(J)directive not supported: '%s'" )
PICK( INTERNAL_UNKNOWN_DT_PARM,	"internal - unknown dirFunc parameter: %d",
				"(J)internal - unknown dirFunc parameter: %d" )
PICK( ILLEGAL_UNARY_EXPR,	"illegal unary expression",
				"(J)illegal unary expression" )
PICK( UNEXPECTED_UNARY_CLASS,	"unexpected class of unary node",
				"(J)unexpected class of unary node" )
PICK( ILLEGAL_BINARY_EXPR,	"illegal binary expression",
				"(J)illegal binary expression" )
PICK( UNEXPECTED_BINARY_CLASS,	"unexpected class of binary node",
				"(J)unexpected class of binary node" )
PICK( UNMATCHED_HIGH_RELOC,	"unmatched 'h^%s' reloc found",
				"(J)unmatched 'h^%s' reloc found" )
PICK( UNRESOLVED_BACK_NUMREF,	"unresolved backward numeric label reference",
				"(J)unresolved backward numeric label reference" )
PICK( NON_EXISTANT_FORWARD_REF,	"non-existant forward reference(s) to numeric label #%d",
				"(J)non-existant forward reference(s) to numeric label #%d" )
PICK( INVALID_RELOC_MODIFIER,	"invalid relocatable modifier",
				"(J)invalid relocatable modifier" )
PICK( IMPROPER_OPERAND,		"improper operand #%d",
				"(J)improper operand #%d" )
PICK( RELOC_NOT_ALLOWED,	"relocatable constant not allowed in operand #%d",
				"(J)relocatable constant not allowed in operand #%d" )
PICK( OPERAND_INCORRECT,	"operand #%d is incorrect",
				"(J)operand #%d is incorrect" )
PICK( OPERAND_UNEXPECTED,	"operand #%d is unexpected",
				"(J)operand #%d is unexpected" )
PICK( NOT_ENOUGH_INSOP,		"not enough operands for instruction",
				"(J)not enough operands for instruction" )
PICK( MAX_NUMOP_EXCEEDED,	"exceeded the maximum number of operands",
				"(J)exceeded the maximum number of operands" )
PICK( SYNTAX_ERROR,		"syntax error",
				"(J)syntax error" )
PICK( PROBLEM_AT_EOL,		"%s at EOL",
				"(J)%s at EOL" )
PICK( PROBLEM_AT_EOS,		"%s at the end of statement",
				"(J)%s at the end of statement" )
PICK( PROBLEM_AT_WHERE,		"%s at '%s'",
				"(J)%s at '%s'" )
#if defined( AS_ALPHA )
PICK( INVALID_PALCODE,		"invalid PALcode",
				"(J)invalid PALcode" )
#elif defined( AS_PPC )
PICK( BAD_REG_ENCODING,		"operand #%d has bad %s encoding",
				"(J)operand #%d has bad %s encoding" )
#endif
				
#ifdef _STANDALONE_
/**** Standalone assembler messages ****/
PICK( GLOBL_DECL_OUT_OF_ORDER,	"out-of-order global declaration; symbol still static",
				"(J)out-of-order global declaration; symbol still static" )
PICK( NO_FILENAME_SPECIFIED,	"no file name specified",
				"(J)no file name specified" )
PICK( UNABLE_TO_OPEN,		"Unable to open '%s'",
				"(J)Unable to open '%s'" )
PICK( UNABLE_TO_CREATE,		"Unable to create '%s'",
				"(J)Unable to create '%s'" )
PICK( INVALID_OPTION,		"invalid option %s",
				"(J)invalid option %s" )
PICK( ABORT_PARSE,		"too many errors - aborted",
				"(J)too many errors - aborted" )
PICK( AS_ERROR_REPORT,		"%s: %d lines, %d warnings, %d errors",
				"(J)%s: %d lines, %d warnings, %d errors" )
PICK( INTERNAL_UNKNOWN_SECTYPE, "internal - unknown section type",
				"(J)internal - unknown section type" )
PICK( INVALID_SECT_ATTR,	"invalid section attribute '%c'",
				"(J)invalid section attribute '%c'" )
PICK( SEC_ATTR_CONFLICT,	"section attributes conflict with previous definition",
				"(J)section attributes conflict with previous definition" )
PICK( INVALID_BSS_DIRECTIVE,	"directive '%s' invalid within an uninitialized data section",
				"(J)directive '%s' invalid within an uninitialized data section" )
PICK( INVALID_BSS_STATEMENT,	"statement invalid within an uninitialized data section",
				"(J)statement invalid within an uninitialized data section" )
PICK( INVALID_RELOC_SYM,	"'%s' is not a valid relocatable symbol",
				"(J)'%s' is not a valid relocatable symbol" )
PICK( IMPROPER_LINE_DIRECTIVE,	"improper #line preprocessing directive",
				"(J)improper #line preprocessing directive" )
#else
/**** Inline assembler messages ****/
PICK( SYMBOL_NOT_DECLARED,	"symbol '%s' has not been declared",
				"(J)symbol '%s' has not been declared" )
PICK( CANNOT_JUMP_TO_STACKVAR,	"cannot transfer control to a stack variable",
				"(J)cannot transfer control to a stack variable" )
PICK( BAD_BASE_REG_FOR_STACKVAR,"base register unexpected for the stack variable",
				"(J)base register unexpected for the stack variable" )
PICK( OFFSET_OUT_OF_RANGE,	"offset out of range",
				"(J)offset out of range" )
PICK( ABS_REF_NOT_ALLOWED,	"absolute reference to '%s' not allowed",
				"(J)absolute reference to '%s' not allowed" )
#endif
				
#ifdef _STANDALONE_			       
/**** Usage messages - MUST GO LAST!!! ****/
#if defined( AS_ALPHA )
PICK( USAGE_1,			"Usage: wasaxp {options} {asm_files}",
				"(J)Usage: wasaxp {options} {asm_files}" )
#elif defined( AS_PPC )
PICK( USAGE_1,			"Usage: wasppc {options} {asm_files}",
				"(J)Usage: wasppc {options} {asm_files}" )
#else
#error unknown AS_CPU
#endif
PICK( USAGE_2,			"Options:          (/option is also supported)",
				"(J)Options:          (/option is also supported)" )
PICK( USAGE_3,			"-d<name>[=text]   define text macro",
				"(J)-d<name>[=text]   define text macro" )
PICK( USAGE_4,			"-e<number>        set error limit number",
				"(J)-e<number>        set error limit number" )
PICK( USAGE_5,			"-fo=<filename>    set output filename (applies to the first asm_file)",
				"(J)-fo=<filename>    set output filename (applies to the first asm_file)" )
PICK( USAGE_6,			"-h or -?          print this message",
				"(J)-h or -?          print this message" )
PICK( USAGE_7,			"-i=<path>         set include path",
				"(J)-i=<path>         set include path" )
PICK( USAGE_8,			"-o{e,c}           object file format (ELF,COFF)",
				"(J)-o{e,c}           object file format (ELF,COFF)" )
PICK( USAGE_9,			"-q or -zq         operate quietly",
				"(J)-q or -zq         operate quietly" )
PICK( USAGE_10,			"-we               treat all warnings as errors",
				"(J)-we               treat all warnings as errors" )
#ifndef NDEBUG
PICK( USAGE_11,			"DEBUG options:",
				"DEBUG options:" )
PICK( USAGE_12,			"-v{i,l,p,s,t}     view (instruction,lex buffer,parse,symbols,ins table)",
				"-v{i,l,p,s,t}     view (instruction,lex buffer,parse,symbols,ins table)" )
#endif
#endif	// _STANDALONE_
