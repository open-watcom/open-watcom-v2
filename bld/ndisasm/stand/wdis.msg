pick( INVALID_OPTION,           "Invalid option:",
                                "不適切なオプション:" )
pick( INVALID_I,                "Invalid use of -i option.",
                                "-iオプションの使用方法が不適切です" )
pick( ONLY_ONE_LISTING,         "Only one listing file should be specified.",
                                "リストファイルは1つだけ指定できます" )
pick( ONLY_ONE_SOURCE,          "Only one source file should be specified.",
                                "ソースファイルは1つだけ指定できます" )
pick( ONLY_ONE_OBJECT,          "Only one object file should be specified.",
                                "オブジェクトファイルは1つだけ指定できます" )
pick( NO_OBJECT,                "No object file specified.",
                                "オブジェクトファイルが指定されていません" )
pick( CANT_OPEN_SOURCE,         "Unable to open source file: ",
                                "ソースファイルをオープンできません" )
pick( NO_LINE_NUMS,             "File contains no line numbers.",
                                "ファイルに行番号がありません" )
pick( EXTERNAL_REFS,            "List of external references",
                                "外部参照のリスト" )
pick( LIST_OF_PUBLICS,          "List of public symbols\n\n",
                                "パブリックシンボルのリスト" )
pick( SYMBOL,                   "SYMBOL",
                                "シンボル" )
pick( SECTION,                  "SECTION",
                                "セクション" )
pick( OFFSET,                   "OFFSET",
                                "オフセット" )
pick( BSS_SIZE,                 "BSS Size:",
                                "BBSの大きさ:" )
pick( BYTES,                    "bytes",
                                "バイト" )
pick( ROUTINE_SIZE,             "Routine Size:",
                                "ルーチンの大きさ:" )
pick( ROUTINE_BASE,             "Routine Base:",
                                "ルーチンのベース:" )
pick( PROCEDURE_DESCRIPTOR,     "Procedure descriptor for ",
                                "プロシージャ・ディスクリプタ:" )
pick( BEGIN_ADDRESS,            "BeginAddress      ",
                                "開始アドレス      " )
pick( END_ADDRESS,              "EndAddress        ",
                                "最終アドレス      " )
pick( EXCEPTION_HANDLER,        "ExceptionHandler  ",
                                "例外ハンドラ      " )
pick( HANDLER_DATA,             "HandlerData       ",
                                "ハンドラ・データ  " )
pick( PROLOG_END,               "PrologEnd         ",
                                "プロログ・エンド  " )
pick( DIS_ERRORS,               "disassembly errors",
                                "逆アセンブル・エラー" )
pick( NO_DIS_ERRORS,            "No disassembly errors",
                                "逆アセンブル・エラーはありません" )
pick( LABEL_BEYOND_SECTION,     "The label referred to here is an undefined location",
                                "ここで参照されているラベルは位置が定義されていません" )
pick( OUT_OF_MEMORY,            "Ran out of memory in ",
                                "メモリ不足です:" )
pick( ERROR_OCCURRED,           "An error occured in ",
                                "エラーが起こりました:" )
pick( WHERE_OPENING_ORL,        "during initial reading of the object file or bad object file record",
                                "オブジェクト・ファイルのORLを初期化します" )
pick( WHERE_CLOSING_ORL,        "while closing the object file",
                                "ORLをクローズアップします" )
pick( WHERE_PRINT_SECTION,      "in printing the section",
                                "セクションをプリントします" )
pick( WHERE_CREATE_SEC_TABLES,  "creating section tables",
                                "セクション・テーブルを作成します" )
pick( WHERE_INIT_HASH_TABLES,   "initializing hash tables",
                                "ハッシュ・テーブルを初期化します" )
pick( WHERE_OBJ_ZERO_LEN,       "The object is zero length",
                                "オブジェクトの長さが0です" )
pick( WHERE_NOT_COFF_ELF,       "The object file is not in OMF, ELF or COFF format",
                                "オブジェクト・ファイルは、OMF, ELF, COFF形式ではありません:" )
pick( WHERE_UNSUPPORTED_PROC,   "Unsupported processor type.",
                                "サポートしていないプロセッサです。" )
pick( WHERE_BIG_ENDIAN,         "The object file is not in little-endian byte format.",
                                "オブジェクト・ファイルはリトル・エンディアン・倍と形式ではありません。" )
pick( WHERE_GEN_EXTERNS,        "generating externs",
                                "externを生成します" )
pick( WHERE_PASS_ONE,           "pass 1",
                                "パス1" )
pick( USAGE_1,                  "Usage:  wdis [options] obj_file [options]",
                                "使用方法:  wdis [options] obj_file [options]" )
pick( USAGE_2,                  "Options:                ( /option is also supported )",
                                "Options:                ( /optionも使えます)" )
pick( USAGE_3,                  "        -l[=list_file]  generate listing file",
                                "        -l[=list_file]  リスト･ファイルを生成します" )
pick( USAGE_4,                  "        -i=char         initial character of internal labels",
                                "        -i=char         内部ラベルの先頭文字" )
pick( USAGE_5,                  "        -a              generate assembleable output",
                                "        -a              アセンブルできる出力を生成します" )
pick( USAGE_6,                  "        -e              generate lists of externs",
                                "        -e              externのリストを生成します" )
pick( USAGE_7,                  "        -m              leave C++ names mangled",
                                "        -m              C++のマングルド名を残す" )
pick( USAGE_8,                  "        -p              generate list of publics",
                                "        -p              パブリックのリストを生成する" )
pick( USAGE_9,                  "        -s[=src_file]   include source lines",
                                "        -s[=src_file]   ソース行を含めて表示する" )
pick( USAGE_10,                 "        -ff             FPU emulator fixups as comment [80(x)86 only]",
                                "        -ff             FPU emulator fixups as comment [80(x)86 only]" )
pick( USAGE_11,                 "        -fi             use alternate indexing format [80(x)86 only]",
                                "        -fi             別のインデクシング形式を使用する[80(x)86のみ]" )
pick( USAGE_12,                 "        -fp             do not use instruction name pseudonyms",
                                "        -fp             命令名として擬似ニーモニックを使用しない" )
pick( USAGE_13,                 "        -fr             do not use register name pseudonyms [RISC only]",
                                "        -fr             レジスタ名として擬似ニーモニックを使用しない[RISCのみ]" )
pick( USAGE_LAST,               "        -fu             instructions/registers in upper case",
                                "        -fu             命令/レジスタを大文字にする" )
