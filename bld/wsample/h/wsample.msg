/* UTF-8 encoding, ¥ */

pick( ERR_DISK_FULL
        , "Fatal Error: Disk full."
        , "重大なエラー:ディスクがいっぱいです"
)
pick( ERR_SAMPLES_LOST
        , "Error: some samples have been lost"
        , "エラー:サンプリング・データの一部が失われました"
)
pick( ERR_SAMPLE_TRUNCATE
        , "Error: Out of disk space. Sample file truncated"
        , "エラー:ディスクの空き領域が不足です.サンプルファイルを切り詰めます "
)
pick( MSG_EXPECTING
        , "Expecting either a '=' or a '#' instead found: "
        , "'=' または '#' があるべき箇所に次のコードがありました: "
)
pick( MSG_INVALID_CHAR
        , "Invalid character after number"
        , "数字の後に無効な文字があります"
)
pick( MSG_OUT_OF_RANGE
        , "number is outside of its allowable range"
        , "数字が有効範囲内にありません"
)
pick( MSG_SAMPLE_BUFF
        , "Can't allocate sample buffer"
        , "サンプリング用バッファのメモリが確保できません"
)
pick( MSG_CALLGRAPH_BUFF
        , "Can't allocate callgraph buffer"
        , "コールグラフ用バッファのメモリを確保できません"
)
pick( MSG_VERSION
        , "Can't work with this version of the operating system"
        , "このバージョンのオペレーティング・システムの上では動作できません"
)
pick( MSG_SAMPLE_FILE
        , "Can't open sample file"
        , "サンプルファイルをオープンできません"
)
pick( MSG_PROGRAM
        , "Can't find program: "
        , "プログラムが見つかりません: "
)
pick( MSG_BUFF_SMALL
        , "Buffer size too small for timer rate"
        , "タイマー間隔に対してバッファが小さすぎます"
)
pick( MSG_INVALID_OPTION
        , "invalid option: "
        , "無効なオプション: "
)

#ifdef __DOS__      /* messages in sampdos */
pick( MSG_SAMPLE_1
        , "Too many sections in program"
        , "プログラムの中のセクションが多すぎます"
)
pick( MSG_SAMPLE_2
        , "Interrupts 34 to 3d are reserved for 80x87 emulation and cannot be monitored"
        , "割込み 34 から 3d は 80x87 エミュレーションのために予約されており、モニターできません"
)
pick( MSG_SAMPLE_3
        , "Cannot monitor more than eight interrupts"
        , "9個以上の割込みはモニターできません"
)
#endif

#if defined( __OS2__ ) && defined( _M_I86 )      /* messages in sampos2 */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "実行中: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ファイルを実行できません"
)
pick( MSG_SAMPLE_4
        , "Cannot create a new thread"
        , "新しいスレッドを作れません"
)
pick( MSG_SAMPLE_5
        , "Cannot boost sleeper thread priority"
        , "スリーパ・スレッドの優先順位を上げられません"
)
pick( MSG_SAMPLE_6
        , "Unexpected notification"
        , "記法が間違っています"
)
pick( MSG_SAMPLE_7
        , "DosPTrace( GO ) failed"
        , "DosPTrace( GO ) が失敗しました"
)
pick( MSG_SAMPLE_8
        , "Cannot find OS/2 2.x sampler"
        , "OS/2 2.x 用サンプラが見つかりません"
)
pick( MSG_SAMPLE_9
        , "Cannot start OS/2 2.x sampler"
        , "OS/2 2.x 用サンプラを開始できません"
)
#endif

