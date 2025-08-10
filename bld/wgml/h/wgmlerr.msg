/* messages for WGML
 *  id, english text, japanese??? text
 *  id  in uppercase please
 *      for the enum definition these will be converted to lower case
 *      for the defines and WRC use used as is
 *
 *      japanese is provided as the templates use it
 */

pick( ERR_INTERN,               "Internal Error: File %s, Line %i",
                                "Internal Error: File %s, Line %i" )

pick( ERR_RES_NOT_FOUND,        "Resources not found",
                                "Resources not found" )

pick( ERR_STR_NOT_FOUND,        "Resourcestring not found",
                                "Resourcestring not found" )

pick( ERR_FILE_IO,              "FileIO %s %s",
                                "FileIO %s %s" )

pick( ERR_NO_MEMORY,            "Out of memory!",
                                "Out of memory!" )

pick( ERR_NO_HANDLES,           "Out of file handles!",
                                "Out of file handles!" )

pick( ERR_LOGIC_MAC,            "Logic error: get_macroline for file",
                                "Logic error: get_macroline for file" )

pick( ERR_INF_REOPEN,           "Calling reopen_inc_fp",
                                "Calling reopen_inc_fp" )

pick( ERR_INF_LINE_FILE,        "\tIncluded from line %s of file '%s'",
                                "\tIncluded from line %s of file '%s'" )

pick( ERR_INF_TAG,              "\tCalled from GML tag '%s' in",
                                "\tCalled from GML tag '%s' in" )

pick( ERR_INF_MAC_DEF,          "\tLine %s(at line %s) of macro '%s' defined at line %s of file '%s'",
                                "\tLine %s(at line %s) of macro '%s' defined at line %s of file '%s'" )

pick( ERR_INC_UNKNOWN,          "\tERR Included from unknown ( master document? )",
                                "\tERR Included from unknown ( master document? )" )

pick( WNG_FILEATTR_IGNORED,     "File attribute ignored (%s) %s",
                                "File attribute ignored (%s) %s" )

pick( ERR_MAX_INPUT_NESTING,    "Max input nesting exceeded %s",
                                "Max input nesting exceeded %s" )

pick( ERR_INPUT_FILE_NOT_FOUND, "Input file not found %s",
                                "Input file not found %s" )

pick( INF_CURR_FILE,            "Current file is '%s'",
                                "Current file is '%s'" )

pick( INF_CURR_MACRO,           "Current macro is '%s'(%s)",
                                "Current macro is '%s'(%s)" )

pick( ERR_GOTO,                 "Goto label '%s' not found",
                                "Goto label '%s' not found" )

pick( ERR_MISSING_MAINFILENAME, "Missing main file name",
                                "Missing main file name" )

pick( ERR_IF_LEVEL,             "If nesting level not zero at EOF %s",
                                "If nesting level not zero at EOF %s" )

pick( INF_INDENT_REM,           "%s<< Indent removed",
                                "%s<< Indent removed" )

pick( INF_INCLUDED,             "\tIncluded from %s",
                                "\tIncluded from %s" )

pick( INF_STAT_0,               "Statistics:",
                                "Statistics:" )

pick( INF_STAT_1,               "Completed passes:   %6s %s",
                                "Completed passes:   %6s %s" )

pick( INF_STAT_2,               "   Max inc level:   %6s",
                                "   Max inc level:   %6s" )

pick( INF_STAT_3,               "     Error count:   %6s",
                                "     Error count:   %6s" )

pick( INF_STAT_4,               "   Warning count:   %6s",
                                "   Warning count:   %6s" )

pick( INF_STAT_5,               "      Returncode:   %6s",
                                "      Returncode:   %6s" )

pick( INF_STAT_6,               "    Memory usage:   %6s",
                                "    Memory usage:   %6s" )

pick( INF_STAT_7,               " Time to process: %s",
                                "Time to process:  %s" )

pick( INF_CMDLINE,              "cmdline=%s",
                                "cmdline=%s" )

pick( INF_CMDLINE_TOK_CNT,      "Token count cmdline=%s",
                                "Token count cmdline=%s" )

pick( INF_PASS_1,               "Starting pass %s of %s ( %s mode )",
                                "Starting pass %s of %s ( %s mode )" )

pick( INF_PASS_2,               "  End of pass %s of %s ( %s mode )",
                                "  End of pass %s of %s ( %s mode )" )

pick( WNG_IGN_OPTION,           "Option ignored: %s",
                                "Option ignored: %s" )

pick( WNG_PASS_1,               "Two passes needed to get TOC or FIGLIST in front material",
                                "Two passes needed to get TOC or FIGLIST in front material")

pick( WNG_PASS_MANY,            "More passes required for TOC or FIGLIST",
                                "More passes required for TOC or FIGLIST")

