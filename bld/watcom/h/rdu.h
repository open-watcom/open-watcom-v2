
#define usergate_allocate_dos_mem 0x00000000
#define usergate_allocate_local_mem 0x00000001
#define usergate_free_mem 0x00000002
#define usergate_available_small_local_linear 0x00000003
#define usergate_used_local_linear 0x00000004
#define usergate_available_vm_linear 0x00000005
#define usergate_used_vm_linear 0x00000006

#define usergate_reserve_pe_mem 0x00000007
#define usergate_set_flat_linear_valid 0x00000008
#define usergate_set_flat_linear_invalid 0x00000009
#define usergate_set_flat_linear_read 0x0000000A
#define usergate_set_flat_linear_readwrite 0x0000000B

#define usergate_get_raw_switch_ads 0x0000000C
#define usergate_raw_switch_pm 0x0000000D
#define usergate_get_exception 0x0000000E
#define usergate_set_exception 0x0000000F
#define usergate_get_pm_int 0x00000010
#define usergate_set_pm_int 0x00000011
#define usergate_get_vm_int 0x00000012
#define usergate_set_vm_int 0x00000013
#define usergate_dpmi_int 0x00000014
#define usergate_dpmi_call_int 0x00000015
#define usergate_dpmi_call 0x00000016
#define usergate_allocate_vm_callback 0x00000017
#define usergate_free_vm_callback 0x00000018

#define usergate_wait_milli 0x00000019
#define usergate_wait_micro 0x0000001A
#define usergate_wait_until 0x0000001B
#define usergate_create_thread 0x0000001C
#define usergate_terminate_thread 0x0000001D
#define usergate_get_thread 0x0000001E
#define usergate_raw_switch_vm 0x0000001F

#define usergate_create_user_section 0x00000020
#define usergate_create_blocked_user_section 0x00000021
#define usergate_delete_user_section 0x00000022
#define usergate_enter_user_section 0x00000023
#define usergate_leave_user_section 0x00000024

#define usergate_get_local_mailslot 0x00000025
#define usergate_get_remote_mailslot 0x00000026
#define usergate_free_mailslot 0x00000027
#define usergate_send_mailslot 0x00000028
#define usergate_define_mailslot 0x00000029
#define usergate_receive_mailslot 0x0000002A
#define usergate_reply_mailslot 0x0000002B

#define usergate_load_exe 0x0000002C
#define usergate_spawn_exe 0x0000002D
#define usergate_unload_exe 0x0000002E
#define usergate_get_exit_code 0x0000002F
#define usergate_get_exe_name 0x00000030
#define usergate_get_cmd_line 0x00000031
#define usergate_get_env 0x00000032
#define usergate_load_dll 0x00000033
#define usergate_free_dll 0x00000034
#define usergate_get_module_proc 0x00000035
#define usergate_get_module_resource 0x00000036
#define usergate_get_module_name 0x00000037
#define usergate_get_module 0x00000038
#define usergate_allocate_app_mem 0x00000039
#define usergate_free_app_mem 0x0000003A
#define usergate_get_psp_sel 0x0000003B

#define usergate_get_debug_thread 0x0000003D
#define usergate_debug_trace 0x0000003E
#define usergate_debug_pace 0x0000003F
#define usergate_debug_go 0x00000040
#define usergate_debug_next 0x00000041

#define usergate_add_wait_for_debug_event 0x00000042
#define usergate_get_debug_event_data 0x00000043
#define usergate_continue_debug_event 0x00000044
#define usergate_notify_pe_exception 0x00000046
#define usergate_read_thread_mem 0x00000047
#define usergate_write_thread_mem 0x00000048
#define usergate_get_thread_tss 0x00000049
#define usergate_set_thread_tss 0x0000004A

#define usergate_get_cpu_time 0x0000004B
#define usergate_get_system_time 0x0000004C
#define usergate_get_time 0x0000004D
#define usergate_time_to_system_time 0x0000004E
#define usergate_system_time_to_time 0x0000004F
#define usergate_days_in_month 0x00000050
#define usergate_adjust_time 0x00000051
#define usergate_passed_days 0x00000052
#define usergate_time_to_binary 0x00000053
#define usergate_binary_to_time 0x00000054
#define usergate_sync_time 0x00000055

#define usergate_set_focus 0x00000056
#define usergate_enable_focus 0x00000057

#define usergate_get_dns 0x00000058
#define usergate_get_ppp_dns 0x00000059
#define usergate_open_tcp_connection 0x0000005A
#define usergate_wait_for_tcp_connection 0x0000005C
#define usergate_close_tcp_connection 0x0000005D
#define usergate_delete_tcp_connection 0x0000005E
#define usergate_is_tcp_connection_closed 0x0000005F
#define usergate_abort_tcp_connection 0x00000060
#define usergate_read_tcp_connection 0x00000061
#define usergate_write_tcp_connection 0x00000062
#define usergate_push_tcp_connection 0x00000063
#define usergate_get_ip_address 0x00000064
#define usergate_name_to_ip 0x00000065
#define usergate_ip_to_name 0x00000066
#define usergate_ping 0x00000067

#define usergate_get_disc_info 0x00000068
#define usergate_format_drive 0x00000069
#define usergate_get_rdfs_info 0x0000006A
#define usergate_read_disc 0x0000006B
#define usergate_write_disc 0x0000006C
#define usergate_get_drive_info 0x0000006D

#define usergate_set_cur_drive 0x0000006E
#define usergate_get_cur_drive 0x0000006F
#define usergate_set_cur_dir 0x00000070
#define usergate_get_cur_dir 0x00000071
#define usergate_make_dir 0x00000072
#define usergate_remove_dir 0x00000073
#define usergate_rename_file 0x00000074
#define usergate_delete_file 0x00000075
#define usergate_get_file_attribute 0x00000076
#define usergate_set_file_attribute 0x00000077
#define usergate_open_dir 0x00000078
#define usergate_close_dir 0x00000079
#define usergate_read_dir 0x0000007A

#define usergate_open_file 0x0000007B
#define usergate_create_file 0x0000007C
#define usergate_close_file 0x0000007D
#define usergate_dupl_file 0x0000007E
#define usergate_get_ioctl_data 0x0000007F
#define usergate_get_file_size 0x00000080
#define usergate_set_file_size 0x00000081
#define usergate_get_file_pos 0x00000082
#define usergate_set_file_pos 0x00000083
#define usergate_get_file_time 0x00000084
#define usergate_set_file_time 0x00000085
#define usergate_read_file 0x00000086
#define usergate_write_file 0x00000087
#define usergate_read_con 0x00000088

#define usergate_create_mapping 0x00000089
#define usergate_create_named_mapping 0x0000008A
#define usergate_create_file_mapping 0x0000008B
#define usergate_create_named_file_mapping 0x0000008C
#define usergate_open_named_mapping 0x0000008D
#define usergate_sync_mapping 0x0000008E
#define usergate_close_mapping 0x0000008F
#define usergate_map_view 0x00000090
#define usergate_unmap_view 0x00000091

#define usergate_read_keyboard 0x00000092
#define usergate_poll_keyboard 0x00000093
#define usergate_flush_keyboard 0x00000094
#define usergate_get_keyboard_state 0x00000097

#define usergate_show_mouse 0x00000098
#define usergate_hide_mouse 0x00000099
#define usergate_get_mouse_position 0x0000009A
#define usergate_set_mouse_position 0x0000009B
#define usergate_set_mouse_window 0x0000009C
#define usergate_set_mouse_mickey 0x0000009D
#define usergate_get_left_button 0x0000009E
#define usergate_get_right_button 0x0000009F
#define usergate_get_left_button_press_position 0x000000A0
#define usergate_get_right_button_press_position 0x000000A1
#define usergate_get_left_button_release_position 0x000000A2
#define usergate_get_right_button_release_position 0x000000A3
#define usergate_hook_mouse 0x000000A4
#define usergate_unhook_mouse 0x000000A5

#define usergate_lock_cd 0x000000A6
#define usergate_unlock_cd 0x000000A7
#define usergate_eject_cd 0x000000A8
#define usergate_insert_cd 0x000000A9

#define usergate_open_com 0x000000AA
#define usergate_close_com 0x000000AB
#define usergate_flush_com 0x000000AC
#define usergate_add_wait_for_com 0x000000AE
#define usergate_read_com 0x000000AF
#define usergate_write_com 0x000000B0
#define usergate_set_dtr 0x000000B1
#define usergate_reset_dtr 0x000000B2
#define usergate_reset_com 0x000000B3

#define usergate_set_cursor_position 0x000000B6
#define usergate_get_cursor_position 0x000000B7
#define usergate_write_char 0x000000B8
#define usergate_write_asciiz 0x000000B9
#define usergate_write_size_string 0x000000BA

#define usergate_set_video_mode 0x000000BB
#define usergate_set_vga_mode 0x000000BC
#define usergate_set_forecolor 0x000000BD
#define usergate_set_backcolor 0x000000BE
#define usergate_get_string_metrics 0x000000C1
#define usergate_set_font 0x000000C2
#define usergate_get_video_mode 0x000000C3
#define usergate_draw_string 0x000000C5

#define usergate_xms_handler 0x000000C6
#define usergate_ems_handler 0x000000C7

#define usergate_resize_flat_linear 0x000000C9

#define usergate_set_drawcolor 0x000000CA
#define usergate_set_lgop 0x000000CB
#define usergate_set_hollow_style 0x000000CC
#define usergate_set_filled_style 0x000000CD
#define usergate_get_pixel 0x000000CE
#define usergate_set_pixel 0x000000CF
#define usergate_draw_line 0x000000D0
#define usergate_draw_rect 0x000000D1
#define usergate_draw_ellipse 0x000000D2
#define usergate_create_bitmap 0x000000D3
#define usergate_close_bitmap 0x000000D4
#define usergate_blit 0x000000D5
#define usergate_open_font 0x000000D7
#define usergate_close_font 0x000000D8
#define usergate_create_string_bitmap 0x000000D9
#define usergate_get_bitmap_info 0x000000DA
#define usergate_draw_mask 0x000000DB

#define usergate_create_sprite 0x000000DC
#define usergate_close_sprite 0x000000DD
#define usergate_show_sprite 0x000000DE
#define usergate_hide_sprite 0x000000DF
#define usergate_move_sprite 0x000000E0

#define usergate_dup_bitmap_handle 0x000000E1
#define usergate_set_clip_rect 0x000000E2
#define usergate_clear_clip_rect 0x000000E3

#define usergate_set_rts 0x000000E4
#define usergate_reset_rts 0x000000E5
#define usergate_get_com_receive_space 0x000000E6
#define usergate_get_com_send_space 0x000000E7

#define usergate_get_char_attrib 0x000000E8

#define usergate_create_wait 0x000000E9
#define usergate_close_wait 0x000000EA
#define usergate_is_wait_idle 0x000000EB
#define usergate_wait_no_timeout 0x000000EC
#define usergate_wait_timeout 0x000000ED
#define usergate_stop_wait 0x000000EE
#define usergate_add_wait_for_keyboard 0x000000EF

#define usergate_peek_key_event 0x000000F0
#define usergate_read_key_event 0x000000F1

#define usergate_add_wait_for_mouse 0x000000F2
#define usergate_remove_wait 0x000000F3

#define usergate_add_wait_for_adc 0x000000F4
#define usergate_open_adc 0x000000F5
#define usergate_close_adc 0x000000F6
#define usergate_define_adc_time 0x000000F7
#define usergate_read_adc 0x000000F8

#define usergate_free_v86 0x000000F9

#define usergate_read_serial_lines 0x000000FA
#define usergate_toggle_serial_line 0x000000FB

#define usergate_read_serial_val 0x000000FC
#define usergate_write_serial_val 0x000000FD

#define usergate_create_file_drive 0x000000FE
#define usergate_open_file_drive 0x000000FF

#define usergate_dos_ext_exec 0x00000100

#define usergate_open_sys_env 0x00000101
#define usergate_open_proc_env 0x00000102
#define usergate_close_env 0x00000103
#define usergate_add_env_var 0x00000104
#define usergate_delete_env_var 0x00000105
#define usergate_find_env_var 0x00000106
#define usergate_get_env_data 0x00000107
#define usergate_set_env_data 0x00000108

#define usergate_open_sys_ini 0x00000109
#define usergate_close_ini 0x0000010A
#define usergate_goto_ini_section 0x0000010B
#define usergate_remove_ini_section 0x0000010C
#define usergate_read_ini 0x0000010D
#define usergate_write_ini 0x0000010E
#define usergate_delete_ini 0x0000010F

#define usergate_get_version 0x00000111

#define usergate_enable_status_led 0x00000112
#define usergate_disable_status_led 0x00000113

#define usergate_start_watchdog 0x00000114
#define usergate_kick_watchdog 0x00000115

#define usergate_erase_disc_sectors 0x00000116

#define usergate_enable_cts 0x00000118
#define usergate_disable_cts 0x00000119

#define usergate_wait_for_send_completed_com 0x0000011A

#define usergate_add_wait_for_tcp_connection 0x0000011B

#define usergate_get_remote_tcp_connection_ip 0x0000011C
#define usergate_get_remote_tcp_connection_port 0x0000011D
#define usergate_get_local_tcp_connection_port 0x0000011E

#define usergate_enable_auto_rts 0x0000011F
#define usergate_disable_auto_rts 0x00000120

#define usergate_put_keyboard_code 0x00000121

#define usergate_poll_tcp_connection 0x00000122

#define usergate_create_signal 0x00000123
#define usergate_free_signal 0x00000124
#define usergate_add_wait_for_signal 0x00000125
#define usergate_set_signal 0x00000126
#define usergate_reset_signal 0x00000127
#define usergate_is_signalled 0x00000128

#define usergate_get_drive_disc_param 0x00000129

#define usergate_get_ide_disc 0x0000012A
#define usergate_get_floppy_disc 0x0000012B
#define usergate_demand_load_drive 0x0000012C
#define usergate_set_disc_info 0x0000012D

#define usergate_get_thread_state 0x0000012E
#define usergate_suspend_thread 0x0000012F
#define usergate_resume_thread 0x00000130

#define usergate_update_time 0x00000131

#define usergate_allocate_static_drive 0x00000132
#define usergate_allocate_fixed_drive 0x00000133
#define usergate_allocate_dynamic_drive 0x00000134

#define usergate_get_focus 0x00000135

#define usergate_add_wait_for_tcp_listen 0x00000136
#define usergate_create_tcp_listen 0x00000137
#define usergate_get_tcp_listen 0x00000138
#define usergate_close_tcp_listen 0x00000139

#define usergate_get_random 0x0000013A

#define usergate_suspend_and_signal_thread 0x0000013B

#define usergate_start_net_capture 0x0000013C
#define usergate_stop_net_capture 0x0000013D

#define usergate_create_crc 0x0000013E
#define usergate_close_crc 0x0000013F
#define usergate_calc_crc 0x00000140

#define usergate_get_usb_device 0x00000141
#define usergate_get_usb_config 0x00000142

