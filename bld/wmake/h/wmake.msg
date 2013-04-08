pick( OUT_OF_MEMORY,                "Out of memory",
                                "メモリが足りません" )
pick( MAKE_ABORT,                "Make execution terminated",
                                "MAKE実行を終了しました" )

/* the two %c's must be together in these two messages */
pick( INVALID_OPTION,                "Option %c%c invalid",
                                "オプション %c%c が無効です" )
pick( INVALID_FILE_OPTION,        "%c%c must be followed by a filename",
                                "%c%cはファイル名に続いて指定しなければなりません" )

pick( NO_TARGETS_SPECIFIED,        "No targets specified",
                                "ターゲットが指定されていません" )
pick( MAKEINIT_HAS_TARGET,        "Ignoring first target in MAKEINIT",
                                "MAKEINITの最初のターゲットを無視します" )
pick( EXPECTING_M,                "Expecting a %M",
                                "%Mでなけばなりません" )
pick( INVALID_MACRO_NAME,        "Invalid macro name %E",
                                "無効なマクロ名 %E" )
pick( IGNORE_OUT_OF_PLACE_M,        "Ignoring out of place %M",
                                "%M を無視します" )
pick( CANNOT_NEST_FURTHER,        "Macros nested too deep",
                                "マクロのネストが深すぎます" )
pick( UNKNOWN_PERCENT_CMD,        "Unknown internal command",
                                "無効な内部コマンドです" )
pick( COMMAND_TOO_LONG,                "Program name is too long",
                                "コマンドラインが長すぎます" )
pick( CTRL_CHAR_IN_CMD,                "No control characters allowed in options",
                                "オプション指定では、制御文字は使用できません" )
pick( UNABLE_TO_EXEC,                "Cannot execute %E: %Z",
                                "実行中 %E: %Z" )
pick( SYNTAX_ERROR_IN,                "Syntax error in %s command",
                                "%s コマンドで構文エラーです" )
pick( NO_NESTED_FOR,                "Nested %s loops not allowed",
                                "ネストされた %s ループはできません" )
pick( MAXIMUM_TOKEN_IS,                "Token too long, maximum size is %d chars",
                                "トークンが長すぎます,最長は %d 文字です" )
pick( UNKNOWN_TOKEN,                "Unrecognized or out of place character '%C'",
                                "認識されなかったか,無効な位置にある文字 '%C'" )
pick( TARGET_ALREADY_M,                "Target %E already declared %M",
                                "ターゲット %E は既に %M と宣言されています" )
pick( CLIST_HAS_NO_OWNER,        "Command list does not belong to any target",
                                "コマンドリストがどのターゲットにも属していません" )
pick( SUFFIX_DOESNT_EXIST,        "Extension(s) %E not defined",
                                "拡張子 %E が未定義です" )
pick( NO_EXISTING_FILE_MATCH,        "No existing file matches %E",
                                "%E と一致するファイルがありません" )
pick( EXTENSIONS_REVERSED,        "Extensions reversed in implicit rule",
                                "拡張子は暗黙のルールで予約されています" )
pick( MORE_THAN_ONE_CLIST,        "More than one command list found for %E",
                                "%E に対して2組以上のコマンドリストがあります" )
pick( REDEF_OF_SUFFIX,                "Extension %E declared more than once",
                                "拡張子 %E が2回以上宣言されています" )
pick( UNK_PREPROC_DIRECTIVE,        "Unknown preprocessor directive: %s",
                                "プリプロセッサ疑似命令ではありません" )
pick( MACRO_UNDEFINED,                "Macro %E is undefined",
                                "マクロ %E が未定義です" )
pick( IF_NESTED_TOO_DEEP,        "!If statements nested too deep",
                                "!if 文のネストが深すぎます" )
pick( UNMATCHED_WITH_IF,        "!%s has no matching !if",
                                "!%S にはマッチする !if がありません" )
pick( SKIPPING_AFTER_ELSE,        "Skipping !%1 block after !%2",
                                "!%s ブロックをスキップしました(!%s の後)" )
pick( NOT_ALLOWED_AFTER_ELSE,        "%1 not allowed after !%2",
                                "%1 は %2 後にはできません" )
pick( UNABLE_TO_INCLUDE,        "Opening file %E: %Z",
                                "オープンファイル %E: %Z" )
pick( USER_ERROR,                "",
                                "" )
pick( EOF_BEFORE_ENDIF,                "!%s pending at end of file",
                                "ファイルの終端まで、!%s は処理されません" )
pick( TRYING_UNDEF_UNDEF,        "Trying to !%s an undefined macro",
                                "!%s を未定義なマクロとして処理します" )
pick( ATTEMPT_MAKE_SPECIAL,        "Illegal attempt to update special target %E",
                                "特別なターゲット %E を不正に更新しようとしています" )
