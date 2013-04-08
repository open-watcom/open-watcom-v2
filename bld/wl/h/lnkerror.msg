pick(    MSG_INTERNAL,          "** internal ** - %s"  ,
                    "**  内部  ** - %s" )
pick(    MSG_LOADING_OBJECT,        "loading object files" ,
                    "オブジェクトファイルを読込み中" )
pick(    MSG_SEARCHING_LIBS,        "searching libraries" ,
                    "ライブラリを検索中" )
pick(    MSG_CREATE_MAP,        "creating map file" ,
                    "マップファイルを作成中" )
pick(    MSG_CREATE_EXE,        "creating %f" ,
                    "%f を作成中" )
pick(    MSG_USING_SPILL,       "using spill file" ,
                    "スピルファイルを使用中" )
pick(    MSG_CANT_OPEN,         "cannot open %1 : %2" ,
                    "%1 をオープン出来ません : %2" )
pick(    MSG_NO_DYN_MEM,        "dynamic memory exhausted" ,
                    "ダイナミックメモリの残量がありません" )
pick(    MSG_IO_PROBLEM,        "I/O error processing %1 : %2" ,
                    "%1 処理中にI/Oエラー : %2" )
pick(    MSG_OBJ_FILE_ATTR,     "invalid object file attribute" ,
                    "無効なオブジェクトファイル属性" )
pick(    MSG_LIB_FILE_ATTR,     "invalid library file attribute" ,
                    "無効なライブラリファイル属性" )
pick(    MSG_BREAK_HIT,         "break key detected" ,
                    "ブレークキーが押されました" )
pick(    MSG_STACK_NOT_FOUND,       "stack segment not found" ,
                    "スタックセグメントがありません" )
pick(    MSG_BAD_RELOC_TYPE,        "bad relocation type specified" ,
                    "間違った利ロケーション型が指定されています" )
pick(    MSG_BAD_ABS_FIXUP,     "%a: absolute target invalid for self-relative relocation" ,
                    "%a: 自己相対リロケーションで絶対ターゲットは無効です" )
pick(    MSG_BAD_REL_FIXUP,     "bad location specified for self-relative relocation at %a" ,
                    "自己相対リロケーションで %a のロケーションは不正です" )
pick(    MSG_FIXUP_OFF_RANGE,       "relocation offset at %a is out of range" ,
                    "%a のリロケーションオフセットが範囲外です" )
pick(    MSG_SEG_RELOC_OUT,     "segment relocation at %a" ,
                    "%a にセグメント・リロケーションがあります" )
pick(    MSG_GROUP_TOO_BIG,     "size of group %s exceeds 64k by %l bytes" ,
                    "%s グループ のサイズが %lバイト分６４Ｋを超えています" )
pick(    MSG_SEG_TOO_BIG,       "size of segment %s exceeds 64k by %l bytes" ,
                    "%s セグメントのサイズが %lバイト分６４Ｋを超えています" )
pick(    MSG_CANT_HAVE_START,       "cannot have a starting address with an imported symbol" ,
                    "インポート・シンボルを開始アドレスにはできません" )
pick(    MSG_NO_START_ADDR,     "no starting address found, using %a" ,
                    "開始アドレスが見つかりません.%aを開始アドレスとします" )
pick(    MSG_NO_OVERLAY_LOADER,     "missing overlay loader" ,
                    "オーバレイ・ローダがありません" )
pick(    MSG_VECT_RANGE,      "%s vector %d is out of range" ,
                    "%s vector %d is out of range" )
pick(    MSG_RESERVED_SYM_DEFINED,      "redefinition of reserved symbol %s" ,
                    "予約シンボル %s を再定義しました" )
pick(    MSG_MULT_DEF,          "redefinition of %S ignored" ,
                    "%S の再定義を無視しました" )
pick(    MSG_UNDEF_REF,         "%S is an undefined reference" ,
                    "%S は未定義参照です" )