#define usergate_open_usb_pipe 0x00000143
#define usergate_close_usb_pipe 0x00000144
#define usergate_add_wait_for_usb_pipe 0x00000145
#define usergate_req_usb_data 0x00000146
#define usergate_get_usb_data_size 0x00000147
#define usergate_write_usb_data 0x00000148
#define usergate_req_usb_status 0x00000149
#define usergate_write_usb_status 0x0000014A
#define usergate_write_usb_control 0x0000014B
#define usergate_lock_usb_pipe 0x0000014D
#define usergate_unlock_usb_pipe 0x0000014E

#define usergate_get_max_com_port 0x0000014F

#define usergate_config_usb_device 0x00000150

#define usergate_start_usb_transaction 0x00000151

#define usergate_open_ini 0x00000152

#define usergate_open_icsp 0x00000153
#define usergate_close_icsp 0x00000154
#define usergate_write_icsp_cmd 0x00000155
#define usergate_write_icsp_data 0x00000156
#define usergate_read_icsp_data 0x00000157

#define usergate_get_gateway 0x00000158

#define usergate_get_free_physical 0x00000159
#define usergate_get_free_gdt 0x0000015A
#define usergate_available_big_linear 0x0000015B
#define usergate_available_small_linear 0x0000015C

#define usergate_get_ip_mask 0x0000015D

#define usergate_get_master_volume 0x0000015E
#define usergate_set_master_volume 0x0000015F

#define usergate_get_line_out_volume 0x00000160
#define usergate_set_line_out_volume 0x00000161

#define usergate_create_audio_out_channel 0x00000162
#define usergate_close_audio_out_channel 0x00000163
#define usergate_write_audio 0x00000164

#define usergate_stop_watchdog 0x00000165

#define usergate_is_valid_usergate 0x00000166

#define usergate_open_fm 0x00000167
#define usergate_close_fm 0x00000168
#define usergate_fm_wait 0x00000169
#define usergate_create_fm_instrument 0x0000016A
#define usergate_free_fm_instrument 0x0000016B
#define usergate_set_fm_attack 0x0000016C
#define usergate_set_fm_sustain 0x0000016D
#define usergate_set_fm_release 0x0000016E
#define usergate_play_fm_note 0x0000016F

#define usergate_get_thread_linear 0x00000170
#define usergate_get_current_module 0x00000171

#define usergate_get_module_focus_key 0x00000172

#define usergate_get_debug_event 0x00000173
#define usergate_clear_debug_event 0x00000174

#define usergate_get_free_handles 0x00000175

#define usergate_get_env_size 0x00000176

#define usergate_show_exception_text 0x00000177

#define usergate_get_watchdog_tics 0x00000178

#define usergate_is_usb_trans_done 0x00000179
#define usergate_was_usb_trans_ok 0x0000017A

#define usergate_is_tcp_connection_idle 0x0000017B

#define usergate_set_codec_gpio0 0x0000017C

#define usergate_free_proc_handle 0x0000017D
#define usergate_add_wait_for_proc_end 0x0000017E
#define usergate_get_proc_exit_code 0x0000017F

#define usergate_get_cpu_version 0x00000180
#define usergate_get_options 0x00000181

#define usergate_fork 0x00000182

#define usergate_get_core_id 0x00000183

#define usergate_create_user_read_write_section 0x00000184
#define usergate_delete_user_read_write_section 0x00000185
#define usergate_enter_user_read_section 0x00000186
#define usergate_leave_user_read_section 0x00000187
#define usergate_enter_user_write_section 0x00000188
#define usergate_leave_user_write_section 0x00000189

#define usergate_power_failure 0x0000018A

#define usergate_define_fault_save 0x0000018B
#define usergate_get_fault_thread_state 0x0000018C
#define usergate_get_fault_thread_tss 0x0000018D
#define usergate_clear_fault_save 0x0000018E

#define usergate_get_free_handle_mem 0x0000018F

#define usergate_get_image_header 0x00000190
#define usergate_get_image_data 0x00000191

#define usergate_allocate_debug_app_mem 0x00000192
#define usergate_free_debug_app_mem 0x00000193

#define usergate_get_key_layout 0x00000194
#define usergate_set_key_layout 0x00000195

#define usergate_get_device_info 0x00000196
#define usergate_get_selector_info 0x00000197

#define usergate_set_code_break 0x00000198
#define usergate_set_read_data_break 0x00000199
#define usergate_set_write_data_break 0x0000019A
#define usergate_clear_break 0x0000019B

#define usergate_get_max_printer 0x0000019C
#define usergate_open_printer 0x0000019D
#define usergate_close_printer 0x0000019E

#define usergate_is_printer_jammed 0x0000019F
#define usergate_is_printer_paper_low 0x000001A0
#define usergate_is_printer_paper_end 0x000001A1
#define usergate_is_printer_ok 0x000001A2
#define usergate_is_printer_head_lifted 0x000001A3
#define usergate_has_printer_paper_in_presenter 0x000001A4

#define usergate_print_test 0x000001A5

#define usergate_create_printer_bitmap 0x000001A6
#define usergate_print_bitmap 0x000001A7

#define usergate_present_printer_media 0x000001A8
#define usergate_eject_printer_media 0x000001A9

#define usergate_wait_for_print 0x000001AA

#define usergate_has_audio 0x000001AB

#define usergate_available_big_local_linear 0x000001AC

#define usergate_get_allocated_usb_blocks 0x000001AD
#define usergate_get_usb_close_count 0x000001AE

#define usergate_reset_usb_pipe 0x000001AF

#define usergate_add_syslog 0x000001B0
#define usergate_open_syslog 0x000001B1
#define usergate_close_syslog 0x000001B2
#define usergate_add_wait_for_syslog 0x000001B3
#define usergate_get_syslog 0x000001B4

#define usergate_soft_reset 0x000001B5
#define usergate_hard_reset 0x000001B6

#define usergate_is_emergency_stopped 0x000001B7

#define usergate_load_device32 0x000001B8

#define usergate_debug_run 0x000001B9

#define usergate_test_gate 0x000001BA

#define usergate_get_acpi_status 0x000001BB
#define usergate_get_acpi_object 0x000001BC
#define usergate_get_cpu_temperature 0x000001BD
#define usergate_get_acpi_method 0x000001BE

#define usergate_write_attrib_string 0x000001BF

#define usergate_has_hard_reset 0x000001C1

#define usergate_get_acpi_device 0x000001C2
#define usergate_get_acpi_device_irq 0x000001C3
#define usergate_get_acpi_device_io 0x000001C4
#define usergate_get_acpi_device_mem 0x000001C5

#define usergate_get_pci_device_name 0x000001C6
#define usergate_get_pci_device_info 0x000001C7
#define usergate_get_pci_device_vendor 0x000001C8
#define usergate_get_pci_device_class 0x000001C9
#define usergate_get_pci_device_irq 0x000001CA

#define usergate_open_hid 0x000001CB
#define usergate_close_hid 0x000001CC
#define usergate_get_hid_pipe 0x000001CD

#define usergate_get_core_load 0x000001CE
#define usergate_get_core_duty 0x000001CF

#define usergate_has_global_timer 0x000001D0

#define usergate_remote_debug 0x000001D1

#define usergate_read_hid 0x000001D2
#define usergate_write_hid 0x000001D3

#define usergate_ansi_to_utf8 0x000001D4
#define usergate_utf8_to_ansi 0x000001D5

#define usergate_get_printer_name 0x000001D6

#define usergate_acquire_futex 0x000001D7
#define usergate_release_futex 0x000001D8
#define usergate_cleanup_futex 0x000001D9

#define usergate_goto_first_inivar 0x000001DA
#define usergate_goto_next_inivar 0x000001DB
#define usergate_get_curr_inivar 0x000001DC

#define usergate_reset_printer 0x000001DD

#define usergate_has_icsp 0x000001DE
#define usergate_reset_icsp 0x000001DF

#define usergate_get_max_carddev 0x000001E0
#define usergate_open_carddev 0x000001E1
#define usergate_close_carddev 0x000001E2
#define usergate_is_carddev_ok 0x000001E3
#define usergate_wait_for_card 0x000001E4
#define usergate_get_carddev_name 0x000001E5

#define usergate_is_carddev_busy 0x000001E6
#define usergate_is_carddev_inserted 0x000001E7

#define usergate_has_touch 0x000001E8

#define usergate_had_carddev_inserted 0x000001E9
#define usergate_clear_carddev_inserted 0x000001EA

#define usergate_is_printer_cutter_jammed 0x000001EB
#define usergate_has_printer_temp_error 0x000001EC
#define usergate_has_printer_feed_error 0x000001ED

#define usergate_is_64_bit_exe 0x000001EE

#define usergate_get_audio_device_count 0x000001EF
#define usergate_get_audio_codec_count 0x000001F0
#define usergate_get_audio_widget_info 0x000001F1
#define usergate_get_audio_widget_connection_list 0x000001F2
#define usergate_get_selected_audio_connection 0x000001F3

#define usergate_get_audio_input_amp_cap 0x000001F4
#define usergate_get_audio_output_amp_cap 0x000001F5

#define usergate_has_audio_input_mute 0x000001F6
#define usergate_has_audio_output_mute 0x000001F7

#define usergate_read_audio_input_amp 0x000001F8
#define usergate_read_audio_output_amp 0x000001F9

#define usergate_is_audio_input_amp_muted 0x000001FA
#define usergate_is_audio_output_amp_muted 0x000001FB

#define usergate_get_fixed_audio_output 0x000001FC
#define usergate_get_jack_audio_output 0x000001FD
#define usergate_get_jack_audio_input 0x000001FE

#define usergate_get_output_volume 0x000001FF
#define usergate_set_output_volume 0x00000200

#define usergate_broadcast_query_udp 0x00000201
#define usergate_send_udp 0x00000202

#define usergate_open_udp_connection 0x00000203
#define usergate_close_udp_connection 0x00000204
#define usergate_send_udp_connection 0x00000205
#define usergate_add_wait_for_udp_connection 0x00000206
#define usergate_peek_udp_connection 0x00000207
#define usergate_read_udp_connection 0x00000208

#define usergate_extract_alpha_bitmap 0x00000209
#define usergate_extract_valid_bitmap_mask 0x0000020A
#define usergate_extract_invalid_bitmap_mask 0x0000020B

#define usergate_create_alpha_bitmap 0x0000020C

#define usergate_create_long_thread 0x0000020D



#ifdef __FLAT__

#define CallGate_allocate_dos_mem 0x67 0x9a 0 0 0 0 3 0
#define CallGate_allocate_local_mem 0x67 0x9a 1 0 0 0 3 0
#define CallGate_free_mem 0x67 0x9a 2 0 0 0 3 0
#define CallGate_available_small_local_linear 0x67 0x9a 3 0 0 0 3 0
#define CallGate_used_local_linear 0x67 0x9a 4 0 0 0 3 0
#define CallGate_available_vm_linear 0x67 0x9a 5 0 0 0 3 0
#define CallGate_used_vm_linear 0x67 0x9a 6 0 0 0 3 0

#define CallGate_reserve_pe_mem 0x67 0x9a 7 0 0 0 3 0
#define CallGate_set_flat_linear_valid 0x67 0x9a 8 0 0 0 3 0
#define CallGate_set_flat_linear_invalid 0x67 0x9a 9 0 0 0 3 0
#define CallGate_set_flat_linear_read 0x67 0x9a 10 0 0 0 3 0
#define CallGate_set_flat_linear_readwrite 0x67 0x9a 11 0 0 0 3 0

#define CallGate_get_raw_switch_ads 0x67 0x9a 12 0 0 0 3 0
#define CallGate_raw_switch_pm 0x67 0x9a 13 0 0 0 3 0
#define CallGate_get_exception 0x67 0x9a 14 0 0 0 3 0
#define CallGate_set_exception 0x67 0x9a 15 0 0 0 3 0
#define CallGate_get_pm_int 0x67 0x9a 16 0 0 0 3 0
#define CallGate_set_pm_int 0x67 0x9a 17 0 0 0 3 0
#define CallGate_get_vm_int 0x67 0x9a 18 0 0 0 3 0
#define CallGate_set_vm_int 0x67 0x9a 19 0 0 0 3 0
#define CallGate_dpmi_int 0x67 0x9a 20 0 0 0 3 0
#define CallGate_dpmi_call_int 0x67 0x9a 21 0 0 0 3 0
#define CallGate_dpmi_call 0x67 0x9a 22 0 0 0 3 0
#define CallGate_allocate_vm_callback 0x67 0x9a 23 0 0 0 3 0
#define CallGate_free_vm_callback 0x67 0x9a 24 0 0 0 3 0

#define CallGate_wait_milli 0x67 0x9a 25 0 0 0 3 0
#define CallGate_wait_micro 0x67 0x9a 26 0 0 0 3 0
#define CallGate_wait_until 0x67 0x9a 27 0 0 0 3 0
#define CallGate_create_thread 0x67 0x9a 28 0 0 0 3 0
#define CallGate_terminate_thread 0x67 0x9a 29 0 0 0 3 0
#define CallGate_get_thread 0x67 0x9a 30 0 0 0 3 0
#define CallGate_raw_switch_vm 0x67 0x9a 31 0 0 0 3 0

#define CallGate_create_user_section 0x67 0x9a 32 0 0 0 3 0
#define CallGate_create_blocked_user_section 0x67 0x9a 33 0 0 0 3 0
#define CallGate_delete_user_section 0x67 0x9a 34 0 0 0 3 0
#define CallGate_enter_user_section 0x67 0x9a 35 0 0 0 3 0
#define CallGate_leave_user_section 0x67 0x9a 36 0 0 0 3 0

#define CallGate_get_local_mailslot 0x67 0x9a 37 0 0 0 3 0
#define CallGate_get_remote_mailslot 0x67 0x9a 38 0 0 0 3 0
#define CallGate_free_mailslot 0x67 0x9a 39 0 0 0 3 0
#define CallGate_send_mailslot 0x67 0x9a 40 0 0 0 3 0
#define CallGate_define_mailslot 0x67 0x9a 41 0 0 0 3 0
#define CallGate_receive_mailslot 0x67 0x9a 42 0 0 0 3 0
#define CallGate_reply_mailslot 0x67 0x9a 43 0 0 0 3 0

#define CallGate_load_exe 0x67 0x9a 44 0 0 0 3 0
#define CallGate_spawn_exe 0x67 0x9a 45 0 0 0 3 0
#define CallGate_unload_exe 0x67 0x9a 46 0 0 0 3 0
#define CallGate_get_exit_code 0x67 0x9a 47 0 0 0 3 0
#define CallGate_get_exe_name 0x67 0x9a 48 0 0 0 3 0
#define CallGate_get_cmd_line 0x67 0x9a 49 0 0 0 3 0
#define CallGate_get_env 0x67 0x9a 50 0 0 0 3 0
#define CallGate_load_dll 0x67 0x9a 51 0 0 0 3 0
#define CallGate_free_dll 0x67 0x9a 52 0 0 0 3 0
#define CallGate_get_module_proc 0x67 0x9a 53 0 0 0 3 0
#define CallGate_get_module_resource 0x67 0x9a 54 0 0 0 3 0
#define CallGate_get_module_name 0x67 0x9a 55 0 0 0 3 0
#define CallGate_get_module 0x67 0x9a 56 0 0 0 3 0
#define CallGate_allocate_app_mem 0x67 0x9a 57 0 0 0 3 0
#define CallGate_free_app_mem 0x67 0x9a 58 0 0 0 3 0
#define CallGate_get_psp_sel 0x67 0x9a 59 0 0 0 3 0

