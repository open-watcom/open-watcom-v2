pick( ERR_BAD_LIBRARY,      "'%s' is an invalid library",
                "'%s'は不適切なライブラリです" )
pick( ERR_BAD_OBJECT,       "'%s' is an invalid object file",
                "'%s'は不適切なオブジェクト・ファイルです" )
pick( ERR_BAD_DLL,      "'%s' is an invalid dll",
                "'%s'不適切なDLLです" )
pick( ERR_CANT_FIND,        "Cannot find file '%s'",
                "ファイル'%s'が見つかりません" )
pick( ERR_CREATING_LIBRARY, "Cannot open '%s' - library will be created",
                "'%s'をオープンできません - ライブラリを作成します" )
pick( ERR_DIR_NOT_EXIST,    "Directory '%s' does not exist",
                "ディレクトリ'%s'は存在しません" )
pick( ERR_DUPLICATE_OPTION, "Duplicate option near '%s'",
                "'%s'の付近のオプションは2度指定されています" )
pick( ERR_DUPLICATE_SYMBOL, "Duplicate symbol '%s' ignored.",
                "２重のシンボル'%s'を無視します" )
pick( ERR_CANT_READ,        "Error reading '%s'. %s.",
                "読み込みエラー '%s'. %s." )
pick( ERR_CANT_WRITE,       "Error writing '%s'. %s.",
                "書き込みエラー '%s'. %s." )
pick( ERR_DELETE_AND_CREATE,    "Files can not be extracted using the -n option",
                "-nオプションを指定するとファイルを取り出すことはできません" )
pick( ERR_BAD_CMDLINE,      "Invalid command line near '%s'",
                "'%s'の付近のコマンドラインの指定が不適切です" )
pick( ERR_BAD_OPTION,       "Invalid option '-%c'",
                "不適切なオプション '-%c'" )
pick( ERR_NO_LIBNAME,       "Library name required on command line",
                "コマンドラインでライブラリ名を指定してください" )
pick( ERR_TWO_LIBNAMES,     "Library name specified twice near '%s'",
                "ライブラリ名が'%s'の付近で２度指定されています" )
pick( ERR_NO_MEMORY,        "Out of memory",
                "メモリ不足です" )
pick( ERR_CANT_DELETE,      "Unable to find module '%s' for deletion",
                "削除するモジュール'%s'が見つかりません" )
pick( ERR_CANT_EXTRACT,     "Unable to find module '%s' for extraction",
                "取り出すモジュール'%s'が見つかりません" )
pick( ERR_CANT_OPEN,        "Unable to open file '%s'. %s.",
                "ファイルをオープンできません '%s'. %s." )
pick( ERR_CANT_REMOVE,      "Unable to remove file '%s'",
                "ファイル'%s'を削除できません" )
pick( ERR_CANT_REPLACE,     "Unable to replace contents of file '%s'. %s",
                "ファイルの内容を置き換えられません '%s'. %s" )
pick( ERR_NO_SYMBOLS,       "Library contains no external symbols",
                "ライブラリの中には外部シンボルが入っていません" )
pick( ERR_NOT_LIB,      "Cannot add %s object to %s library",
                "%s オブジェクトをLIBライブラリに追加できません" )
pick( ERR_CANT_DO_IMPORT,   "Unable to create %s import library from '%s'",
                "%sインポート・ライブラリを'%s'から作れません" )
pick( ERR_NO_PROCESSOR,     "No processor specified.  Using %s",
                "プロセッサが指定されていません。%sとして扱います" )
pick( ERR_NO_TYPE,      "No import type specified.  Using %s",
                "インポートの方が指定されていません。%sとして扱います" )
pick( ERR_NO_EXPORTS,       "No exports in '%s'",
                "'%s'にエクスポートはありません" )
pick( ERR_PAGE_RANGE,       "Page size out of range",
                "ページの大きさは範囲を超えています" )
pick( ERR_MIXED_OBJ,        "Can not put %s and %s objects in same library",
                "オブジェクト%sと%sを同じライブラリに入れられません" )
pick( ERR_LIB_TOO_LARGE,    "Library too large.  Recommend splitting the library into two or trying a page_bound larger than %s.",
                "ライブラリが大きすぎます。ライブラリを２つに分割するか、%sよりpage_boundをもっと大きくすることを推奨します。" )
pick( USAGE_WLIB_FIRST,     "wlib  { <option> } <library_name> [ <commands> ]" ,
                "wlib  { <option> } <library_name> [ <commands> ]" )
pick( USAGE_WLIB_01,        "Where:" ,
                "各フィールドは以下の通りです:" )
pick( USAGE_WLIB_02,        "  <option>    ::= -b                       (don't create .bak file)" ,
                "  <option>    ::= -b                       (.bakファイルを作成しません)" )
pick( USAGE_WLIB_03,        "                | -c                       (case sensitive)" ,
                "                | -c                       (大文字小文字を区別します)" )