pick(    MSG_EARLY_EOF,         "premature end of file encountered" ,
                    "途中でＥＯＦを検出しました" )
pick(    MSG_MULT_START_ADDRS,      "multiple starting addresses found" ,
                    "複数の開始アドレスを発見しました" )
pick(    MSG_SEG_IN_TWO_GROUPS,     "segment %1 is in group %2 and group %3" ,
                    "セグメント %1 が %2 グループと %3 グループの両方に存在します" )
pick(    MSG_REC_NOT_DONE,      "record (type 0x%x) not processed" ,
                    "レコード (ﾀｲﾌﾟ0x%x) は処理されません" )
pick(    MSG_DIRECTIVE_ERR,     "directive error near '%s'" ,
                    "'%s'付近にディレクティブエラーがあります" )
pick(    MSG_NO_OFFSET_WITH_DLL,        "%a: cannot have an offset with an imported symbol" ,
                    "%a: インポート・シンボルをオフセットには指定出来ません" )
pick(    MSG_EXE_NAME,          "Executable Image: %s" ,
                    "実行可能イメージ: %s" )
pick(    MSG_CREATED_ON,        "Created on:       %1 %2" ,
                    "新規作成:       %1 %2" )
pick(    MSG_UNDEF_SYM,         "file %1(%2): undefined symbol %S" ,
                    "ファイル %1(%2):　未定義シンボル %S" )
pick(    MSG_DEBUG_AFTER_FILES,     "DEBUG directive appears after object files" ,
                    "オブジェクトファイル指定の後にDEBUGディレクティブがあります" )
pick(    MSG_ALIGN_TOO_SMALL,       "ALIGNMENT value too small" ,
                    "ALIGNMENT値が小さすぎます" )
pick(    MSG_IMPORT_ORD_INVALID,        "ordinal in IMPORT directive not valid" ,
                    "IMPORTディレクティブの序数が不正です" )
pick(    MSG_EXPORT_ORD_INVALID,        "ordinal in EXPORT directive not valid" ,
                    "EXPORTディレクティブの序数が不正です" )
pick(    MSG_TOO_MANY_IOPL_WORDS,   "too many IOPL words in EXPORT directive" ,
                    "EXPORTディレクティブのIOPLワードが多すぎます" )
pick(    MSG_DUP_EXP_ORDINAL,       "duplicate exported ordinal" ,
                    "エクスポート序数が重複しています" )
pick(    MSG_EXP_SYM_NOT_FOUND,     "exported symbol %s not found" ,
                    "エクスポート・シンボル %s が見つかりません" )
pick(    MSG_SEG_FLAG_MULT_DEFD,        "segment attribute defined more than once" ,
                    "セグメント属性が２回以上定義されています" )
pick(    MSG_SEG_NAME_NOT_FOUND,        "segment name %s not found" ,
                    "セグメント名 %s が見つかりません" )
pick(    MSG_CLASS_NAME_NOT_FOUND,      "class name %s not found" ,
                    "クラス名 %s が見つかりません" )
pick(    MSG_AUTO_SEG_MULT_DEFD,        "inconsistent attributes for automatic data segment" ,
                    "オートマティック・データセグメントの属性が矛盾しています" )
pick(    MSG_INV_STUB_FILE,     "invalid STUB file" ,
                    "無効なSTUBファイルです" )
pick(    MSG_INV_OLD_DLL,       "invalid DLL specified in OLDLIBRARY option" ,
                    "OLDLIBRARYオプションで無効なDLLが指定されています" )
pick(    MSG_STUB_SAME_AS_LOAD,     "STUB file name same as executable file name" ,
                    "STUBファイル名が実行可能ファイル名と同じです" )
pick(    MSG_REL_NOT_SAME_SEG,      "relocation at %a not in the same segment" ,
                    "%a のリロケーションが同一セグメントではありません" )