pick( WNG_WNG_OPTION,           "Option ignored: %s, formatting may be different than expected",
                                "Option ignored: %s, formatting may be different than expected" )

pick( ERR_PU_NUM,               "Workfile number not 1 - 9 Line %s of %s '%s'",
                                "Workfile number not 1 - 9 Line %s of %s '%s'" )

pick( ERR_SUB_OUT_OF_RANGE,     "Subscript out of range (-1000000 - +1000000) %s",
                                "Subscript out of range (-1000000 - +1000000) %s" )

pick( ERR_LOGIC_ERR,            "Logic error in %s",
                                "Logic error in %s"  )

pick( INF_TRMEM_CLOSE,          "trmem close() unfreed chunks: %s",
                                "trmem close() unfreed chunks: %s" )

pick( ERR_NOMEM_AVAIL,          "alloc() no memory available",
                                "alloc() no memory available" )

pick( ERR_TAG_NAME,             "Tag name too long '%s' Line %s of %s '%s'",
                                "Tag name too long '%s' Line %s of %s '%s'" )

pick( ERR_TAG_MACRO,            "The macro '%s' for the gml tag '%s' is not defined Line %s of %s '%s'",
                                "The macro '%s' for the gml tag '%s' is not defined Line %s of %s '%s'" )

pick( ERR_IF_INTERN,            "If internal logic error Line %s of %s '%s'",
                                "If internal logic error Line %s of %s '%s'" )

pick( INF_SUBST_LINE,           ">-V->%s<-V-<",
                                ">-V->%s<-V-<" )

pick( INF_TEXT_LINE,            ")-T-)%s(-T-(",
                                ")-T-)%s(-T-(" )

pick( INF_SKIP_LINE,            "--- Skip line ---",
                                "--- Skip line ---" )

pick( ERR_MISS_INV_OPT_VALUE,   "Invalid / missing option value for %s: %s",
                                "Invalid / missing option value for %s: %s" )

pick( ERR_MISSING_OPT_VALUE,    "Missing option value for %s",
                                "Missing option value for %s" )

pick( ERR_MISSING_DEVICE_NAME,  "Missing device name %s",
                                "Missing device name %s" )

pick( ERR_MISSING_FONT_NUMBER,  "Missing font number %s",
                                "Missing font number %s" )

pick( ERR_INVALID_FONT_NUMBER,  "Invalid font number %s",
                                "Invalid font number %s" )

pick( ERR_MISSING_FONT_NAME,    "Missing font name %s",
                                "Missing font name %s" )

pick( ERR_INVALID_FONT_SPACE,   "Invalid font space %s",
                                "Invalid font space %s" )

pick( ERR_INVALID_FONT_HEIGHT,  "Invalid font height %s",
                                "Invalid font height %s" )

pick( ERR_INVALID_OPTION,       "Invalid option %s",
                                "Invalid option %s" )

pick( ERR_DOC_DUPLICATE,        "Master document specified more than once %s",
                                "Master document specified more than once %s" )

pick( INF_RECOGNIZED_XXX,       "Recognized %s %s",
                                "Recognized %s %s" )

pick( ERR_PASSES_VALUE,         "Passes value out of range (1 - %s) %s",
                                "Passes value out of range (1 - %s) %s" )

pick( ERR_OUT_RANGE,            "The %s option value is out of range",
                                "The %s option value is out of range" )

pick( ERR_MISSING_NAME,         "Missing name %s",
                                "Missing name %s" )

pick( ERR_MISSING_VALUE,        "Missing value for %s",
                                "Missing value for %s" )

pick( ERR_RECURSIVE_OPTION,     "The option file '%s' is recursively included",
                                "The option file '%s' is recursively included" )

pick( ERR_FILE_NOT_FOUND,       "File not found '%s'",
                                "File not found '%s'" )

pick( ERR_MAX_NESTING_OPT,      "Max nesting exceeded for option files",
                                "Max nesting exceeded for option files" )

pick( ERR_INV_PAGE_RANGE,       "Page range invalid %s - %s",
                                "Page range invalid %s - %s" )

pick( INF_MAC_LINE,             "\tLine %s of macro '%s'",
                                "\tLine %s of macro '%s'" )

pick( INF_FILE_LINE,            "\tLine %s of file '%s'",
                                "\tLine %s of file '%s'" )

pick( ERR_EQ_EXPECTED,          "Expecting an equal sign",
                                "Expecting an equal sign" )

pick( ERR_EQ_MISSING,           "Missing required equals sign",
                                "Missing required equals sign" )

pick( ERR_ATT_MISSING,          "Required attribute not found",
                                "Required attribute not found" )

pick( ERR_ATT_DUP,              "Attribute already found",
                                "Attribute already found" )

pick( ERR_ATT_VAL_MISSING,      "Missing attribute value",
                                "Missing attribute value" )