pick( USAGE_WLIB_04,        "                | -d = <object_output_directory>" ,
                "                | -d = <object_output_directory>" )
pick( USAGE_WLIB_05,        "                | -l [ = <list_file_name> ]" ,
                "                | -l [ = <list_file_name> ]" )
pick( USAGE_WLIB_06,        "                | -m                       (display C++ mangled names)" ,
                "                | -m                       (C++のマングルド名を表示します)" )
pick( USAGE_WLIB_07,        "                | -n                       (always create a new library)" ,
                "                | -n                       (常に新しいライブラリを作成します)" )
pick( USAGE_WLIB_08,        "                | -o = <out_library_name>" ,
                "                | -o = <out_library_name>" )
pick( USAGE_WLIB_09,        "                | -p = <record_size>       (set OMF library page size)" ,
                "                | -p = <record_size>       (set OMF library page size)" )
pick( USAGE_WLIB_10,        "                | -q                       (don't print header)" ,
                "                | -q                       (起動メッセージを表示しません)" )
pick( USAGE_WLIB_11,        "                | -s                       (strip line number debug info)" ,
                "                | -s                       (strip line number debug info)" )
pick( USAGE_WLIB_12,        "                | -t                       (trim THEADR pathnames)" ,
                "                | -t                       (THEADR パス名を削除します)" )
pick( USAGE_WLIB_13,        "                | -v                       (print header)" ,
                "                | -v                       (print header)" )
pick( USAGE_WLIB_14,        "                | -x                       (explode all objects in library)" ,
                "                | -x                       (ライブラリからすべてのオブジェクトを取り出します)" )
pick( USAGE_WLIB_15,        "                | -zld                     (strip file dependency info)" ,
                "                | -zld                     (strip file dependency info)" )
pick( USAGE_WLIB_16,        "                | -h                       (display this screen)" ,
                "                | -h                       (この画面を表示します)" )
pick( USAGE_WLIB_17,        "                | -?                       (display this screen)" ,
                "                | -?                       (この画面を表示します)" )
pick( USAGE_WLIB_18,        "" ,
                "" )
pick( USAGE_WLIB_19A,       "                | -fa                      (output AR format library)" ,
                "                | -fa                      (AR形式ﾗｲﾌﾞﾗﾘを出力します)" )
pick( USAGE_WLIB_19B,       "                | -fab                     (output AR format (BSD) library)" ,
                "                | -fab                     (AR形式ﾗｲﾌﾞﾗﾘを出力します)" )
pick( USAGE_WLIB_19C,       "                | -fac                     (output AR format (COFF) library)" ,
                "                | -fac                     (AR形式ﾗｲﾌﾞﾗﾘを出力します)" )
pick( USAGE_WLIB_19D,       "                | -fag                     (output AR format (GNU) library)" ,
                "                | -fag                     (AR形式ﾗｲﾌﾞﾗﾘを出力します)" )
pick( USAGE_WLIB_20,        "                | -fm                      (output MLIB format library)" ,
                "                | -fm                      (MLIB形式ライブラリを出力します)" )
pick( USAGE_WLIB_21,        "                | -fo                      (output OMF format library)" ,
                "                | -fo                      (OMF形式ライブラリを出力します)" )
pick( USAGE_WLIB_22,        "                | -i<r|n><o|n>      (resident|non-resident table)(ordinal|name)" ,
                "                | -i<r|n><o|n>      (r:ﾚｼﾞﾃﾞﾝﾄ|n:非ﾚｼﾞﾃﾞﾝﾄﾃｰﾌﾞﾙ)(o:序数|n:名前)" )
pick( USAGE_WLIB_23,        "                | -ia                      (generate AXP import records)" ,
                "                | -ia                      (AXPｲﾝﾎﾟｰﾄ･ﾚｺｰﾄﾞを生成します)" )
pick( USAGE_WLIB_24,        "                | -ii                      (generate X86 import records)" ,
                "                | -ii                      (X86インポート･レコードを生成します)" )
pick( USAGE_WLIB_25,        "                | -ip                      (generate PPC import records)" ,
                "                | -ip                      (PPCインポート･レコードを生成します)" )
pick( USAGE_WLIB_26,        "                | -ie                      (generate ELF import records)" ,
                "                | -ie                      (ELFインポート･レコードを生成します)" )
pick( USAGE_WLIB_27,        "                | -ic                      (generate COFF import records)" ,
                "                | -ic                      (COFFｲﾝﾎﾟｰﾄ･ﾚｺｰﾄﾞを生成します)" )
pick( USAGE_WLIB_28,        "                | -io                      (generate OMF import records)" ,
                "                | -io                      (OMFインポート･レコードを生成します)" )
pick( USAGE_WLIB_29,        "" ,
                "" )
pick( USAGE_WLIB_30,        "  <commands>  ::= <cmd> <commands>" ,
                "  <commands>  ::= <cmd> <commands>" )
pick( USAGE_WLIB_31,        "                | @<env_var> <commands>" ,
                "                | @<環境変数> <commands>" )