pick(    MSG_DLL_IN_REL_RELOC,      "%a: cannot reach a DLL with a relative relocation" ,
                    "%a: relativeリロケーションでDLLとつながりません" )
pick(    MSG_CANT_USE_LOCALS,       "debugging information incompatible: using line numbers only" ,
                    "デバッグ情報に互換性がありません: 行番号のみを使用して下さい" )
pick(    MSG_FRAME_EQ_TARGET,       "%a: frame must be the same as the target in protect mode" ,
                    "%a: プロテクトモードではターゲットとフレームが同一でなければなりません" )
pick(    MSG_CANT_FIND_MEMBER,      "cannot find library member %1(%2)" ,
                    "ライブラリメンバ %1(%2)が見つかりません" )
pick(    MSG_MULTIPLE_MODES_FOUND,      "executable format has been established" ,
                    "実行可能フォーマットは既に設定されています" )
pick(    MSG_FORMAT_BAD_OPTION,     "%s option not valid for %f" ,
                    "%sオプションが%fに対して無効です" )
pick(    MSG_VALUE_TOO_LARGE,       "value for %s too large" ,
                    "%s の値が大きすぎます" )
pick(    MSG_VALUE_INCORRECT,       "value for %s incorrect" ,
                    "%s の値が間違っています" )
pick(    MSG_MULT_REALBREAKS,       "multiple values specified for REALBREAK" ,
                    "REALBREAKに複数の値が設定されています" )
pick(    MSG_DLL_WITH_386,      "export and import records not valid for %f" ,
                    "DLL COMENTレコードはOS2以外では無効です" )
pick(    MSG_INVALID_FLAT_RELOC,        "invalid relocation for flat memory model at %a" ,
                    "%aはフラット・メモリモデルに対して無効なリロケーションです" )
pick(    MSG_CANT_COMBINE_32_AND_16,    "cannot combine 32-bit segments (%1) with 16-bit segments (%2)",
                    "32ビット・セグメントと16ビット・セグメントを結合させることはできません %1, %2" )
pick(    MSG_BREAKSYM_NOT_FOUND,    "REALBREAK symbol %s not found" ,
                    "REALBREAKシンボル %s が見つかりません" )
pick(    MSG_BAD_IMP_REL_RELOC,     "invalid relative relocation type for an import at %a" ,
                    "%a インポートの relativeリロケーション・タイプが無効です" )
pick(    MSG_NOV_NO_CODE_DATA_RELOC,    "%a: cannot relocate between code and data in Novell formats" ,
                    "%a: ノベル・フォーマットではコードとデータ間のリロケーションが出来ません" )
pick(    MSG_ABS_SEG_FIXUP_BAD,     "absolute segment fixup not valid in protect mode" ,
                    "absoluteセグメントのフィックスアップはプロテクトモードでは無効です" )
pick(    MSG_CHECK_NOT_FOUND,       "unload CHECK procedure not found" ,
                    "CHECKプロシージャが見つかりません" )
pick(    MSG_START_PROC_NOT_FOUND,      "START procedure not found" ,
                    "STARTプロシージャが見つかりません" )
pick(    MSG_EXIT_PROC_NOT_FOUND,       "EXIT procedure not found" ,
                    "EXITプロシージャが見つかりません" )
pick(    MSG_NO_SECTION_IN_ROOT,        "SECTION directive not allowed in root" ,
                    "ルートにSECTIONディレクティブが指定されています" )
pick(    MSG_BAD_NOV_FILE_TYPE,     "bad Novell file format specified" ,
                    "不正なノベルのファイル・フォーマットを指定しました" )
pick(    MSG_CIRCULAR_ALIAS_FOUND,      "circular alias found for %s",
                    "%sに循環するエイリアスがあります" )
pick(    MSG_EXPECTING_END,     "expecting an END directive" ,
                    "ENDディレクティブがありません" )
