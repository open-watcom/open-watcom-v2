pick( RERR_MIN,                   "RERR_MIN",
                                  "JAPAN_RERR_MIN")

pick( RERR_INV_CHAR,              "Invalid character encountered",
                                  "JAPAN_RERR_INV_CHAR")

pick( RERR_INV_PREPROCESSOR,      "Invalid preprocessor directive",
                                  "JAPAN_RERR_INV_PREPROCESSOR")

pick( RERR_EOF_IN_COMMENT,        "EOF encountered inside a comment",
                                  "JAPAN_RERR_EOF_IN_COMMENT")

pick( RERR_EOF_IN_STRING,         "EOF encountered inside a string",
                                  "JAPAN_RERR_EOF_IN_STRING")

pick( RERR_INV_CHAR_CONST,        "Invalid char constant",
                                  "JAPAN_RERR_INV_CHAR_CONST")

pick( RERR_INV_INT_CONST,         "Invalid integer constant",
                                  "JAPAN_RERR_INV_INT_CONST")

pick( RERR_INV_FLOAT_CONST,       "Invalid floating point constant",
                                  "JAPAN_RERR_INV_FLOAT_CONST")

pick( RERR_INV_STRING_CONST,      "Invalid string constant",
                                  "JAPAN_RERR_INV_STRING_CONST")

pick( RERR_INV_TYPE_COMBO,        "Invalid types/specifiers/qualifiers combination",
                                  "JAPAN_RERR_INV_TYPE_COMBO")

pick( RERR_2_OR_MORE_ARG_LISTS,   "Only one argument list is allowed per function",
                                  "JAPAN_RERR_2_OR_MORE_ARG_LISTS")

pick( RERR_CANT_HAVE_ARR_OF_FUNC, "Arrays of functions are not allowed",
                                  "JAPAN_RERR_CANT_HAVE_ARR_OF_FUNC")

pick( RERR_FUNC_CANT_RET_ARR,     "Function cannot return an array",
                                  "JAPAN_RERR_FUNC_CANT_RET_ARR")

pick( RERR_CPARSE_WITH_CONTEXT,   "Parser Syntax error. Context: %s %s",
                                  "JAPAN_RERR_CPARSE_WITH_CONTEXT")

pick( RERR_INCOMPLETE_TYPE,       "Incomplete type",
                                  "JAPAN_RERR_INCOMPLETE_TYPE")

pick( RERR_DCLR_NOT_PRESENT,      "Declarator is missing",
                                  "JAPAN_RERR_DCLR_NOT_PRESENT")

pick( RERR_DCLR_CANT_BE_INIT_TWICE, "Declarator cannot be initialized twice",
                                  "JAPAN_RERR_DCLR_CANT_BE_INIT_TWICE")

pick( RERR_INV_MEM_MODEL_COMBO,   "Invalid memory model combination",
                                  "JAPAN_RERR_INV_MEM_MODEL_COMBO")

pick( RERR_CANT_COMBINE_2_TOKENS, "Cannot combine %s and %s.  Choosing the first token",
                                  "JAPAN_RERR_CANT_COMBINE_2_TOKENS")

pick( RERR_TYPEDEF_CANT_BE_INITIALIZED, "Cannot initialize typedef",
                                  "JAPAN_RERR_TYPEDEF_CANT_BE_INITIALIZED")

pick( RERR_CANT_CONV_PARAM_OF_FUNC, "Cannot convert a parameter of a function",
                                  "JAPAN_RERR_CANT_CONV_PARAM_OF_FUNC")

pick( RERR_SYNTAX_PRE_PARSER,     "Preprocessor syntax error",
                                  "JAPAN_RERR_SYNTAX_PRE_PARSER")

pick( RERR_TYPEDEF_REDECLARED,    "Cannot redefine symbol '%s'",
                                  "JAPAN_RERR_TYPEDEF_REDECLARED")

pick( RERR_MISSING_RIGHT_BRACE,   "Missing '}'",
                                  "JAPAN_RERR_MISSING_RIGHT_BRACE")

pick( RERR_INV_INCLUDE_FILE_NAME, "Invalid include file name",
                                  "JAPAN_RERR_INV_INCLUDE_FILE_NAME")

pick( RERR_INCOMPLETE_ENUM_DECL, "Incomplete enum declaration",
                                  "JAPAN_RERR_INCOMPLETE_ENUM_DECL")

pick( RERR_NOT_ENOUGH_ARGS,       "Not enough arguments for %s",
                                  "RERR_NOT_ENOUGH_ARGS")

pick( RERR_EXPECTING_BUT_FOUND,   "Expecting %s but found %s",
                                  "RERR_EXPECTING_BUT_FOUND")

