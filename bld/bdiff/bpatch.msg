#if defined(BPATCH)     /* messages used by bpatch, bpatchp, bdump */
pick( MSG_NOT_PATCHED,      "File '%s' has not been patched\n",
                            "ファイル %s をパッチしていません" )
pick( MSG_PATCHED_TO_LEVEL, "File '%1s' has been patched to level '%2s'\n",
                            "ファイル '%1s' をレベル '%2s'にパッチしました" )
pick( MSG_ALREADY_PATCHED,  "File '%1s' has already been patched to level '%2s' - skipping\n",
                            "ファイル '%1s' は既にレベル '%2s' にパッチされていますので飛ばします" )
pick( MSG_MODIFY,           "\r\nOk to modify '%s'? [y|n]",
                            "\r\n'%s' を修正しますか？ [y|n]" )
pick( ERR_TWO_NAMES,        "Command line may only contain one file name",
                            "コマンドラインにファイル名を１つだけ指定します" )
pick( ERR_NO_NAME,          "Command line must specify a file name",
                            "コマンドラインにファイル名を指定して下さい" )
pick( ERR_WRONG_SIZE,       "'%1s' is the wrong size (%2lu) - should be (%3lu)",
                            "'%1s' のサイズ (%2lu) は不正です - 正しいサイズは (%3lu) です" )
pick( ERR_CANT_RENAME,      "Cannot rename '%1s' to '%2s'",
                            "'%1s' から '%2s' へのファイル名の変更が出来ません" )
pick( ERR_PATCH_ABORTED,    "Patch program aborted!",
                            "パッチ・プログラムがアボートしました" )
pick( ERR_WRONG_CHECKSUM,   "Resulting file has wrong checksum (%1lu) - should be (%2lu)",
                            "パッチ後のチェックサム (%1lu) が不正です - 正しい値は (%2lu) です" )
pick( MSG_SUCCESSFULLY_PATCHED,  "File '%1s' successfully patched to level '%2s'\n",
/* TRANSLATE ME!!*/         "File '%1s' successfully patched to level '%2s'\n" )
/* bpatchp only */
pick( ERR_USE_REAL,         "Not enough memory -  Use real mode version",
                            "メモリが足りません - リアルモード版を使用して下さい" )
#endif

pick( ERR_CANT_GET_ATTRIBUTES, "Can't get attributes for '%1s'",
                               "%1s の属性が得られません" )

/*
        BDIFF is an internal WATCOM tool.
        These messages do not need to be translated.
*/

#if defined(BDIFF)      /* messages used by bdiff */
pick( MSG_IO_ERR,          "I/O error", "" )
pick( MSG_DEBUG_INFO,      "invalid debugging information", "" )
pick( MSG_OVERLAYS,        "application uses overlays", "" )
pick( MSG_IO_ERROR,        "I/O error accessing file: %s\n", "" )
pick( ERR_NO_MEMORY,       "No memory for %s", "" )
pick( ERR_PATCH_BUNGLED,   "Patch file bungled at %1lx ('%2x' != '%3x')", "" )
#endif

/*
        These messages must be translated.
*/
pick( ERR_NOT_PATCHFILE,    "'%s' is not a WATCOM patch file",
                            "'%s' はＷＡＴＣＯＭパッチファイルではありません" )
pick( ERR_BAD_PATCHFILE,    "'%s' is not a valid WATCOM patch file",
                            "'%s' は正しいＷＡＴＣＯＭパッチファイルではありません" )
pick( ERR_CANT_FIND,        "Cannot find '%s'",
                            "'%s' が見つかりません" )
pick( ERR_CANT_OPEN,        "Cannot open '%s'",
                            "'%s' をオープン出来ません" )
pick( ERR_CANT_READ,        "Cannot read '%s'",
                            "'%s' を読み込めません" )
pick( ERR_CANT_WRITE,       "Cannot write to '%s'",
                            "'%s' への書き込みが出来ません" )
pick( ERR_IO_ERROR,         "I/O error processing file '%s'",
                            "'%s' ファイル処理中にＩ／Ｏエラーが発生しました" )
pick( ERR_MEMORY_OUT,       "Out of memory",
                            "メモリ不足です" )
pick( ERR_NO_SYNCSTRING,    "Cannot locate Sync String",
                            "Cannot locate Sync String" )
pick( ERR_CANT_DELETE,      "Cannot delete '%s'",
                            "Cannot delete '%s'" )
pick( ERR_CANT_CREATE,      "Cannot create '%s'",
                            "Cannot create '%s'" )
pick( ERR_CANT_CREATE_TMP,  "Cannot create temporary file",
                            "Cannot create temporary file" )

/* Error message prefix */

pick( MSG_ERROR,            "Error! ",
                            "エラー！" )