pick( RECURSIVE_DEFINITION,        "Target %E is defined recursively",
                                "ターゲット %E が再帰的に定義されています" )
pick( UNABLE_TO_MAKE,                "%E does not exist and cannot be made from existing files",
                                "%E はありませんし、既存のファイルから作成できません" )
pick( TARGET_NOT_MENTIONED,        "Target %E not mentioned in any makefile",
                                "ターゲット %E はメークファイルのどこにもありません" )
pick( COULD_NOT_TOUCH,                "Could not touch %E",
                                "%E のタイム・スタンプを更新できません" )
pick( NO_DEF_CMDS_FOR_MAKE,        "No %s commands for making %E",
                                "%s コマンドがありません(%E の作成に対応)" )
pick( LAST_CMD_MAKING_RET_BAD,        "Last command making (%L) returned a bad status",
                                 "(%L)を作成する最後のコマンドによりエラーが返されました" )
pick( SYSERR_DELETING_FILE,        "Deleting %E: %Z",
                                "%Eを消します: %Z" )
pick( S_COMMAND_RET_BAD,        "%s command returned a bad status",
                                "%S コマンドがエラー・ステータスを返しました" )
pick( MAXIMUM_STRING_LENGTH,        "Maximum string length exceeded",
                                "最大文字列長を越えます" )
pick( BARF_CHARACTER,                "Illegal character value %xH in file",
                                "ファイルに不正な文字の値 %xH があります" )
pick( ASSUMING_SYMBOLIC,        "Assuming %s target is .SYMBOLIC",
                                "ターゲットは .%s であると仮定します" )
pick( PERCENT_MAKE_DEPTH,        "Maximum %%make depth exceeded",
                                "%%make の深さの最大を越えます" )
pick( OPENING_FOR_WRITE,        "Opening (%s) for write: %Z",
                                "書込むために (%s) をオープンします: %Z" )
pick( DOING_THE_WRITE,                "Unable to write: %Z",
                                "書込めません: %Z" )
pick( CHANGING_DIR,                "CD'ing to %E: %Z",
                                "%E にディレクトリを変更します: %Z" )
pick( CHANGING_DRIVE,                "Changing to drive %C:",
                                "ドライフ %C: に変ります" )
pick( PARSE_IFEXPR,             "!IF Parse Error",
                                "!IF フェーズエラー" )
pick( TMP_PATH_TOO_LONG,        "TMP Path/File Too Long",
                                "TMP パス/ファイル名が長すぎます" )
pick( UNEXPECTED_EOF,           "Unexpected End of File",
                                "ファイルが途中で終わっています" )
pick( NOKEEP_ONLY,              "Only NO(KEEP) allowed here",
                                "ここでは NO(KEEP) だけができます")
pick( NON_MATCHING_QUOTE,       "Non-matching \"",
                                "一致しません \"")
pick( INVALID_STRING_SUBSTITUTE,"Invalid String Macro Substitution",
                                "不適切な文字列マクロ置換です")
pick( MAX_FILE_LENGTH_EXCEED,   "File Name Length Exceeded",
                                "ファイル名の長さが制限を越えました")
pick( DOT_DEFAULT_REDEFINITION, "Redefinition of .DEFAULT Command List",
                                ".DEFAULT コマンド･･リストの再定義です")
pick( NON_MATCHING_CURL_PAREN,  "Non-matching { In Implicit Rule",
                                "暗黙のルールの中の \{ が一致しません")
pick( INVALID_SUFSUF,           "Invalid Implicit Rule Definition",
                                "不適切な暗黙のルール定義です")
pick( PATH_TOO_LONG,            "Path Too Long",
                                "パスが長すぎます")
pick( UNABLE_TO_LOAD_DLL,       "Cannot Load/Unload DLL %E",
                                "DLL をロード/アンロードできません %E")
pick( DLL_BAD_INIT_STATUS,      "Initialization of DLL %E returned a bad status",
                                "エラー･ステータスを返すDLL %E 初期化")
pick( DLL_BAD_RETURN_STATUS,    "DLL %E returned a bad status",
                                "DLL %E はエラーステータスを返します")
pick( ILLEGAL_CHARACTER_IN_MAC, "Illegal Character %C in macro name",
                                "マクロの中に不正な文字 %C があります")
pick( ERROR_CLOSING_FILE,       "in closing file %E",
                                "ファイル %E を閉じるとき")
pick( ERROR_OPENING_FILE,       "in opening file %E",
                                "ファイル %E を開くとき")
pick( ERROR_WRITING_FILE,       "in writing file %E",
                                "ファイル %E を書き込むとき")
pick( USER_BREAK_ENCOUNTERED,   "User Break Encountered",
                                "ユーザにより中断されました")