pick( ERR_ATT_VAL_OPEN,         "Missing or invalid closing quote on attribute",
                                "Missing or invalid closing quote on attribute")

pick( ERR_SYM_LONG,             "Symbol name too long '%s'",
                                "Symbol name too long '%s'" )

pick( INF_SYM_10,               "The length of a symbol cannot exceed 10 characters\n",
                                "The length of a symbol cannot exceed 10 characters\n" )

pick( ERR_SUB_INVALID,          "Subscript invalid",
                                "Subscript invalid" )

pick( WNG_MISS_INV_VALUE,       "Invalid / missing value for %s: %s",
                                "Invalid / missing value for %s: %s" )

pick( ERR_FUNC_PARM,            "Function parm %s invalid",
                                "Function parm %s invalid" )

pick( ERR_FUNC_PARM_MISS,       "Function parms missing",
                                "Function parms missing" )

pick( ERR_FUNC_PARM_END,        "The symbol function is missing a right parenthesis",
                                "The symbol function is missing a right parenthesis" )

pick( WNG_FUNC_NAME,            "Unknown function '%s'",
                                "Unknown function '%s'" )

pick( WNG_FUNC_UNSUPPORT,       "Function unsupported &%s",
                                "Function unsupported &%s" )

pick( WNG_FUNC_UNIMPL,          "Function not yet implemented &%s",
                                "Function not yet implemented &%s" )

pick( ERR_TAG_NAME_INV,         "Tag name missing / invalid",
                                "Tag name missing / invalid" )

pick( ERR_ATT_NAME_INV,         "Attribute name missing / invalid",
                                "Attribute name missing / invalid" )

pick( ERR_ATT_VAL_INV,          "Attribute value missing / invalid",
                                "Attribute value missing / invalid" )

pick( ERR_ATT_RANGE_INV,        "Attribute range missing / invalid",
                                "Attribute range missing / invalid" )

pick( ERR_ATT_DEFAULT,          "Attribute default outside range",
                                "Attribute default outside range" )

pick( ERR_USER_TAG,             "User tag '%s' has not been defined",
                                "User tag '%s' has not been defined" )

pick( ERR_TAG_TOOMANY,          "Too many attributes",
                                "Too many attributes" )

pick( ERR_LABEL_LINE,           "Label lineno not as expected %s",
                                "Label lineno not as expected %s" )

pick( WNG_LABEL_NUM,            "Numeric Label not implemented",
                                "Numeric Label not implemented" )

pick( ERR_LABEL_DUP,            "Label name duplicate at different line",
                                "Label name duplicate at different line" )

pick( ERR_LABEL_ZERO,           "Label lineno not greater 0",
                                "Label lineno not greater 0" )

pick( ERR_TAG_MAC_NAME,         "Tag macro name missing / invalid",
                                "Tag macro name missing / invalid" )

pick( ERR_TAG_FUNC_INV,         "Tag function missing / invalid (not ADD, CHANGE, OFF, ON, DELETE, PRINT)",
                                "Tag function missing / invalid (not ADD, CHANGE, OFF, ON, DELETE, PRINT)" )

pick( ERR_TAG_OPT_INV,          "Tag option invalid",
                                "Tag option invalid" )

pick( ERR_IF_TERM,              "If condition missing term",
                                "If condition missing term" )

pick( ERR_IF_RELOP,             "If condition invalid relational operator",
                                "If condition invalid relational operator" )

pick( ERR_IF_NESTING,           "If condition nesting too deep",
                                "If condition nesting too deep" )

pick( ERR_IF_THEN,              ".th then can only follow .if",
                                ".th then can only follow .if" )

pick( ERR_IF_ELSE,              ".el else not after .if",
                                ".el else not after .if" )

pick( ERR_IF_DO,                ".do not after .if or .el",
                                ".do not after .if or .el" )

pick( ERR_IF_DO_END,            ".do END does not match",
                                ".do END does not match" )

pick( ERR_IF_DO_FUN,            ".do invalid operand",
                                ".do invalid operand" )

pick( ERR_MAC_DEF_FUN,          ".dm '%s' macro define expecting BEGIN END /macro/lines/",
                                ".dm '%s' macro define expecting BEGIN END /macro/lines/" )

pick( ERR_MAC_DEF_NEST,         "Nested macro define '%s' expecting END",
                                "Nested macro define '%s' expecting END" )

pick( ERR_MAC_DEF_END,          "Macro define END without BEGIN",
                                "Macro define END without BEGIN" )

pick( ERR_MAC_DEF_NOT,          "Macro '%s' is not being defined",
                                "Macro '%s' is not being defined" )

pick( ERR_MAC_DEF_MISS,         "Macro control word parameter missing",
                                "Macro control word parameter missing" )

