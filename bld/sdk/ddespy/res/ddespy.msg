pick(   STR_ACK,
        "ack",
        "通知" )
pick(   STR_ADVISE,
        "advise",
        "アドバイス" )
pick(   STR_DATA,
        "data",
        "デ－タ" )
pick(   STR_EXECUTE,
        "execute",
        "実行" )
pick(   STR_INITIATE,
        "initiate",
        "初期化" )
pick(   STR_POKE,
        "poke",
        "探査" )
pick(   STR_REQUEST,
        "request",
        "要求" )
pick(   STR_TERMINATE,
        "terminate",
        "終了" )
pick(   STR_UNADVISE,
        "unadvise",
        "アンアドバイス" )

pick(   STR_UNKNOWN_FMT_ID,
        "0x%0*X (unknown)",
        "0x%0*X (未知)" )
pick(   STR_UNKNOWN,
        "unknown",
        "未知" )


// Formatting strings for the various types of events
pick(   STR_ADVSTART_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         returned: 0x%08lX %s fmt: %s\n         Topic: 0x%08lX (%s) Item: 0x%08lX (%s)",
        "時間: %lu %s コ－ルバック: %s\n         戻り値: 0x%08lX %s 形式: %s\n         トピック: 0x%08lX (%s) アイテム: 0x%08lX (%s)" )
pick(   STR_CONNECT_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         returned: 0x%08lX %s fmt: %s\n         Topic: 0x%08lX (%s) Service: 0x%08lX (%s)\n         data1: 0x%08lX data2: 0x%08lX",
        "時間: %lu %s コ－ルバック: %s\n         戻り値: 0x%08lX %s 形式: %s\n         トピック: 0x%08lX (%s) サ－ビス: 0x%08lX (%s)\n         data1: 0x%08lX data2: 0x%08lX" )
pick(   STR_ADVREQ_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         returned: 0x%08lX %s fmt: %s\n         Topic: 0x%08lX (%s) Item: 0x%08lX (%s)\n         data1: 0x%08lX",
        "時間: %lu %s コ－ルバック: %s\n         戻り値: 0x%08lX %s 形式: %s\n         トピック: 0x%08lX (%s) アイテム: 0x%08lX (%s)\n         data1: 0x%08lX" )
pick(   STR_ADVDATA_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         returned: 0x%08lX %s fmt: %s\n         Topic: 0x%08lX (%s) Item: 0x%08lX (%s)\n         HData: 0x%08lX",
        "時間: %lu %s コ－ルバック: %s\n         戻り値: 0x%08lX %s 形式: %s\n         トピック: 0x%08lX (%s) アイテム: 0x%08lX (%s)\n         HData: 0x%08lX" )
pick(   STR_EXECUTE_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         returned: 0x%08lX %s fmt: %s\n         Topic: 0x%08lX (%s) Command (HData): 0x%08lX\n",
        "時間: %lu %s コ－ルバック: %s\n         戻り値: 0x%08lX %s 形式: %s\n         トピック: 0x%08lX (%s) コマンド (HData): 0x%08lX\n" )
pick(   STR_CONFIRM_FRM_STR,
        "time: %lu %s CALLBACK: %s\n         returned: 0x%08lX %s fmt: %s\n         Topic: 0x%08lX (%s) Service: 0x%08lX (%s)\n         data2: 0x%08lX",
        "時間: %lu %s コ－ルバック: %s\n         戻り値: 0x%08lX %s 形式: %s\n         トピック: 0x%08lX (%s) サ－ビス: 0x%08lX (%s)\n         data2: 0x%08lX" )
pick(   STR_DISCONNECT_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         returned: 0x%08lX %s fmt: %s\n         data2: 0x%08lX",
        "時間: %lu %s コ－ルバック: %s\n         戻り値: 0x%08lX %s 形式: %s\n         data2: 0x%08lX" )