pick(    MSG_OPTION_MULTIPLY_DEFD,      "%s option multiply specified" ,
                    "%s オプションが複数指定されています" )
pick(    MSG_NOT_MULTIPLE_OF_8,     "%s is not a multiple of 8" ,
                    "%s は8の倍数ではありません" )
pick(    MSG_BOTH_RELOC_OPTIONS,        "both relocation options specified" ,
                    "両方のリロケーション・オプションが指定されています" )
pick(    MSG_TRANS_RELOCS_NEEDED,       "relocations needed for transparent data" ,
                    "トランスペアレント・データのリロケーションが必要とされています" )
pick(    MSG_FOUND_XXBIT_OBJ,       "file %s is a %d-bit object file" ,
                    "%s ファイルは32ビットのオブジェクトファイルです" )
pick(    MSG_TOO_MANY_SELECTORS,        "too many selectors needed" ,
                    "セレクタが多すぎます" )
pick(    MSG_BAD_REC_TYPE,      "invalid record type 0x%x" ,
                    "レコードタイプ0x%x が無効です" )
pick(    MSG_FRAME_INVALID,     "cannot reference address %A from frame %x" ,
                    "%A アドレスへの %x フレームからの参照は出来ません" )
pick(    MSG_BAD_TARG_OFF,      "target offset exceeds 64K at %a" ,
                    "%a のターゲットオフセットが64Kを越えています" )
#ifdef __UNIX__
pick(    MSG_PRESS_CTRL_Z,      "Press CTRL/D to finish" ,
                    "終了はCTRL/Dを押して下さい" )
#else
pick(    MSG_PRESS_CTRL_Z,      "Press CTRL/Z and then RETURN to finish" ,
                        "終了はCTRL/Zとﾘﾀｰﾝ･ｷｰを押して下さい" )
#endif
pick(    MSG_INV_COM_START_ADDR,        "invalid starting address for .COM file" ,
                    ".COMファイルに対して無効な開始アドレスです" )
pick(    MSG_STACK_SEG_IGNORED,     "stack segment ignored in .COM file" ,
                    ".COMファイルのスタックセグメントを無視します" )
pick(    MSG_NO_VIRT_MEM,       "virtual memory exhausted" ,
                    "仮想メモリ残量がありません" )
pick(    MSG_COM_TOO_LARGE,     "program too large for a .COM file" ,
                    ".COMファイルに対してプログラムが大きすぎます" )
pick(    MSG_MULT_DEF_BY,       "redefinition of %1 by %2 ignored" ,
                    "%1の%2による再定義を無視します" )
pick(    MSG_OVL_GROUP_SPLIT,       "group %s is in more than one overlay" ,
                    "%sグループが複数のオーバレイにまたがっています" )
pick(    MSG_NEWSEG_BEFORE_OBJ,     "NEWSEGMENT directive appears before object files" ,
                    "NEWSEGMENTディレクティブがオブジェクトファイルの前に指定されています" )
pick(    MSG_CANT_OPEN_NO_REASON,       "cannot open %s" ,
                    "%sをオープンできません" )
pick(    MSG_IO_PROBLEM_NO_REASON,      "I/O error processing %s" ,
                    "%s 処理中にI/Oエラーが発生しました" )
pick(    MSG_DEBUG_TOO_LARGE,       "%s debugging information too large" ,
                    "%s デバッグ情報が大きすぎます" )
pick(    MSG_INCOMPATIBLE_DBI_FOUND,    "incompatible types of debugging information found" ,
                    "互換性のないタイプのデバッグ情報があります" )
pick(    MSG_TOO_MANY_LIB_MODS,     "too many library modules" ,
                    "ライブラリモジュールが多すぎます" )
pick(    MSG_OFFSET_MUST_BE_ALIGNED,    "Offset option must be a multiple of %l" ,
                    "オフセットオプションは%lの倍数でなければなりません" )