pick( ERR_MAC_DEF_INV,          "Macro control word parameter '%s' is invalid",
                                "Macro control word parameter '%s' is invalid" )

pick( ERR_MAC_DEF_EOF,          "End of file reached, macro '%s' is still being defined",
                                "End of file reached, macro '%s' is still being defined" )

pick( INF_MAC_DEFINED,          "Macro '%s' defined with %s lines",
                                "Macro '%s' defined with %s lines" )

pick( ERR_MAC_DEF_LOGIC,        "Macro '%s' define logic error",
                                "Macro '%s' define logic error" )

pick( ERR_MAC_NAME_INV,         "Macro name missing / invalid",
                                "Macro name missing / invalid" )

pick( ERR_TAG_EXIST,            "User tag '%s' already exists",
                                "User tag '%s' already exists" )

pick( ERR_AUTO_ATT,             "Cannot specify the automatic attribute '%s'",
                                "Cannot specify the automatic attribute '%s'" )

pick( ERR_NO_ATT_VAL,           "A value must be specified for the attribute '%s'",
                                "A value must be specified for the attribute '%s'" )

pick( ERR_ATT_LEN_INV,          "Length value too large",
                                "Length value too large" )

pick( WNG_ID_XXX,               "For id '%s'",
                                "For id '%s'" )

pick( INF_ID_LEN,               "Identifier name should have no more than seven characters",
                                "Identifier name should have no more than seven characters" )

pick( INF_ID_DUPLICATE,         "Duplicate %s id",
                                "Duplicate %s id" )

pick( INF_ID_FORWARD,           "More passes required for %s processing",
                                "More passes required for %s processing" )

pick( ERR_ID_UNDEFINED,         "Id not defined",
                                "Id not defined" )

pick( INF_ID_UNKNOWN,           "%s id not defined",
                                "%s id not defined" )

pick( ERR_REFID_NOT_ALLOWED,    "REFID not allowed",
                                "REFID not allowed" )

pick( ERR_ATT_VAL,              "Value '%s' for the '%s' attribute is not defined",
                                "Value '%s' for the '%s' attribute is not defined" )

pick( ERR_ATT_TEXT,             "Tag text may not be specified with the '%s' tag",
                                "Tag text may not be specified with the '%s' tag" )

pick( ERR_ATT_TEXT_REQ,         "Tag text must be specified with the '%s' tag",
                                "Tag text must be specified with the '%s' tag" )

pick( WNG_ATT_NAME,             "'%s' is not a valid attribute name",
                                "'%s' is not a valid attribute name" )

pick( ERR_ATT_REQ,              "For the tag '%s', the required attribute(s)",
                                "For the tag '%s', the required attribute(s)" )

pick( INFO_ATT_REQ,             "have not been specified",
                                "have not been specified" )

pick( INFO_ATT_REQ_NAME,        "%s",
                                "%s" )

pick( ERR_CW_UNRECOGNIZED,      "Unrecognized SCRIPT control word or macro .%s",
                                "Unrecognized SCRIPT control word or macro .%s" )

pick( WNG_DC_OPT,               "Unimplemented .dc option %s",
                                "Unimplemented .dc option %s" )

pick( ERR_XX_OPT,               "Invalid %s option %s",
                                "Invalid %s option %s" )

pick( ERR_NESTED_PE,            "Nested .pe not supported",
                                "Nested .pe not supported" )

pick( INF_ERROR_STOP,           "  Remaining pass%s skipped due to error count",
                                "  Remaining pass%s skipped due to error count" )

pick( ERR_PARM_INVALID,         "The control word parameter '%s' is invalid",
                                "The control word parameter '%s' is invalid" )

pick( ERR_PARM_MISSING,         "A control word parameter is required for %s",
                                "A control word parameter is required for %s" )

pick( ERR_EXTRA_IGNORED,        "Extra operand(s) for '%s' ignored: '%s'",
                                "Extra operand(s) for '%s' ignored: '%s'" )

pick( WNG_UNSUPP_CW,            "Unsupported SCRIPT control word: '%s'",
                                "Unsupported SCRIPT control word: '%s'" )

pick( WNG_UNSUPP_CW_OPT,        "Unsupported SCRIPT control word option: '%s'",
                                "Unsupported SCRIPT control word option: '%s'" )

pick( WNG_UNSUPP_TAG,           "Unsupported GML tag: '%s'",
                                "Unsupported GML tag: '%s'" )

pick( WNG_UNSUPP_ATT,           "Unsupported GML attribute: '%s'",
                                "Unsupported GML attribute: '%s'" )

pick( WNG_NOT_YET_SUPP_ATT,     "Not yet supported GML attribute: '%s'",
                                "Not yet supported GML attribute: '%s'" )

pick( WNG_UNSUPP_LAY,           "Unsupported GML layout tag: '%s'",
                                "Unsupported GML layout tag: '%s'" )