pick(   STR_ERROR_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         returned: 0x%08lX %s fmt: %s\n         data1: 0x%08lX",
        "時間: %lu %s コ－ルバック: %s\n         戻り値: 0x%08lX %s 形式: %s\n         data1: 0x%08lX" )
pick(   STR_XACT_CPLT_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         type: %s returned: 0x%08lX %s fmt: %s\n         Topic: 0x%08lX (%s) Item: 0x%08lX (%s)\n         hdata: 0x%08lX data1: 0x%08lX data2: 0x%08lX",
        "時間: %lu %s コ－ルバック: %s\n         タイプ: %s 戻り値: 0x%08lX %s 形式: %s\n         トピック: 0x%08lX (%s) アイテム: 0x%08lX (%s)\n         hdata: 0x%08lX data1: 0x%08lX data2: 0x%08lX" )
pick(   STR_UNREGISTER_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         hsz1: 0x%08lX (%s) hsz2: 0x%08lX (%s)",
        "時間: %lu %s コ－ルバック: %s\n         hsz1: 0x%08lX (%s) hsz2: 0x%08lX (%s)" )
pick(   STR_REGISTER_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         hsz1: 0x%08lX (%s) hsz2: 0x%08lX (%s)",
        "時間: %lu %s コ－ルバック: %s\n         hsz1: 0x%08lX (%s) hsz2: 0x%08lX (%s)" )
pick(   STR_DEFAULT_FMT_STR,
        "time: %lu %s CALLBACK: %s\n         returned: 0x%08lX %s fmt: %s\n         hsz1: 0x%08lX (%s) hsz2: 0x%08lX (%s)\n         hdata: 0x%08lX data1: 0x%08lX data2: 0x%08lX",
        "時間: %lu %s コ－ルバック: %s\n         戻り値: 0x%08lX %s 形式: %s\n         hsz1: 0x%08lX (%s) hsz2: 0x%08lX (%s)\n         hdata: 0x%08lX data1: 0x%08lX data2: 0x%08lX" )

pick(   STR_CONV_EST_FMT_STR,
        "time: %lu %s CONVERSATION ESTABLISHED\n         service: 0x%08lX (%s) topic: 0x%08lX (%s)\n         %s %s",
        "時間: %lu %s 対話確立\n         サ－ビス: 0x%08lX (%s) トピック: 0x%08lX (%s)\n         %s %s" )
pick(   STR_CONV_TERM_FMT_STR,
        "time: %lu %s CONVERSATION TERMINATED\n         service: 0x%08lX (%s) topic: 0x%08lX (%s)\n         %s %s",
        "時間: %lu %s 対話終了\n         サ－ビス: 0x%08lX (%s) トピック: 0x%08lX (%s)\n         %s %s" )

pick(   STR_ERR_STRUCT_FMT_STR,
        "time: %lu %s ERROR: %s",
        "時間: %lu %s エラ－: %s" )
pick(   STR_HSZ_STRUCT_DEL,
        "time: %lu %s STRING DELETED 0x%0*lX (%s) ",
        "時間: %lu %s 削除文字列 0x%0*lX (%s) " )
pick(   STR_HSZ_STRUCT_CREATE,
        "time: %lu %s STRING CREATED 0x%0*lX (%s) ",
        "時間: %lu %s 作成文字列 0x%0*lX (%s) " )
pick(   STR_HSZ_STRUCT_KEPT,
        "time: %lu %s STRING KEPT 0x%0*lX (%s) ",
        "時間: %lu %s 保持文字列 0x%0*lX (%s) " )

// THESE MESSAGES have changed
pick(   STR_LNK_STRUCT_FMT_STR_1,
        "time: %lu %s WARM LINK ESTABLISHED\n         service: 0x%08lX (%s) topic: 0x%08lX (%s)\n         item: 0x%08lX (%s)\n         fmt: %s %s %s",
        "時間: %lu %s ウォ－ムリンク確立\n         サ－ビス: 0x%08lX (%s) トピック: 0x%08lX (%s)\n         アイテム: 0x%08lX (%s)\n         形式: %s %s %s" )