#define CallGate_get_debug_thread 0x67 0x9a 61 0 0 0 3 0
#define CallGate_debug_trace 0x67 0x9a 62 0 0 0 3 0
#define CallGate_debug_pace 0x67 0x9a 63 0 0 0 3 0
#define CallGate_debug_go 0x67 0x9a 64 0 0 0 3 0
#define CallGate_debug_next 0x67 0x9a 65 0 0 0 3 0

#define CallGate_add_wait_for_debug_event 0x67 0x9a 66 0 0 0 3 0
#define CallGate_get_debug_event_data 0x67 0x9a 67 0 0 0 3 0
#define CallGate_continue_debug_event 0x67 0x9a 68 0 0 0 3 0
#define CallGate_notify_pe_exception 0x67 0x9a 70 0 0 0 3 0
#define CallGate_read_thread_mem 0x67 0x9a 71 0 0 0 3 0
#define CallGate_write_thread_mem 0x67 0x9a 72 0 0 0 3 0
#define CallGate_get_thread_tss 0x67 0x9a 73 0 0 0 3 0
#define CallGate_set_thread_tss 0x67 0x9a 74 0 0 0 3 0

#define CallGate_get_cpu_time 0x67 0x9a 75 0 0 0 3 0
#define CallGate_get_system_time 0x67 0x9a 76 0 0 0 3 0
#define CallGate_get_time 0x67 0x9a 77 0 0 0 3 0
#define CallGate_time_to_system_time 0x67 0x9a 78 0 0 0 3 0
#define CallGate_system_time_to_time 0x67 0x9a 79 0 0 0 3 0
#define CallGate_days_in_month 0x67 0x9a 80 0 0 0 3 0
#define CallGate_adjust_time 0x67 0x9a 81 0 0 0 3 0
#define CallGate_passed_days 0x67 0x9a 82 0 0 0 3 0
#define CallGate_time_to_binary 0x67 0x9a 83 0 0 0 3 0
#define CallGate_binary_to_time 0x67 0x9a 84 0 0 0 3 0
#define CallGate_sync_time 0x67 0x9a 85 0 0 0 3 0

#define CallGate_set_focus 0x67 0x9a 86 0 0 0 3 0
#define CallGate_enable_focus 0x67 0x9a 87 0 0 0 3 0

#define CallGate_get_dns 0x67 0x9a 88 0 0 0 3 0
#define CallGate_get_ppp_dns 0x67 0x9a 89 0 0 0 3 0
#define CallGate_open_tcp_connection 0x67 0x9a 90 0 0 0 3 0
#define CallGate_wait_for_tcp_connection 0x67 0x9a 92 0 0 0 3 0
#define CallGate_close_tcp_connection 0x67 0x9a 93 0 0 0 3 0
#define CallGate_delete_tcp_connection 0x67 0x9a 94 0 0 0 3 0
#define CallGate_is_tcp_connection_closed 0x67 0x9a 95 0 0 0 3 0
#define CallGate_abort_tcp_connection 0x67 0x9a 96 0 0 0 3 0
#define CallGate_read_tcp_connection 0x67 0x9a 97 0 0 0 3 0
#define CallGate_write_tcp_connection 0x67 0x9a 98 0 0 0 3 0
#define CallGate_push_tcp_connection 0x67 0x9a 99 0 0 0 3 0
#define CallGate_get_ip_address 0x67 0x9a 100 0 0 0 3 0
#define CallGate_name_to_ip 0x67 0x9a 101 0 0 0 3 0
#define CallGate_ip_to_name 0x67 0x9a 102 0 0 0 3 0
#define CallGate_ping 0x67 0x9a 103 0 0 0 3 0

#define CallGate_get_disc_info 0x67 0x9a 104 0 0 0 3 0
#define CallGate_format_drive 0x67 0x9a 105 0 0 0 3 0
#define CallGate_get_rdfs_info 0x67 0x9a 106 0 0 0 3 0
#define CallGate_read_disc 0x67 0x9a 107 0 0 0 3 0
#define CallGate_write_disc 0x67 0x9a 108 0 0 0 3 0
#define CallGate_get_drive_info 0x67 0x9a 109 0 0 0 3 0

#define CallGate_set_cur_drive 0x67 0x9a 110 0 0 0 3 0
#define CallGate_get_cur_drive 0x67 0x9a 111 0 0 0 3 0
#define CallGate_set_cur_dir 0x67 0x9a 112 0 0 0 3 0
#define CallGate_get_cur_dir 0x67 0x9a 113 0 0 0 3 0
#define CallGate_make_dir 0x67 0x9a 114 0 0 0 3 0
#define CallGate_remove_dir 0x67 0x9a 115 0 0 0 3 0
#define CallGate_rename_file 0x67 0x9a 116 0 0 0 3 0
#define CallGate_delete_file 0x67 0x9a 117 0 0 0 3 0
#define CallGate_get_file_attribute 0x67 0x9a 118 0 0 0 3 0
#define CallGate_set_file_attribute 0x67 0x9a 119 0 0 0 3 0
#define CallGate_open_dir 0x67 0x9a 120 0 0 0 3 0
#define CallGate_close_dir 0x67 0x9a 121 0 0 0 3 0
#define CallGate_read_dir 0x67 0x9a 122 0 0 0 3 0

#define CallGate_open_file 0x67 0x9a 123 0 0 0 3 0
#define CallGate_create_file 0x67 0x9a 124 0 0 0 3 0
#define CallGate_close_file 0x67 0x9a 125 0 0 0 3 0
#define CallGate_dupl_file 0x67 0x9a 126 0 0 0 3 0
#define CallGate_get_ioctl_data 0x67 0x9a 127 0 0 0 3 0
#define CallGate_get_file_size 0x67 0x9a 128 0 0 0 3 0
#define CallGate_set_file_size 0x67 0x9a 129 0 0 0 3 0
#define CallGate_get_file_pos 0x67 0x9a 130 0 0 0 3 0
#define CallGate_set_file_pos 0x67 0x9a 131 0 0 0 3 0
#define CallGate_get_file_time 0x67 0x9a 132 0 0 0 3 0
#define CallGate_set_file_time 0x67 0x9a 133 0 0 0 3 0
#define CallGate_read_file 0x67 0x9a 134 0 0 0 3 0
#define CallGate_write_file 0x67 0x9a 135 0 0 0 3 0
#define CallGate_read_con 0x67 0x9a 136 0 0 0 3 0

#define CallGate_create_mapping 0x67 0x9a 137 0 0 0 3 0
#define CallGate_create_named_mapping 0x67 0x9a 138 0 0 0 3 0
#define CallGate_create_file_mapping 0x67 0x9a 139 0 0 0 3 0
#define CallGate_create_named_file_mapping 0x67 0x9a 140 0 0 0 3 0
#define CallGate_open_named_mapping 0x67 0x9a 141 0 0 0 3 0
#define CallGate_sync_mapping 0x67 0x9a 142 0 0 0 3 0
#define CallGate_close_mapping 0x67 0x9a 143 0 0 0 3 0
#define CallGate_map_view 0x67 0x9a 144 0 0 0 3 0
#define CallGate_unmap_view 0x67 0x9a 145 0 0 0 3 0

#define CallGate_read_keyboard 0x67 0x9a 146 0 0 0 3 0
#define CallGate_poll_keyboard 0x67 0x9a 147 0 0 0 3 0
#define CallGate_flush_keyboard 0x67 0x9a 148 0 0 0 3 0
#define CallGate_get_keyboard_state 0x67 0x9a 151 0 0 0 3 0

#define CallGate_show_mouse 0x67 0x9a 152 0 0 0 3 0
#define CallGate_hide_mouse 0x67 0x9a 153 0 0 0 3 0
#define CallGate_get_mouse_position 0x67 0x9a 154 0 0 0 3 0
#define CallGate_set_mouse_position 0x67 0x9a 155 0 0 0 3 0
#define CallGate_set_mouse_window 0x67 0x9a 156 0 0 0 3 0
#define CallGate_set_mouse_mickey 0x67 0x9a 157 0 0 0 3 0
#define CallGate_get_left_button 0x67 0x9a 158 0 0 0 3 0
#define CallGate_get_right_button 0x67 0x9a 159 0 0 0 3 0
#define CallGate_get_left_button_press_position 0x67 0x9a 160 0 0 0 3 0
#define CallGate_get_right_button_press_position 0x67 0x9a 161 0 0 0 3 0
#define CallGate_get_left_button_release_position 0x67 0x9a 162 0 0 0 3 0
#define CallGate_get_right_button_release_position 0x67 0x9a 163 0 0 0 3 0
#define CallGate_hook_mouse 0x67 0x9a 164 0 0 0 3 0
#define CallGate_unhook_mouse 0x67 0x9a 165 0 0 0 3 0

#define CallGate_lock_cd 0x67 0x9a 166 0 0 0 3 0
#define CallGate_unlock_cd 0x67 0x9a 167 0 0 0 3 0
#define CallGate_eject_cd 0x67 0x9a 168 0 0 0 3 0
#define CallGate_insert_cd 0x67 0x9a 169 0 0 0 3 0

#define CallGate_open_com 0x67 0x9a 170 0 0 0 3 0
#define CallGate_close_com 0x67 0x9a 171 0 0 0 3 0
#define CallGate_flush_com 0x67 0x9a 172 0 0 0 3 0
#define CallGate_add_wait_for_com 0x67 0x9a 174 0 0 0 3 0
#define CallGate_read_com 0x67 0x9a 175 0 0 0 3 0
#define CallGate_write_com 0x67 0x9a 176 0 0 0 3 0
#define CallGate_set_dtr 0x67 0x9a 177 0 0 0 3 0
#define CallGate_reset_dtr 0x67 0x9a 178 0 0 0 3 0
#define CallGate_reset_com 0x67 0x9a 179 0 0 0 3 0

#define CallGate_set_cursor_position 0x67 0x9a 182 0 0 0 3 0
#define CallGate_get_cursor_position 0x67 0x9a 183 0 0 0 3 0
#define CallGate_write_char 0x67 0x9a 184 0 0 0 3 0
#define CallGate_write_asciiz 0x67 0x9a 185 0 0 0 3 0
#define CallGate_write_size_string 0x67 0x9a 186 0 0 0 3 0

#define CallGate_set_video_mode 0x67 0x9a 187 0 0 0 3 0
#define CallGate_set_vga_mode 0x67 0x9a 188 0 0 0 3 0
#define CallGate_set_forecolor 0x67 0x9a 189 0 0 0 3 0
#define CallGate_set_backcolor 0x67 0x9a 190 0 0 0 3 0
#define CallGate_get_string_metrics 0x67 0x9a 193 0 0 0 3 0
#define CallGate_set_font 0x67 0x9a 194 0 0 0 3 0
#define CallGate_get_video_mode 0x67 0x9a 195 0 0 0 3 0
#define CallGate_draw_string 0x67 0x9a 197 0 0 0 3 0

#define CallGate_xms_handler 0x67 0x9a 198 0 0 0 3 0
#define CallGate_ems_handler 0x67 0x9a 199 0 0 0 3 0

#define CallGate_resize_flat_linear 0x67 0x9a 201 0 0 0 3 0

#define CallGate_set_drawcolor 0x67 0x9a 202 0 0 0 3 0
#define CallGate_set_lgop 0x67 0x9a 203 0 0 0 3 0
#define CallGate_set_hollow_style 0x67 0x9a 204 0 0 0 3 0
#define CallGate_set_filled_style 0x67 0x9a 205 0 0 0 3 0
#define CallGate_get_pixel 0x67 0x9a 206 0 0 0 3 0
#define CallGate_set_pixel 0x67 0x9a 207 0 0 0 3 0
#define CallGate_draw_line 0x67 0x9a 208 0 0 0 3 0
#define CallGate_draw_rect 0x67 0x9a 209 0 0 0 3 0
#define CallGate_draw_ellipse 0x67 0x9a 210 0 0 0 3 0
#define CallGate_create_bitmap 0x67 0x9a 211 0 0 0 3 0
#define CallGate_close_bitmap 0x67 0x9a 212 0 0 0 3 0
#define CallGate_blit 0x67 0x9a 213 0 0 0 3 0
#define CallGate_open_font 0x67 0x9a 215 0 0 0 3 0
#define CallGate_close_font 0x67 0x9a 216 0 0 0 3 0
#define CallGate_create_string_bitmap 0x67 0x9a 217 0 0 0 3 0
#define CallGate_get_bitmap_info 0x67 0x9a 218 0 0 0 3 0
#define CallGate_draw_mask 0x67 0x9a 219 0 0 0 3 0

#define CallGate_create_sprite 0x67 0x9a 220 0 0 0 3 0
#define CallGate_close_sprite 0x67 0x9a 221 0 0 0 3 0
#define CallGate_show_sprite 0x67 0x9a 222 0 0 0 3 0
#define CallGate_hide_sprite 0x67 0x9a 223 0 0 0 3 0
#define CallGate_move_sprite 0x67 0x9a 224 0 0 0 3 0

#define CallGate_dup_bitmap_handle 0x67 0x9a 225 0 0 0 3 0
#define CallGate_set_clip_rect 0x67 0x9a 226 0 0 0 3 0
#define CallGate_clear_clip_rect 0x67 0x9a 227 0 0 0 3 0

#define CallGate_set_rts 0x67 0x9a 228 0 0 0 3 0
#define CallGate_reset_rts 0x67 0x9a 229 0 0 0 3 0
#define CallGate_get_com_receive_space 0x67 0x9a 230 0 0 0 3 0
#define CallGate_get_com_send_space 0x67 0x9a 231 0 0 0 3 0

#define CallGate_get_char_attrib 0x67 0x9a 232 0 0 0 3 0

#define CallGate_create_wait 0x67 0x9a 233 0 0 0 3 0
#define CallGate_close_wait 0x67 0x9a 234 0 0 0 3 0
#define CallGate_is_wait_idle 0x67 0x9a 235 0 0 0 3 0
#define CallGate_wait_no_timeout 0x67 0x9a 236 0 0 0 3 0
#define CallGate_wait_timeout 0x67 0x9a 237 0 0 0 3 0
#define CallGate_stop_wait 0x67 0x9a 238 0 0 0 3 0
#define CallGate_add_wait_for_keyboard 0x67 0x9a 239 0 0 0 3 0

#define CallGate_peek_key_event 0x67 0x9a 240 0 0 0 3 0
#define CallGate_read_key_event 0x67 0x9a 241 0 0 0 3 0

#define CallGate_add_wait_for_mouse 0x67 0x9a 242 0 0 0 3 0
#define CallGate_remove_wait 0x67 0x9a 243 0 0 0 3 0