pick(    MSG_SYMBOL_NAME_TOO_LONG,      "symbol name too long: %s" ,
                    "シンボル名が長すぎます: %s" )
pick(    MSG_MOD_TRACE,                 "%S referenced by module %s" ,
                    "%S が %sモジュールで参照されています" )
pick(    MSG_INV_INC_FILE,      "invalid incremental information file",
                    "不適切なインクリメンタル情報ファイルです" )
pick(    MSG_TRACE_OBJ_NOT_FOUND,       "object file %s not found for tracing" ,
                    "トレースするオブジェクトファイル%sが見つかりません" )
pick(    MSG_TRACE_LIB_NOT_FOUND,       "library module %1(%2) not found for tracing" ,
                    "トレースするライブラリモジュール %1(%2)が見つかりません" )
pick(    MSG_MOD_IND_TRACE,     "%S referenced indirectly by module %s" ,
                    "%S がモジュール %s で間接参照されています" )
pick(    MSG_CANT_RESERVE_SPACE,        "cannot reserve %l bytes of extra overlay space" ,
                    "%l バイトのエクストラ・オーバレイ・スペースが確保できません" )
pick(    MSG_SYSTEM_UNDEFINED,      "undefined system name: %s" ,
                    "システム名が未定義です: %s" )
pick(    MSG_SYSTEM_ALREADY_DEFINED,    "system %s defined more than once" ,
                    "システム %s が２回以上定義されています" )
pick(    MSG_QNX_BASE_LT_STACK,     "OFFSET option is less than the stack size" ,
                    "OFFSETオプションはスタックサイズより小さいです" )
pick(    MSG_NO_MEMB_IN_LIBFILE,        "library members not allowed in libfile" ,
                    "LIBFILEにライブラリメンバは指定できません" )
pick(    MSG_ERROR_IN_SYSTEM_BLOCK,     "error in default system block" ,
                    "デフォルトのシステムブロックにエラーがあります" )
pick(    MSG_NO_PREVIOUS_INPUT,     "no previous input source" ,
                    "入力ソースが指定されていません" )
pick(    MSG_NO_INPUT_LEFT,     "no more input found for ^ command" ,
                    "^ コマンドの指定が不完全です" )
pick(    MSG_ENV_NAME_INCORRECT,        "environment name specified incorrectly" ,
                    "環境名の指定が不正確です" )
pick(    MSG_ENV_NOT_FOUND,     "environment name %s not found" ,
                    "環境名 %s が見つかりません" )
pick(    MSG_AREA_TOO_SMALL,        "overlay area must be at least %l bytes" ,
                    "オーバレイ・エリアが少なくとも%lバイト必要です" )
pick(    MSG_BAD_MOVABLE_SEG_NUM,       "segment number too high for a movable entry point" ,
                    "movableエントリ・ポイントのセグメント番号が大きすぎます" )
pick(    MSG_HEAP_TOO_BIG,      "heap size too large" ,
                    "ヒープサイズが大きすぎます" )
pick(    MSG_BAD_WLIB_IMPORT,       "wlib import statement incorrect" ,
                    "wlibのインポート・ステートメントが不正確です" )
pick(    MSG_APP_TOO_BIG_FOR_DOS,       "application too large to run under DOS" ,
                    "DOS上で動作させるにはアプリケーションが大きすぎます" )
pick(    MSG_DUP_EXP_NAME,      "'%s' has already been exported" ,
                    "'%s' は既にエクスポートされています" )
pick(    MSG_NO_FILES_FOUND,        "no FILE directives found" ,
                    "FILEディレクティブが見つかりません" )
pick(    MSG_OVERLAYS_NOT_SUPPORTED,    "overlays are not supported in this version of WLINK" ,
                    "オーバーレイはこのバージョンのWLINKではサポートされていません" )
pick(    MSG_LAZY_EXTDEF_MISMATCH,      "lazy reference for %S has different default resolutions" ,
                    "%Sの参照がデフォルトの解決と異なります" )
