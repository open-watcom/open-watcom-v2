/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2009-2010 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  RDOS API Call in-line assembly code definition. This file
*               is generated automatically, do not edit it.
*
****************************************************************************/

#define usergate_allocate_dos_mem 0
#define usergate_allocate_local_mem 1
#define usergate_free_mem 2
#define usergate_available_small_local_linear 3
#define usergate_used_local_linear 4
#define usergate_available_vm_linear 5
#define usergate_used_vm_linear 6

#define usergate_reserve_pe_mem 7
#define usergate_set_flat_linear_valid 8
#define usergate_set_flat_linear_invalid 9
#define usergate_set_flat_linear_read 10
#define usergate_set_flat_linear_readwrite 11

#define usergate_get_raw_switch_ads 12
#define usergate_raw_switch 13
#define usergate_get_exception 14
#define usergate_set_exception 15
#define usergate_get_pm_int 16
#define usergate_set_pm_int 17
#define usergate_get_vm_int 18
#define usergate_set_vm_int 19
#define usergate_dpmi_int 20
#define usergate_dpmi_call_int 21
#define usergate_dpmi_call 22
#define usergate_allocate_vm_callback 23
#define usergate_free_vm_callback 24

#define usergate_wait_milli 25
#define usergate_wait_micro 26
#define usergate_wait_until 27
#define usergate_create_thread 28
#define usergate_terminate_thread 29
#define usergate_get_thread 30

#define usergate_create_user_section 32
#define usergate_create_blocked_user_section 33
#define usergate_delete_user_section 34
#define usergate_enter_user_section 35
#define usergate_leave_user_section 36

#define usergate_get_local_mailslot 37
#define usergate_get_remote_mailslot 38
#define usergate_free_mailslot 39
#define usergate_send_mailslot 40
#define usergate_define_mailslot 41
#define usergate_receive_mailslot 42
#define usergate_reply_mailslot 43

#define usergate_load_exe 44
#define usergate_spawn_exe 45
#define usergate_unload_exe 46
#define usergate_get_exit_code 47
#define usergate_get_exe_name 48
#define usergate_get_cmd_line 49
#define usergate_get_env 50
#define usergate_load_dll 51
#define usergate_free_dll 52
#define usergate_get_module_proc 53
#define usergate_get_module_resource 54
#define usergate_get_module_name 55
#define usergate_get_module 56
#define usergate_allocate_app_mem 57
#define usergate_free_app_mem 58
#define usergate_get_psp_sel 59

#define usergate_get_debug_thread 61
#define usergate_debug_trace 62
#define usergate_debug_pace 63
#define usergate_debug_go 64
#define usergate_debug_next 65

#define usergate_add_wait_for_debug_event 66
#define usergate_get_debug_event_data 67
#define usergate_continue_debug_event 68
#define usergate_notify_pe_exception 70
#define usergate_read_thread_mem 71
#define usergate_write_thread_mem 72
#define usergate_get_thread_tss 73
#define usergate_set_thread_tss 74

#define usergate_get_cpu_time 75
#define usergate_get_system_time 76
#define usergate_get_time 77
#define usergate_time_to_system_time 78
#define usergate_system_time_to_time 79
#define usergate_days_in_month 80
#define usergate_adjust_time 81
#define usergate_passed_days 82
#define usergate_time_to_binary 83
#define usergate_binary_to_time 84
#define usergate_sync_time 85

#define usergate_set_focus 86
#define usergate_enable_focus 87

#define usergate_get_dns 88
#define usergate_get_ppp_dns 89
#define usergate_open_tcp_connection 90
#define usergate_wait_for_tcp_connection 92
#define usergate_close_tcp_connection 93
#define usergate_delete_tcp_connection 94
#define usergate_is_tcp_connection_closed 95
#define usergate_abort_tcp_connection 96
#define usergate_read_tcp_connection 97
#define usergate_write_tcp_connection 98
#define usergate_push_tcp_connection 99
#define usergate_get_ip_address 100
#define usergate_name_to_ip 101
#define usergate_ip_to_name 102
#define usergate_ping 103

#define usergate_get_disc_info 104
#define usergate_format_drive 105
#define usergate_get_rdfs_info 106
#define usergate_read_disc 107
#define usergate_write_disc 108
#define usergate_get_drive_info 109

#define usergate_set_cur_drive 110
#define usergate_get_cur_drive 111
#define usergate_set_cur_dir 112
#define usergate_get_cur_dir 113
#define usergate_make_dir 114
#define usergate_remove_dir 115
#define usergate_rename_file 116
#define usergate_delete_file 117
#define usergate_get_file_attribute 118
#define usergate_set_file_attribute 119
#define usergate_open_dir 120
#define usergate_close_dir 121
#define usergate_read_dir 122

