pick( MSG_INV_OPT,		"Invalid option '%s'" ,
				"無効なオプション '%s'" )
pick( MSG_LOW_MEM,		"Too low on memory" ,
				"ファイルバッファのためのメモリが不足です" )
pick( MSG_CANT_FIND,		"Unable to find '%s'" ,
				"ファイルが見つかりません '%s'" )
pick( MSG_CANT_CREATE_TMP,	"Cannot create temporary file" ,
				"一時ファイルが作れません" )
pick( MSG_CANT_OPEN,		"Cannot open file '%s'" ,
				"ファイルをオープンできません '%s'" )
pick( MSG_NOT_VALID_EXE,	"'%s' is not a valid executable file" ,
				"'%s' は正しくない実行可能ファイルです" )
pick( MSG_CANT_CREATE_OUTPUT,	"Unable to create output file '%s'" ,
				"出力ファイルが作れません '%s'" )
pick( MSG_SEEK_ERROR,		"Seek error on '%s'" ,
				"シーク・エラー '%s'" )
pick( MSG_READ_ERROR,		"Error reading '%s'" ,
				"読み込みエラー '%s'" )
pick( MSG_WRITE_ERROR,		"Error writing to '%s'" ,
				"書き込みエラー '%s'" )
pick( MSG_ADD_HEADER_ERROR,	"Error adding resource header" ,
				"リソースヘッダー追加エラー" )
pick( MSG_CANT_ERASE,		"Cannot erase file '%s'" ,
				"ファイルを削除できません '%s'" )
pick( MSG_CANT_RENAME,		"Cannot rename file '%s'" ,
				"ファイル名を変更できません '%s'" )
pick( MSG_WSTRIP_ABORT,		"\nwstrip aborted" ,
				"\nwstrip の処理を中止します" )
pick( MSG_NO_SPECIFIED_0,	"No symbol file specified" ,
				"シンボルファイルが指定されていません" )
pick( MSG_NO_SPECIFIED_1,	"No resource file specified" ,
				"リソースファイルが指定されていません" )
pick( MSG_NO_INFO_0,		"'%s' does not contain debugging information" ,
				"'%s' はデバッグ情報を含んでいません" )
pick( MSG_NO_INFO_1,		"'%s' does not contain resource information" ,
				"'%s' はリソース情報を含んでいません" )
pick( MSG_HAS_INFO_0,		"'%s' already has debugging information" ,
				"'%s' は既にデバッグ情報を持っています" )
pick( MSG_HAS_INFO_1,		"'%s' already has resource information" ,
				"'%s' は既にリソース情報を持っています" )
pick( MSG_INV_FILE_0,		"'%s' is an invalid symbol file" ,
				"'%s' は無効なシンボルファイルです" )
pick( MSG_INV_FILE_1,		"'%s' is an invalid resource file" ,
				"'%s' は無効なリソースファイルです" )
pick( MSG_CANT_CREATE_0,	"Unable to create symbol file '%s'" ,
				"シンボルファイルを作れません '%s'" )
pick( MSG_CANT_CREATE_1,	"Unable to create resource file '%s'" ,
				"リソースファイルを作れません '%s'" )
pick( MSG_USAGE_FIRST,		"Usage: wstrip [options] input_file [output_file] [info_file]" ,
				"使用方法: wstrip [options] input_file [output_file] [info_file]" )
#ifdef RC_UNIX
pick( MSG_USAGE_1,		"options:" ,
				"オプション:" )
#else
pick( MSG_USAGE_1,		"options: (/option is also accepted)" ,
				"オプション: (/ｵﾌﾟｼｮﾝ も受け付けます)" )
#endif
pick( MSG_USAGE_2,		"        -q     don't print informational messages" ,
				"        -q     メッセージを表示しません" )
pick( MSG_USAGE_3,		"        -n     don't print warning messages" ,
				"        -n     警告メッセージを表示しません" )
pick( MSG_USAGE_4,		"        -r     process resource information rather than debugging information" ,
				"        -r     デバッグ情報ではなく、リソース情報を処理します" )
pick( MSG_USAGE_5,		"        -a     add information rather than delete information" ,
				"        -a     情報削除ではなく、情報付加をします" )
pick( MSG_USAGE_6,		"" ,
				"" )
pick( MSG_USAGE_7,		"        input_file:  executable file" ,
				"        input_file:  実行可能ファイル" )
pick( MSG_USAGE_8,		"        output_file: optional output executable or '.'" ,
				"        output_file: オプションで出力実行可能ファイルまたは '.'を指定可能" )
pick( MSG_USAGE_9,		"        info_file:   optional output debugging or resource information file" ,
				"        info_file:   オプションでデバッグまたはリソース情報ファイルの出力又は" )
pick( MSG_USAGE_LAST,		"                     or input debugging or resource information file" ,
				"                     入力を指定可能" )