#define CallGate_add_wait_for_adc 0x67 0x9a 244 0 0 0 3 0
#define CallGate_open_adc 0x67 0x9a 245 0 0 0 3 0
#define CallGate_close_adc 0x67 0x9a 246 0 0 0 3 0
#define CallGate_define_adc_time 0x67 0x9a 247 0 0 0 3 0
#define CallGate_read_adc 0x67 0x9a 248 0 0 0 3 0

#define CallGate_free_v86 0x67 0x9a 249 0 0 0 3 0

#define CallGate_read_serial_lines 0x67 0x9a 250 0 0 0 3 0
#define CallGate_toggle_serial_line 0x67 0x9a 251 0 0 0 3 0

#define CallGate_read_serial_val 0x67 0x9a 252 0 0 0 3 0
#define CallGate_write_serial_val 0x67 0x9a 253 0 0 0 3 0

#define CallGate_create_file_drive 0x67 0x9a 254 0 0 0 3 0
#define CallGate_open_file_drive 0x67 0x9a 255 0 0 0 3 0

#define CallGate_dos_ext_exec 0x67 0x9a 0 1 0 0 3 0

#define CallGate_open_sys_env 0x67 0x9a 1 1 0 0 3 0
#define CallGate_open_proc_env 0x67 0x9a 2 1 0 0 3 0
#define CallGate_close_env 0x67 0x9a 3 1 0 0 3 0
#define CallGate_add_env_var 0x67 0x9a 4 1 0 0 3 0
#define CallGate_delete_env_var 0x67 0x9a 5 1 0 0 3 0
#define CallGate_find_env_var 0x67 0x9a 6 1 0 0 3 0
#define CallGate_get_env_data 0x67 0x9a 7 1 0 0 3 0
#define CallGate_set_env_data 0x67 0x9a 8 1 0 0 3 0

#define CallGate_open_sys_ini 0x67 0x9a 9 1 0 0 3 0
#define CallGate_close_ini 0x67 0x9a 10 1 0 0 3 0
#define CallGate_goto_ini_section 0x67 0x9a 11 1 0 0 3 0
#define CallGate_remove_ini_section 0x67 0x9a 12 1 0 0 3 0
#define CallGate_read_ini 0x67 0x9a 13 1 0 0 3 0
#define CallGate_write_ini 0x67 0x9a 14 1 0 0 3 0
#define CallGate_delete_ini 0x67 0x9a 15 1 0 0 3 0

#define CallGate_get_version 0x67 0x9a 17 1 0 0 3 0

#define CallGate_enable_status_led 0x67 0x9a 18 1 0 0 3 0
#define CallGate_disable_status_led 0x67 0x9a 19 1 0 0 3 0

#define CallGate_start_watchdog 0x67 0x9a 20 1 0 0 3 0
#define CallGate_kick_watchdog 0x67 0x9a 21 1 0 0 3 0

#define CallGate_erase_disc_sectors 0x67 0x9a 22 1 0 0 3 0

#define CallGate_enable_cts 0x67 0x9a 24 1 0 0 3 0
#define CallGate_disable_cts 0x67 0x9a 25 1 0 0 3 0

#define CallGate_wait_for_send_completed_com 0x67 0x9a 26 1 0 0 3 0

#define CallGate_add_wait_for_tcp_connection 0x67 0x9a 27 1 0 0 3 0

#define CallGate_get_remote_tcp_connection_ip 0x67 0x9a 28 1 0 0 3 0
#define CallGate_get_remote_tcp_connection_port 0x67 0x9a 29 1 0 0 3 0
#define CallGate_get_local_tcp_connection_port 0x67 0x9a 30 1 0 0 3 0

#define CallGate_enable_auto_rts 0x67 0x9a 31 1 0 0 3 0
#define CallGate_disable_auto_rts 0x67 0x9a 32 1 0 0 3 0

#define CallGate_put_keyboard_code 0x67 0x9a 33 1 0 0 3 0

#define CallGate_poll_tcp_connection 0x67 0x9a 34 1 0 0 3 0

#define CallGate_create_signal 0x67 0x9a 35 1 0 0 3 0
#define CallGate_free_signal 0x67 0x9a 36 1 0 0 3 0
#define CallGate_add_wait_for_signal 0x67 0x9a 37 1 0 0 3 0
#define CallGate_set_signal 0x67 0x9a 38 1 0 0 3 0
#define CallGate_reset_signal 0x67 0x9a 39 1 0 0 3 0
#define CallGate_is_signalled 0x67 0x9a 40 1 0 0 3 0

#define CallGate_get_drive_disc_param 0x67 0x9a 41 1 0 0 3 0

#define CallGate_get_ide_disc 0x67 0x9a 42 1 0 0 3 0
#define CallGate_get_floppy_disc 0x67 0x9a 43 1 0 0 3 0
#define CallGate_demand_load_drive 0x67 0x9a 44 1 0 0 3 0
#define CallGate_set_disc_info 0x67 0x9a 45 1 0 0 3 0

#define CallGate_get_thread_state 0x67 0x9a 46 1 0 0 3 0
#define CallGate_suspend_thread 0x67 0x9a 47 1 0 0 3 0
#define CallGate_resume_thread 0x67 0x9a 48 1 0 0 3 0

#define CallGate_update_time 0x67 0x9a 49 1 0 0 3 0

#define CallGate_allocate_static_drive 0x67 0x9a 50 1 0 0 3 0
#define CallGate_allocate_fixed_drive 0x67 0x9a 51 1 0 0 3 0
#define CallGate_allocate_dynamic_drive 0x67 0x9a 52 1 0 0 3 0

#define CallGate_get_focus 0x67 0x9a 53 1 0 0 3 0

#define CallGate_add_wait_for_tcp_listen 0x67 0x9a 54 1 0 0 3 0
#define CallGate_create_tcp_listen 0x67 0x9a 55 1 0 0 3 0
#define CallGate_get_tcp_listen 0x67 0x9a 56 1 0 0 3 0
#define CallGate_close_tcp_listen 0x67 0x9a 57 1 0 0 3 0

#define CallGate_get_random 0x67 0x9a 58 1 0 0 3 0

#define CallGate_suspend_and_signal_thread 0x67 0x9a 59 1 0 0 3 0

#define CallGate_start_net_capture 0x67 0x9a 60 1 0 0 3 0
#define CallGate_stop_net_capture 0x67 0x9a 61 1 0 0 3 0

#define CallGate_create_crc 0x67 0x9a 62 1 0 0 3 0
#define CallGate_close_crc 0x67 0x9a 63 1 0 0 3 0
#define CallGate_calc_crc 0x67 0x9a 64 1 0 0 3 0

#define CallGate_get_usb_device 0x67 0x9a 65 1 0 0 3 0
#define CallGate_get_usb_config 0x67 0x9a 66 1 0 0 3 0

#define CallGate_open_usb_pipe 0x67 0x9a 67 1 0 0 3 0
#define CallGate_close_usb_pipe 0x67 0x9a 68 1 0 0 3 0
#define CallGate_add_wait_for_usb_pipe 0x67 0x9a 69 1 0 0 3 0
#define CallGate_req_usb_data 0x67 0x9a 70 1 0 0 3 0
#define CallGate_get_usb_data_size 0x67 0x9a 71 1 0 0 3 0
#define CallGate_write_usb_data 0x67 0x9a 72 1 0 0 3 0
#define CallGate_req_usb_status 0x67 0x9a 73 1 0 0 3 0
#define CallGate_write_usb_status 0x67 0x9a 74 1 0 0 3 0
#define CallGate_write_usb_control 0x67 0x9a 75 1 0 0 3 0
#define CallGate_lock_usb_pipe 0x67 0x9a 77 1 0 0 3 0
#define CallGate_unlock_usb_pipe 0x67 0x9a 78 1 0 0 3 0

#define CallGate_get_max_com_port 0x67 0x9a 79 1 0 0 3 0

#define CallGate_config_usb_device 0x67 0x9a 80 1 0 0 3 0

#define CallGate_start_usb_transaction 0x67 0x9a 81 1 0 0 3 0

#define CallGate_open_ini 0x67 0x9a 82 1 0 0 3 0

#define CallGate_open_icsp 0x67 0x9a 83 1 0 0 3 0
#define CallGate_close_icsp 0x67 0x9a 84 1 0 0 3 0
#define CallGate_write_icsp_cmd 0x67 0x9a 85 1 0 0 3 0
#define CallGate_write_icsp_data 0x67 0x9a 86 1 0 0 3 0
#define CallGate_read_icsp_data 0x67 0x9a 87 1 0 0 3 0

#define CallGate_get_gateway 0x67 0x9a 88 1 0 0 3 0

#define CallGate_get_free_physical 0x67 0x9a 89 1 0 0 3 0
#define CallGate_get_free_gdt 0x67 0x9a 90 1 0 0 3 0
#define CallGate_available_big_linear 0x67 0x9a 91 1 0 0 3 0
#define CallGate_available_small_linear 0x67 0x9a 92 1 0 0 3 0

#define CallGate_get_ip_mask 0x67 0x9a 93 1 0 0 3 0

#define CallGate_get_master_volume 0x67 0x9a 94 1 0 0 3 0
#define CallGate_set_master_volume 0x67 0x9a 95 1 0 0 3 0

#define CallGate_get_line_out_volume 0x67 0x9a 96 1 0 0 3 0
#define CallGate_set_line_out_volume 0x67 0x9a 97 1 0 0 3 0

#define CallGate_create_audio_out_channel 0x67 0x9a 98 1 0 0 3 0
#define CallGate_close_audio_out_channel 0x67 0x9a 99 1 0 0 3 0
#define CallGate_write_audio 0x67 0x9a 100 1 0 0 3 0

#define CallGate_stop_watchdog 0x67 0x9a 101 1 0 0 3 0

#define CallGate_is_valid_usergate 0x67 0x9a 102 1 0 0 3 0

#define CallGate_open_fm 0x67 0x9a 103 1 0 0 3 0
#define CallGate_close_fm 0x67 0x9a 104 1 0 0 3 0
#define CallGate_fm_wait 0x67 0x9a 105 1 0 0 3 0
#define CallGate_create_fm_instrument 0x67 0x9a 106 1 0 0 3 0
#define CallGate_free_fm_instrument 0x67 0x9a 107 1 0 0 3 0
#define CallGate_set_fm_attack 0x67 0x9a 108 1 0 0 3 0
#define CallGate_set_fm_sustain 0x67 0x9a 109 1 0 0 3 0
#define CallGate_set_fm_release 0x67 0x9a 110 1 0 0 3 0
#define CallGate_play_fm_note 0x67 0x9a 111 1 0 0 3 0

#define CallGate_get_thread_linear 0x67 0x9a 112 1 0 0 3 0
#define CallGate_get_current_module 0x67 0x9a 113 1 0 0 3 0

#define CallGate_get_module_focus_key 0x67 0x9a 114 1 0 0 3 0

#define CallGate_get_debug_event 0x67 0x9a 115 1 0 0 3 0
#define CallGate_clear_debug_event 0x67 0x9a 116 1 0 0 3 0

#define CallGate_get_free_handles 0x67 0x9a 117 1 0 0 3 0

#define CallGate_get_env_size 0x67 0x9a 118 1 0 0 3 0

#define CallGate_show_exception_text 0x67 0x9a 119 1 0 0 3 0

#define CallGate_get_watchdog_tics 0x67 0x9a 120 1 0 0 3 0

#define CallGate_is_usb_trans_done 0x67 0x9a 121 1 0 0 3 0
#define CallGate_was_usb_trans_ok 0x67 0x9a 122 1 0 0 3 0

#define CallGate_is_tcp_connection_idle 0x67 0x9a 123 1 0 0 3 0

#define CallGate_set_codec_gpio0 0x67 0x9a 124 1 0 0 3 0

#define CallGate_free_proc_handle 0x67 0x9a 125 1 0 0 3 0
#define CallGate_add_wait_for_proc_end 0x67 0x9a 126 1 0 0 3 0
#define CallGate_get_proc_exit_code 0x67 0x9a 127 1 0 0 3 0

#define CallGate_get_cpu_version 0x67 0x9a 128 1 0 0 3 0
#define CallGate_get_options 0x67 0x9a 129 1 0 0 3 0

#define CallGate_fork 0x67 0x9a 130 1 0 0 3 0

#define CallGate_get_core_id 0x67 0x9a 131 1 0 0 3 0

#define CallGate_create_user_read_write_section 0x67 0x9a 132 1 0 0 3 0
#define CallGate_delete_user_read_write_section 0x67 0x9a 133 1 0 0 3 0
#define CallGate_enter_user_read_section 0x67 0x9a 134 1 0 0 3 0
#define CallGate_leave_user_read_section 0x67 0x9a 135 1 0 0 3 0
#define CallGate_enter_user_write_section 0x67 0x9a 136 1 0 0 3 0
#define CallGate_leave_user_write_section 0x67 0x9a 137 1 0 0 3 0

#define CallGate_power_failure 0x67 0x9a 138 1 0 0 3 0

#define CallGate_define_fault_save 0x67 0x9a 139 1 0 0 3 0
#define CallGate_get_fault_thread_state 0x67 0x9a 140 1 0 0 3 0
#define CallGate_get_fault_thread_tss 0x67 0x9a 141 1 0 0 3 0
#define CallGate_clear_fault_save 0x67 0x9a 142 1 0 0 3 0

#define CallGate_get_free_handle_mem 0x67 0x9a 143 1 0 0 3 0

#define CallGate_get_image_header 0x67 0x9a 144 1 0 0 3 0
#define CallGate_get_image_data 0x67 0x9a 145 1 0 0 3 0

#define CallGate_allocate_debug_app_mem 0x67 0x9a 146 1 0 0 3 0
#define CallGate_free_debug_app_mem 0x67 0x9a 147 1 0 0 3 0

#define CallGate_get_key_layout 0x67 0x9a 148 1 0 0 3 0
#define CallGate_set_key_layout 0x67 0x9a 149 1 0 0 3 0

#define CallGate_get_device_info 0x67 0x9a 150 1 0 0 3 0
#define CallGate_get_selector_info 0x67 0x9a 151 1 0 0 3 0

#define CallGate_set_code_break 0x67 0x9a 152 1 0 0 3 0
#define CallGate_set_read_data_break 0x67 0x9a 153 1 0 0 3 0
#define CallGate_set_write_data_break 0x67 0x9a 154 1 0 0 3 0
#define CallGate_clear_break 0x67 0x9a 155 1 0 0 3 0

#define CallGate_get_max_printer 0x67 0x9a 156 1 0 0 3 0
#define CallGate_open_printer 0x67 0x9a 157 1 0 0 3 0
#define CallGate_close_printer 0x67 0x9a 158 1 0 0 3 0

#define CallGate_is_printer_jammed 0x67 0x9a 159 1 0 0 3 0
#define CallGate_is_printer_paper_low 0x67 0x9a 160 1 0 0 3 0
#define CallGate_is_printer_paper_end 0x67 0x9a 161 1 0 0 3 0
#define CallGate_is_printer_ok 0x67 0x9a 162 1 0 0 3 0
#define CallGate_is_printer_head_lifted 0x67 0x9a 163 1 0 0 3 0
#define CallGate_has_printer_paper_in_presenter 0x67 0x9a 164 1 0 0 3 0