pick(    MSG_MULTIPLE_ALIASES,      "multiple aliases found for %S" ,
                    "%Sに複数のエリアスがあります" )
pick(    MSG_EXE_DATE_CHANGED,      "%s has been modified: doing full relink" ,
                    "%sが修正されました: 完全な再リンクをします" )
pick(    MSG_CANT_EXPORT_ABSOLUTE,  "cannot export symbol %S" ,
                    "シンボル%Sをエクスポートできません" )
pick(    MSG_DIRECTIVE_ERR_BEGINNING,   "directive error near beginning of input" ,
                    "入力開始付近にディレクティブ・エラーがあります" )
pick(    MSG_ADDR_INFO_TOO_LARGE,   "address information too large" ,
                    "アドレス情報が大きすぎます" )
pick(    MSG_INV_SHARED_NLM_FILE,   "%s is an invalid shared NLM file" ,
                    "%s は無効な共有ＮＬＭファイルです" )
pick(    MSG_CANT_OPEN_SPILL,       "cannot open temporary file: file already exists" ,
                    "スピル・ファイルをオープンできません: 既に存在しています" )
pick(    MSG_BAD_CURLY_LIST,        "curly brace delimited list incorrect" ,
                    "大括弧で閉じられたリストが不正確です" )
pick(    MSG_NO_REALBREAK_WITH_16BIT,   "no realbreak specified for 16-bit code" ,
                    "16ビット・コードのリアルブレイクが指定されていません" )
pick(    MSG_INV_MESSAGE_FILE,      "%s is an invalid message file" ,
                    "%s は無効なメッセージ・ファイルです" )
pick(    MSG_INCORRECT_NUM_AREAS,   "need exactly 1 overlay area with dynamic overlay manager" ,
                    "ダイナミック・オーバレイ・マネージャのオーバレイ領域が必要です" )
pick(    MSG_RELOC_TO_RWDATA_SEG,   "segment relocation to a read/write data segment found at %a(%S)" ,
                    "%a(%S) に READ/WRITE データセグメントへのリロケーションがあります" )
pick(    MSG_TOO_MANY_ERRORS,       "too many errors encountered" ,
                    "エラーが多すぎます" )
pick(    MSG_INV_FILENAME,      "invalid filename '%s'" ,
                    "無効なファイル名 '%s'" )
pick(    MSG_CANNOT_HAVE_16_AND_32, "cannot have both 16-bit and 32-bit object files" ,
                    "16ビットと32ビットの両方のオブジェクト・ファイルは持てません" )
pick(    MSG_INVALID_MSG_NUM,       "invalid message number" ,
                    "不適切なメッセージ番号です" )
pick(    MSG_VF_TABLE_MISMATCH,     "virtual function table record for %S mismatched" ,
                    "%S の仮想関数テーブルレコードが一致しません" )
pick(    MSG_CANT_SORT_SYMBOLS,     "not enough memory to sort map file symbols" ,
                    "マップファイルのシンボルをソートするためのメモリがありません" )
pick(    MSG_SYMBOL_DEAD,       "symbol %S removed" ,
                    "シンボル %S は削除されました" )
pick(    MSG_VF_PURE_MISMATCH,      "%S is both pure virtual and non-pure virtual" ,
                    "%S は pure virtual と non-pure virtual の両方です" )
pick(    MSG_BAD_OBJECT,        "%s is an invalid object file" ,
                    "%s は正しくないオブジェクトファイルです" )
pick(    MSG_AMBIG_FORMAT,      "ambiguous format specified" ,
                    "形式の指定が曖昧です" )
pick(    MSG_INVALID_TYPE_DESC,     "invalid segment type specified" ,
                    "不適切なセグメント・タイプの指定です" )
pick(    MSG_MULT_DBI_FORMATS,      "only one debugging format can be specified" ,
                    "複数のデバッグ形式の指定はできません" )