pick( WNG_UNSUPP_LAY_ATT,       "Unsupported GML layout tag attribute: '%s'",
                                "Unsupported GML layout tag attribute: '%s'" )

pick( ERR_NESTED_TAG,           "Nested %s not supported",
                                "Nested %s not supported" )

pick( ERR_CW_TAG_X_IN_Y,        "%s cannot be in %s",
                                "%s cannot be in %s" )

pick( INF_NESTED_BLOCKS1,       "Footnotes, figures, examples, floating blocks and floating keeps",
                                "Footnotes, figures, examples, floating blocks and floating keeps" )

pick( INF_NESTED_BLOCKS2,       "cannot nest in one another",
                                "cannot nest in one another" )

pick( ERR_NO_LAY,               "No %s active for '%s'",
                                "No %s active for '%s'" )

pick( ERR_LAY_TEXT,             "Expecting a layout tag or attribute, found text",
                                "Expecting a layout tag or attribute, found text" )

pick( ERR_GML_IN_LAY,           "GML tag '%s' not allowed in LAYOUT",
                                "GML tag '%s' not allowed in LAYOUT" )

pick( ERR_LAY_IN_GML,           "LAYOUT tag '%s' not allowed outside LAYOUT",
                                "LAYOUT tag '%s' not allowed outside LAYOUT" )

pick( ERR_LAY_TOO_LATE,         "'LAYOUT' tag is specified too late (must come before text / :GDOC)",
                                "'LAYOUT' tag is specified too late (must come before text / :GDOC)" )

pick( ERR_DOC_SECT,             "'%s' tag cannot be specified here (must come earlier)",
                                "'%s' tag cannot be specified here (must come earlier)" )

pick( ERR_VAR_NOT_IMPL,         "'%s' variable referenced but not yet implemented",
                                "'%s' variable referenced but not yet implemented" )

pick( ERR_TAG_EXPECTED,         "Expecting %s tag",
                                "Expecting %s tag" )

pick( ERR_TAG_NOT_EXPECTED,     "Not expecting %s tag",
                                "Not expecting %s tag" )

pick( ERR_TAG_NOT_TEXT,         "Expecting a GML tag, found text",
                                "Expecting a GML tag, found text" )

pick( ERR_TEXT_NOT_TAG_CW,      "Expecting text, found a GML tag or Script control word/macro",
                                "Expecting text, found a GML tag or Script control word/macro" )

pick( ERR_TAG_PRECEDING,        "Missing preceding %s tag",
                                "Missing preceding %s tag" )

pick( ERR_TAG_PRECEDING_2,      "Missing %s tag to precede %s tag",
                                "Missing %s tag to precede %s tag")

pick( ERR_TAG_WRONG_SECT,       "'%s' tag only allowed in %s",
                                "'%s' tag only allowed in %s" )

pick( ERR_TAG_BEFORE_GDOC,      "%s tag must appear after the GDOC tag",
                                "%s tag must appear after the GDOC tag" )

pick( ERR_TAG_STARTING,         "\tStarting %s tag on",
                                "\tStarting %s tag on" )

pick( ERR_REF_NOT_ALLOWED,      "Referencing not allowed for %s tag",
                                "Referencing not allowed for %s tag" )

pick( ERR_CHAR_ONLY,            "Only a single character is allowed",
                                "Only a single character is allowed")

pick( ERR_CW_NOT_CHAR,          "A single character or a two character hexadecimal value must be specified",
                                "A single character or a two character hexadecimal value must be specified")

pick( ERR_DC_NOT_OFF,           "'OFF' is the only text value allowed",
                                "'OFF' is the only text value allowed")

pick( ERR_INV_CW_SEP,           "Invalid control word separator",
                                "Invalid control word separator" )

/* BINCLUDE/GRAPHIC-specific error messages */

pick( ERR_INV_DEPTH_BINCLUDE,   "Depth of BINCLUDE file is greater than a page",
                                "Depth of BINCLUDE file is greater than a page" )

pick( WNG_REC_TYPE_BINCLUDE,    "BINCLUDE tag treats all record types as '(t)'",
                                "BINCLUDE tag treats all record types as '(t)'" )

pick( WNG_REC_TYPE_GRAPHIC,     "GRAPHIC tag ignores record types",
                                "GRAPHIC tag ignores record types" )

pick( ERR_INV_DEPTH_GRAPHIC_1,  "The GRAPHIC depth must be greater than zero",
                                "The GRAPHIC depth must be greater than zero" )

pick( ERR_INV_DEPTH_GRAPHIC_2,  "The GRAPHIC depth is greater than the page depth",
                                "The GRAPHIC depth is greater than the page depth" )

pick( ERR_INV_WIDTH_GRAPHIC_1,  "The GRAPHIC width must be greater than zero",
                                "The GRAPHIC width must be greater than zero" )