#define CallGate_print_test 0x67 0x9a 165 1 0 0 3 0

#define CallGate_create_printer_bitmap 0x67 0x9a 166 1 0 0 3 0
#define CallGate_print_bitmap 0x67 0x9a 167 1 0 0 3 0

#define CallGate_present_printer_media 0x67 0x9a 168 1 0 0 3 0
#define CallGate_eject_printer_media 0x67 0x9a 169 1 0 0 3 0

#define CallGate_wait_for_print 0x67 0x9a 170 1 0 0 3 0

#define CallGate_has_audio 0x67 0x9a 171 1 0 0 3 0

#define CallGate_available_big_local_linear 0x67 0x9a 172 1 0 0 3 0

#define CallGate_get_allocated_usb_blocks 0x67 0x9a 173 1 0 0 3 0
#define CallGate_get_usb_close_count 0x67 0x9a 174 1 0 0 3 0

#define CallGate_reset_usb_pipe 0x67 0x9a 175 1 0 0 3 0

#define CallGate_add_syslog 0x67 0x9a 176 1 0 0 3 0
#define CallGate_open_syslog 0x67 0x9a 177 1 0 0 3 0
#define CallGate_close_syslog 0x67 0x9a 178 1 0 0 3 0
#define CallGate_add_wait_for_syslog 0x67 0x9a 179 1 0 0 3 0
#define CallGate_get_syslog 0x67 0x9a 180 1 0 0 3 0

#define CallGate_soft_reset 0x67 0x9a 181 1 0 0 3 0
#define CallGate_hard_reset 0x67 0x9a 182 1 0 0 3 0

#define CallGate_is_emergency_stopped 0x67 0x9a 183 1 0 0 3 0

#define CallGate_load_device32 0x67 0x9a 184 1 0 0 3 0

#define CallGate_debug_run 0x67 0x9a 185 1 0 0 3 0

#define CallGate_test_gate 0x67 0x9a 186 1 0 0 3 0

#define CallGate_get_acpi_status 0x67 0x9a 187 1 0 0 3 0
#define CallGate_get_acpi_object 0x67 0x9a 188 1 0 0 3 0
#define CallGate_get_cpu_temperature 0x67 0x9a 189 1 0 0 3 0
#define CallGate_get_acpi_method 0x67 0x9a 190 1 0 0 3 0

#define CallGate_write_attrib_string 0x67 0x9a 191 1 0 0 3 0

#define CallGate_has_hard_reset 0x67 0x9a 193 1 0 0 3 0

#define CallGate_get_acpi_device 0x67 0x9a 194 1 0 0 3 0
#define CallGate_get_acpi_device_irq 0x67 0x9a 195 1 0 0 3 0
#define CallGate_get_acpi_device_io 0x67 0x9a 196 1 0 0 3 0
#define CallGate_get_acpi_device_mem 0x67 0x9a 197 1 0 0 3 0

#define CallGate_get_pci_device_name 0x67 0x9a 198 1 0 0 3 0
#define CallGate_get_pci_device_info 0x67 0x9a 199 1 0 0 3 0
#define CallGate_get_pci_device_vendor 0x67 0x9a 200 1 0 0 3 0
#define CallGate_get_pci_device_class 0x67 0x9a 201 1 0 0 3 0
#define CallGate_get_pci_device_irq 0x67 0x9a 202 1 0 0 3 0

#define CallGate_open_hid 0x67 0x9a 203 1 0 0 3 0
#define CallGate_close_hid 0x67 0x9a 204 1 0 0 3 0
#define CallGate_get_hid_pipe 0x67 0x9a 205 1 0 0 3 0

#define CallGate_get_core_load 0x67 0x9a 206 1 0 0 3 0
#define CallGate_get_core_duty 0x67 0x9a 207 1 0 0 3 0

#define CallGate_has_global_timer 0x67 0x9a 208 1 0 0 3 0

#define CallGate_remote_debug 0x67 0x9a 209 1 0 0 3 0

#define CallGate_read_hid 0x67 0x9a 210 1 0 0 3 0
#define CallGate_write_hid 0x67 0x9a 211 1 0 0 3 0

#define CallGate_ansi_to_utf8 0x67 0x9a 212 1 0 0 3 0
#define CallGate_utf8_to_ansi 0x67 0x9a 213 1 0 0 3 0

#define CallGate_get_printer_name 0x67 0x9a 214 1 0 0 3 0

#define CallGate_acquire_futex 0x67 0x9a 215 1 0 0 3 0
#define CallGate_release_futex 0x67 0x9a 216 1 0 0 3 0
#define CallGate_cleanup_futex 0x67 0x9a 217 1 0 0 3 0

#define CallGate_goto_first_inivar 0x67 0x9a 218 1 0 0 3 0
#define CallGate_goto_next_inivar 0x67 0x9a 219 1 0 0 3 0
#define CallGate_get_curr_inivar 0x67 0x9a 220 1 0 0 3 0

#define CallGate_reset_printer 0x67 0x9a 221 1 0 0 3 0

#define CallGate_has_icsp 0x67 0x9a 222 1 0 0 3 0
#define CallGate_reset_icsp 0x67 0x9a 223 1 0 0 3 0

#define CallGate_get_max_carddev 0x67 0x9a 224 1 0 0 3 0
#define CallGate_open_carddev 0x67 0x9a 225 1 0 0 3 0
#define CallGate_close_carddev 0x67 0x9a 226 1 0 0 3 0
#define CallGate_is_carddev_ok 0x67 0x9a 227 1 0 0 3 0
#define CallGate_wait_for_card 0x67 0x9a 228 1 0 0 3 0
#define CallGate_get_carddev_name 0x67 0x9a 229 1 0 0 3 0

#define CallGate_is_carddev_busy 0x67 0x9a 230 1 0 0 3 0
#define CallGate_is_carddev_inserted 0x67 0x9a 231 1 0 0 3 0

#define CallGate_has_touch 0x67 0x9a 232 1 0 0 3 0

#define CallGate_had_carddev_inserted 0x67 0x9a 233 1 0 0 3 0
#define CallGate_clear_carddev_inserted 0x67 0x9a 234 1 0 0 3 0

#define CallGate_is_printer_cutter_jammed 0x67 0x9a 235 1 0 0 3 0
#define CallGate_has_printer_temp_error 0x67 0x9a 236 1 0 0 3 0
#define CallGate_has_printer_feed_error 0x67 0x9a 237 1 0 0 3 0

#define CallGate_is_64_bit_exe 0x67 0x9a 238 1 0 0 3 0

#define CallGate_get_audio_device_count 0x67 0x9a 239 1 0 0 3 0
#define CallGate_get_audio_codec_count 0x67 0x9a 240 1 0 0 3 0
#define CallGate_get_audio_widget_info 0x67 0x9a 241 1 0 0 3 0
#define CallGate_get_audio_widget_connection_list 0x67 0x9a 242 1 0 0 3 0
#define CallGate_get_selected_audio_connection 0x67 0x9a 243 1 0 0 3 0

#define CallGate_get_audio_input_amp_cap 0x67 0x9a 244 1 0 0 3 0
#define CallGate_get_audio_output_amp_cap 0x67 0x9a 245 1 0 0 3 0

#define CallGate_has_audio_input_mute 0x67 0x9a 246 1 0 0 3 0
#define CallGate_has_audio_output_mute 0x67 0x9a 247 1 0 0 3 0

#define CallGate_read_audio_input_amp 0x67 0x9a 248 1 0 0 3 0
#define CallGate_read_audio_output_amp 0x67 0x9a 249 1 0 0 3 0

#define CallGate_is_audio_input_amp_muted 0x67 0x9a 250 1 0 0 3 0
#define CallGate_is_audio_output_amp_muted 0x67 0x9a 251 1 0 0 3 0

#define CallGate_get_fixed_audio_output 0x67 0x9a 252 1 0 0 3 0
#define CallGate_get_jack_audio_output 0x67 0x9a 253 1 0 0 3 0
#define CallGate_get_jack_audio_input 0x67 0x9a 254 1 0 0 3 0

#define CallGate_get_output_volume 0x67 0x9a 255 1 0 0 3 0
#define CallGate_set_output_volume 0x67 0x9a 0 2 0 0 3 0

#define CallGate_broadcast_query_udp 0x67 0x9a 1 2 0 0 3 0
#define CallGate_send_udp 0x67 0x9a 2 2 0 0 3 0

#define CallGate_open_udp_connection 0x67 0x9a 3 2 0 0 3 0
#define CallGate_close_udp_connection 0x67 0x9a 4 2 0 0 3 0
#define CallGate_send_udp_connection 0x67 0x9a 5 2 0 0 3 0
#define CallGate_add_wait_for_udp_connection 0x67 0x9a 6 2 0 0 3 0
#define CallGate_peek_udp_connection 0x67 0x9a 7 2 0 0 3 0
#define CallGate_read_udp_connection 0x67 0x9a 8 2 0 0 3 0

#define CallGate_extract_alpha_bitmap 0x67 0x9a 9 2 0 0 3 0
#define CallGate_extract_valid_bitmap_mask 0x67 0x9a 10 2 0 0 3 0
#define CallGate_extract_invalid_bitmap_mask 0x67 0x9a 11 2 0 0 3 0

#define CallGate_create_alpha_bitmap 0x67 0x9a 12 2 0 0 3 0

#define CallGate_create_long_thread 0x67 0x9a 13 2 0 0 3 0

#else

#define CallGate_allocate_dos_mem 0x3e 0x67 0x9a 0 0 0 0 3 0
#define CallGate_allocate_local_mem 0x3e 0x67 0x9a 1 0 0 0 3 0
#define CallGate_free_mem 0x3e 0x67 0x9a 2 0 0 0 3 0
#define CallGate_available_small_local_linear 0x3e 0x67 0x9a 3 0 0 0 3 0
#define CallGate_used_local_linear 0x3e 0x67 0x9a 4 0 0 0 3 0
#define CallGate_available_vm_linear 0x3e 0x67 0x9a 5 0 0 0 3 0
#define CallGate_used_vm_linear 0x3e 0x67 0x9a 6 0 0 0 3 0

#define CallGate_reserve_pe_mem 0x3e 0x67 0x9a 7 0 0 0 3 0
#define CallGate_set_flat_linear_valid 0x3e 0x67 0x9a 8 0 0 0 3 0
#define CallGate_set_flat_linear_invalid 0x3e 0x67 0x9a 9 0 0 0 3 0
#define CallGate_set_flat_linear_read 0x3e 0x67 0x9a 10 0 0 0 3 0
#define CallGate_set_flat_linear_readwrite 0x3e 0x67 0x9a 11 0 0 0 3 0

#define CallGate_get_raw_switch_ads 0x3e 0x67 0x9a 12 0 0 0 3 0
#define CallGate_raw_switch_pm 0x3e 0x67 0x9a 13 0 0 0 3 0
#define CallGate_get_exception 0x3e 0x67 0x9a 14 0 0 0 3 0
#define CallGate_set_exception 0x3e 0x67 0x9a 15 0 0 0 3 0
#define CallGate_get_pm_int 0x3e 0x67 0x9a 16 0 0 0 3 0
#define CallGate_set_pm_int 0x3e 0x67 0x9a 17 0 0 0 3 0
#define CallGate_get_vm_int 0x3e 0x67 0x9a 18 0 0 0 3 0
#define CallGate_set_vm_int 0x3e 0x67 0x9a 19 0 0 0 3 0
#define CallGate_dpmi_int 0x3e 0x67 0x9a 20 0 0 0 3 0
#define CallGate_dpmi_call_int 0x3e 0x67 0x9a 21 0 0 0 3 0
#define CallGate_dpmi_call 0x3e 0x67 0x9a 22 0 0 0 3 0
#define CallGate_allocate_vm_callback 0x3e 0x67 0x9a 23 0 0 0 3 0
#define CallGate_free_vm_callback 0x3e 0x67 0x9a 24 0 0 0 3 0

#define CallGate_wait_milli 0x3e 0x67 0x9a 25 0 0 0 3 0
#define CallGate_wait_micro 0x3e 0x67 0x9a 26 0 0 0 3 0
#define CallGate_wait_until 0x3e 0x67 0x9a 27 0 0 0 3 0
#define CallGate_create_thread 0x3e 0x67 0x9a 28 0 0 0 3 0
#define CallGate_terminate_thread 0x3e 0x67 0x9a 29 0 0 0 3 0
#define CallGate_get_thread 0x3e 0x67 0x9a 30 0 0 0 3 0
#define CallGate_raw_switch_vm 0x3e 0x67 0x9a 31 0 0 0 3 0

#define CallGate_create_user_section 0x3e 0x67 0x9a 32 0 0 0 3 0
#define CallGate_create_blocked_user_section 0x3e 0x67 0x9a 33 0 0 0 3 0
#define CallGate_delete_user_section 0x3e 0x67 0x9a 34 0 0 0 3 0
#define CallGate_enter_user_section 0x3e 0x67 0x9a 35 0 0 0 3 0
#define CallGate_leave_user_section 0x3e 0x67 0x9a 36 0 0 0 3 0

#define CallGate_get_local_mailslot 0x3e 0x67 0x9a 37 0 0 0 3 0
#define CallGate_get_remote_mailslot 0x3e 0x67 0x9a 38 0 0 0 3 0
#define CallGate_free_mailslot 0x3e 0x67 0x9a 39 0 0 0 3 0
#define CallGate_send_mailslot 0x3e 0x67 0x9a 40 0 0 0 3 0
#define CallGate_define_mailslot 0x3e 0x67 0x9a 41 0 0 0 3 0
#define CallGate_receive_mailslot 0x3e 0x67 0x9a 42 0 0 0 3 0
#define CallGate_reply_mailslot 0x3e 0x67 0x9a 43 0 0 0 3 0

#define CallGate_load_exe 0x3e 0x67 0x9a 44 0 0 0 3 0
#define CallGate_spawn_exe 0x3e 0x67 0x9a 45 0 0 0 3 0
#define CallGate_unload_exe 0x3e 0x67 0x9a 46 0 0 0 3 0
#define CallGate_get_exit_code 0x3e 0x67 0x9a 47 0 0 0 3 0
#define CallGate_get_exe_name 0x3e 0x67 0x9a 48 0 0 0 3 0
#define CallGate_get_cmd_line 0x3e 0x67 0x9a 49 0 0 0 3 0
#define CallGate_get_env 0x3e 0x67 0x9a 50 0 0 0 3 0
#define CallGate_load_dll 0x3e 0x67 0x9a 51 0 0 0 3 0
#define CallGate_free_dll 0x3e 0x67 0x9a 52 0 0 0 3 0
#define CallGate_get_module_proc 0x3e 0x67 0x9a 53 0 0 0 3 0
#define CallGate_get_module_resource 0x3e 0x67 0x9a 54 0 0 0 3 0
#define CallGate_get_module_name 0x3e 0x67 0x9a 55 0 0 0 3 0
#define CallGate_get_module 0x3e 0x67 0x9a 56 0 0 0 3 0
#define CallGate_allocate_app_mem 0x3e 0x67 0x9a 57 0 0 0 3 0
#define CallGate_free_app_mem 0x3e 0x67 0x9a 58 0 0 0 3 0
#define CallGate_get_psp_sel 0x3e 0x67 0x9a 59 0 0 0 3 0