pick( ERROR_TRMEM,              "Error in Memory Tracking Encountered",
                                "メモリトラッキング中にエラーになりました")
pick( MICROSOFT_MAKEFILE,       "Makefile may be Microsoft; try /ms switch",
                                "Makefile may be Microsoft; try /ms switch")
pick( ERROR_STRING_OPEN,    "Expecting double quote character to end open string.",
                "Expecting double quote character to end open string.")

#if defined( __DOS__ )
pick( OS_CORRUPTED,                "DOS memory inconsistency detected! System may halt ...",
                                "DOSﾒﾓﾘ管理に矛盾が見つかりました！ｼｽﾃﾑがﾎｰﾙﾄするかもしれません･･･" )
#else
pick( OS_CORRUPTED,             "OS corruption detected",
                                "ＯＳが壊れているのが、見つかりました" )
#endif
pick( READ_ERROR,                "While reading (%s): %Z",
                                "(%s)を読み出しています: %Z" )
pick( INCOMPATIBLE__OPTIONS,    "Incompatible options",
                                "Incompatible options" )

/*
 * all msgs beyond here should not have a number printed with them
 * ie: these are inform/debug/%M messages only
 */

/* %M msgs */

pick( M_FORM_QUALIFIER,                "Form Qualifier",
                                "フォーム・クォリファイア(形式限定子)" )
pick( M_UNKNOWN_TOKEN,                "Unknown Token",
                                "不明なトークン" )
pick( M_SCOLON,                        "Single-colon",
                                "シングル・コロン" )
pick( M_DCOLON,                        "Double-colon",
                                "ダブル・コロン" )
pick( M_FILENAME,                "Filename",
                                "ファイル名" )
pick( M_DOTNAME,                "Dot Name",
                                "ドット名" )
pick( M_CMD,                        "Command",
                                "コマンド" )
pick( M_SUF,                        "Extension",
                                "拡張子" )
pick( M_SUFSUF,                        "Implied Extension",
                                "暗黙の拡張" )
pick( M_PATH,                        "Path Specifier",
                                "パスの指定" )
pick( M_ERROR,                        "Error",
                                "エラー" )
pick( M_WARNING,                "Warning",
                                "警告" )

/* inform/debug msgs */

pick( SKIPPING_BLOCK,                "Skipping !%s block",
                                "!%s ブロックをスキップします" )
pick( ENTERING_BLOCK,                "Entering !%s block",
                                "!%s ブロックに入ります" )
pick( AT_ENDIF,                        "Ending a !if nesting",
                                "!if ネストを終わります" )
pick( ENTERING_FILE,                "Entering file %E...",
                                "ファイル %E に入ります・・・" )
pick( FINISHED_FILE,                "...Finished file %E",
                                "・・・ファイル %E を終わります" )
pick( PTARG_IS_TYPE_M,                "%E is a %M type target",
                                "%E は %M タイプのターゲットです" )
pick( PTARG_DEPENDS_ON,                "    Depends on:",
                                "    関連：" )
pick( PTARG_TAB_TAB_ENV,        "\t%E",
                                "\t%E" )
pick( PTARG_WOULD_EXECUTE_CMDS,        "    Updated with these commands:",
                                "    これらのコマンドを使って、更新します" )
pick( PTARG_NO_DEPENDENTS,        "    Has no dependents",
                                "    関連がありません" )
pick( PTARG_NAME,                "%E:",
                                "%E:" )
pick( PTARG_DOTNAME,                " (.%s)",
                                " (.%s)" )
pick( PDOT_CMDS,                "%s commands are:",
                                "%s コマンドは以下のものです：" )
pick( PSUF_SUFFIX,                "Extension (.%s):",
                                "拡張子 (.%s):" )
pick( PSUF_FOUND_IN,                "    Found in: %s",
                                "    %s の中にあります" )
pick( PSUF_MADE_FROM,                "    Can be made from extension (.%s):",
                                "    拡張子 (.%s) から作られます" )
pick( PSUF_USING_CMDS,                "    Using these commands:",
                                "    これらのコマンドを使っています" )
pick( PMAC_NAME_HAS_VALUE,        "$%E has the value (%L)",
                                "$%E の値は (%L) です" )
pick( UPDATING_TARGET,                "Updating %E:",
                                "更新しました %E:" )
pick( M_EXPLICIT_RULE,                "has %M explicit rule",
                                "%M の様な明示ルールがあります" )
pick( M_NO,                        "no",
                                "いいえ" )
pick( TARGET_IS_UPDATED,        "%E is up to date",
                                "%E は更新しました" )
pick( IMP_ENV_M,                "\t%E %M",
                                "\t%E %M" )
pick( M_CHECKING_IMPLICIT,        "checking implicit rules",
                                "暗黙のルールをチェック中" )