pick(   STR_LNK_STRUCT_FMT_STR_2,
        "time: %lu %s HOT LINK ESTABLISHED\n         service: 0x%08lX (%s) topic: 0x%08lX (%s)\n         item: 0x%08lX (%s)\n         fmt: %s %s %s",
        "時間: %lu %s ホットリンク確立\n         サ－ビス: 0x%08lX (%s) トピック: 0x%08lX (%s)\n         アイテム: 0x%08lX (%s)\n         形式: %s %s %s" )
pick(   STR_LNK_STRUCT_FMT_STR_3,
        "time: %lu %s LINK TERMINATED\n         service: 0x%08lX (%s) topic: 0x%08lX (%s)\n         item: 0x%08lX (%s)\n         fmt: %s %s %s",
        "時間: %lu %s リンク終了\n         サ－ビス: 0x%08lX (%s) トピック: 0x%08lX (%s)\n         アイテム: 0x%08lX (%s)\n         形式: %s %s %s" )
pick(   STR_LNK_STRUCT_FMT_STR_4,
        "time: %lu %s HOT LINK TERMINATED\n         service: 0x%08lX (%s) topic: 0x%08lX (%s)\n         item: 0x%08lX (%s)\n         fmt: %s %s %s",
        "時間: %lu %s ホットリンク終了\n         サ－ビス: 0x%08lX (%s) トピック: 0x%08lX (%s)\n         アイテム: 0x%08lX (%s)\n         形式: %s %s %s" )
pick(   STR_MSG_STRUC_FMT_STR_1,
        "time: %lu %s MESSAGE SENT: %s\n         %s wparam: 0x%0*X lparam: 0x%08lX",
        "時間: %lu %s 送信メッセ－ジ: %s\n         %s wparam: 0x%0*X lparam: 0x%08lX" )
pick(   STR_MSG_STRUC_FMT_STR_2,
        "time: %lu %s MESSAGE POSTED: %s\n         %s wparam: 0x%0*X lparam: 0x%08lX",
        "時間: %lu %s ポストメッセ－ジ: %s\n         %s wparam: 0x%0*X lparam: 0x%08lX" )

pick(   STR_TASK_PREFIX,
        "task: ",
        "タスク: " )
pick(   STR_CONV_PREFIX,
        "conversation: ",
        "対話: " )
pick(   STR_CLICONV_PREFIX,
        "client conv: ",
        "クライアント対話: " )
pick(   STR_SERVCONV_PREFIX,
        "server conv: ",
        "サ－バ－対話: " )
pick(   STR_HWND_PREFIX,
        "to hwnd: ",
        "受信hwnd: " )

pick(   STR_HANDLE,
        "Handle",
        "ハンドル" )
pick(   STR_COUNT,
        "Count",
        "カウント" )
pick(   STR_STRING,
        "String",
        "文字列" )
pick(   STR_CLIENT,
        "Client",
        "クライアント" )
pick(   STR_SERVER,
        "Server",
        "サ－バ－" )
pick(   STR_SERVER_NAME,
        "Server Name",
        "サ－バ－名" )
pick(   STR_SERVICE,
        "Service",
        "サ－ビス" )
pick(   STR_TOPIC,
        "Topic",
        "トピック" )
pick(   STR_TYPE,
        "Type",
        "タイプ" )
pick(   STR_ITEM,
        "Item",
        "アイテム" )
pick(   STR_FORMAT,
        "Format",
        "形式" )
pick(   STR_INSTANCE,
        "Instance Name",
        "インスタンス名" )

pick(   STR_WARM,
        "WARM",
        "ウォ－ム" )
pick(   STR_HOT,
        "HOT",
        "ホット" )
pick(   STR_CONVERSATIONS,
        "Conversations",
        "対話" )
pick(   STR_STRINGS,
        "Strings",
        "文字列" )
pick(   STR_LINKS,
        "Links",
        "リンク" )
pick(   STR_REG_SERVERS,
        "Registered Servers",
        "登録サ－バ－" )