#if defined( __OS2__ ) && !defined( _M_I86 )     /* messages in sampos22 */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "実行中: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ファイルを実行できません"
)
pick( MSG_SAMPLE_4
        , "Cannot create a new thread"
        , "新しいスレッドを作れません"
)
pick( MSG_SAMPLE_5
        , "Cannot boost sleeper thread priority"
        , "スリーパ・スレッドの優先順位を上げられません"
)
pick( MSG_SAMPLE_6
        , "Unexpected notification"
        , "記法が間違っています"
)
pick( MSG_SAMPLE_7
        , "DosDebug call failed"
        , "DosDebug コールが失敗しました"
)
pick( MSG_SAMPLE_8
        , "DosDebug( STOP ) failed"
        , "DosDebug( STOP ) が失敗しました"
)
pick( MSG_SAMPLE_9
        , "Unable to attach to process"
        , "プロセスに接触できませんでした"
)
pick( MSG_SAMPLE_10
        , "CPU exception reported"
        , "CPU 例外が報告されています"
)
#endif

#ifdef __NT__       /* messages in sampnt */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "実行中: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ファイルを実行できません"
)
pick( MSG_SAMPLE_4
        , "Unexpected notification"
        , "記法が間違っています"
)
#endif

#ifdef __LINUX__    /* messages in samplnx */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "実行中: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ファイルを実行できません"
)
pick( MSG_SAMPLE_4
        , "ptrace() failed"
        , "ptrace() が失敗しました"
)
pick( MSG_SAMPLE_5
        , "Unable to attach to process"
        , "プロセスに接触できませんでした"
)
pick( MSG_SAMPLE_6
        , "setitimer() failed"
        , "setitimer() が失敗しました"
)
#endif

#ifdef __PHARLAP__  /* messages in samppls */
pick( MSG_SAMPLE_1
        , "Internal error"
        , "内部エラー"
)
pick( MSG_SAMPLE_2
        , "Unable to load: "
        , "ロードできません: "
)
pick( MSG_SAMPLE_3
        , "Task Exception: "
        , "タスク例外: "
)
pick( MSG_SAMPLE_4
        , "Unexpected Exception: "
        , "予期しない例外: "
)
#endif

#ifdef __NETWARE__      /* messages in sampnlm */
pick( MSG_SAMPLE_1
        , "Error! Cannot execute: "
        , "エラー! 実行できません: "
)
#endif

#ifdef __DOS4G__        /* messages in samprsi */
pick( MSG_SAMPLE_1
        , "Task Exception: "
        , "タスク例外: "
)
pick( MSG_SAMPLE_2
        , "Unable to load: "
        , "ロードできません: "
)
#endif

#ifdef __WINDOWS__      /* messages in sampwin */
pick( MSG_SAMPLE_1
        , "Couldn't find any modules!"
        , "モジュールが見つかりません!"
)
pick( MSG_SAMPLE_2
        , "Failed to hook interrupts"
        , "割込みのフックに失敗しました"
)
pick( MSG_SAMPLE_3
        , "Failed to get notify hook"
        , "フックの通知を受けるのに失敗しました"
)
pick( MSG_SAMPLE_4
        , "Could not start sampling!"
        , "サンプリングを開始できません!"
)
pick( MSG_SAMPLE_5
        , "Sample buffer flushed"
        , "サンプルバッファをフラッシュしました"
)
pick( MSG_SAMPLE_6
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_7
        , "Timer rate set to "
        , "タイマ間隔を "
)
pick( MSG_SAMPLE_8
        , " ms"
        , " ms"
)
pick( MSG_SAMPLE_9
        , "Determining what other modules are present..."
        , "他のどのようなモジュールがあるかを探索中..."
)
pick( MSG_SAMPLE_10
        , "Found "
        , "ほかに "
)
pick( MSG_SAMPLE_11
        , " other modules"
        , " 個のモジュールがあります"
)
pick( MSG_SAMPLE_12
        , "Will use WDEBUG.386"
        , "WDEBUG.386を使用します"
)
pick( MSG_SAMPLE_13
        , "Execution finished, total time="
        , "実行終了, 全時間="
)
pick( MSG_SAMPLE_14
        , " seconds"
        , " 秒"
)
pick( MSG_SAMPLE_15
        , "Error loading sampler!"
        , "サンプラ・ロード・エラー!"
)
pick( MSG_SAMPLE_16
        , "Cannot close sampler!"
        , "サンプラをクローズできません!"
)
#endif