pick( M_HAS_NO_IMPLICIT,        "has no implicit rules",
                                "暗黙のルールがありません" )
pick( M_IS_CLEAR_WITH,                "is clear with implicit rules",
                                "暗黙のルールでクリヤしました" )
pick( M_HAD_TO_BE_UPDATED,        "had to be updated",
                                "更新が必要です" )
pick( M_COULD_NOT_BE_IMPLIED,        "could not be implied",
                                "暗黙のルールが適用できません" )
pick( M_YOUNGEST_DATE,                "Youngest Possible  ",
                                "可能な中での最新  " )
pick( M_OLDEST_DATE,                "Oldest Possible    ",
                                "可能な中での最古  " )
pick( GETDATE_MSG,                "\t%s\t%E",
                                "\t%s\t%E" )
pick( WILL_BE_BUILT_BECAUSE_OF,                "%s will be built because of %s",
                                "%s will be built because of %s" )
pick( DELETING_FILE,            "Deleting file %s",
                                "Deleting file %s" )
pick( TARGET_FORCED_UPTODATE,   "%E is considered uptodate",
                                "%E is considered uptodate" )
pick( EXECING_CMD,              "About to execute [%s]",
                                "About to execute [%s]" )
pick( CMD_RETCODE,              "Return code was %d",
                                "Return code was %d" )


pick( JUST_A_TAB,                "\t",
                                "\t" )
pick( NEWLINE,                        "",
                                "" )
pick( FAR_STRING,                "%F",
                                "%F" )
pick( DO_YOU_WISH_TO_CONT,        "Do you wish to continue",
                                "続行しますか" )
pick( SHOULD_FILE_BE_DELETED,        "Should this file be deleted",
                                "このファイルを消しますか" )
pick( STRING_YES_NO,                "%M [Yes/No] ? ",
                                "%M ［(Y)はい／(N)いいえ］？" )
pick( PRESS_KEY,                "Press any key...",
                                "どれかキーを押してください..." )


/* system error messages */
pick( SYS_ERR_0,                "Error 0",
                                "エラー 0" )
pick( SYS_ERR_1,                "No such file or directory",
                                "ファイルまたはディレクトリがありません" )
pick( SYS_ERR_2,                "Argument list too big",
                                "引数リストが長すぎます" )
pick( SYS_ERR_3,                "Exec format error",
                                "実行(Exec)フォーマット・エラーです" )
pick( SYS_ERR_4,                "Bad file number",
                                "不正なファイル番号です" )
pick( SYS_ERR_5,                "Not enough memory",
                                "メモリ不足です" )
pick( SYS_ERR_6,                "Permission denied",
                                "アクセス権がありません" )
pick( SYS_ERR_7,                "File exists",
                                "ファイルがあります" )
pick( SYS_ERR_8,                "Cross-device link",
                                "クロス・デバイス・リンクです" )
pick( SYS_ERR_9,                "Invalid argument",
                                "無効な引数です" )
pick( SYS_ERR_10,                "File table overflow",
                                "ファイル・テーブルがいっぱいです" )
pick( SYS_ERR_11,                "Too many open files",
                                "オープンするファイルが多すぎます" )
pick( SYS_ERR_12,                "No space left on device",
                                "ドライブに空き領域が残っていません" )
pick( SYS_ERR_13,                "Argument too large",
                                "引数が大きすぎます" )
pick( SYS_ERR_14,                "Result too large",
                                "結果が大きすぎます" )
pick( SYS_ERR_15,                "Resource deadlock would occur",
                                "リソース・デッド・ロックが起こります" )

/* common signals that cause abnormal termination (cf. sys_siglist) */
#if defined( __UNIX__ )
pick( SIG_ERR_0,                 "Abnormal termination by signal %d", "" )
pick( SIG_ERR_1,                 "Hangup detected", "" )
pick( SIG_ERR_2,                 "Keyboard interrupt", "" )
pick( SIG_ERR_3,                 "Keyboard quit", "" )
pick( SIG_ERR_4,                 "Illegal instruction", "" )
pick( SIG_ERR_5,                 "Trap", "" )
pick( SIG_ERR_6,                 "Abort", "" )
pick( SIG_ERR_7,                 "Bus error", "" )
pick( SIG_ERR_8,                 "Floating point exception", "" )
pick( SIG_ERR_9,                 "Killed", "" )
pick( SIG_ERR_10,                "User defined signal 1", "" )
pick( SIG_ERR_11,                "Segmentation fault", "" )
pick( SIG_ERR_12,                "User defined signal 2", "" )
pick( SIG_ERR_13,                "Broken pipe", "" )
pick( SIG_ERR_14,                "Alarm", "" )
pick( SIG_ERR_15,                "Abnormal termination", "" )
#endif
