pick( SCAN_NAME,       ScanName           )  // identifier
pick( SCAN_WIDE,       ScanWide           )  // L"abc" or L'a' or Lname
pick( SCAN_NUM,        ScanNum            )  // number that starts with a digit
pick( SCAN_QUESTION,   ScanQuestionMark   )  // '?'
pick( SCAN_SLASH,      ScanSlash          )  // '/'
pick( SCAN_MINUS,      ScanMinus          )  // '-'
pick( SCAN_EQUAL,      ScanEqual          )  // '='
pick( SCAN_COLON,      ScanColon          )  // ':'
pick( SCAN_STAR,       ScanStar           )  // '*'
pick( SCAN_DELIM1,     ScanDelim1         )  // single character delimiter
pick( SCAN_DELIM2,     ScanDelim2         )  // one, two, or three byte delimiter
pick( SCAN_DOT,        ScanDot            )  // .
pick( SCAN_STRING,     ScanString         )  // "string"
pick( SCAN_CHARCONST,  ScanCharConst      )  // 'a'
pick( SCAN_CR,         ScanCarriageReturn )  // '\r'
pick( SCAN_NEWLINE,    ScanNewline        )  // '\n'
pick( SCAN_WHITESPACE, ScanWhiteSpace     )  // all whitespace
pick( SCAN_INVALID,    ScanInvalid        )  // all other characters
pick( SCAN_MACRO,      ScanMacroToken     )  // get next token from macro
pick( SCAN_EOF,        ScanEof            )  // end-of-file