pick( ERR_INV_WIDTH_GRAPHIC_2,  "The GRAPHIC width is greater than the page width",
                                "The GRAPHIC width is greater than the page width" )

/* Attribute value/control word operand parsing error messages */

pick( ERR_INV_ATT_VAL,          "Invalid attribute value",
                                "Invalid attribute value" )

pick( ERR_INV_CW_OP_VAL,        "Invalid control word operand value",
                                "Invalid control word operand value" )

pick( ERR_NUM_TOO_LARGE,        "Number is too large or contains invalid characters",
                                "Number is too large or contains invalid characters" )

pick( ERR_NUM_ZERO,             "Number value must be greater than zero",
                                "Number value must be greater than zero" )

pick( ERR_NUM_S16_NEG,          "Number value cannot be greater than 32,767",
                                "Number value cannot be greater than 32,767" )

pick( ERR_VAL_NEG,              "Value cannot be negative",
                                "Value cannot be negative" )

pick( WNG_TOO_MANY_OPS,         "Too many operands in %s control word",
                                "Too many operands in %s control word" )

/* General parsing error messages */

pick( ERR_RIGHT_DELIM,          "Right string delimiter missing",
                                "Right string delimiter missing" )

/* Control words FB/FK error messages */

pick( ERR_NO_FB_BEGIN,          "FB END not preceded by FB BEGIN",
                                "FB END not preceded by FB BEGIN" )

pick( ERR_NO_FK_BEGIN,          "FK END not preceded by FK BEGIN",
                                "FK END not preceded by FK BEGIN" )

pick( ERR_NO_OPERAND,           "%s must have one of operands BEGIN, END or DUMP",
                                "%s must have one of operands BEGIN, END or DUMP" )

/* Control word TB value error messages */

pick( ERR_INV_TAB_STOP,         "Invalid tab-stop position",
                                "Invalid tab-stop position" )

pick( ERR_INV_TEXT_AFTER_TAB,   "Invalid text after tab information",
                                "Invalid text after tab information" )

pick( ERR_INV_TEXT_BEFORE_TAB,  "Invalid text before tab position",
                                "Invalid text before tab position" )

pick( ERR_TAB_STOP_ORDER,       "Tab-stop positions not in ascending order",
                                "Tab-stop positions not in ascending order" )

pick( ERR_TAB_CHAR,             "Tab character defined by control word TB SET must be one character",
                                "Tab character defined by control word TB SET must be one character" )

pick( ERR_TAB_ALIGN_CHAR,       "Character alignment is not supported with control word TB",
                                "Character alignment is not supported with control word TB" )

pick( ERR_TAB_FILL_STRING,      "Fill strings are not supported with control word TB",
                                "Fill strings are not supported with control word TB" )

/* Document structure error messages */

pick( ERR_DOC_SEC_EXPECTED_1,   "Expecting a FRONTM, BODY, APPENDIX or BACKM tag",
                                "Expecting a FRONTM, BODY, APPENDIX or BACKM tag" )

pick( ERR_DOC_SEC_EXPECTED_2,   "Expecting an ABSTRACT, PREFACE, BODY, APPENDIX or BACKM tag",
                                "Expecting an ABSTRACT, PREFACE, BODY, APPENDIX or BACKM tag" )

pick( ERR_EOF_EXPECTED,         "Expecting the end of the file",
                                "Expecting the end of the file" )

pick( WNG_HEADING_LEVEL,        "Heading level(s) omitted:",
                                "Heading level(s) omitted:" )

pick( INF_HEADING_LEVEL,        "Encountered %s, expecting %s",
                                "Encountered %s, expecting %s" )

/* Page Geometry error messages. */

pick( ERR_MARGINS_INVERTED,     "The right margin must be greater than the left margin",
                                "The right margin must be greater than the left margin" )

pick( ERR_PAGE_DEPTH_TOO_SMALL, "The page depth is too small when adjusted for the unprintable area of the device page",
                                "The page depth is too small when adjusted for the unprintable area of the device page" )

pick( ERR_PAGE_DEPTH_TOO_BIG,   "The page depth is larger than the printable area of the device page",
                                "The page depth is larger than the printable area of the device page" )

pick( ERR_PAGE_WIDTH_TOO_SMALL, "Left and right margins are too close together",
                                "Left and right margins are too close together" )

pick( ERR_RIGHT_MARGIN_TOO_BIG, "The layout right margin is greater than the device page width",
                                "The layout right margin is greater than the device page width" )

pick( ERR_RIGHT_MARGIN_2_SMALL, "The right margin is too small when adjusted for the unprintable area of the device page",
                                "The right margin is too small when adjusted for the unprintable area of the device page" )

/* element can't be split and won't fit on any page */