#define CallGate_get_debug_thread 0x3e 0x67 0x9a 61 0 0 0 3 0
#define CallGate_debug_trace 0x3e 0x67 0x9a 62 0 0 0 3 0
#define CallGate_debug_pace 0x3e 0x67 0x9a 63 0 0 0 3 0
#define CallGate_debug_go 0x3e 0x67 0x9a 64 0 0 0 3 0
#define CallGate_debug_next 0x3e 0x67 0x9a 65 0 0 0 3 0

#define CallGate_add_wait_for_debug_event 0x3e 0x67 0x9a 66 0 0 0 3 0
#define CallGate_get_debug_event_data 0x3e 0x67 0x9a 67 0 0 0 3 0
#define CallGate_continue_debug_event 0x3e 0x67 0x9a 68 0 0 0 3 0
#define CallGate_notify_pe_exception 0x3e 0x67 0x9a 70 0 0 0 3 0
#define CallGate_read_thread_mem 0x3e 0x67 0x9a 71 0 0 0 3 0
#define CallGate_write_thread_mem 0x3e 0x67 0x9a 72 0 0 0 3 0
#define CallGate_get_thread_tss 0x3e 0x67 0x9a 73 0 0 0 3 0
#define CallGate_set_thread_tss 0x3e 0x67 0x9a 74 0 0 0 3 0

#define CallGate_get_cpu_time 0x3e 0x67 0x9a 75 0 0 0 3 0
#define CallGate_get_system_time 0x3e 0x67 0x9a 76 0 0 0 3 0
#define CallGate_get_time 0x3e 0x67 0x9a 77 0 0 0 3 0
#define CallGate_time_to_system_time 0x3e 0x67 0x9a 78 0 0 0 3 0
#define CallGate_system_time_to_time 0x3e 0x67 0x9a 79 0 0 0 3 0
#define CallGate_days_in_month 0x3e 0x67 0x9a 80 0 0 0 3 0
#define CallGate_adjust_time 0x3e 0x67 0x9a 81 0 0 0 3 0
#define CallGate_passed_days 0x3e 0x67 0x9a 82 0 0 0 3 0
#define CallGate_time_to_binary 0x3e 0x67 0x9a 83 0 0 0 3 0
#define CallGate_binary_to_time 0x3e 0x67 0x9a 84 0 0 0 3 0
#define CallGate_sync_time 0x3e 0x67 0x9a 85 0 0 0 3 0

#define CallGate_set_focus 0x3e 0x67 0x9a 86 0 0 0 3 0
#define CallGate_enable_focus 0x3e 0x67 0x9a 87 0 0 0 3 0

#define CallGate_get_dns 0x3e 0x67 0x9a 88 0 0 0 3 0
#define CallGate_get_ppp_dns 0x3e 0x67 0x9a 89 0 0 0 3 0
#define CallGate_open_tcp_connection 0x3e 0x67 0x9a 90 0 0 0 3 0
#define CallGate_wait_for_tcp_connection 0x3e 0x67 0x9a 92 0 0 0 3 0
#define CallGate_close_tcp_connection 0x3e 0x67 0x9a 93 0 0 0 3 0
#define CallGate_delete_tcp_connection 0x3e 0x67 0x9a 94 0 0 0 3 0
#define CallGate_is_tcp_connection_closed 0x3e 0x67 0x9a 95 0 0 0 3 0
#define CallGate_abort_tcp_connection 0x3e 0x67 0x9a 96 0 0 0 3 0
#define CallGate_read_tcp_connection 0x3e 0x67 0x9a 97 0 0 0 3 0
#define CallGate_write_tcp_connection 0x3e 0x67 0x9a 98 0 0 0 3 0
#define CallGate_push_tcp_connection 0x3e 0x67 0x9a 99 0 0 0 3 0
#define CallGate_get_ip_address 0x3e 0x67 0x9a 100 0 0 0 3 0
#define CallGate_name_to_ip 0x3e 0x67 0x9a 101 0 0 0 3 0
#define CallGate_ip_to_name 0x3e 0x67 0x9a 102 0 0 0 3 0
#define CallGate_ping 0x3e 0x67 0x9a 103 0 0 0 3 0

#define CallGate_get_disc_info 0x3e 0x67 0x9a 104 0 0 0 3 0
#define CallGate_format_drive 0x3e 0x67 0x9a 105 0 0 0 3 0
#define CallGate_get_rdfs_info 0x3e 0x67 0x9a 106 0 0 0 3 0
#define CallGate_read_disc 0x3e 0x67 0x9a 107 0 0 0 3 0
#define CallGate_write_disc 0x3e 0x67 0x9a 108 0 0 0 3 0
#define CallGate_get_drive_info 0x3e 0x67 0x9a 109 0 0 0 3 0

#define CallGate_set_cur_drive 0x3e 0x67 0x9a 110 0 0 0 3 0
#define CallGate_get_cur_drive 0x3e 0x67 0x9a 111 0 0 0 3 0
#define CallGate_set_cur_dir 0x3e 0x67 0x9a 112 0 0 0 3 0
#define CallGate_get_cur_dir 0x3e 0x67 0x9a 113 0 0 0 3 0
#define CallGate_make_dir 0x3e 0x67 0x9a 114 0 0 0 3 0
#define CallGate_remove_dir 0x3e 0x67 0x9a 115 0 0 0 3 0
#define CallGate_rename_file 0x3e 0x67 0x9a 116 0 0 0 3 0
#define CallGate_delete_file 0x3e 0x67 0x9a 117 0 0 0 3 0
#define CallGate_get_file_attribute 0x3e 0x67 0x9a 118 0 0 0 3 0
#define CallGate_set_file_attribute 0x3e 0x67 0x9a 119 0 0 0 3 0
#define CallGate_open_dir 0x3e 0x67 0x9a 120 0 0 0 3 0
#define CallGate_close_dir 0x3e 0x67 0x9a 121 0 0 0 3 0
#define CallGate_read_dir 0x3e 0x67 0x9a 122 0 0 0 3 0

#define CallGate_open_file 0x3e 0x67 0x9a 123 0 0 0 3 0
#define CallGate_create_file 0x3e 0x67 0x9a 124 0 0 0 3 0
#define CallGate_close_file 0x3e 0x67 0x9a 125 0 0 0 3 0
#define CallGate_dupl_file 0x3e 0x67 0x9a 126 0 0 0 3 0
#define CallGate_get_ioctl_data 0x3e 0x67 0x9a 127 0 0 0 3 0
#define CallGate_get_file_size 0x3e 0x67 0x9a 128 0 0 0 3 0
#define CallGate_set_file_size 0x3e 0x67 0x9a 129 0 0 0 3 0
#define CallGate_get_file_pos 0x3e 0x67 0x9a 130 0 0 0 3 0
#define CallGate_set_file_pos 0x3e 0x67 0x9a 131 0 0 0 3 0
#define CallGate_get_file_time 0x3e 0x67 0x9a 132 0 0 0 3 0
#define CallGate_set_file_time 0x3e 0x67 0x9a 133 0 0 0 3 0
#define CallGate_read_file 0x3e 0x67 0x9a 134 0 0 0 3 0
#define CallGate_write_file 0x3e 0x67 0x9a 135 0 0 0 3 0
#define CallGate_read_con 0x3e 0x67 0x9a 136 0 0 0 3 0

#define CallGate_create_mapping 0x3e 0x67 0x9a 137 0 0 0 3 0
#define CallGate_create_named_mapping 0x3e 0x67 0x9a 138 0 0 0 3 0
#define CallGate_create_file_mapping 0x3e 0x67 0x9a 139 0 0 0 3 0
#define CallGate_create_named_file_mapping 0x3e 0x67 0x9a 140 0 0 0 3 0
#define CallGate_open_named_mapping 0x3e 0x67 0x9a 141 0 0 0 3 0
#define CallGate_sync_mapping 0x3e 0x67 0x9a 142 0 0 0 3 0
#define CallGate_close_mapping 0x3e 0x67 0x9a 143 0 0 0 3 0
#define CallGate_map_view 0x3e 0x67 0x9a 144 0 0 0 3 0
#define CallGate_unmap_view 0x3e 0x67 0x9a 145 0 0 0 3 0

#define CallGate_read_keyboard 0x3e 0x67 0x9a 146 0 0 0 3 0
#define CallGate_poll_keyboard 0x3e 0x67 0x9a 147 0 0 0 3 0
#define CallGate_flush_keyboard 0x3e 0x67 0x9a 148 0 0 0 3 0
#define CallGate_get_keyboard_state 0x3e 0x67 0x9a 151 0 0 0 3 0

#define CallGate_show_mouse 0x3e 0x67 0x9a 152 0 0 0 3 0
#define CallGate_hide_mouse 0x3e 0x67 0x9a 153 0 0 0 3 0
#define CallGate_get_mouse_position 0x3e 0x67 0x9a 154 0 0 0 3 0
#define CallGate_set_mouse_position 0x3e 0x67 0x9a 155 0 0 0 3 0
#define CallGate_set_mouse_window 0x3e 0x67 0x9a 156 0 0 0 3 0
#define CallGate_set_mouse_mickey 0x3e 0x67 0x9a 157 0 0 0 3 0
#define CallGate_get_left_button 0x3e 0x67 0x9a 158 0 0 0 3 0
#define CallGate_get_right_button 0x3e 0x67 0x9a 159 0 0 0 3 0
#define CallGate_get_left_button_press_position 0x3e 0x67 0x9a 160 0 0 0 3 0
#define CallGate_get_right_button_press_position 0x3e 0x67 0x9a 161 0 0 0 3 0
#define CallGate_get_left_button_release_position 0x3e 0x67 0x9a 162 0 0 0 3 0
#define CallGate_get_right_button_release_position 0x3e 0x67 0x9a 163 0 0 0 3 0
#define CallGate_hook_mouse 0x3e 0x67 0x9a 164 0 0 0 3 0
#define CallGate_unhook_mouse 0x3e 0x67 0x9a 165 0 0 0 3 0

#define CallGate_lock_cd 0x3e 0x67 0x9a 166 0 0 0 3 0
#define CallGate_unlock_cd 0x3e 0x67 0x9a 167 0 0 0 3 0
#define CallGate_eject_cd 0x3e 0x67 0x9a 168 0 0 0 3 0
#define CallGate_insert_cd 0x3e 0x67 0x9a 169 0 0 0 3 0

#define CallGate_open_com 0x3e 0x67 0x9a 170 0 0 0 3 0
#define CallGate_close_com 0x3e 0x67 0x9a 171 0 0 0 3 0
#define CallGate_flush_com 0x3e 0x67 0x9a 172 0 0 0 3 0
#define CallGate_add_wait_for_com 0x3e 0x67 0x9a 174 0 0 0 3 0
#define CallGate_read_com 0x3e 0x67 0x9a 175 0 0 0 3 0
#define CallGate_write_com 0x3e 0x67 0x9a 176 0 0 0 3 0
#define CallGate_set_dtr 0x3e 0x67 0x9a 177 0 0 0 3 0
#define CallGate_reset_dtr 0x3e 0x67 0x9a 178 0 0 0 3 0
#define CallGate_reset_com 0x3e 0x67 0x9a 179 0 0 0 3 0

#define CallGate_set_cursor_position 0x3e 0x67 0x9a 182 0 0 0 3 0
#define CallGate_get_cursor_position 0x3e 0x67 0x9a 183 0 0 0 3 0
#define CallGate_write_char 0x3e 0x67 0x9a 184 0 0 0 3 0
#define CallGate_write_asciiz 0x3e 0x67 0x9a 185 0 0 0 3 0
#define CallGate_write_size_string 0x3e 0x67 0x9a 186 0 0 0 3 0

#define CallGate_set_video_mode 0x3e 0x67 0x9a 187 0 0 0 3 0
#define CallGate_set_vga_mode 0x3e 0x67 0x9a 188 0 0 0 3 0
#define CallGate_set_forecolor 0x3e 0x67 0x9a 189 0 0 0 3 0
#define CallGate_set_backcolor 0x3e 0x67 0x9a 190 0 0 0 3 0
#define CallGate_get_string_metrics 0x3e 0x67 0x9a 193 0 0 0 3 0
#define CallGate_set_font 0x3e 0x67 0x9a 194 0 0 0 3 0
#define CallGate_get_video_mode 0x3e 0x67 0x9a 195 0 0 0 3 0
#define CallGate_draw_string 0x3e 0x67 0x9a 197 0 0 0 3 0

#define CallGate_xms_handler 0x3e 0x67 0x9a 198 0 0 0 3 0
#define CallGate_ems_handler 0x3e 0x67 0x9a 199 0 0 0 3 0

#define CallGate_resize_flat_linear 0x3e 0x67 0x9a 201 0 0 0 3 0

#define CallGate_set_drawcolor 0x3e 0x67 0x9a 202 0 0 0 3 0
#define CallGate_set_lgop 0x3e 0x67 0x9a 203 0 0 0 3 0
#define CallGate_set_hollow_style 0x3e 0x67 0x9a 204 0 0 0 3 0
#define CallGate_set_filled_style 0x3e 0x67 0x9a 205 0 0 0 3 0
#define CallGate_get_pixel 0x3e 0x67 0x9a 206 0 0 0 3 0
#define CallGate_set_pixel 0x3e 0x67 0x9a 207 0 0 0 3 0
#define CallGate_draw_line 0x3e 0x67 0x9a 208 0 0 0 3 0
#define CallGate_draw_rect 0x3e 0x67 0x9a 209 0 0 0 3 0
#define CallGate_draw_ellipse 0x3e 0x67 0x9a 210 0 0 0 3 0
#define CallGate_create_bitmap 0x3e 0x67 0x9a 211 0 0 0 3 0
#define CallGate_close_bitmap 0x3e 0x67 0x9a 212 0 0 0 3 0
#define CallGate_blit 0x3e 0x67 0x9a 213 0 0 0 3 0
#define CallGate_open_font 0x3e 0x67 0x9a 215 0 0 0 3 0
#define CallGate_close_font 0x3e 0x67 0x9a 216 0 0 0 3 0
#define CallGate_create_string_bitmap 0x3e 0x67 0x9a 217 0 0 0 3 0
#define CallGate_get_bitmap_info 0x3e 0x67 0x9a 218 0 0 0 3 0
#define CallGate_draw_mask 0x3e 0x67 0x9a 219 0 0 0 3 0

#define CallGate_create_sprite 0x3e 0x67 0x9a 220 0 0 0 3 0
#define CallGate_close_sprite 0x3e 0x67 0x9a 221 0 0 0 3 0
#define CallGate_show_sprite 0x3e 0x67 0x9a 222 0 0 0 3 0
#define CallGate_hide_sprite 0x3e 0x67 0x9a 223 0 0 0 3 0
#define CallGate_move_sprite 0x3e 0x67 0x9a 224 0 0 0 3 0