#define usergate_open_file 123
#define usergate_create_file 124
#define usergate_close_file 125
#define usergate_dupl_file 126
#define usergate_get_ioctl_data 127
#define usergate_get_file_size 128
#define usergate_set_file_size 129
#define usergate_get_file_pos 130
#define usergate_set_file_pos 131
#define usergate_get_file_time 132
#define usergate_set_file_time 133
#define usergate_read_file 134
#define usergate_write_file 135
#define usergate_read_con 136

#define usergate_create_mapping 137
#define usergate_create_named_mapping 138
#define usergate_create_file_mapping 139
#define usergate_create_named_file_mapping 140
#define usergate_open_named_mapping 141
#define usergate_sync_mapping 142
#define usergate_close_mapping 143
#define usergate_map_view 144
#define usergate_unmap_view 145

#define usergate_read_keyboard 146
#define usergate_poll_keyboard 147
#define usergate_flush_keyboard 148
#define usergate_get_keyboard_state 151

#define usergate_show_mouse 152
#define usergate_hide_mouse 153
#define usergate_get_mouse_position 154
#define usergate_set_mouse_position 155
#define usergate_set_mouse_window 156
#define usergate_set_mouse_mickey 157
#define usergate_get_left_button 158
#define usergate_get_right_button 159
#define usergate_get_left_button_press_position 160
#define usergate_get_right_button_press_position 161
#define usergate_get_left_button_release_position 162
#define usergate_get_right_button_release_position 163
#define usergate_hook_mouse 164
#define usergate_unhook_mouse 165

#define usergate_lock_cd 166
#define usergate_unlock_cd 167
#define usergate_eject_cd 168
#define usergate_insert_cd 169

#define usergate_open_com 170
#define usergate_close_com 171
#define usergate_flush_com 172
#define usergate_add_wait_for_com 174
#define usergate_read_com 175
#define usergate_write_com 176
#define usergate_set_dtr 177
#define usergate_reset_dtr 178

#define usergate_set_cursor_position 182
#define usergate_get_cursor_position 183
#define usergate_write_char 184
#define usergate_write_asciiz 185
#define usergate_write_size_string 186

#define usergate_set_video_mode 187
#define usergate_set_vga_mode 188
#define usergate_set_forecolor 189
#define usergate_set_backcolor 190
#define usergate_get_string_metrics 193
#define usergate_set_font 194
#define usergate_get_video_mode 195
#define usergate_draw_string 197

#define usergate_xms_handler 198
#define usergate_ems_handler 199

#define usergate_update_rtc 200

#define usergate_resize_flat_linear 201

#define usergate_set_drawcolor 202
#define usergate_set_lgop 203
#define usergate_set_hollow_style 204
#define usergate_set_filled_style 205
#define usergate_get_pixel 206
#define usergate_set_pixel 207
#define usergate_draw_line 208
#define usergate_draw_rect 209
#define usergate_draw_ellipse 210
#define usergate_create_bitmap 211
#define usergate_close_bitmap 212
#define usergate_blit 213
#define usergate_open_font 215
#define usergate_close_font 216
#define usergate_create_string_bitmap 217
#define usergate_get_bitmap_info 218
#define usergate_draw_mask 219

#define usergate_create_sprite 220
#define usergate_close_sprite 221
#define usergate_show_sprite 222
#define usergate_hide_sprite 223
#define usergate_move_sprite 224

#define usergate_dup_bitmap_handle 225
#define usergate_set_clip_rect 226
#define usergate_clear_clip_rect 227

#define usergate_set_rts 228
#define usergate_reset_rts 229
#define usergate_get_com_receive_space 230
#define usergate_get_com_send_space 231

#define usergate_get_char_attrib 232

#define usergate_create_wait 233
#define usergate_close_wait 234
#define usergate_is_wait_idle 235
#define usergate_wait_no_timeout 236
#define usergate_wait_timeout 237
#define usergate_stop_wait 238
#define usergate_add_wait_for_keyboard 239

#define usergate_peek_key_event 240
#define usergate_read_key_event 241

#define usergate_add_wait_for_mouse 242
#define usergate_remove_wait 243

#define usergate_add_wait_for_adc 244
#define usergate_open_adc 245
#define usergate_close_adc 246
#define usergate_define_adc_time 247
#define usergate_read_adc 248

#define usergate_free_v86 249

#define usergate_read_serial_lines 250
#define usergate_toggle_serial_line 251