pick(   STR_LOG_HEADER,
        "Open Watcom DDE SPY LOG started at: %s",
        "Open Watcom DDE SPY LOG ログ開始: %s" )
pick(   STR_EVENT_LIST_LINE,
        "--------------------------------------------------------------------------------\n\n",
        "--------------------------------------------------------------------------------\n\n" )
pick(   STR_EVENT_LIST_HEADER,
        "Open Watcom DDE SPY event listing made: %s\n\n",
        "Open Watcom DDE SPY イベントリスト作成: %s\n\n" )

pick(   STR_ABOUT_VERSION,
        "", //NOT USED
        "" )
pick(   STR_ABOUT_COPYRIGHT_YEAR,
        "2002",
        "2002" )
pick(   STR_ABOUT_TITLE,
        "About Open Watcom DDE Spy",
        "Open Watcom DDE Spyバ－ジョン情報" )
pick(   STR_ADD_HWND_ALIAS,
        "Add HWND Alias",
        "HWND別名の追加" )
pick(   STR_ADD_TASK_ALIAS,
        "Add Task Alias",
        "タスク別名の追加" )
pick(   STR_ADD_CONV_ALIAS,
        "Add Conversation Alias",
        "対話別名の追加" )
pick(   STR_APP_NAME,
        "Open Watcom DDE Spy",
        "Open Watcom DDE Spy" )
pick(   STR_ABOUT_NAME,
        "Open Watcom DDE Spy for Windows" STR_BITNEES,
        "Open Watcom DDE Spy Windows版" STR_BITNEES )
pick(   STR_HINT_SAVE,
        "Save information in the event list to a file.",
        "イベントリストの情報のファイルへの保存." )

pick(   STR_HINT_SAVE_AS,
        "Save information in the event list to a file.",
        "イベントリストの情報を名前を付けてファイルに保存." )

pick(   STR_HINT_LOG_FILE,
        "Start/stop logging monitored events to a file.",
        "モニタ－するイベントのファイルへのログの開始/終了." )

pick(   STR_HINT_LOG_PAUSE,
        "Pause/resume logging monitored events to a file.",
        "モニタ－するイベントのファイルへのログの停止/続行." )

pick(   STR_HINT_LOG_CONFIG,
        "Configure the behavior of the logging function.",
        "ログ機能の設定." )

pick(   STR_HINT_FONT,
        "Set the font for the DDE Spy windows.",
        "DDE Spyウィンドウのフォントの設定." )

pick(   STR_HINT_TOOLBAR,
        "Display/hide the toolbar.",
        "ツ－ルバ－の表示." )

pick(   STR_HINT_TOP,
        "Ensure DDE Spy is never covered by other windows.",
        "Ensure DDE Spy is never covered by other windows." )

pick(   STR_HINT_EXIT,
        "Exit DDE Spy.",
        "DDE Spyの終了." )

pick(   STR_HINT_CLEAR,
        "Erase the information in the event list.",
        "イベントリストの情報を消去します." )

pick(   STR_HINT_MARK,
        "Put text in the event list.",
        "イベントリストにテキストを入れます." )

pick(   STR_HINT_SCREEN_OUT,
        "Enable/disable writing output to the screen.",
        "スクリ－ンへの出力書き込みの許可メッセ－ジ情報のモニタ－の許可/禁止." )

pick(   STR_HINT_SCROLL,
        "Scroll the display to show the newest events.",
        "最新イベントの表示のためのスクロ－ル." )

pick(   STR_HINT_MON_POST,
        "Enable/disable monitoring of posted message information.",
        "ポストメッセ－ジ情報のモニタ－の許可/禁止." )

pick(   STR_HINT_MON_SENT,
        "Enable/disable monitoring of sent message information.",
        "送信メッセ－ジ情報のモニタ－の許可/禁止." )

pick(   STR_HINT_MON_STR,
        "Enable/disable monitoring of string management events.",
        "文字列管理イベントのモニタ－の許可/禁止." )