pick( RERR_MAX,                   "RERR_MAX",
                                  "JAPAN_RERR_MAX")


/*-------- CERR_ -- These occur during conversion/output process ----------*/
pick( CERR_MIN,                   "CERR_MIN",
                                  "JAPAN_CERR_MIN")

pick( CERR_PREDIR_NOT_SUPPORTED,  "Cannot convert preprocessor directive '%s'",
                                  "JAPAN_CERR_PREDIR_NOT_SUPPORTED")

pick( CERR_CANT_CONVERT_BIT_FIELDS, "Cannot convert bit fields",
                                  "JAPAN_CERR_CANT_CONVERT_BIT_FIELDS")

pick( CERR_CANT_CONVERT_QUALIFIER, "Cannot convert 'const' or 'volatile' qualifiers",
                                  "JAPAN_CERR_CANT_CONVERT_QUALIFIER")

pick( CERR_EXTERN_NOT_SUPPORTED,  "External variables not supported: '%s'",
                                  "JAPAN_CERR_EXTERN_NOT_SUPPORTED")

pick( CERR_FORW_DEFN_NOT_ALLOWED, "Cannot convert forward definition: '%s'",
                                  "JAPAN_CERR_FORW_DEFN_NOT_ALLOWED")

pick( CERR_REDEFN_IGNORED,        "Redefinition of symbol '%s' igonred",
                                  "JAPAN_CERR_REDEFN_IGNORED")

pick( CERR_CANT_CONVERT,          "Cannot convert '%s'",
                                  "JAPAN_CERR_CANT_CONVERT")

pick( CERR_MAX,                   "CERR_MAX",
                                  "JAPAN_CERR_MAX")

/*------------------ ERR_ -- These are general errors --------------*/
pick( ERR_MIN,                    "ERR_MIN",
                                  "JAPAN_ERR_MIN")

pick( ERR_OPEN_FILE,              "Could not open file '%s'. Reason: %s",
                                  "JAPAN_ERR_OPEN_FILE")

pick( ERR_CLOSE_FILE,             "Could not close file. Reason: %s",
                                  "JAPAN_ERR_CLOSE_FILE")

pick( ERR_COULD_NOT_SEEK,         "Could not seek file '%s'. Reason: %s",
                                  "JAPAN_ERR_COULD_NOT_SEEK")

pick( ERR_FILE_WAS_INCLUDED,      "File '%s' was already included",
                                  "JAPAN_ERR_FILE_WAS_INCLUDED")

pick( ERR_NEST_INCLUDE,           "Maximum nested #include level exceeded",
                                  "JAPAN_ERR_NEST_INCLUDE")

pick( ERR_INC_FILE_NOT_FOUND,     "Include file '%s' not found",
                                  "JAPAN_ERR_INC_FILE_NOT_FOUND")

pick( ERR_INTERNAL_STACK_OVERFLOW, "Internal stack overflow",
                                  "JAPAN_ERR_INTERNAL_STACK_OVERFLOW")

pick( ERR_LINE_LEN_OUT_OF_RANGE,  "Line length of %d is out of range",
                                  "JAPAN_ERR_LINE_LEN_OUT_OF_RANGE")

pick( ERR_INV_CMD_LINE_OPTION,    "Invalid option: %s",
                                  "JAPAN_ERR_INV_CMD_LINE_OPTION")

pick( ERR_MAX,                    "ERR_MAX",
                                  "JAPAN_ERR_MAX")


/*----- FATAL_ -- These errors cause the WIC to perform a suicide ---*/
pick( FATAL_MIN,                  "FATAL_MIN",
                                  "JAPAN_FATAL_MIN")

pick( FATAL_FILE_NOT_SPECIFIED,   "File not specified",
                                  "JAPAN_FATAL_FILE_NOT_SPECIFIED")

pick( FATAL_INIT_FAILED,          "Initialization failed",
                                  "JAPAN_FATAL_INIT_FAILED")

pick( FATAL_PARSER_FAILED,        "Parser failed",
                                  "JAPAN_FATAL_PARSER_FAILED")

pick( FATAL_OUT_OF_MEM,           "Out of memory",
                                  "JAPAN_FATAL_OUT_OF_MEM")

pick( FATAL_INTERNAL,             "Internal error: '%s'",
                                  "JAPAN_FATAL_INTERNAL")

pick( FATAL_INTERNAL_ASSERTION_FAILED, "Assertion failed: %s. File: %s. Line: %d",
                                  "JAPAN_FATAL_INTERNAL_ASSERTION_FAILED")

pick( FATAL_DIE,                  "Could not recover from previous errors",
                                  "JAPAN_FATAL_DIE")

pick( FATAL_MAX,                  "FATAL_MAX",
                                  "JAPAN_FATAL_MAX")
