###################################################################
# This file is preprocessed into a C file by parsectl.exe.
# Add the data controls for the dialog here.
#
###################################################################


ctl_radio DLG_RADIO_START
    offsetof( dlg_data, radio )
    DLG_RADIO_END
    
ctl_combo DLG_LIST_1
    offsetof( dlg_data, list_1 )
    0, STR_LIST_1_START, STR_LIST_1_END
    
ctl_combo DLG_LIST_2
    offsetof( dlg_data, list_2 )
    1, STR_LIST_2_START, STR_LIST_2_END

ctl_check DLG_CHECK
    offsetof( dlg_data, check )
    *

ctl_text DLG_TEXT
    offsetof( dlg_data, text )
    100
    
ctl_int DLG_NUMBER
    offsetof( dlg_data, number )
    *
    
ctl_float DLG_VALUE
    offsetof( dlg_data, value )
    *
    
ctl_dcombo DLG_LIST_3
    offsetof( dlg_data, list_3 )
    1, GetDcomboData
    
ctl_rint DLG_RANGE_NUM
    offsetof( dlg_data, range_num )
    0, -1
    
ctl_rfloat DLG_RANGE_VALUE
    offsetof( dlg_data, range_value )
    -5, 10