pick(   STR_HINT_MON_CB,
        "Enable/disable monitoring of callback events.",
        "コ－ルバックイベントのモニタ－の許可/禁止." )

pick(   STR_HINT_MON_ERR,
        "Enable/disable monitoring of error events.",
        "エラ－イベントのモニタ－の許可/禁止." )

pick(   STR_HINT_MON_CONV,
        "Enable/disable monitoring of conversation management events.",
        "対話管理イベントのモニタ－の許可/禁止." )

pick(   STR_HINT_MON_LNK,
        "Enable/disable monitoring of link management events.",
        "リンク管理イベントのモニタ－の許可/禁止." )

pick(   STR_HINT_MSG_FILTER,
        "Select sent/posted messages on which to report.",
        "報告する送信/ポストメッセ－ジの選択." )

pick(   STR_HINT_CB_FILTER,
        "Select callback events on which to report.",
        "報告するコ－ルバックイベントの選択." )

pick(   STR_HINT_TRK_STR,
        "Display currently registered strings.",
        "現在登録されている文字列の表示." )

pick(   STR_HINT_TRK_CONV,
        "Display currently active conversations.",
        "現在アクティブな対話の表示." )

pick(   STR_HINT_TRK_LINK,
        "Display currently active links.",
        "現在のアクティブリンクの表示." )

pick(   STR_HINT_TRK_SERVER,
        "Display currently registered servers.",
        "現在登録されているサ－バの表示." )

pick(   STR_HINT_NO_ALIAS,
        "Disable/Enable replacement of aliases.",
        "別名の書き換えの禁止/許可." )

pick(   STR_HINT_ALIAS_PURGE,
        "Delete all currently defined aliases.",
        "現在定義されている別名をすべて削除します." )

pick(   STR_HINT_HWND_ALIAS,
        "Replace HWND values with meaningful text.",
        "HWNDの値を意味のあるテキストに置き換えます." )

pick(   STR_HINT_TASK_ALIAS,
        "Replace task identifiers with meaningful text.",
        "タスクidを意味のあるテキストに置き換えます." )

pick(   STR_HINT_CONV_ALIAS,
        "Replace conversation identifiers with meaningful text.",
        "対話idを意味のあるテキストに置き換えます." )

pick(   STR_HINT_ABOUT,
        "Display program information.",
        "プログラム情報の表示" )

pick(   STR_HINT_HINTBAR,
        "Show/hide the hint bar.",
        "ヒントバ－の表示/非表示" )

pick(   STR_HINT_HELP_CONTENTS,
        "List Open Watcom DDE Spy help topics.",
        "ヘルプの表示" )

pick(   STR_HINT_HELP_SRCH,
        "Search for help on a specific topic.",
        "特定のトピックに関するヘルプの検索" )

pick(   STR_HINT_HELP_ON_HELP,
        "Display information on how to use help.",
        "ヘルプの使用方法に関する情報の表示" )

pick(   STR_TIP_TRK_STR,
        "Track Strings",
        "Track Strings" )
pick(   STR_TIP_TRK_CONV,
        "Track Conversations",
        "Track Conversations" )
pick(   STR_TIP_TRK_LINK,
        "Track Links",
        "Track Links" )
pick(   STR_TIP_TRK_SERVER,
        "Track Servers",
        "Track Servers" )
pick(   STR_TIP_LOG_FILE,
        "Log To File",
        "Log To File" )
pick(   STR_TIP_MARK,
        "Mark",
        "Mark" )
pick(   STR_TIP_CLEAR,
        "Clear",
        "Clear" )
pick(   STR_TIP_HWND_ALIAS,
        "Add HWND Alias",
        "Add HWND Alias" )
pick(   STR_TIP_TASK_ALIAS,
        "Add Task Alias",
        "Add Task Alias" )
pick(   STR_TIP_CONV_ALIAS,
        "Add Conversation Alias",
        "Add Conversation Alias" )