#define CallGate_dup_bitmap_handle 0x3e 0x67 0x9a 225 0 0 0 3 0
#define CallGate_set_clip_rect 0x3e 0x67 0x9a 226 0 0 0 3 0
#define CallGate_clear_clip_rect 0x3e 0x67 0x9a 227 0 0 0 3 0

#define CallGate_set_rts 0x3e 0x67 0x9a 228 0 0 0 3 0
#define CallGate_reset_rts 0x3e 0x67 0x9a 229 0 0 0 3 0
#define CallGate_get_com_receive_space 0x3e 0x67 0x9a 230 0 0 0 3 0
#define CallGate_get_com_send_space 0x3e 0x67 0x9a 231 0 0 0 3 0

#define CallGate_get_char_attrib 0x3e 0x67 0x9a 232 0 0 0 3 0

#define CallGate_create_wait 0x3e 0x67 0x9a 233 0 0 0 3 0
#define CallGate_close_wait 0x3e 0x67 0x9a 234 0 0 0 3 0
#define CallGate_is_wait_idle 0x3e 0x67 0x9a 235 0 0 0 3 0
#define CallGate_wait_no_timeout 0x3e 0x67 0x9a 236 0 0 0 3 0
#define CallGate_wait_timeout 0x3e 0x67 0x9a 237 0 0 0 3 0
#define CallGate_stop_wait 0x3e 0x67 0x9a 238 0 0 0 3 0
#define CallGate_add_wait_for_keyboard 0x3e 0x67 0x9a 239 0 0 0 3 0

#define CallGate_peek_key_event 0x3e 0x67 0x9a 240 0 0 0 3 0
#define CallGate_read_key_event 0x3e 0x67 0x9a 241 0 0 0 3 0

#define CallGate_add_wait_for_mouse 0x3e 0x67 0x9a 242 0 0 0 3 0
#define CallGate_remove_wait 0x3e 0x67 0x9a 243 0 0 0 3 0

#define CallGate_add_wait_for_adc 0x3e 0x67 0x9a 244 0 0 0 3 0
#define CallGate_open_adc 0x3e 0x67 0x9a 245 0 0 0 3 0
#define CallGate_close_adc 0x3e 0x67 0x9a 246 0 0 0 3 0
#define CallGate_define_adc_time 0x3e 0x67 0x9a 247 0 0 0 3 0
#define CallGate_read_adc 0x3e 0x67 0x9a 248 0 0 0 3 0

#define CallGate_free_v86 0x3e 0x67 0x9a 249 0 0 0 3 0

#define CallGate_read_serial_lines 0x3e 0x67 0x9a 250 0 0 0 3 0
#define CallGate_toggle_serial_line 0x3e 0x67 0x9a 251 0 0 0 3 0

#define CallGate_read_serial_val 0x3e 0x67 0x9a 252 0 0 0 3 0
#define CallGate_write_serial_val 0x3e 0x67 0x9a 253 0 0 0 3 0

#define CallGate_create_file_drive 0x3e 0x67 0x9a 254 0 0 0 3 0
#define CallGate_open_file_drive 0x3e 0x67 0x9a 255 0 0 0 3 0

#define CallGate_dos_ext_exec 0x3e 0x67 0x9a 0 1 0 0 3 0

#define CallGate_open_sys_env 0x3e 0x67 0x9a 1 1 0 0 3 0
#define CallGate_open_proc_env 0x3e 0x67 0x9a 2 1 0 0 3 0
#define CallGate_close_env 0x3e 0x67 0x9a 3 1 0 0 3 0
#define CallGate_add_env_var 0x3e 0x67 0x9a 4 1 0 0 3 0
#define CallGate_delete_env_var 0x3e 0x67 0x9a 5 1 0 0 3 0
#define CallGate_find_env_var 0x3e 0x67 0x9a 6 1 0 0 3 0
#define CallGate_get_env_data 0x3e 0x67 0x9a 7 1 0 0 3 0
#define CallGate_set_env_data 0x3e 0x67 0x9a 8 1 0 0 3 0

#define CallGate_open_sys_ini 0x3e 0x67 0x9a 9 1 0 0 3 0
#define CallGate_close_ini 0x3e 0x67 0x9a 10 1 0 0 3 0
#define CallGate_goto_ini_section 0x3e 0x67 0x9a 11 1 0 0 3 0
#define CallGate_remove_ini_section 0x3e 0x67 0x9a 12 1 0 0 3 0
#define CallGate_read_ini 0x3e 0x67 0x9a 13 1 0 0 3 0
#define CallGate_write_ini 0x3e 0x67 0x9a 14 1 0 0 3 0
#define CallGate_delete_ini 0x3e 0x67 0x9a 15 1 0 0 3 0

#define CallGate_get_version 0x3e 0x67 0x9a 17 1 0 0 3 0

#define CallGate_enable_status_led 0x3e 0x67 0x9a 18 1 0 0 3 0
#define CallGate_disable_status_led 0x3e 0x67 0x9a 19 1 0 0 3 0

#define CallGate_start_watchdog 0x3e 0x67 0x9a 20 1 0 0 3 0
#define CallGate_kick_watchdog 0x3e 0x67 0x9a 21 1 0 0 3 0

#define CallGate_erase_disc_sectors 0x3e 0x67 0x9a 22 1 0 0 3 0

#define CallGate_enable_cts 0x3e 0x67 0x9a 24 1 0 0 3 0
#define CallGate_disable_cts 0x3e 0x67 0x9a 25 1 0 0 3 0

#define CallGate_wait_for_send_completed_com 0x3e 0x67 0x9a 26 1 0 0 3 0

#define CallGate_add_wait_for_tcp_connection 0x3e 0x67 0x9a 27 1 0 0 3 0

#define CallGate_get_remote_tcp_connection_ip 0x3e 0x67 0x9a 28 1 0 0 3 0
#define CallGate_get_remote_tcp_connection_port 0x3e 0x67 0x9a 29 1 0 0 3 0
#define CallGate_get_local_tcp_connection_port 0x3e 0x67 0x9a 30 1 0 0 3 0

#define CallGate_enable_auto_rts 0x3e 0x67 0x9a 31 1 0 0 3 0
#define CallGate_disable_auto_rts 0x3e 0x67 0x9a 32 1 0 0 3 0

#define CallGate_put_keyboard_code 0x3e 0x67 0x9a 33 1 0 0 3 0

#define CallGate_poll_tcp_connection 0x3e 0x67 0x9a 34 1 0 0 3 0

#define CallGate_create_signal 0x3e 0x67 0x9a 35 1 0 0 3 0
#define CallGate_free_signal 0x3e 0x67 0x9a 36 1 0 0 3 0
#define CallGate_add_wait_for_signal 0x3e 0x67 0x9a 37 1 0 0 3 0
#define CallGate_set_signal 0x3e 0x67 0x9a 38 1 0 0 3 0
#define CallGate_reset_signal 0x3e 0x67 0x9a 39 1 0 0 3 0
#define CallGate_is_signalled 0x3e 0x67 0x9a 40 1 0 0 3 0

#define CallGate_get_drive_disc_param 0x3e 0x67 0x9a 41 1 0 0 3 0

#define CallGate_get_ide_disc 0x3e 0x67 0x9a 42 1 0 0 3 0
#define CallGate_get_floppy_disc 0x3e 0x67 0x9a 43 1 0 0 3 0
#define CallGate_demand_load_drive 0x3e 0x67 0x9a 44 1 0 0 3 0
#define CallGate_set_disc_info 0x3e 0x67 0x9a 45 1 0 0 3 0

#define CallGate_get_thread_state 0x3e 0x67 0x9a 46 1 0 0 3 0
#define CallGate_suspend_thread 0x3e 0x67 0x9a 47 1 0 0 3 0
#define CallGate_resume_thread 0x3e 0x67 0x9a 48 1 0 0 3 0

#define CallGate_update_time 0x3e 0x67 0x9a 49 1 0 0 3 0

#define CallGate_allocate_static_drive 0x3e 0x67 0x9a 50 1 0 0 3 0
#define CallGate_allocate_fixed_drive 0x3e 0x67 0x9a 51 1 0 0 3 0
#define CallGate_allocate_dynamic_drive 0x3e 0x67 0x9a 52 1 0 0 3 0

#define CallGate_get_focus 0x3e 0x67 0x9a 53 1 0 0 3 0

#define CallGate_add_wait_for_tcp_listen 0x3e 0x67 0x9a 54 1 0 0 3 0
#define CallGate_create_tcp_listen 0x3e 0x67 0x9a 55 1 0 0 3 0
#define CallGate_get_tcp_listen 0x3e 0x67 0x9a 56 1 0 0 3 0
#define CallGate_close_tcp_listen 0x3e 0x67 0x9a 57 1 0 0 3 0

#define CallGate_get_random 0x3e 0x67 0x9a 58 1 0 0 3 0

#define CallGate_suspend_and_signal_thread 0x3e 0x67 0x9a 59 1 0 0 3 0

#define CallGate_start_net_capture 0x3e 0x67 0x9a 60 1 0 0 3 0
#define CallGate_stop_net_capture 0x3e 0x67 0x9a 61 1 0 0 3 0

#define CallGate_create_crc 0x3e 0x67 0x9a 62 1 0 0 3 0
#define CallGate_close_crc 0x3e 0x67 0x9a 63 1 0 0 3 0
#define CallGate_calc_crc 0x3e 0x67 0x9a 64 1 0 0 3 0

#define CallGate_get_usb_device 0x3e 0x67 0x9a 65 1 0 0 3 0
#define CallGate_get_usb_config 0x3e 0x67 0x9a 66 1 0 0 3 0

#define CallGate_open_usb_pipe 0x3e 0x67 0x9a 67 1 0 0 3 0
#define CallGate_close_usb_pipe 0x3e 0x67 0x9a 68 1 0 0 3 0
#define CallGate_add_wait_for_usb_pipe 0x3e 0x67 0x9a 69 1 0 0 3 0
#define CallGate_req_usb_data 0x3e 0x67 0x9a 70 1 0 0 3 0
#define CallGate_get_usb_data_size 0x3e 0x67 0x9a 71 1 0 0 3 0
#define CallGate_write_usb_data 0x3e 0x67 0x9a 72 1 0 0 3 0
#define CallGate_req_usb_status 0x3e 0x67 0x9a 73 1 0 0 3 0
#define CallGate_write_usb_status 0x3e 0x67 0x9a 74 1 0 0 3 0
#define CallGate_write_usb_control 0x3e 0x67 0x9a 75 1 0 0 3 0
#define CallGate_lock_usb_pipe 0x3e 0x67 0x9a 77 1 0 0 3 0
#define CallGate_unlock_usb_pipe 0x3e 0x67 0x9a 78 1 0 0 3 0

#define CallGate_get_max_com_port 0x3e 0x67 0x9a 79 1 0 0 3 0

#define CallGate_config_usb_device 0x3e 0x67 0x9a 80 1 0 0 3 0

#define CallGate_start_usb_transaction 0x3e 0x67 0x9a 81 1 0 0 3 0

#define CallGate_open_ini 0x3e 0x67 0x9a 82 1 0 0 3 0

#define CallGate_open_icsp 0x3e 0x67 0x9a 83 1 0 0 3 0
#define CallGate_close_icsp 0x3e 0x67 0x9a 84 1 0 0 3 0
#define CallGate_write_icsp_cmd 0x3e 0x67 0x9a 85 1 0 0 3 0
#define CallGate_write_icsp_data 0x3e 0x67 0x9a 86 1 0 0 3 0
#define CallGate_read_icsp_data 0x3e 0x67 0x9a 87 1 0 0 3 0

#define CallGate_get_gateway 0x3e 0x67 0x9a 88 1 0 0 3 0

#define CallGate_get_free_physical 0x3e 0x67 0x9a 89 1 0 0 3 0
#define CallGate_get_free_gdt 0x3e 0x67 0x9a 90 1 0 0 3 0
#define CallGate_available_big_linear 0x3e 0x67 0x9a 91 1 0 0 3 0
#define CallGate_available_small_linear 0x3e 0x67 0x9a 92 1 0 0 3 0

#define CallGate_get_ip_mask 0x3e 0x67 0x9a 93 1 0 0 3 0

#define CallGate_get_master_volume 0x3e 0x67 0x9a 94 1 0 0 3 0
#define CallGate_set_master_volume 0x3e 0x67 0x9a 95 1 0 0 3 0

#define CallGate_get_line_out_volume 0x3e 0x67 0x9a 96 1 0 0 3 0
#define CallGate_set_line_out_volume 0x3e 0x67 0x9a 97 1 0 0 3 0

#define CallGate_create_audio_out_channel 0x3e 0x67 0x9a 98 1 0 0 3 0
#define CallGate_close_audio_out_channel 0x3e 0x67 0x9a 99 1 0 0 3 0
#define CallGate_write_audio 0x3e 0x67 0x9a 100 1 0 0 3 0

#define CallGate_stop_watchdog 0x3e 0x67 0x9a 101 1 0 0 3 0

#define CallGate_is_valid_usergate 0x3e 0x67 0x9a 102 1 0 0 3 0

#define CallGate_open_fm 0x3e 0x67 0x9a 103 1 0 0 3 0
#define CallGate_close_fm 0x3e 0x67 0x9a 104 1 0 0 3 0
#define CallGate_fm_wait 0x3e 0x67 0x9a 105 1 0 0 3 0
#define CallGate_create_fm_instrument 0x3e 0x67 0x9a 106 1 0 0 3 0
#define CallGate_free_fm_instrument 0x3e 0x67 0x9a 107 1 0 0 3 0
#define CallGate_set_fm_attack 0x3e 0x67 0x9a 108 1 0 0 3 0
#define CallGate_set_fm_sustain 0x3e 0x67 0x9a 109 1 0 0 3 0
#define CallGate_set_fm_release 0x3e 0x67 0x9a 110 1 0 0 3 0
#define CallGate_play_fm_note 0x3e 0x67 0x9a 111 1 0 0 3 0

#define CallGate_get_thread_linear 0x3e 0x67 0x9a 112 1 0 0 3 0
#define CallGate_get_current_module 0x3e 0x67 0x9a 113 1 0 0 3 0

#define CallGate_get_module_focus_key 0x3e 0x67 0x9a 114 1 0 0 3 0

#define CallGate_get_debug_event 0x3e 0x67 0x9a 115 1 0 0 3 0
#define CallGate_clear_debug_event 0x3e 0x67 0x9a 116 1 0 0 3 0

#define CallGate_get_free_handles 0x3e 0x67 0x9a 117 1 0 0 3 0

#define CallGate_get_env_size 0x3e 0x67 0x9a 118 1 0 0 3 0

#define CallGate_show_exception_text 0x3e 0x67 0x9a 119 1 0 0 3 0

#define CallGate_get_watchdog_tics 0x3e 0x67 0x9a 120 1 0 0 3 0