pick( USAGE_WLIB_32,        "                | @<cmd_file> <commands>" ,
                "                | @<cmd_file> <commands>" )
pick( USAGE_WLIB_33,        "" ,
                "" )
pick( USAGE_WLIB_34,        "  <cmd_file>  ::= file which contains { <cmd> }" ,
                "  <cmd_file>  ::= { <cmd> }を含むファイル" )
pick( USAGE_WLIB_35,        "" ,
                "" )
pick( USAGE_WLIB_36,        "  <cmd>       ::= +<object_file_name>      (add file)" ,
                "  <cmd>       ::= +<ｵﾌﾞｼﾞｪｸﾄ･ﾌｧｲﾙ名>       (ファイルの追加)" )
pick( USAGE_WLIB_37,        "                | +<library_file_name>.lib (add library)" ,
                "                | +<ﾗｲﾌﾞﾗﾘ･ﾌｧｲﾙ名>.lib     (ライブラリの追加)" )
pick( USAGE_WLIB_38,        "                | ++<symbol_name>.<DLL_name>[.<export_name>][.<ordinal>]",
                "                | ++<シンボル名>.<DLL名>[.<エクスポート名>][.<序数>]" )
pick( USAGE_WLIB_39,        "                | -<module_name>           (delete)" ,
                "                | -<モジュール名>          (削除)" )
pick( USAGE_WLIB_40,        "                | -+<object_file_name>     (replace)" ,
                "                | -+<ｵﾌﾞｼﾞｪｸﾄ･ﾌｧｲﾙ名>      (入れ替え)" )
pick( USAGE_WLIB_41,        "                | +-<object_file_name>     (replace)" ,
                "                | +-<ｵﾌﾞｼﾞｪｸﾄ･ﾌｧｲﾙ名>      (入れ替え)" )
#ifdef RC_UNIX
pick( USAGE_WLIB_42,        "                | :<module_name>[=<object_file_name>]  (extract)" ,
                "                | *<ｵﾌﾞｼﾞｪｸﾄ･ﾌｧｲﾙ名>       (取り出し)" )
#else
pick( USAGE_WLIB_42,        "                | *<module_name>[=<object_file_name>]  (extract)" ,
                "                | *<ｵﾌﾞｼﾞｪｸﾄ･ﾌｧｲﾙ名>       (取り出し)" )
#endif
#ifdef RC_UNIX
pick( USAGE_WLIB_43,        "                | -:<module_name>[=<object_file_name>] (extract and delete)" ,
                "                | -*<ｵﾌﾞｼﾞｪｸﾄ･ﾌｧｲﾙ名>      (取り出しと削除)" )
#else
pick( USAGE_WLIB_43,        "                | -*<module_name>[=<object_file_name>] (extract and delete)" ,
                "                | -*<ｵﾌﾞｼﾞｪｸﾄ･ﾌｧｲﾙ名>      (取り出しと削除)" )
#endif
#ifdef RC_UNIX
pick( USAGE_WLIB_44,        "                | :-<module_name>[=<object_file_name>] (extract and delete)" ,
                "                | *-<ｵﾌﾞｼﾞｪｸﾄ･ﾌｧｲﾙ名>      (取り出しと削除)" )
#else
pick( USAGE_WLIB_44,        "                | *-<module_name>[=<object_file_name>] (extract and delete)" ,
                "                | *-<ｵﾌﾞｼﾞｪｸﾄ･ﾌｧｲﾙ名>      (取り出しと削除)" )
#endif
pick( USAGE_WLIB_45,        "" ,
                "" )
#ifdef RC_UNIX
pick( USAGE_WLIB_46,        "The default filename extensions are lib, bak, lst, o and lbc." ,
                "デフォルトのファイル拡張子は、lib, bak, lst, obj, lbcです。" )
#else
pick( USAGE_WLIB_46,        "The default filename extensions are lib, bak, lst, obj and lbc." ,
                "デフォルトのファイル拡張子は、lib, bak, lst, obj, lbcです。" )
#endif
pick( USAGE_WLIB_LAST,      "Enclose a file name with quotes, i.e. 'filename', if it contains '-' or '@'.",
                "ﾌｧｲﾙ名に'-'や'@'が含まれるときは、'filename'のように引用符でﾌｧｲﾙ名を囲みます" )
pick( USAGE_AR_FIRST,       "ar  -d [-v] archive file..." ,
                "ar  -d [-v] アーカイブ名 ファイル名..." )
pick( USAGE_AR_1,       "ar  -r [-cuv] archive file..." ,
                "ar  -r [-cuv] アーカイブ名 ファイル名..." )
pick( USAGE_AR_2,       "ar  -t [-v] archive [file...]" ,
                "ar  -t [-v] アーカイブ名 [ファイル名...]" )
pick( USAGE_AR_LAST,        "ar  -x [-v] archive [file...]" ,
                "ar  -x [-v] アーカイブ名 [ファイル名...]" )
