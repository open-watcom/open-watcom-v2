ctl_combo SETFS_LANGUAGESELECT
    offsetof( dlg_data, Language )
    0, VI_LANG_FIRST, VI_LANG_LAST

ctl_check SETFS_PPKEYWORDONLY
    offsetof( dlg_data, PPKeywordOnly )
    *
    
ctl_check SETFS_CMODE
    offsetof( dlg_data, CMode )
    *
    
ctl_check SETFS_READENTIREFILE
    offsetof( dlg_data, ReadEntireFile )
    *
    
ctl_check SETFS_READONLYCHECK
    offsetof( dlg_data, ReadOnlyCheck )
    *
    
ctl_check SETFS_IGNORECTRLZ
    offsetof( dlg_data, IgnoreCtrlZ )
    *
    
ctl_check SETFS_CRLFAUTODETECT
    offsetof( dlg_data, CRLFAutoDetect )
    *
    
ctl_check SETFS_WRITECRLF
    offsetof( dlg_data, WriteCRLF )
    *
   
ctl_check SETFS_EIGHTBITS
    offsetof( dlg_data, EightBits )
    *
    
ctl_rint SETFS_TABAMOUNT
    offsetof( dlg_data, TabAmount )
    0, -1
    
ctl_check SETFS_REALTABS
    offsetof( dlg_data, RealTabs )
    *
    
ctl_rint SETFS_HARDTAB
    offsetof( dlg_data, HardTab )
    0, -1
    
ctl_check SETFS_AUTOINDENT
    offsetof( dlg_data, AutoIndent )
    *
    
ctl_rint SETFS_SHIFTWIDTH
    offsetof( dlg_data, ShiftWidth )
    0, -1
   
ctl_text SETFS_TAGFILENAME
    offsetof( dlg_data, TagFileName )
    TAGFILENAMEWIDTH
    
ctl_check SETFS_IGNORETAGCASE
    offsetof( dlg_data, IgnoreTagCase )
    *
    
ctl_check SETFS_TAGPROMPT
    offsetof( dlg_data, TagPrompt )
    *
    
ctl_text SETFS_GREPDEFAULT
    offsetof( dlg_data, GrepDefault )
    GREPDEFAULTWIDTH
    
ctl_check SETFS_SHOWMATCH
    offsetof( dlg_data, ShowMatch )
    *