pick( ERR_HEADING_TOO_DEEP,     "Heading is too large for the output page",
                                "Heading is too large for the output page")

pick( ERR_TEXT_LINE_TOO_DEEP,   "Output line is too large to fit on a page",
                                "Output line is too large to fit on a page")

/* title page error messages */

pick( ERR_2ND_DATE,             "Date already defined",
                                "Date already defined")

pick( ERR_2ND_DOCNUM,           "Document number already defined",
                                "Document number already defined")

/* list error message */

pick( ERR_NO_LI_LP,             "LI or LP tag must follow the start of a list",
                                "LI or LP tag must follow the start of a list")

pick( ERR_NO_LIST,              "List tag used outside of any list",
                                "List tag used outside of any list" )

pick( ERR_LEVEL_SKIPPED,        "For the level attribute (layout tag %s)",
                                "For the level attribute (layout tag %s)" )
                                
pick( INFO_LEVEL_SKIPPED,       "Level number %d must be specified",
                                "Level number %d must be specified" )
                                
pick( WNG_HDREF_CO_OFF,         "HDREF with concatenation OFF is not supported",
                                "HDREF with concatenation OFF is not supported" )

/* cop file errors */

pick( ERR_BLOCK_NOT_FOUND,      "For %s block %s: name not defined\n",
                                "For %s block %s: name not defined\n")

pick( ERR_DEV_DATA_FILE,        "Error reading device/driver/font file %s\n",
                                "Error reading device/driver/font file %s\n")

pick( ERR_DEV_LIB_DATA,         "Device library corrupt or wrong version: %s\n",
                                "Device library corrupt or wrong version: %s\n")

pick( ERR_DEV_LIB_FILE,         "Error reading device/font library in %s\n",
                                "Error reading device/font library in %s\n")

pick( ERR_DEV_NOT_FOUND,        "File for device/driver/font %s not found\n",
                                "File for device/driver/font %s not found\n")

pick( ERR_FONT_OPT_SCALED,      "For the FONT option with font %d, the font_height attribute must be specified when the font is scaled.",
                                "For the FONT option with font %d, the font_height attribute must be specified when the font is scaled.")

pick( ERR_FONT_SCALED,          "For default font %d, the font_height attribute must be specified when the font is scaled.",
                                "For default font %d, the font_height attribute must be specified when the font is scaled.")

pick( ERR_REC_DEV_FUNC,         "Recursive use of %s, check device library.",
                                "Recursive use of %s, check device library.")

pick( ERR_WRONG_GENDEV,         "Device/font library was created with a different version of GENDEV",
                                "Device/font library was created with a different version of GENDEV")

pick( ERR_ZERO_DIVISOR,         "Zero divisor found in device function %s.",
                                "Zero divisor found in device function %s.")

/* outbuff.c specific errors */

pick( ERR_CLOSE_OUT_FILE,       "Unable to close output file %s\n",
                                "Unable to close output file %s\n")

pick( ERR_IN_FILE,              "Error reading file for tag %s: %s",
                                "Error reading file for tag %s: %s")

pick( ERR_OPEN_OUT_FILE,        "Unable to open output file %s\n",
                                "Unable to open output file %s\n")

pick( ERR_WRITE_OUT_FILE,       "Error writing output file %s\n",
                                "Error writing output file %s\n")

pick( ERR_OUT_REC_SIZE,         "Output file's record size is too small for the device '%s'\n",
                                "Output file's record size is too small for the device '%s'\n")

pick( ERR_OUT_REC_SIZE2,        "Output record length is too large, max is %s\n",
                                "Output record length is too large, max is %s\n")

pick( ERR_REC_ATT_NOT_SUP,      "Record attribute %s is not currently supported\n",
                                "Record attribute %s is not currently supported\n")

pick( ERR_REC_ATT_BAD_FMT,      "Record attribute %s is not formatted correctly\n",
                                "Record attribute %s is not formatted correctly\n")

/* find file errors */

pick( ERR_FILE_MAX,             "File name is too long and will not be searched for: %s%s\n",
                                "File name is too long and will not be searched for: %s%s\n")

pick( ERR_FILE_NAME,            "File names cannot contain path information! Filename: %s\n",
                                "File names cannot contain path information! Filename: %s\n")

pick( ERR_MEM_DIR,              "Member file not found in same directory as directory file: %s%s\n",
                                "Member file not found in same directory as directory file: %s%s\n")

pick( ERR_PATH_MAX,             "Directory path is too long and will not be used: %s\n",
                                "Directory path is too long and will not be used: %s\n")

/* BX/FIG/IXHEAD error messages */

pick( ERR_BOX_BAD_ORDER,        "Box positions not in ascending order",
                                "Box positions not in ascending order")

pick( ERR_FIG_ID_CAP,           "Figure with an Id must have a caption",
                                "Figure with an Id must have a caption")