pick(    MSG_MACHTYPE_DIFFERENT,    "file %s has code for a different processor",
                    "ファイル %s には異なるプロセッサ用のコードが入っています" )
pick(    MSG_NO_BIG_ENDIAN,     "big endian code not supported",
                    "ビッグ・エンディアン・コードはサポートされていません" )
pick(    MSG_NO_DICT_FOUND,     "no dictionary found",
                    "辞書(ﾃﾞｨｸｼｮﾅﾘ)が見つかりません" )
pick(    MSG_CANT_EXECUTE,      "cannot execute %1 : %2" ,
                    "実行できません %1 : %2" )
pick(    MSG_REL_NOT_ALIGNED,       "relocation at %a to an improperly aligned target" ,
                    "%a において不適切な整列(ｱﾗｲﾝﾒﾝﾄ)のターゲットへのリロケーションがあります" )
pick(    MSG_INC_NEAR_START,        "OPTION INCREMENTAL must be one of the first commands specified " ,
                    "OPTION INCREMENTALを最初のコマンドとして指定しなければなりません" )
pick(    MSG_NO_CODE_OR_DATA,       "no code or data present" ,
                    "コードまたはデータがありません" )
pick(    MSG_PROBLEM_IN_RESOURCE,       "problem adding resource information" ,
                    "リソース情報の追加に問題があります" )
pick(    MSG_INC_ONLY_SUPPORTS_DWARF,   "incremental linking only supports DWARF debugging information" ,
                    "インクリメンタル･リンクはDWARFデバッグ情報のみをサポートします" )
pick(    MSG_INC_AND_DCE_NOT_ALLOWED,   "incremental linking does not support dead code elimination" ,
                    "インクリメンタル･リンクはデッド･コードの削除をサポートしません" )
pick(    MSG_TOC_TOO_BIG,               "Table of Contents too big to fit into 64K",
                    "コンテンツのテーブルは64Kより大きすぎます" )
pick(    MSG_REL_IN_LIDATA,             "relocations on iterated data not supported",
                    "繰り返しデータの再配置はサポートしていません" )
pick(    MSG_NOT_COMPILED_VF_ELIM,      "module has not been compiled with -zv",
                    "モジュールは-zvオプションをつけてコンパイルされていません" )
pick(    MSG_INC_AND_VFR_NOT_ALLOWED,   "incremental linking does not support virtual function removal",
                    "インクリメンタル･リンクは仮想関数の削除をサポートしていません" )
pick(    MSG_RESOURCE_TOO_BIG,      "resource file %s too big",
                    "リソースファイル %s は大きすぎます" )
pick(    MSG_MULT_START_ADDRS_BY,   "both %1 and %2 marked as starting symbols" ,
                    "%1と%2の両方は開始シンボルとしてマークされています" )
pick(    MSG_INTERNAL_MOD_NAME_DIFF_FROM_FILE,  "NLM internal name (%s) truncated" ,
                    "NLM internal name (%s) truncated" )
pick(    MSG_VXD_INCORRECT_EXPORT,  "exactly one export must exist for VxD format" ,
                    "exactly one export must exist for VxD format" )
pick(    MSG_FIXED_LOC_BEFORE_CUR_LOC,  "location counter already beyond fixed Segment address %a" ,
                     "location counter already beyond fixed segment address %a" )
pick(    MSG_DUP_DIRECTIVE,         "directive %s can only occur once" ,
                     "directive %s can only occur once" )
pick(    MSG_IMPORT_LOCAL,          "locally defined symbol %s imported" ,
                     "locally defined symbol %s imported" )
pick(    MSG_STACK_SMALL,           "stack size is less than %d bytes" ,
                     "stack size is less than %d bytes" )
pick(    MSG_DEFDATA_TOO_BIG,       "default data segment exceeds maximum size by %l bytes" ,
                    "default data segment exceeds maximum size by %l bytes" )
