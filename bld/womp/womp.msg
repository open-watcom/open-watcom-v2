pick( MSG_UNS_MEMORY_MODEL,	"Unsupported memory model" ,
				"サポートされていないメモリモデルです" )				
pick( MSG_UNS_FLOAT_OPTION,	"Unsupported floating-point option" ,
				"サポートされていない浮動小数点オプションです" )
pick( MSG_UNS_SCOPE,		"Unknown SCOPE (%s)" ,
				"未知のSCOPE (%s)が検出されました" )				
pick( MSG_UNS_ARRAY_DESC,	"ARRAY_DESC types not supported" ,
				"ARRAY_DESCタイプはサポートされていません" )				
pick( MSG_UNS_CHARBLOCK_IND,	"CHARBLOCK_IND types not supported" ,
				"CHARBLOCK_INDタイプはサポートされていません" )				
pick( MSG_NAME_TOO_LONG,	"Name (%s) too long, truncated." ,
				"名前(%s)が長すぎるので切り捨てました" )				
pick( MSG_CHANGE_SEG_IN_BLOCK,	"Unable to CHANGE_SEG inside code block" ,
				"コードブロック内ではCHANGE_SEGは実行できません" )
pick( MSG_UNK_REGISTER,		"Unknown register type, assuming AX" ,
				"未知のレジスタ型が検出されました. AXと仮定します" )
pick( MSG_NO_MULTI_REG,		"No support for multi-register symbols" ,
				"multi-registerシンボルはサポートされていません" )
pick( MSG_ARRAY_TOO_LARGE,	"Array too large (%1X elements of size %2X)" ,
				"配列が大きすぎます(サイズ%2X の要素 %1X)" )				
pick( MSG_UNABLE_TO_OPEN_FILE,	"Unable to open file %s: %Z" ,
				"ファイル %s をオープン出来ません: %Z" )
pick( MSG_OUT_OF_MEMORY,	"Out of memory" ,
				"メモリ不足です" )
pick( MSG_DISK_ERROR,		"Disk error during %s: %Z" ,
				"%s: %Zの間にディスクエラーが生じました" )
pick( MSG_PREMATURE_EOF,	"Premature end of file encountered" ,
				"途中でEOFが検出されました" )
pick( MSG_TOO_MANY_REGS,	"Too many registers in aggregate" ,
				"全体でレジスタが多すぎます" )
pick( MSG_UNS_IND_REG,		"IND_REG not supported; probable cause: function returning a union/struct" ,
				"IND_REGはサポートされていません; 関数の戻り値が構造体／共用体であることが原因として考えられます" )
pick( MSG_ADD_PREV_SEG,		"ADD_PREV_SEG no longer supported; probable cause: old version of WATCOM C used" ,
				"ADD_PREV_SEGは現在サポートされていません; 古いバージョンのWATCOM Cを使用したことが原因として考えられます" )
pick( MSG_DEBUG_VERSION,	"Incompatible version of the debugging information" ,
				"デバッグ情報のバージョンに互換性がありません" )
pick( MSG_INVALID_FIXUP,	"Invalid fixup" ,
				"無効なフィックスアップです" )
pick( MSG_THEADR_MISSING,	"THEADR record missing" ,
				"THEADR レコードが見あたりません" )
pick( MSG_UNK_SEGDEF_ALIGN,	"Unknown SEGDEF align value (%t)" ,
				"未知のSEGDEF整列値 (%t)が検出されました" )
pick( MSG_NO_SUPPORT_BIG_32,	"No support for BIG 32-bit SEGDEFs" ,
				"BIG32ビットSEGDEFはサポートされていません" )
pick( MSG_UNK_COMDEF_LEAF,	"Unknown COMDEF LEAF type (%t)" ,
				"未知のCOMDEF LEAFタイプ (%t)が検出されました" )
pick( MSG_INV_WAT_PROC_MODEL,	"Invalid processor/model COMENT record" ,
				"プロセッサ／モデル COMENTレコードが無効です" )
pick( MSG_NO_LIDATA_WAT,	"No LIDATAs allowed for WATCOM debug info" ,
				"LIDATAはWATCOMデバッグ情報として許可されません" )