pick( ERR_INV_BOX_POS,          "Invalid box position",
                                "Invalid box position")

pick( ERR_INV_WIDTH_FIG_1,      "The FIG width must be greater than zero",
                                "The FIG width must be greater than zero" )

pick( ERR_INV_WIDTH_FIG_2,      "For a multiple column figure, place attribute must equal TOP",
                                "For a multiple column figure, place attribute must equal TOP" )

pick( ERR_INV_WIDTH_FIG_3,      "Figure is too wide",
                                "Figure is too wide" )

pick( ERR_INV_MARGINS_1,        "Cannot fit the figure in the adjusted left and right margins",
                                "Cannot fit the figure in the adjusted left and right margins" )

pick( ERR_INV_MARGINS_2,        "Cannot fit the figure with a frame in the adjusted left and right margins",
                                "Cannot fit the figure with a frame in the adjusted left and right margins" )

pick( ERR_SPC_NOT_VALID,        "Space value is not valid",
                                "Space value is not valid" )

pick( ERR_TOO_MANY_OPS,         "Too many operands in .BX command",
                                "Too many operands in .BX command" )

/* Indexing error messages */

pick( ERR_BAD_DP_VALUE,         "Invalid operand, expecting dump|purge",
                                "Invalid operand, expecting dump|purge" )

pick( ERR_EXTRA_DATA,           "Not expecting more operand data",
                                "Not expecting more operand data" )

pick( ERR_NO_RANGE_START,       "No page range starting entry specified",
                                "No page range starting entry specified" )

pick( ERR_OPEN_PAGE_RANGE,      "Page range incomplete",
                                "Page range incomplete" )

pick( ERR_PARENT_UNDEF,         "Parent index entry not defined",
                                "Parent index entry not defined" )

pick( ERR_STRUCT_RANGE,         "The ix attribute has a range of 1 through 9",
                                "The ix attribute has a range of 1 through 9" )

pick( WNG_INDEX_OPT,            "Index option must be specified to process index tags",
                                "Index option must be specified to process index tags" )

/* Banner and Region Messages */

pick( ERR_ALL_BAN_ATT_RQRD,     "All attributes required when defining new banner",
                                "All attributes required when defining new banner" )

pick( ERR_ALL_REG_ATT_RQRD,     "All attributes required when defining new banner region",
                                "All attributes required when defining new banner region" )

pick( ERR_BAN_DEPTH1,           "Banner depth is too small",
                                "Banner depth is too small" )

pick( ERR_BAN_DEPTH2,           "Depth of banner(s) too large for a page",
                                "Depth of banner(s) too large for a page" )

pick( ERR_BAN_WIDTH,            "Banner width is too small",
                                "Banner width is too small" )

pick( ERR_BANREG_EXTEND,        "Cannot extend two regions into each other",
                                "Cannot extend two regions into each other" )

pick( ERR_BANREG_OVERLAP,       "Banner regions overlap",
                                "Banner regions overlap" )

pick( ERR_BANREG_DEPTH,         "Banner region exceeds banner depth",
                                "Banner region exceeds banner depth" )

pick( ERR_BANREG_WIDTH,         "Banner region exceeds banner width",
                                "Banner region exceeds banner width" )

pick( ERR_BOTH_REFS,            "If one of refdoc or refplace is specified both are required",
                                "If one of refdoc or refplace is specified both are required" )

pick( ERR_ILLEGAL_BAN_REF,      "Illegal banner reference, banner does not exist",
                                "Illegal banner reference, banner does not exist" )

pick( ERR_ILLEGAL_REG_REF,      "Illegal banner region reference",
                                "Illegal banner region reference" )

pick( ERR_SCR_FMT,              "The script_format attribute has been set to NO for the banner region with a keyword content value",
                                "The script_format attribute has been set to NO for the banner region with a keyword content value" )

pick( ERR_SELF_REF,             "Cannot reference banner currently being defined",
                                "Cannot reference banner currently being defined" )

pick( INF_BAN_ID,               "For banner with docsect = %s and place = %s",
                                "For banner with docsect = %s and place = %s" )

pick( INF_REG_ID,               "For region with hoffset = %s, voffset = %s, and indent = %s",
                                "For region with hoffset = %s, voffset = %s, and indent = %s" )

/* Value Parsing Messages */

pick( ERR_I_8,                  "The value entered must be in the range [-255, 255]",
                                "The value entered must be in the range [-255, 255]" )

pick( ERR_XX_STRING,            "The string given is too long",
                                "The string given is too long" )

pick( ERR_UI_8,                 "The value entered must be in the range [0, 255]",
                                "The value entered must be in the range [0, 255]" )

pick( ERR_UI_16,                "The value entered must be in the range [0, 65535]",
                                "The value entered must be in the range [0, 65535]" )