#define usergate_read_serial_val 252
#define usergate_write_serial_val 253

#define usergate_create_file_drive 254
#define usergate_open_file_drive 255

#define usergate_dos_ext_exec 256

#define usergate_open_sys_env 257
#define usergate_open_proc_env 258
#define usergate_close_env 259
#define usergate_add_env_var 260
#define usergate_delete_env_var 261
#define usergate_find_env_var 262
#define usergate_get_env_data 263
#define usergate_set_env_data 264

#define usergate_open_sys_ini 265
#define usergate_close_ini 266
#define usergate_goto_ini_section 267
#define usergate_remove_ini_section 268
#define usergate_read_ini 269
#define usergate_write_ini 270
#define usergate_delete_ini 271

#define usergate_get_version 273

#define usergate_enable_status_led 274
#define usergate_disable_status_led 275

#define usergate_start_watchdog 276
#define usergate_kick_watchdog 277

#define usergate_erase_disc_sectors 278

#define usergate_enable_cts 280
#define usergate_disable_cts 281

#define usergate_wait_for_send_completed_com 282

#define usergate_add_wait_for_tcp_connection 283

#define usergate_get_remote_tcp_connection_ip 284
#define usergate_get_remote_tcp_connection_port 285
#define usergate_get_local_tcp_connection_port 286

#define usergate_enable_auto_rts 287
#define usergate_disable_auto_rts 288

#define usergate_put_keyboard_code 289

#define usergate_poll_tcp_connection 290

#define usergate_create_signal 291
#define usergate_free_signal 292
#define usergate_add_wait_for_signal 293
#define usergate_set_signal 294
#define usergate_reset_signal 295
#define usergate_is_signalled 296

#define usergate_get_drive_disc_param 297

#define usergate_get_ide_disc 298
#define usergate_get_floppy_disc 299
#define usergate_demand_load_drive 300
#define usergate_set_disc_info 301

#define usergate_get_thread_state 302
#define usergate_suspend_thread 303
#define usergate_resume_thread 304

#define usergate_update_time 305

#define usergate_allocate_static_drive 306
#define usergate_allocate_fixed_drive 307
#define usergate_allocate_dynamic_drive 308

#define usergate_get_focus 309

#define usergate_add_wait_for_tcp_listen 310
#define usergate_create_tcp_listen 311
#define usergate_get_tcp_listen 312
#define usergate_close_tcp_listen 313

#define usergate_get_random 314

#define usergate_suspend_and_signal_thread 315

#define usergate_start_net_capture 316
#define usergate_stop_net_capture 317

#define usergate_create_crc 318
#define usergate_close_crc 319
#define usergate_calc_crc 320

#define usergate_get_usb_device 321
#define usergate_get_usb_config 322

#define usergate_open_usb_pipe 323
#define usergate_close_usb_pipe 324
#define usergate_add_wait_for_usb_pipe 325
#define usergate_req_usb_data 326
#define usergate_get_usb_data_size 327
#define usergate_write_usb_data 328
#define usergate_req_usb_status 329
#define usergate_write_usb_status 330
#define usergate_write_usb_control 331
#define usergate_lock_usb_pipe 333
#define usergate_unlock_usb_pipe 334

#define usergate_get_max_com_port 335

#define usergate_config_usb_device 336

#define usergate_start_usb_transaction 337

#define usergate_open_ini 338

#define usergate_open_icsp 339
#define usergate_close_icsp 340
#define usergate_write_icsp_cmd 341
#define usergate_write_icsp_data 342
#define usergate_read_icsp_data 343

#define usergate_get_gateway 344

#define usergate_get_free_physical 345
#define usergate_get_free_gdt 346
#define usergate_available_big_linear 347
#define usergate_available_small_linear 348

#define usergate_get_ip_mask 349

#define usergate_get_master_volume 350
#define usergate_set_master_volume 351

#define usergate_get_line_out_volume 352
#define usergate_set_line_out_volume 353

#define usergate_create_audio_out_channel 354
#define usergate_close_audio_out_channel 355
#define usergate_write_audio 356

#define usergate_stop_watchdog 357

#define usergate_is_valid_usergate 358

#define usergate_open_fm 359
#define usergate_close_fm 360
#define usergate_fm_wait 361
#define usergate_create_fm_instrument 362
#define usergate_free_fm_instrument 363
#define usergate_set_fm_attack 364
#define usergate_set_fm_sustain 365
#define usergate_set_fm_release 366
#define usergate_play_fm_note 367

#define usergate_get_thread_linear 368
#define usergate_get_current_module 369

#define usergate_get_module_focus_key 370