pick( MSG_TOO_MUCH_SEG_DATA,	"More data in LEDATAs than defined in SEGDEF for seg %x" ,
				"LEDATAのデータがSEGDEFでseg %xに関して定義した値よりも多くなっています" )
pick( MSG_MAX_REC_EXCEEDED,	"Maximum object record size (%u byte) exceeded" ,
                                "オブジェクト・レコードの最大サイズ(%u ﾊﾞｲﾄ)を越えています" )
pick( MSG_NO_SUPPORT_LTL,	"No support for LTL segments" ,
				"LTLセグメントはサポートされていません" )
pick( MSG_NO_LRO_PHARLAP,	"No loader resolved fixups allowed in EASY OMF" ,
                                "EASY OMFではローダで解決するフィックスアップは許されていません" )
pick( MSG_MS386_NO_4KPAGE,	"No 4k Page-aligned segments in MS386 OMF" ,
				"MS386 OMFには4Kページ整列セグメントがありません" )
pick( MSG_MS386_NO_ACCESS,	"Access classes (RW, EO, RO, ER) not supported in MS386 OMF" ,
                                "MS386 OMFではアクセスクラス(RW,EO,RO,ER)はサポートされていません" )
pick( MSG_32BIT_RPT_COUNT,	"Unable to process 32-bit repeat count in LIDATA" ,
                                "LIDATAでは32ビットのリピート数は処理できません" )
pick( MSG_UNS_SCALAR_SIZE,	"Unsupported scalar size" ,
				"サポートされていないスカラー・サイズが検出されました" )
pick( MSG_INCLUDE_TOO_LARGE,	"@Include file is too large (max 64k)" ,
				"@インクルードファイルが大きすぎます(最大64K)" )
pick( MSG_SUBR_TOO_LARGE,	"Subrange too large to represent" ,
				"部分範囲が大きすぎて表示できません" )
pick( MSG_ENUM_TOO_LARGE,	"Enumerated type too large to represent" ,
				"列挙された型が大きすぎて表示できません" )
pick( MSG_IMPOSSIBLE_REG,	"Impossible to translate register variable" ,
				"レジスタ変数への変換が不可能です" )
pick( MSG_TOO_MANY_TYPES,	"Too many types to continue conversion" ,
				"型が多すぎて変換を続行できません" )
pick( MSG_INVALID_OBJECT,	"Object file contains invalid data" ,
				"オブジェクト・ファイルに無効なデータが含まれています" )
pick( MSG_SEGMENT_TOO_LARGE,	"Debugging segment too large (max is 64k)" ,
				"デバッグ・セグメントが大きすぎます(最大64K)" )
pick( MSG_WLIB_ERROR,		"WLIB return code non-zero" ,
				"WLIBリターン・コードが０以外です" )
pick( MSG_SYNTAX_ERR_IN_OPT,	"Syntax error in '%c' option" ,
				"'%c'オプションの文法エラーです" )
pick( MSG_WARNING,		"Warning! " ,
				"警告!" )
pick( MSG_CONVERTING,		"Converting %s" ,
				"%s を変換します" )
pick( MSG_PRESS_KEY,		"Press any key ..." ,
				"どれかキーを押して下さい..." )
pick( MSG_NO_16BIT_BAKPAT,	"EASY OMF has no support for 16-bit BAKPAT" ,
				"EASY OMFは16ビットのBAKPATをサポートしていません" )
pick( MSG_CPP_EXTENSION,	"C++ debugging extensions not supported" ,
				"C++ デバッグ拡張情報はまだサポートしていません" ) 
pick( MSG_TXT_MISC_INFO,	"Miscellaneous Information" ,
				"多方面の情報" )
pick( MSG_TXT_TYPE_INFO,	"Type Information" ,
				"タイプ情報" )
pick( MSG_TXT_SYMB_INFO,	"Symbol Information" ,
				"シンボル情報" )
pick( MSG_TXT_COMPILER,		"Compiler        : \"%s\"" ,
				"コンパイラ      : \"%s\"" )
pick( MSG_TXT_8086_PRCSR,	"Processor       : 8086" ,
				"プロセッサ      : 8086" )
pick( MSG_TXT_80X86_PRCSR,	"Processor       : 80%u86" ,
				"プロセッサ      : 80%u86" )
pick( MSG_TXT_MEM_MODEL,	"Memory Model    : %M" ,
				"メモリモデル    : %M" )
