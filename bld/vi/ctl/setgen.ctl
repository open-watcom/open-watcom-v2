ctl_check SETGEN_UNDO
    offsetof( dlg_data, Undo )
    *
    
ctl_check SETGEN_AUTOSAVE
    offsetof( dlg_data, AutoSave )
    *
    
ctl_check SETGEN_SAVECONFIG
    offsetof( dlg_data, SaveConfig )
    *
    
ctl_check SETGEN_SAVEONBUILD
    offsetof( dlg_data, SaveOnBuild )
    *
    
ctl_check SETGEN_BEEPFLAG
    offsetof( dlg_data, BeepFlag )
    *
    
ctl_check SETGEN_QUITMOVESFORWARD
    offsetof( dlg_data, QuitMovesForward )
    *
    
ctl_check SETGEN_SAMEFILECHECK
    offsetof( dlg_data, SameFileCheck )
    *
    
ctl_check SETGEN_MODAL
    offsetof( dlg_data, Modal )
    *
    
ctl_check SETGEN_CASEIGNORE
    offsetof( dlg_data, CaseIgnore )
    *
    
ctl_check SETGEN_SEARCHWRAP
    offsetof( dlg_data, SearchWrap )
    *
    
ctl_text SETGEN_WORD
    offsetof( dlg_data, Word )
    WORDWIDTH
    
ctl_text SETGEN_WORDALT
    offsetof( dlg_data, WordAlt )
    WORDWIDTH
    
ctl_rint SETGEN_AUTOSAVEINTERVAL
    offsetof( dlg_data, AutoSaveInterval )
    1, -1
    
ctl_text SETGEN_TMPDIR
    offsetof( dlg_data, TmpDir )
    TMPDIRWIDTH
    
ctl_text SETGEN_HISTORYFILE
    offsetof( dlg_data, HistoryFile )
    HISTORYFILEWIDTH 
