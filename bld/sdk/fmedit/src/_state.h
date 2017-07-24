/*    id              state cursor  KeyDownActions  KeyUpActions MousePressActions            MouseMoveActions        MouseReleaseActions */
pick( DORMANT,        CURSOR_ARROW, CheckKbdMove,    IgnoreKbd,  ActionBegin,                 CheckMousePosn,         UnexpectedStateRecover ) /* DORMANT          */
pick( OVERBOX,        0,            CheckKbdMove,    IgnoreKbd,  ResizeBegin,                 CheckMousePosn,         UnexpectedStateRecover ) /* OVERBOX          */
pick( MOVING,         CURSOR_CROSS, IgnoreKbd,       IgnoreKbd,  UnexpectedPressStateRecover, DoObjectMove,           FinishMove             ) /* MOVING           */
pick( EDITING,        CURSOR_ARROW, IgnoreKbd,       IgnoreKbd,  ResetEdit,                   CheckMousePosn,         IgnoreMouse            ) /* EDITING          */
pick( SIZING,         0,            IgnoreKbd,       IgnoreKbd,  UnexpectedPressStateRecover, DoObjectResize,         FinishResize           ) /* SIZING           */
pick( CREATING,       CURSOR_ARROW, IgnoreKbd,       IgnoreKbd,  UnexpectedPressStateRecover, DoObjectRecreate,       FinishCreate           ) /* CREATING         */
pick( ALIGNING,       CURSOR_ARROW, IgnoreKbd,       IgnoreKbd,  UnexpectedPressStateRecover, UnexpectedStateRecover, UnexpectedStateRecover ) /* ALIGNING         */
pick( PASTE_PENDING,  CURSOR_CROSS, IgnoreKbd,       IgnoreKbd,  BeginPaste,                  UnexpectedStateRecover, UnexpectedStateRecover ) /* PASTE_PENDING    */
pick( PASTEING,       CURSOR_CROSS, IgnoreKbd,       IgnoreKbd,  UnexpectedPressStateRecover, DoPasteMove,            FinishPaste            ) /* PASTEING         */
pick( SELECTING,      CURSOR_CROSS, IgnoreKbd,       IgnoreKbd,  UnexpectedPressStateRecover, DoSelectRecreate,       FinishSelect           ) /* SELECTING        */
pick( MOVE_PENDING,   CURSOR_CROSS, IgnoreKbd,       IgnoreKbd,  UnexpectedPressStateRecover, BeginMove,              FinishMovePending      ) /* MOVE_PENDING     */
pick( ACTION_ABORTED, 0,            IgnoreKbd,       IgnoreKbd,  UnexpectedPressStateRecover, IgnoreMouse,            FinishActionAborted    ) /* ACTION_ABORTED   */
pick( KBD_MOVING,     0,            ContinueKbdMove, EndKbdMove, IgnoreMousePress,            IgnoreMouse,            IgnoreMouse            ) /* KBD_MOVING       */