#define CallGate_is_usb_trans_done 0x3e 0x67 0x9a 121 1 0 0 3 0
#define CallGate_was_usb_trans_ok 0x3e 0x67 0x9a 122 1 0 0 3 0

#define CallGate_is_tcp_connection_idle 0x3e 0x67 0x9a 123 1 0 0 3 0

#define CallGate_set_codec_gpio0 0x3e 0x67 0x9a 124 1 0 0 3 0

#define CallGate_free_proc_handle 0x3e 0x67 0x9a 125 1 0 0 3 0
#define CallGate_add_wait_for_proc_end 0x3e 0x67 0x9a 126 1 0 0 3 0
#define CallGate_get_proc_exit_code 0x3e 0x67 0x9a 127 1 0 0 3 0

#define CallGate_get_cpu_version 0x3e 0x67 0x9a 128 1 0 0 3 0
#define CallGate_get_options 0x3e 0x67 0x9a 129 1 0 0 3 0

#define CallGate_fork 0x3e 0x67 0x9a 130 1 0 0 3 0

#define CallGate_get_core_id 0x3e 0x67 0x9a 131 1 0 0 3 0

#define CallGate_create_user_read_write_section 0x3e 0x67 0x9a 132 1 0 0 3 0
#define CallGate_delete_user_read_write_section 0x3e 0x67 0x9a 133 1 0 0 3 0
#define CallGate_enter_user_read_section 0x3e 0x67 0x9a 134 1 0 0 3 0
#define CallGate_leave_user_read_section 0x3e 0x67 0x9a 135 1 0 0 3 0
#define CallGate_enter_user_write_section 0x3e 0x67 0x9a 136 1 0 0 3 0
#define CallGate_leave_user_write_section 0x3e 0x67 0x9a 137 1 0 0 3 0

#define CallGate_power_failure 0x3e 0x67 0x9a 138 1 0 0 3 0

#define CallGate_define_fault_save 0x3e 0x67 0x9a 139 1 0 0 3 0
#define CallGate_get_fault_thread_state 0x3e 0x67 0x9a 140 1 0 0 3 0
#define CallGate_get_fault_thread_tss 0x3e 0x67 0x9a 141 1 0 0 3 0
#define CallGate_clear_fault_save 0x3e 0x67 0x9a 142 1 0 0 3 0

#define CallGate_get_free_handle_mem 0x3e 0x67 0x9a 143 1 0 0 3 0

#define CallGate_get_image_header 0x3e 0x67 0x9a 144 1 0 0 3 0
#define CallGate_get_image_data 0x3e 0x67 0x9a 145 1 0 0 3 0

#define CallGate_allocate_debug_app_mem 0x3e 0x67 0x9a 146 1 0 0 3 0
#define CallGate_free_debug_app_mem 0x3e 0x67 0x9a 147 1 0 0 3 0

#define CallGate_get_key_layout 0x3e 0x67 0x9a 148 1 0 0 3 0
#define CallGate_set_key_layout 0x3e 0x67 0x9a 149 1 0 0 3 0

#define CallGate_get_device_info 0x3e 0x67 0x9a 150 1 0 0 3 0
#define CallGate_get_selector_info 0x3e 0x67 0x9a 151 1 0 0 3 0

#define CallGate_set_code_break 0x3e 0x67 0x9a 152 1 0 0 3 0
#define CallGate_set_read_data_break 0x3e 0x67 0x9a 153 1 0 0 3 0
#define CallGate_set_write_data_break 0x3e 0x67 0x9a 154 1 0 0 3 0
#define CallGate_clear_break 0x3e 0x67 0x9a 155 1 0 0 3 0

#define CallGate_get_max_printer 0x3e 0x67 0x9a 156 1 0 0 3 0
#define CallGate_open_printer 0x3e 0x67 0x9a 157 1 0 0 3 0
#define CallGate_close_printer 0x3e 0x67 0x9a 158 1 0 0 3 0

#define CallGate_is_printer_jammed 0x3e 0x67 0x9a 159 1 0 0 3 0
#define CallGate_is_printer_paper_low 0x3e 0x67 0x9a 160 1 0 0 3 0
#define CallGate_is_printer_paper_end 0x3e 0x67 0x9a 161 1 0 0 3 0
#define CallGate_is_printer_ok 0x3e 0x67 0x9a 162 1 0 0 3 0
#define CallGate_is_printer_head_lifted 0x3e 0x67 0x9a 163 1 0 0 3 0
#define CallGate_has_printer_paper_in_presenter 0x3e 0x67 0x9a 164 1 0 0 3 0

#define CallGate_print_test 0x3e 0x67 0x9a 165 1 0 0 3 0

#define CallGate_create_printer_bitmap 0x3e 0x67 0x9a 166 1 0 0 3 0
#define CallGate_print_bitmap 0x3e 0x67 0x9a 167 1 0 0 3 0

#define CallGate_present_printer_media 0x3e 0x67 0x9a 168 1 0 0 3 0
#define CallGate_eject_printer_media 0x3e 0x67 0x9a 169 1 0 0 3 0

#define CallGate_wait_for_print 0x3e 0x67 0x9a 170 1 0 0 3 0

#define CallGate_has_audio 0x3e 0x67 0x9a 171 1 0 0 3 0

#define CallGate_available_big_local_linear 0x3e 0x67 0x9a 172 1 0 0 3 0

#define CallGate_get_allocated_usb_blocks 0x3e 0x67 0x9a 173 1 0 0 3 0
#define CallGate_get_usb_close_count 0x3e 0x67 0x9a 174 1 0 0 3 0

#define CallGate_reset_usb_pipe 0x3e 0x67 0x9a 175 1 0 0 3 0

#define CallGate_add_syslog 0x3e 0x67 0x9a 176 1 0 0 3 0
#define CallGate_open_syslog 0x3e 0x67 0x9a 177 1 0 0 3 0
#define CallGate_close_syslog 0x3e 0x67 0x9a 178 1 0 0 3 0
#define CallGate_add_wait_for_syslog 0x3e 0x67 0x9a 179 1 0 0 3 0
#define CallGate_get_syslog 0x3e 0x67 0x9a 180 1 0 0 3 0

#define CallGate_soft_reset 0x3e 0x67 0x9a 181 1 0 0 3 0
#define CallGate_hard_reset 0x3e 0x67 0x9a 182 1 0 0 3 0

#define CallGate_is_emergency_stopped 0x3e 0x67 0x9a 183 1 0 0 3 0

#define CallGate_load_device32 0x3e 0x67 0x9a 184 1 0 0 3 0

#define CallGate_debug_run 0x3e 0x67 0x9a 185 1 0 0 3 0

#define CallGate_test_gate 0x3e 0x67 0x9a 186 1 0 0 3 0

#define CallGate_get_acpi_status 0x3e 0x67 0x9a 187 1 0 0 3 0
#define CallGate_get_acpi_object 0x3e 0x67 0x9a 188 1 0 0 3 0
#define CallGate_get_cpu_temperature 0x3e 0x67 0x9a 189 1 0 0 3 0
#define CallGate_get_acpi_method 0x3e 0x67 0x9a 190 1 0 0 3 0

#define CallGate_write_attrib_string 0x3e 0x67 0x9a 191 1 0 0 3 0

#define CallGate_has_hard_reset 0x3e 0x67 0x9a 193 1 0 0 3 0

#define CallGate_get_acpi_device 0x3e 0x67 0x9a 194 1 0 0 3 0
#define CallGate_get_acpi_device_irq 0x3e 0x67 0x9a 195 1 0 0 3 0
#define CallGate_get_acpi_device_io 0x3e 0x67 0x9a 196 1 0 0 3 0
#define CallGate_get_acpi_device_mem 0x3e 0x67 0x9a 197 1 0 0 3 0

#define CallGate_get_pci_device_name 0x3e 0x67 0x9a 198 1 0 0 3 0
#define CallGate_get_pci_device_info 0x3e 0x67 0x9a 199 1 0 0 3 0
#define CallGate_get_pci_device_vendor 0x3e 0x67 0x9a 200 1 0 0 3 0
#define CallGate_get_pci_device_class 0x3e 0x67 0x9a 201 1 0 0 3 0
#define CallGate_get_pci_device_irq 0x3e 0x67 0x9a 202 1 0 0 3 0

#define CallGate_open_hid 0x3e 0x67 0x9a 203 1 0 0 3 0
#define CallGate_close_hid 0x3e 0x67 0x9a 204 1 0 0 3 0
#define CallGate_get_hid_pipe 0x3e 0x67 0x9a 205 1 0 0 3 0

#define CallGate_get_core_load 0x3e 0x67 0x9a 206 1 0 0 3 0
#define CallGate_get_core_duty 0x3e 0x67 0x9a 207 1 0 0 3 0

#define CallGate_has_global_timer 0x3e 0x67 0x9a 208 1 0 0 3 0

#define CallGate_remote_debug 0x3e 0x67 0x9a 209 1 0 0 3 0

#define CallGate_read_hid 0x3e 0x67 0x9a 210 1 0 0 3 0
#define CallGate_write_hid 0x3e 0x67 0x9a 211 1 0 0 3 0

#define CallGate_ansi_to_utf8 0x3e 0x67 0x9a 212 1 0 0 3 0
#define CallGate_utf8_to_ansi 0x3e 0x67 0x9a 213 1 0 0 3 0

#define CallGate_get_printer_name 0x3e 0x67 0x9a 214 1 0 0 3 0

#define CallGate_acquire_futex 0x3e 0x67 0x9a 215 1 0 0 3 0
#define CallGate_release_futex 0x3e 0x67 0x9a 216 1 0 0 3 0
#define CallGate_cleanup_futex 0x3e 0x67 0x9a 217 1 0 0 3 0

#define CallGate_goto_first_inivar 0x3e 0x67 0x9a 218 1 0 0 3 0
#define CallGate_goto_next_inivar 0x3e 0x67 0x9a 219 1 0 0 3 0
#define CallGate_get_curr_inivar 0x3e 0x67 0x9a 220 1 0 0 3 0

#define CallGate_reset_printer 0x3e 0x67 0x9a 221 1 0 0 3 0

#define CallGate_has_icsp 0x3e 0x67 0x9a 222 1 0 0 3 0
#define CallGate_reset_icsp 0x3e 0x67 0x9a 223 1 0 0 3 0

#define CallGate_get_max_carddev 0x3e 0x67 0x9a 224 1 0 0 3 0
#define CallGate_open_carddev 0x3e 0x67 0x9a 225 1 0 0 3 0
#define CallGate_close_carddev 0x3e 0x67 0x9a 226 1 0 0 3 0
#define CallGate_is_carddev_ok 0x3e 0x67 0x9a 227 1 0 0 3 0
#define CallGate_wait_for_card 0x3e 0x67 0x9a 228 1 0 0 3 0
#define CallGate_get_carddev_name 0x3e 0x67 0x9a 229 1 0 0 3 0

#define CallGate_is_carddev_busy 0x3e 0x67 0x9a 230 1 0 0 3 0
#define CallGate_is_carddev_inserted 0x3e 0x67 0x9a 231 1 0 0 3 0

#define CallGate_has_touch 0x3e 0x67 0x9a 232 1 0 0 3 0

#define CallGate_had_carddev_inserted 0x3e 0x67 0x9a 233 1 0 0 3 0
#define CallGate_clear_carddev_inserted 0x3e 0x67 0x9a 234 1 0 0 3 0

#define CallGate_is_printer_cutter_jammed 0x3e 0x67 0x9a 235 1 0 0 3 0
#define CallGate_has_printer_temp_error 0x3e 0x67 0x9a 236 1 0 0 3 0
#define CallGate_has_printer_feed_error 0x3e 0x67 0x9a 237 1 0 0 3 0

#define CallGate_is_64_bit_exe 0x3e 0x67 0x9a 238 1 0 0 3 0

#define CallGate_get_audio_device_count 0x3e 0x67 0x9a 239 1 0 0 3 0
#define CallGate_get_audio_codec_count 0x3e 0x67 0x9a 240 1 0 0 3 0
#define CallGate_get_audio_widget_info 0x3e 0x67 0x9a 241 1 0 0 3 0
#define CallGate_get_audio_widget_connection_list 0x3e 0x67 0x9a 242 1 0 0 3 0
#define CallGate_get_selected_audio_connection 0x3e 0x67 0x9a 243 1 0 0 3 0

#define CallGate_get_audio_input_amp_cap 0x3e 0x67 0x9a 244 1 0 0 3 0
#define CallGate_get_audio_output_amp_cap 0x3e 0x67 0x9a 245 1 0 0 3 0

#define CallGate_has_audio_input_mute 0x3e 0x67 0x9a 246 1 0 0 3 0
#define CallGate_has_audio_output_mute 0x3e 0x67 0x9a 247 1 0 0 3 0

#define CallGate_read_audio_input_amp 0x3e 0x67 0x9a 248 1 0 0 3 0
#define CallGate_read_audio_output_amp 0x3e 0x67 0x9a 249 1 0 0 3 0

#define CallGate_is_audio_input_amp_muted 0x3e 0x67 0x9a 250 1 0 0 3 0
#define CallGate_is_audio_output_amp_muted 0x3e 0x67 0x9a 251 1 0 0 3 0

#define CallGate_get_fixed_audio_output 0x3e 0x67 0x9a 252 1 0 0 3 0
#define CallGate_get_jack_audio_output 0x3e 0x67 0x9a 253 1 0 0 3 0
#define CallGate_get_jack_audio_input 0x3e 0x67 0x9a 254 1 0 0 3 0

#define CallGate_get_output_volume 0x3e 0x67 0x9a 255 1 0 0 3 0
#define CallGate_set_output_volume 0x3e 0x67 0x9a 0 2 0 0 3 0

#define CallGate_broadcast_query_udp 0x3e 0x67 0x9a 1 2 0 0 3 0
#define CallGate_send_udp 0x3e 0x67 0x9a 2 2 0 0 3 0

#define CallGate_open_udp_connection 0x3e 0x67 0x9a 3 2 0 0 3 0
#define CallGate_close_udp_connection 0x3e 0x67 0x9a 4 2 0 0 3 0
#define CallGate_send_udp_connection 0x3e 0x67 0x9a 5 2 0 0 3 0
#define CallGate_add_wait_for_udp_connection 0x3e 0x67 0x9a 6 2 0 0 3 0
#define CallGate_peek_udp_connection 0x3e 0x67 0x9a 7 2 0 0 3 0
#define CallGate_read_udp_connection 0x3e 0x67 0x9a 8 2 0 0 3 0

#define CallGate_extract_alpha_bitmap 0x3e 0x67 0x9a 9 2 0 0 3 0
#define CallGate_extract_valid_bitmap_mask 0x3e 0x67 0x9a 10 2 0 0 3 0
#define CallGate_extract_invalid_bitmap_mask 0x3e 0x67 0x9a 11 2 0 0 3 0

#define CallGate_create_alpha_bitmap 0x3e 0x67 0x9a 12 2 0 0 3 0

#define CallGate_create_long_thread 0x3e 0x67 0x9a 13 2 0 0 3 0

#endif