#define usergate_get_debug_event 371
#define usergate_clear_debug_event 372

#define usergate_get_free_handles 373

#define usergate_get_env_size 374

#define usergate_show_exception_text 375

#define usergate_get_watchdog_tics 376

#define usergate_is_usb_trans_done 377
#define usergate_was_usb_trans_ok 378

#define usergate_is_tcp_connection_idle 379

#define usergate_set_codec_gpio0 380

#define usergate_free_proc_handle 381
#define usergate_add_wait_for_proc_end 382
#define usergate_get_proc_exit_code 383

#define usergate_get_cpu_version 384
#define usergate_get_options 385

#define usergate_fork 386

#define usergate_get_core_id 387

#define usergate_create_user_read_write_section 388
#define usergate_delete_user_read_write_section 389
#define usergate_enter_user_read_section 390
#define usergate_leave_user_read_section 391
#define usergate_enter_user_write_section 392
#define usergate_leave_user_write_section 393

#define usergate_power_failure 394

#define usergate_define_fault_save 395
#define usergate_get_fault_thread_state 396
#define usergate_get_fault_thread_tss 397
#define usergate_clear_fault_save 398

#define usergate_get_free_handle_mem 399

#define usergate_get_image_header 400
#define usergate_get_image_data 401

#define usergate_allocate_debug_app_mem 402
#define usergate_free_debug_app_mem 403

#define usergate_get_key_layout 404
#define usergate_set_key_layout 405

#define usergate_get_device_info 406
#define usergate_get_selector_info 407

#define usergate_set_code_break 408
#define usergate_set_read_data_break 409
#define usergate_set_write_data_break 410
#define usergate_clear_break 411

#define usergate_get_max_printer 412
#define usergate_open_printer 413
#define usergate_close_printer 414

#define usergate_is_printer_jammed 415
#define usergate_is_printer_paper_low 416
#define usergate_is_printer_paper_end 417
#define usergate_is_printer_ok 418
#define usergate_is_printer_head_lifted 419
#define usergate_has_printer_paper_in_presenter 420

#define usergate_print_test 421

#define usergate_create_printer_bitmap 422
#define usergate_print_bitmap 423

#define usergate_present_printer_media 424
#define usergate_eject_printer_media 425

#define usergate_wait_for_print 426

#define usergate_has_audio 427

#define usergate_available_big_local_linear 428

#define usergate_get_allocated_usb_blocks 429
#define usergate_get_usb_close_count 430

#define usergate_reset_usb_pipe 431

#define usergate_add_syslog 432
#define usergate_open_syslog 433
#define usergate_close_syslog 434
#define usergate_add_wait_for_syslog 435
#define usergate_get_syslog 436

#define usergate_soft_reset 437
#define usergate_hard_reset 438

#define usergate_is_emergency_stopped 439

#define usergate_load_device32 440

#define usergate_debug_run 441

#define usergate_test_gate 442

#define usergate_get_acpi_status 443
#define usergate_get_acpi_object 444
#define usergate_get_cpu_temperature 445
#define usergate_get_acpi_method 446

#define usergate_write_attrib_string 447

#define usergate_debug_break 448

#define usergate_has_hard_reset 449

#define usergate_get_acpi_device 450
#define usergate_get_acpi_device_irq 451
#define usergate_get_acpi_device_io 452
#define usergate_get_acpi_device_mem 453

#define usergate_get_pci_device_name 454
#define usergate_get_pci_device_info 455
#define usergate_get_pci_device_vendor 456
#define usergate_get_pci_device_class 457
#define usergate_get_pci_device_irq 458

#define usergate_open_hid 459
#define usergate_close_hid 460
#define usergate_get_hid_pipe 461

#define usergate_get_core_load 462
#define usergate_get_core_duty 463

#define usergate_has_global_timer 464

#define usergate_remote_debug 465

#define usergate_read_hid 466
#define usergate_write_hid 467

#define usergate_ansi_to_utf8 468
#define usergate_utf8_to_ansi 469

#define usergate_get_printer_name 470



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
#define CallGate_raw_switch 0x67 0x9a 13 0 0 0 3 0
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

#define CallGate_update_rtc 0x67 0x9a 200 0 0 0 3 0

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

#define CallGate_debug_break 0x67 0x9a 192 1 0 0 3 0

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
#define CallGate_raw_switch 0x3e 0x67 0x9a 13 0 0 0 3 0
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

#define CallGate_update_rtc 0x3e 0x67 0x9a 200 0 0 0 3 0

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

#define CallGate_debug_break 0x3e 0x67 0x9a 192 1 0 0 3 0

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

#endif