pick( MSG_TXT_MODEL_0,		"Small" ,
				"スモール" )
pick( MSG_TXT_MODEL_1,		"Medium" ,
				"ミディアム" )
pick( MSG_TXT_MODEL_2,		"Compact" ,
				"コンパクト" )
pick( MSG_TXT_MODEL_3,		"Large" ,
				"ラージ" )
pick( MSG_TXT_MODEL_4,		"Huge" ,
				"ヒュージ" )
pick( MSG_TXT_MODEL_5,		"Flat" ,
				"フラット" )
pick( MSG_TXT_MODEL_6,		"Tiny" ,
				"タイニ" )
pick( MSG_TXT_FLOAT_PT,		"Floating-point  : %M" ,
				"浮動小数点      : %M" )
pick( MSG_TXT_FP_0,		"Inline Emulation" ,
				"インライン・エミュレーション" )
pick( MSG_TXT_FP_1,		"Emulation Calls" ,
				"エミュレーション・コール" )
pick( MSG_TXT_FP_2,		"80x87 Inline Code" ,
				"80X87インライン・コード" )
pick( MSG_TXT_FP_3,		"Alternate Math" ,
				"代替演算" )
pick( MSG_TXT_OPTIMIZED,	"Optimized       : %M" ,
				"オプティマイズ  : %M" )
pick( MSG_TXT_LINES_PRST,	"Lines Present   : %M" ,
				"ライン表示      : %M" )
pick( MSG_TXT_TYPES_PRST,	"Types Present   : %M" ,
				"タイプ表示      : %M" )
pick( MSG_TXT_SYMBS_PRST,	"Symbs Present   : %M" ,
				"シンボル表示    : %M" )
pick( MSG_TXT_YES,		"Yes" ,
				"はい" )
pick( MSG_TXT_NO,		"No" ,
				"いいえ" )
pick( MSG_TXT_SOURCE_LANG,	"Source Language : %M" ,
				"ソース言語      : %M" )
pick( MSG_TXT_LANG_0,		"Assembly" ,
				"アセンブリ言語" )
pick( MSG_TXT_LANG_1,		"C" ,
				"C" )
pick( MSG_TXT_LANG_2,		"Fortran" ,
				"Fortran" )
pick( MSG_TXT_LANG_3,		"Pascal" ,
				"Pascal" )
pick( MSG_TXT_LANG_4,		"Basic" ,
				"Basic" )
pick( MSG_TXT_NO_ADDR,		"No address info" ,
				"アドレス情報がありません" )
pick( MSG_SYS_ERR_0,		"Error 0" ,
				"エラー 0" )
pick( MSG_SYS_ERR_1,		"No such file or directory" ,
				"指定されたファイル又はディレクトリがありません" )
pick( MSG_SYS_ERR_2,		"Argument list too big" ,
				"引数リストが大きすぎます" )
pick( MSG_SYS_ERR_3,		"Exec format error" ,
				"実行フォーマット・エラー" )
pick( MSG_SYS_ERR_4,		"Bad file number" ,
				"不正なファイル番号" )
pick( MSG_SYS_ERR_5,		"Not enough memory" ,
				"メモリが足りません" )
pick( MSG_SYS_ERR_6,		"Permission denied" ,
				"パーミッション拒否" )
pick( MSG_SYS_ERR_7,		"File exists" ,
				"ファイルが存在します" )
pick( MSG_SYS_ERR_8,		"Cross-device link" ,
				"デバイスを変更しました" )
pick( MSG_SYS_ERR_9,		"Invalid argument" ,
				"無効な引数" )
pick( MSG_SYS_ERR_10,		"File table overflow" ,
				"ファイル・テーブルのオーバフロー" )
pick( MSG_SYS_ERR_11,		"Too many open files" ,
				"オープン・ファイルが多すぎます" )
pick( MSG_SYS_ERR_12,		"No space left on device" ,
				"デバイスに空きがありません" )
pick( MSG_SYS_ERR_13,		"Argument too large" ,
				"引数が大きすぎます" )
pick( MSG_SYS_ERR_14,		"Result too large" ,
				"結果が大きすぎます" )
pick( MSG_SYS_ERR_15,		"Resource deadlock would occur" ,
				"リソース・デッドロックが発生します" )
