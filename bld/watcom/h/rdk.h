/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2009-2010 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  RDOS device-driver in-line assembly code definition. This file
*               is generated automatically, do not edit it.
*
****************************************************************************/

#define osgate_register_osgate 0
#define osgate_is_valid_osgate 1
#define osgate_register_usergate 2
#define osgate_register_bimodal_usergate 3
#define osgate_register_usergate16 4
#define osgate_register_usergate32 5
#define osgate_create_alias_sel16 7
#define osgate_create_core_gdt 8
#define osgate_emulate_opcode 9

#define osgate_allocate_gdt 10
#define osgate_free_gdt 11
#define osgate_allocate_ldt 12
#define osgate_free_ldt 13
#define osgate_allocate_multiple_ldt 14
#define osgate_get_selector_base_size 15
#define osgate_create_data_sel16 16
#define osgate_create_data_sel32 17
#define osgate_create_code_sel16 18
#define osgate_create_code_sel32 19
#define osgate_create_conform_sel16 20
#define osgate_create_conform_sel32 21
#define osgate_create_ldt_sel 22
#define osgate_create_tss_sel 23
#define osgate_create_call_gate_sel16 24
#define osgate_create_call_gate_sel32 25
#define osgate_create_task_gate_sel 26
#define osgate_selector_to_segment 29
#define osgate_segment_to_selector 30
#define osgate_free_selector 31
#define osgate_segment_not_present 32

#define osgate_get_physical_page 33
#define osgate_set_physical_page 34
#define osgate_get_thread_physical_page 35
#define osgate_set_thread_physical_page 36
#define osgate_read_thread_selector 37
#define osgate_write_thread_selector 38
#define osgate_read_thread_segment 39
#define osgate_write_thread_segment 40
#define osgate_install_device_file 41
#define osgate_check_device_file 42
#define osgate_set_page_emulate 43
#define osgate_hook_page 45
#define osgate_unhook_page 46

#define osgate_allocate_global_mem 47
#define osgate_allocate_small_global_mem 48
#define osgate_allocate_small_mem 49
#define osgate_allocate_big_mem 50
#define osgate_allocate_big_linear 51
#define osgate_allocate_small_linear 52
#define osgate_allocate_local_linear 53
#define osgate_reserve_local_linear 54
#define osgate_allocate_vm_linear 55
#define osgate_free_linear 56
#define osgate_resize_linear 57
#define osgate_used_big_linear 60
#define osgate_used_small_linear 61
#define osgate_used_local_linear_thread 62
#define osgate_allocate_fixed_process_mem 64
#define osgate_allocate_fixed_system_mem 65
#define osgate_allocate_process_linear 67
#define osgate_allocate_system_linear 68
#define osgate_allocate_fixed_vm_linear 69

#define osgate_allocate_physical 70
#define osgate_allocate_dma_physical 71
#define osgate_free_physical 72

#define osgate_create_process 74

#define osgate_init_trap_gates 75
#define osgate_init_tss_gates 76

#define osgate_setup_int_gate 77
#define osgate_setup_trap_gate 78

#define osgate_wake_thread 79
#define osgate_sleep_thread 80
#define osgate_irq_schedule 81
#define osgate_clear_signal 83
#define osgate_signal 84
#define osgate_wait_for_signal 85
#define osgate_enter_section 86
#define osgate_leave_section 87
#define osgate_cond_enter_section 88
#define osgate_start_timer 89
#define osgate_stop_timer 90
#define osgate_debug_exc_break 92
#define osgate_hook_create_thread 93
#define osgate_hook_create_process 94
#define osgate_hook_terminate_thread 95
#define osgate_hook_terminate_process 96
#define osgate_hook_init_tasking 97
#define osgate_hook_state 98

#define osgate_sim_sti 99
#define osgate_sim_cli 100
#define osgate_sim_set_flags 101
#define osgate_sim_get_flags 102
#define osgate_save_context 103
#define osgate_restore_context 104
#define osgate_call_vm 105
#define osgate_call_pm16 106
#define osgate_call_pm32 107
#define osgate_hook_vm_int 108
#define osgate_hook_pm16_int 109
#define osgate_hook_pm32_int 110
#define osgate_hook_exception 111
#define osgate_hook_get_vm_int 112
#define osgate_hook_set_vm_int 113
#define osgate_hook_get_pm16_int 114
#define osgate_hook_set_pm16_int 115
#define osgate_hook_get_pm32_int 116
#define osgate_hook_set_pm32_int 117
#define osgate_reflect_exception 118
#define osgate_reflect_pm_to_vm 119
#define osgate_v86_bios_int 120
#define osgate_set_bitness 121

#define osgate_hook_in 122
#define osgate_hook_out 123

#define osgate_disable_all_irq 126

#define osgate_set_system_time 128

#define osgate_register_handle 129
#define osgate_allocate_handle 130
#define osgate_free_handle 131
#define osgate_deref_handle 132

#define osgate_register_net_class 133
#define osgate_register_net_protocol 134
#define osgate_register_net_driver 135
#define osgate_register_ppp_driver 136
#define osgate_send_net 137
#define osgate_send_ppp 138
#define osgate_net_received 139
#define osgate_hook_ip 140
#define osgate_create_ip_header 141
#define osgate_send_ip_data 142
#define osgate_hook_ip_cache 143
#define osgate_allocate_ip_cache_mem 144
#define osgate_lookup_ip_cache 145
#define osgate_get_host_timeout 146
#define osgate_update_round_trip_time 147
#define osgate_get_ppp_ip 148
#define osgate_open_ppp 149
#define osgate_close_ppp 150
#define osgate_query_udp 151
#define osgate_listen_udp_port 152

#define osgate_allocate_dos_linear 153
#define osgate_free_dos_linear 154
#define osgate_resize_dos_linear 155
#define osgate_available_dos_linear 156
#define osgate_query_xms 157
#define osgate_query_dpmi 158
#define osgate_query_dpmi16 159
#define osgate_get_bios_data 160
#define osgate_set_bios_data 161
#define osgate_hook_get_bios_data 162
#define osgate_hook_set_bios_data 163
#define osgate_hook_load_dos_exe 164
#define osgate_read_keyboard_serial 165
#define osgate_poll_keyboard_serial 166
#define osgate_write_dos_string 167
#define osgate_get_psp 168
#define osgate_set_psp 169
#define osgate_enter_dpmi 170
#define osgate_enter_dos16 171
#define osgate_enter_dos32 172
#define osgate_register_dos_device 173
#define osgate_check_dos_device 174

#define osgate_hook_open_app 177
#define osgate_hook_close_app 178
#define osgate_hook_load_exe 179
#define osgate_open_app 180
#define osgate_close_app 181

#define osgate_hook_enable_focus 182
#define osgate_hook_lost_focus 183
#define osgate_hook_got_focus 184
#define osgate_get_focus_thread 185
#define osgate_get_thread_focus_key 186
#define osgate_allocate_focus_linear 187
#define osgate_allocate_fixed_focus_mem 188

#define osgate_bios_pci_int 189
#define osgate_read_pci_byte 190
#define osgate_read_pci_word 191
#define osgate_read_pci_dword 192
#define osgate_write_pci_byte 193
#define osgate_write_pci_word 194
#define osgate_write_pci_dword 195
#define osgate_find_pci_class 196
#define osgate_find_pci_device 197

#define osgate_hook_init_disc 198
#define osgate_install_disc 199
#define osgate_register_disc_change 200
#define osgate_start_disc 201
#define osgate_stop_disc 202
#define osgate_set_disc_param 203
#define osgate_wait_for_disc_request 204
#define osgate_get_disc_request 205
#define osgate_poll_disc_request 206
#define osgate_disc_request_completed 207
#define osgate_new_disc_request 208
#define osgate_get_disc_request_array 209
#define osgate_open_drive 214
#define osgate_close_drive 215
#define osgate_flush_drive 216
#define osgate_get_drive_param 217
#define osgate_lock_sector 218
#define osgate_unlock_sector 219
#define osgate_modify_sector 220
#define osgate_new_sector 221
#define osgate_create_disc_seq 222
#define osgate_modify_seq_sector 223
#define osgate_perform_disc_seq 224
#define osgate_req_sector 225
#define osgate_define_sector 226
#define osgate_wait_for_sector 227
#define osgate_reset_drive 228

#define osgate_hook_init_file_system 229
#define osgate_register_file_system 230
#define osgate_define_media_check 231
#define osgate_demand_load_file_system 232
#define osgate_is_file_system_available 233
#define osgate_install_file_system 234
#define osgate_format_file_system 235
#define osgate_start_file_system 236
#define osgate_stop_file_system 237
#define osgate_get_file_list_entry 238
#define osgate_free_file_list_entry 239
#define osgate_cache_dir 240
#define osgate_insert_dir_entry 241
#define osgate_insert_file_entry 242
#define osgate_hook_file_system_started 243

#define osgate_get_file_info 247
#define osgate_dupl_file_info 248

#define osgate_get_utf8_bitmap 250
#define osgate_register_video_mode 251

#define osgate_init_mouse 252
#define osgate_update_mouse 253

#define osgate_init_video_bitmap 254

#define osgate_allocate_small_kernel_mem 255

#define osgate_hide_sprite_line 256
#define osgate_show_sprite_line 257

#define osgate_set_keyboard_state 264

#define osgate_add_wait 266
#define osgate_signal_wait 267

#define osgate_invert_mouse 268

#define osgate_load_dos_exe 269

#define osgate_get_exception_stack16 270
#define osgate_get_exception_stack32 271

#define osgate_lock_sys_env 272
#define osgate_unlock_sys_env 273

#define osgate_lock_proc_env 274
#define osgate_unlock_proc_env 275

#define osgate_erase_sectors 276
#define osgate_flush_sector 277

#define osgate_lock_disc_request 278
#define osgate_modify_disc_request 279
#define osgate_unlock_disc_request 280

#define osgate_get_net_buffer 281
#define osgate_get_ppp_buffer 282

#define osgate_allocate_multiple_physical 283

#define osgate_add_net_source_address 286

#define osgate_get_broadcast_buffer 287
#define osgate_send_broadcast 288

#define osgate_create_broadcast_ip 289
#define osgate_send_broadcast_ip 290

#define osgate_net_broadcast 291
#define osgate_add_dhcp_option 292

#define osgate_define_protocol_addr 293

#define osgate_register_swap_proc 294

#define osgate_setup_irq_detect 296
#define osgate_poll_irq_detect 297

#define osgate_find_pci_cap 298

#define osgate_wait_for_signal_timeout 299

#define osgate_notify_ethernet_packet 300

#define osgate_init_usb_device 301
#define osgate_notify_usb_attach 302

#define osgate_notify_usb_detach 304

#define osgate_add_com_port 305

#define osgate_hook_usb_attach 306
#define osgate_hook_usb_detach 307

#define osgate_set_mouse 308
#define osgate_set_mouse_limit 309

#define osgate_get_net_driver 310

#define osgate_create_usb_req 311
#define osgate_add_write_usb_control_req 312
#define osgate_add_write_usb_data_req 313
#define osgate_add_read_usb_data_req 314
#define osgate_add_usb_status_in_req 315
#define osgate_add_usb_status_out_req 316
#define osgate_start_usb_req 317
#define osgate_is_usb_req_started 318
#define osgate_is_usb_req_ready 319
#define osgate_get_usb_req_data 320
#define osgate_usb_req_done 321
#define osgate_close_usb_req 322

#define osgate_notify_ir_data 323

#define osgate_read_codec 324
#define osgate_write_codec 325

#define osgate_get_audio_dac_rate 326
#define osgate_set_audio_dac_rate 327
#define osgate_get_audio_adc_rate 328
#define osgate_set_audio_adc_rate 329

#define osgate_open_audio_out 330
#define osgate_close_audio_out 331
#define osgate_send_audio_out 332

#define osgate_get_audio_out_buf 333
#define osgate_post_audio_out_buf 334
#define osgate_is_audio_out_completed 335

#define osgate_set_module 336
#define osgate_create_module 337
#define osgate_free_module 338
#define osgate_reset_module 339
#define osgate_deref_module_handle 340
#define osgate_alias_module_handle 341

#define osgate_thread_to_sel 342

#define osgate_stop_usb_req 344

#define osgate_get_usb_info 345

#define osgate_reserve_com_line 346

#define osgate_device_set_dtr 347
#define osgate_device_reset_dtr 348

#define osgate_wait_for_line_state 349
#define osgate_get_line_state 350

#define osgate_create_proc_handle 351
#define osgate_deref_proc_handle 352

#define osgate_notify_time_drift 353
#define osgate_set_options 354

#define osgate_clone_handle_mem 355
#define osgate_clone_app 356

#define osgate_get_apic_id 357

#define osgate_debug_exception 360

#define osgate_create_core 362
#define osgate_get_core_num 363

#define osgate_locked_debug_exception 364

#define osgate_lock_task 365
#define osgate_unlock_task 366

#define osgate_get_core 368
#define osgate_get_core_count 369
#define osgate_run_ap_core 370

#define osgate_preempt_timer_expired 371
#define osgate_timer_expired 372

#define osgate_reload_sys_timer 373
#define osgate_start_sys_timer 374

#define osgate_is_net_address_valid 375

#define osgate_get_acpi_table 376

#define osgate_allocate_debug_local_linear 377

#define osgate_process_key_scan 378

#define osgate_get_debug_thread_sel 379

#define osgate_find_pci_class_all 380

#define osgate_show_proc_debug 381

#define osgate_get_pci_irq 382

#define osgate_preempt_expired 383

#define osgate_send_eoi 384
#define osgate_send_int 385
#define osgate_send_nmi 386

#define osgate_add_printer 387

#define osgate_crash_gate 388
#define osgate_crash_tss 389
#define osgate_crash_fault 390

#define osgate_start_crash_core 391

#define osgate_read_pnp_byte 393
#define osgate_write_pnp_byte 394

#define osgate_add_sys_env_var 395
#define osgate_delete_sys_env_var 396
#define osgate_find_sys_env_var 397

#define osgate_start_preempt_timer 398
#define osgate_reload_preempt_timer 399

#define osgate_start_sys_preempt_timer 400
#define osgate_reload_sys_preempt_timer 401

#define osgate_flush_tlb 402

#define osgate_is_valid_usb_pipe_sel 403

#define osgate_hook_net_link_up 404
#define osgate_req_arp 405

#define osgate_begin_disc_handler 406
#define osgate_end_disc_handler 407

#define osgate_hook_init_pci 408

#define osgate_get_msi_param 409

#define osgate_allocate_ints 410
#define osgate_free_int 411

#define osgate_get_acpi_pci_device_name 412
#define osgate_get_acpi_pci_device_info 413
#define osgate_get_acpi_pci_device_irq 414

#define osgate_start_core 415
#define osgate_shutdown_core 416

#define osgate_update_pstate 417

#define osgate_enter_c3 418

#define osgate_get_pci_msi 419
#define osgate_setup_pci_msi 420
#define osgate_request_msi_handler 421
#define osgate_request_irq_handler 422
#define osgate_request_pci_irq_handler 423

#define osgate_register_syscall 424
#define osgate_register_bimodal_syscall 425

#define osgate_syscall_patch 426
#define osgate_start_syscall 427
#define osgate_setup_sysleave 428

#define osgate_start_dev32 429

#define osgate_get_ioapic_state 430

#define osgate_lock_file 431
#define osgate_unlock_file 432




#define OsGate_register_osgate 0x3E 0x67 0x9a 0 0 0 0 2 0
#define OsGate_is_valid_osgate 0x3E 0x67 0x9a 1 0 0 0 2 0
#define OsGate_register_usergate 0x3E 0x67 0x9a 2 0 0 0 2 0
#define OsGate_register_bimodal_usergate 0x3E 0x67 0x9a 3 0 0 0 2 0
#define OsGate_register_usergate16 0x3E 0x67 0x9a 4 0 0 0 2 0
#define OsGate_register_usergate32 0x3E 0x67 0x9a 5 0 0 0 2 0
#define OsGate_create_alias_sel16 0x3E 0x67 0x9a 7 0 0 0 2 0
#define OsGate_create_core_gdt 0x3E 0x67 0x9a 8 0 0 0 2 0
#define OsGate_emulate_opcode 0x3E 0x67 0x9a 9 0 0 0 2 0

#define OsGate_allocate_gdt 0x3E 0x67 0x9a 10 0 0 0 2 0
#define OsGate_free_gdt 0x3E 0x67 0x9a 11 0 0 0 2 0
#define OsGate_allocate_ldt 0x3E 0x67 0x9a 12 0 0 0 2 0
#define OsGate_free_ldt 0x3E 0x67 0x9a 13 0 0 0 2 0
#define OsGate_allocate_multiple_ldt 0x3E 0x67 0x9a 14 0 0 0 2 0
#define OsGate_get_selector_base_size 0x3E 0x67 0x9a 15 0 0 0 2 0
#define OsGate_create_data_sel16 0x3E 0x67 0x9a 16 0 0 0 2 0
#define OsGate_create_data_sel32 0x3E 0x67 0x9a 17 0 0 0 2 0
#define OsGate_create_code_sel16 0x3E 0x67 0x9a 18 0 0 0 2 0
#define OsGate_create_code_sel32 0x3E 0x67 0x9a 19 0 0 0 2 0
#define OsGate_create_conform_sel16 0x3E 0x67 0x9a 20 0 0 0 2 0
#define OsGate_create_conform_sel32 0x3E 0x67 0x9a 21 0 0 0 2 0
#define OsGate_create_ldt_sel 0x3E 0x67 0x9a 22 0 0 0 2 0
#define OsGate_create_tss_sel 0x3E 0x67 0x9a 23 0 0 0 2 0
#define OsGate_create_call_gate_sel16 0x3E 0x67 0x9a 24 0 0 0 2 0
#define OsGate_create_call_gate_sel32 0x3E 0x67 0x9a 25 0 0 0 2 0
#define OsGate_create_task_gate_sel 0x3E 0x67 0x9a 26 0 0 0 2 0
#define OsGate_selector_to_segment 0x3E 0x67 0x9a 29 0 0 0 2 0
#define OsGate_segment_to_selector 0x3E 0x67 0x9a 30 0 0 0 2 0
#define OsGate_free_selector 0x3E 0x67 0x9a 31 0 0 0 2 0
#define OsGate_segment_not_present 0x3E 0x67 0x9a 32 0 0 0 2 0

#define OsGate_get_physical_page 0x3E 0x67 0x9a 33 0 0 0 2 0
#define OsGate_set_physical_page 0x3E 0x67 0x9a 34 0 0 0 2 0
#define OsGate_get_thread_physical_page 0x3E 0x67 0x9a 35 0 0 0 2 0
#define OsGate_set_thread_physical_page 0x3E 0x67 0x9a 36 0 0 0 2 0
#define OsGate_read_thread_selector 0x3E 0x67 0x9a 37 0 0 0 2 0
#define OsGate_write_thread_selector 0x3E 0x67 0x9a 38 0 0 0 2 0
#define OsGate_read_thread_segment 0x3E 0x67 0x9a 39 0 0 0 2 0
#define OsGate_write_thread_segment 0x3E 0x67 0x9a 40 0 0 0 2 0
#define OsGate_install_device_file 0x3E 0x67 0x9a 41 0 0 0 2 0
#define OsGate_check_device_file 0x3E 0x67 0x9a 42 0 0 0 2 0
#define OsGate_set_page_emulate 0x3E 0x67 0x9a 43 0 0 0 2 0
#define OsGate_hook_page 0x3E 0x67 0x9a 45 0 0 0 2 0
#define OsGate_unhook_page 0x3E 0x67 0x9a 46 0 0 0 2 0

#define OsGate_allocate_global_mem 0x3E 0x67 0x9a 47 0 0 0 2 0
#define OsGate_allocate_small_global_mem 0x3E 0x67 0x9a 48 0 0 0 2 0
#define OsGate_allocate_small_mem 0x3E 0x67 0x9a 49 0 0 0 2 0
#define OsGate_allocate_big_mem 0x3E 0x67 0x9a 50 0 0 0 2 0
#define OsGate_allocate_big_linear 0x3E 0x67 0x9a 51 0 0 0 2 0
#define OsGate_allocate_small_linear 0x3E 0x67 0x9a 52 0 0 0 2 0
#define OsGate_allocate_local_linear 0x3E 0x67 0x9a 53 0 0 0 2 0
#define OsGate_reserve_local_linear 0x3E 0x67 0x9a 54 0 0 0 2 0
#define OsGate_allocate_vm_linear 0x3E 0x67 0x9a 55 0 0 0 2 0
#define OsGate_free_linear 0x3E 0x67 0x9a 56 0 0 0 2 0
#define OsGate_resize_linear 0x3E 0x67 0x9a 57 0 0 0 2 0
#define OsGate_used_big_linear 0x3E 0x67 0x9a 60 0 0 0 2 0
#define OsGate_used_small_linear 0x3E 0x67 0x9a 61 0 0 0 2 0
#define OsGate_used_local_linear_thread 0x3E 0x67 0x9a 62 0 0 0 2 0
#define OsGate_allocate_fixed_process_mem 0x3E 0x67 0x9a 64 0 0 0 2 0
#define OsGate_allocate_fixed_system_mem 0x3E 0x67 0x9a 65 0 0 0 2 0
#define OsGate_allocate_process_linear 0x3E 0x67 0x9a 67 0 0 0 2 0
#define OsGate_allocate_system_linear 0x3E 0x67 0x9a 68 0 0 0 2 0
#define OsGate_allocate_fixed_vm_linear 0x3E 0x67 0x9a 69 0 0 0 2 0

#define OsGate_allocate_physical 0x3E 0x67 0x9a 70 0 0 0 2 0
#define OsGate_allocate_dma_physical 0x3E 0x67 0x9a 71 0 0 0 2 0
#define OsGate_free_physical 0x3E 0x67 0x9a 72 0 0 0 2 0

#define OsGate_create_process 0x3E 0x67 0x9a 74 0 0 0 2 0

#define OsGate_init_trap_gates 0x3E 0x67 0x9a 75 0 0 0 2 0
#define OsGate_init_tss_gates 0x3E 0x67 0x9a 76 0 0 0 2 0

#define OsGate_setup_int_gate 0x3E 0x67 0x9a 77 0 0 0 2 0
#define OsGate_setup_trap_gate 0x3E 0x67 0x9a 78 0 0 0 2 0

#define OsGate_wake_thread 0x3E 0x67 0x9a 79 0 0 0 2 0
#define OsGate_sleep_thread 0x3E 0x67 0x9a 80 0 0 0 2 0
#define OsGate_irq_schedule 0x3E 0x67 0x9a 81 0 0 0 2 0
#define OsGate_clear_signal 0x3E 0x67 0x9a 83 0 0 0 2 0
#define OsGate_signal 0x3E 0x67 0x9a 84 0 0 0 2 0
#define OsGate_wait_for_signal 0x3E 0x67 0x9a 85 0 0 0 2 0
#define OsGate_enter_section 0x3E 0x67 0x9a 86 0 0 0 2 0
#define OsGate_leave_section 0x3E 0x67 0x9a 87 0 0 0 2 0
#define OsGate_cond_enter_section 0x3E 0x67 0x9a 88 0 0 0 2 0
#define OsGate_start_timer 0x3E 0x67 0x9a 89 0 0 0 2 0
#define OsGate_stop_timer 0x3E 0x67 0x9a 90 0 0 0 2 0
#define OsGate_debug_exc_break 0x3E 0x67 0x9a 92 0 0 0 2 0
#define OsGate_hook_create_thread 0x3E 0x67 0x9a 93 0 0 0 2 0
#define OsGate_hook_create_process 0x3E 0x67 0x9a 94 0 0 0 2 0
#define OsGate_hook_terminate_thread 0x3E 0x67 0x9a 95 0 0 0 2 0
#define OsGate_hook_terminate_process 0x3E 0x67 0x9a 96 0 0 0 2 0
#define OsGate_hook_init_tasking 0x3E 0x67 0x9a 97 0 0 0 2 0
#define OsGate_hook_state 0x3E 0x67 0x9a 98 0 0 0 2 0

#define OsGate_sim_sti 0x3E 0x67 0x9a 99 0 0 0 2 0
#define OsGate_sim_cli 0x3E 0x67 0x9a 100 0 0 0 2 0
#define OsGate_sim_set_flags 0x3E 0x67 0x9a 101 0 0 0 2 0
#define OsGate_sim_get_flags 0x3E 0x67 0x9a 102 0 0 0 2 0
#define OsGate_save_context 0x3E 0x67 0x9a 103 0 0 0 2 0
#define OsGate_restore_context 0x3E 0x67 0x9a 104 0 0 0 2 0
#define OsGate_call_vm 0x3E 0x67 0x9a 105 0 0 0 2 0
#define OsGate_call_pm16 0x3E 0x67 0x9a 106 0 0 0 2 0
#define OsGate_call_pm32 0x3E 0x67 0x9a 107 0 0 0 2 0
#define OsGate_hook_vm_int 0x3E 0x67 0x9a 108 0 0 0 2 0
#define OsGate_hook_pm16_int 0x3E 0x67 0x9a 109 0 0 0 2 0
#define OsGate_hook_pm32_int 0x3E 0x67 0x9a 110 0 0 0 2 0
#define OsGate_hook_exception 0x3E 0x67 0x9a 111 0 0 0 2 0
#define OsGate_hook_get_vm_int 0x3E 0x67 0x9a 112 0 0 0 2 0
#define OsGate_hook_set_vm_int 0x3E 0x67 0x9a 113 0 0 0 2 0
#define OsGate_hook_get_pm16_int 0x3E 0x67 0x9a 114 0 0 0 2 0
#define OsGate_hook_set_pm16_int 0x3E 0x67 0x9a 115 0 0 0 2 0
#define OsGate_hook_get_pm32_int 0x3E 0x67 0x9a 116 0 0 0 2 0
#define OsGate_hook_set_pm32_int 0x3E 0x67 0x9a 117 0 0 0 2 0
#define OsGate_reflect_exception 0x3E 0x67 0x9a 118 0 0 0 2 0
#define OsGate_reflect_pm_to_vm 0x3E 0x67 0x9a 119 0 0 0 2 0
#define OsGate_v86_bios_int 0x3E 0x67 0x9a 120 0 0 0 2 0
#define OsGate_set_bitness 0x3E 0x67 0x9a 121 0 0 0 2 0

#define OsGate_hook_in 0x3E 0x67 0x9a 122 0 0 0 2 0
#define OsGate_hook_out 0x3E 0x67 0x9a 123 0 0 0 2 0

#define OsGate_disable_all_irq 0x3E 0x67 0x9a 126 0 0 0 2 0

#define OsGate_set_system_time 0x3E 0x67 0x9a 128 0 0 0 2 0

#define OsGate_register_handle 0x3E 0x67 0x9a 129 0 0 0 2 0
#define OsGate_allocate_handle 0x3E 0x67 0x9a 130 0 0 0 2 0
#define OsGate_free_handle 0x3E 0x67 0x9a 131 0 0 0 2 0
#define OsGate_deref_handle 0x3E 0x67 0x9a 132 0 0 0 2 0

#define OsGate_register_net_class 0x3E 0x67 0x9a 133 0 0 0 2 0
#define OsGate_register_net_protocol 0x3E 0x67 0x9a 134 0 0 0 2 0
#define OsGate_register_net_driver 0x3E 0x67 0x9a 135 0 0 0 2 0
#define OsGate_register_ppp_driver 0x3E 0x67 0x9a 136 0 0 0 2 0
#define OsGate_send_net 0x3E 0x67 0x9a 137 0 0 0 2 0
#define OsGate_send_ppp 0x3E 0x67 0x9a 138 0 0 0 2 0
#define OsGate_net_received 0x3E 0x67 0x9a 139 0 0 0 2 0
#define OsGate_hook_ip 0x3E 0x67 0x9a 140 0 0 0 2 0
#define OsGate_create_ip_header 0x3E 0x67 0x9a 141 0 0 0 2 0
#define OsGate_send_ip_data 0x3E 0x67 0x9a 142 0 0 0 2 0
#define OsGate_hook_ip_cache 0x3E 0x67 0x9a 143 0 0 0 2 0
#define OsGate_allocate_ip_cache_mem 0x3E 0x67 0x9a 144 0 0 0 2 0
#define OsGate_lookup_ip_cache 0x3E 0x67 0x9a 145 0 0 0 2 0
#define OsGate_get_host_timeout 0x3E 0x67 0x9a 146 0 0 0 2 0
#define OsGate_update_round_trip_time 0x3E 0x67 0x9a 147 0 0 0 2 0
#define OsGate_get_ppp_ip 0x3E 0x67 0x9a 148 0 0 0 2 0
#define OsGate_open_ppp 0x3E 0x67 0x9a 149 0 0 0 2 0
#define OsGate_close_ppp 0x3E 0x67 0x9a 150 0 0 0 2 0
#define OsGate_query_udp 0x3E 0x67 0x9a 151 0 0 0 2 0
#define OsGate_listen_udp_port 0x3E 0x67 0x9a 152 0 0 0 2 0

#define OsGate_allocate_dos_linear 0x3E 0x67 0x9a 153 0 0 0 2 0
#define OsGate_free_dos_linear 0x3E 0x67 0x9a 154 0 0 0 2 0
#define OsGate_resize_dos_linear 0x3E 0x67 0x9a 155 0 0 0 2 0
#define OsGate_available_dos_linear 0x3E 0x67 0x9a 156 0 0 0 2 0
#define OsGate_query_xms 0x3E 0x67 0x9a 157 0 0 0 2 0
#define OsGate_query_dpmi 0x3E 0x67 0x9a 158 0 0 0 2 0
#define OsGate_query_dpmi16 0x3E 0x67 0x9a 159 0 0 0 2 0
#define OsGate_get_bios_data 0x3E 0x67 0x9a 160 0 0 0 2 0
#define OsGate_set_bios_data 0x3E 0x67 0x9a 161 0 0 0 2 0
#define OsGate_hook_get_bios_data 0x3E 0x67 0x9a 162 0 0 0 2 0
#define OsGate_hook_set_bios_data 0x3E 0x67 0x9a 163 0 0 0 2 0
#define OsGate_hook_load_dos_exe 0x3E 0x67 0x9a 164 0 0 0 2 0
#define OsGate_read_keyboard_serial 0x3E 0x67 0x9a 165 0 0 0 2 0
#define OsGate_poll_keyboard_serial 0x3E 0x67 0x9a 166 0 0 0 2 0
#define OsGate_write_dos_string 0x3E 0x67 0x9a 167 0 0 0 2 0
#define OsGate_get_psp 0x3E 0x67 0x9a 168 0 0 0 2 0
#define OsGate_set_psp 0x3E 0x67 0x9a 169 0 0 0 2 0
#define OsGate_enter_dpmi 0x3E 0x67 0x9a 170 0 0 0 2 0
#define OsGate_enter_dos16 0x3E 0x67 0x9a 171 0 0 0 2 0
#define OsGate_enter_dos32 0x3E 0x67 0x9a 172 0 0 0 2 0
#define OsGate_register_dos_device 0x3E 0x67 0x9a 173 0 0 0 2 0
#define OsGate_check_dos_device 0x3E 0x67 0x9a 174 0 0 0 2 0

#define OsGate_hook_open_app 0x3E 0x67 0x9a 177 0 0 0 2 0
#define OsGate_hook_close_app 0x3E 0x67 0x9a 178 0 0 0 2 0
#define OsGate_hook_load_exe 0x3E 0x67 0x9a 179 0 0 0 2 0
#define OsGate_open_app 0x3E 0x67 0x9a 180 0 0 0 2 0
#define OsGate_close_app 0x3E 0x67 0x9a 181 0 0 0 2 0

#define OsGate_hook_enable_focus 0x3E 0x67 0x9a 182 0 0 0 2 0
#define OsGate_hook_lost_focus 0x3E 0x67 0x9a 183 0 0 0 2 0
#define OsGate_hook_got_focus 0x3E 0x67 0x9a 184 0 0 0 2 0
#define OsGate_get_focus_thread 0x3E 0x67 0x9a 185 0 0 0 2 0
#define OsGate_get_thread_focus_key 0x3E 0x67 0x9a 186 0 0 0 2 0
#define OsGate_allocate_focus_linear 0x3E 0x67 0x9a 187 0 0 0 2 0
#define OsGate_allocate_fixed_focus_mem 0x3E 0x67 0x9a 188 0 0 0 2 0

#define OsGate_bios_pci_int 0x3E 0x67 0x9a 189 0 0 0 2 0
#define OsGate_read_pci_byte 0x3E 0x67 0x9a 190 0 0 0 2 0
#define OsGate_read_pci_word 0x3E 0x67 0x9a 191 0 0 0 2 0
#define OsGate_read_pci_dword 0x3E 0x67 0x9a 192 0 0 0 2 0
#define OsGate_write_pci_byte 0x3E 0x67 0x9a 193 0 0 0 2 0
#define OsGate_write_pci_word 0x3E 0x67 0x9a 194 0 0 0 2 0
#define OsGate_write_pci_dword 0x3E 0x67 0x9a 195 0 0 0 2 0
#define OsGate_find_pci_class 0x3E 0x67 0x9a 196 0 0 0 2 0
#define OsGate_find_pci_device 0x3E 0x67 0x9a 197 0 0 0 2 0

#define OsGate_hook_init_disc 0x3E 0x67 0x9a 198 0 0 0 2 0
#define OsGate_install_disc 0x3E 0x67 0x9a 199 0 0 0 2 0
#define OsGate_register_disc_change 0x3E 0x67 0x9a 200 0 0 0 2 0
#define OsGate_start_disc 0x3E 0x67 0x9a 201 0 0 0 2 0
#define OsGate_stop_disc 0x3E 0x67 0x9a 202 0 0 0 2 0
#define OsGate_set_disc_param 0x3E 0x67 0x9a 203 0 0 0 2 0
#define OsGate_wait_for_disc_request 0x3E 0x67 0x9a 204 0 0 0 2 0
#define OsGate_get_disc_request 0x3E 0x67 0x9a 205 0 0 0 2 0
#define OsGate_poll_disc_request 0x3E 0x67 0x9a 206 0 0 0 2 0
#define OsGate_disc_request_completed 0x3E 0x67 0x9a 207 0 0 0 2 0
#define OsGate_new_disc_request 0x3E 0x67 0x9a 208 0 0 0 2 0
#define OsGate_get_disc_request_array 0x3E 0x67 0x9a 209 0 0 0 2 0
#define OsGate_open_drive 0x3E 0x67 0x9a 214 0 0 0 2 0
#define OsGate_close_drive 0x3E 0x67 0x9a 215 0 0 0 2 0
#define OsGate_flush_drive 0x3E 0x67 0x9a 216 0 0 0 2 0
#define OsGate_get_drive_param 0x3E 0x67 0x9a 217 0 0 0 2 0
#define OsGate_lock_sector 0x3E 0x67 0x9a 218 0 0 0 2 0
#define OsGate_unlock_sector 0x3E 0x67 0x9a 219 0 0 0 2 0
#define OsGate_modify_sector 0x3E 0x67 0x9a 220 0 0 0 2 0
#define OsGate_new_sector 0x3E 0x67 0x9a 221 0 0 0 2 0
#define OsGate_create_disc_seq 0x3E 0x67 0x9a 222 0 0 0 2 0
#define OsGate_modify_seq_sector 0x3E 0x67 0x9a 223 0 0 0 2 0
#define OsGate_perform_disc_seq 0x3E 0x67 0x9a 224 0 0 0 2 0
#define OsGate_req_sector 0x3E 0x67 0x9a 225 0 0 0 2 0
#define OsGate_define_sector 0x3E 0x67 0x9a 226 0 0 0 2 0
#define OsGate_wait_for_sector 0x3E 0x67 0x9a 227 0 0 0 2 0
#define OsGate_reset_drive 0x3E 0x67 0x9a 228 0 0 0 2 0

#define OsGate_hook_init_file_system 0x3E 0x67 0x9a 229 0 0 0 2 0
#define OsGate_register_file_system 0x3E 0x67 0x9a 230 0 0 0 2 0
#define OsGate_define_media_check 0x3E 0x67 0x9a 231 0 0 0 2 0
#define OsGate_demand_load_file_system 0x3E 0x67 0x9a 232 0 0 0 2 0
#define OsGate_is_file_system_available 0x3E 0x67 0x9a 233 0 0 0 2 0
#define OsGate_install_file_system 0x3E 0x67 0x9a 234 0 0 0 2 0
#define OsGate_format_file_system 0x3E 0x67 0x9a 235 0 0 0 2 0
#define OsGate_start_file_system 0x3E 0x67 0x9a 236 0 0 0 2 0
#define OsGate_stop_file_system 0x3E 0x67 0x9a 237 0 0 0 2 0
#define OsGate_get_file_list_entry 0x3E 0x67 0x9a 238 0 0 0 2 0
#define OsGate_free_file_list_entry 0x3E 0x67 0x9a 239 0 0 0 2 0
#define OsGate_cache_dir 0x3E 0x67 0x9a 240 0 0 0 2 0
#define OsGate_insert_dir_entry 0x3E 0x67 0x9a 241 0 0 0 2 0
#define OsGate_insert_file_entry 0x3E 0x67 0x9a 242 0 0 0 2 0
#define OsGate_hook_file_system_started 0x3E 0x67 0x9a 243 0 0 0 2 0

#define OsGate_get_file_info 0x3E 0x67 0x9a 247 0 0 0 2 0
#define OsGate_dupl_file_info 0x3E 0x67 0x9a 248 0 0 0 2 0

#define OsGate_get_utf8_bitmap 0x3E 0x67 0x9a 250 0 0 0 2 0
#define OsGate_register_video_mode 0x3E 0x67 0x9a 251 0 0 0 2 0

#define OsGate_init_mouse 0x3E 0x67 0x9a 252 0 0 0 2 0
#define OsGate_update_mouse 0x3E 0x67 0x9a 253 0 0 0 2 0

#define OsGate_init_video_bitmap 0x3E 0x67 0x9a 254 0 0 0 2 0

#define OsGate_allocate_small_kernel_mem 0x3E 0x67 0x9a 255 0 0 0 2 0

#define OsGate_hide_sprite_line 0x3E 0x67 0x9a 0 1 0 0 2 0
#define OsGate_show_sprite_line 0x3E 0x67 0x9a 1 1 0 0 2 0

#define OsGate_set_keyboard_state 0x3E 0x67 0x9a 8 1 0 0 2 0

#define OsGate_add_wait 0x3E 0x67 0x9a 10 1 0 0 2 0
#define OsGate_signal_wait 0x3E 0x67 0x9a 11 1 0 0 2 0

#define OsGate_invert_mouse 0x3E 0x67 0x9a 12 1 0 0 2 0

#define OsGate_load_dos_exe 0x3E 0x67 0x9a 13 1 0 0 2 0

#define OsGate_get_exception_stack16 0x3E 0x67 0x9a 14 1 0 0 2 0
#define OsGate_get_exception_stack32 0x3E 0x67 0x9a 15 1 0 0 2 0

#define OsGate_lock_sys_env 0x3E 0x67 0x9a 16 1 0 0 2 0
#define OsGate_unlock_sys_env 0x3E 0x67 0x9a 17 1 0 0 2 0

#define OsGate_lock_proc_env 0x3E 0x67 0x9a 18 1 0 0 2 0
#define OsGate_unlock_proc_env 0x3E 0x67 0x9a 19 1 0 0 2 0

#define OsGate_erase_sectors 0x3E 0x67 0x9a 20 1 0 0 2 0
#define OsGate_flush_sector 0x3E 0x67 0x9a 21 1 0 0 2 0

#define OsGate_lock_disc_request 0x3E 0x67 0x9a 22 1 0 0 2 0
#define OsGate_modify_disc_request 0x3E 0x67 0x9a 23 1 0 0 2 0
#define OsGate_unlock_disc_request 0x3E 0x67 0x9a 24 1 0 0 2 0

#define OsGate_get_net_buffer 0x3E 0x67 0x9a 25 1 0 0 2 0
#define OsGate_get_ppp_buffer 0x3E 0x67 0x9a 26 1 0 0 2 0

#define OsGate_allocate_multiple_physical 0x3E 0x67 0x9a 27 1 0 0 2 0

#define OsGate_add_net_source_address 0x3E 0x67 0x9a 30 1 0 0 2 0

#define OsGate_get_broadcast_buffer 0x3E 0x67 0x9a 31 1 0 0 2 0
#define OsGate_send_broadcast 0x3E 0x67 0x9a 32 1 0 0 2 0

#define OsGate_create_broadcast_ip 0x3E 0x67 0x9a 33 1 0 0 2 0
#define OsGate_send_broadcast_ip 0x3E 0x67 0x9a 34 1 0 0 2 0

#define OsGate_net_broadcast 0x3E 0x67 0x9a 35 1 0 0 2 0
#define OsGate_add_dhcp_option 0x3E 0x67 0x9a 36 1 0 0 2 0

#define OsGate_define_protocol_addr 0x3E 0x67 0x9a 37 1 0 0 2 0

#define OsGate_register_swap_proc 0x3E 0x67 0x9a 38 1 0 0 2 0

#define OsGate_setup_irq_detect 0x3E 0x67 0x9a 40 1 0 0 2 0
#define OsGate_poll_irq_detect 0x3E 0x67 0x9a 41 1 0 0 2 0

#define OsGate_find_pci_cap 0x3E 0x67 0x9a 42 1 0 0 2 0

#define OsGate_wait_for_signal_timeout 0x3E 0x67 0x9a 43 1 0 0 2 0

#define OsGate_notify_ethernet_packet 0x3E 0x67 0x9a 44 1 0 0 2 0

#define OsGate_init_usb_device 0x3E 0x67 0x9a 45 1 0 0 2 0
#define OsGate_notify_usb_attach 0x3E 0x67 0x9a 46 1 0 0 2 0

#define OsGate_notify_usb_detach 0x3E 0x67 0x9a 48 1 0 0 2 0

#define OsGate_add_com_port 0x3E 0x67 0x9a 49 1 0 0 2 0

#define OsGate_hook_usb_attach 0x3E 0x67 0x9a 50 1 0 0 2 0
#define OsGate_hook_usb_detach 0x3E 0x67 0x9a 51 1 0 0 2 0

#define OsGate_set_mouse 0x3E 0x67 0x9a 52 1 0 0 2 0
#define OsGate_set_mouse_limit 0x3E 0x67 0x9a 53 1 0 0 2 0

#define OsGate_get_net_driver 0x3E 0x67 0x9a 54 1 0 0 2 0

#define OsGate_create_usb_req 0x3E 0x67 0x9a 55 1 0 0 2 0
#define OsGate_add_write_usb_control_req 0x3E 0x67 0x9a 56 1 0 0 2 0
#define OsGate_add_write_usb_data_req 0x3E 0x67 0x9a 57 1 0 0 2 0
#define OsGate_add_read_usb_data_req 0x3E 0x67 0x9a 58 1 0 0 2 0
#define OsGate_add_usb_status_in_req 0x3E 0x67 0x9a 59 1 0 0 2 0
#define OsGate_add_usb_status_out_req 0x3E 0x67 0x9a 60 1 0 0 2 0
#define OsGate_start_usb_req 0x3E 0x67 0x9a 61 1 0 0 2 0
#define OsGate_is_usb_req_started 0x3E 0x67 0x9a 62 1 0 0 2 0
#define OsGate_is_usb_req_ready 0x3E 0x67 0x9a 63 1 0 0 2 0
#define OsGate_get_usb_req_data 0x3E 0x67 0x9a 64 1 0 0 2 0
#define OsGate_usb_req_done 0x3E 0x67 0x9a 65 1 0 0 2 0
#define OsGate_close_usb_req 0x3E 0x67 0x9a 66 1 0 0 2 0

#define OsGate_notify_ir_data 0x3E 0x67 0x9a 67 1 0 0 2 0

#define OsGate_read_codec 0x3E 0x67 0x9a 68 1 0 0 2 0
#define OsGate_write_codec 0x3E 0x67 0x9a 69 1 0 0 2 0

#define OsGate_get_audio_dac_rate 0x3E 0x67 0x9a 70 1 0 0 2 0
#define OsGate_set_audio_dac_rate 0x3E 0x67 0x9a 71 1 0 0 2 0
#define OsGate_get_audio_adc_rate 0x3E 0x67 0x9a 72 1 0 0 2 0
#define OsGate_set_audio_adc_rate 0x3E 0x67 0x9a 73 1 0 0 2 0

#define OsGate_open_audio_out 0x3E 0x67 0x9a 74 1 0 0 2 0
#define OsGate_close_audio_out 0x3E 0x67 0x9a 75 1 0 0 2 0
#define OsGate_send_audio_out 0x3E 0x67 0x9a 76 1 0 0 2 0

#define OsGate_get_audio_out_buf 0x3E 0x67 0x9a 77 1 0 0 2 0
#define OsGate_post_audio_out_buf 0x3E 0x67 0x9a 78 1 0 0 2 0
#define OsGate_is_audio_out_completed 0x3E 0x67 0x9a 79 1 0 0 2 0

#define OsGate_set_module 0x3E 0x67 0x9a 80 1 0 0 2 0
#define OsGate_create_module 0x3E 0x67 0x9a 81 1 0 0 2 0
#define OsGate_free_module 0x3E 0x67 0x9a 82 1 0 0 2 0
#define OsGate_reset_module 0x3E 0x67 0x9a 83 1 0 0 2 0
#define OsGate_deref_module_handle 0x3E 0x67 0x9a 84 1 0 0 2 0
#define OsGate_alias_module_handle 0x3E 0x67 0x9a 85 1 0 0 2 0

#define OsGate_thread_to_sel 0x3E 0x67 0x9a 86 1 0 0 2 0

#define OsGate_stop_usb_req 0x3E 0x67 0x9a 88 1 0 0 2 0

#define OsGate_get_usb_info 0x3E 0x67 0x9a 89 1 0 0 2 0

#define OsGate_reserve_com_line 0x3E 0x67 0x9a 90 1 0 0 2 0

#define OsGate_device_set_dtr 0x3E 0x67 0x9a 91 1 0 0 2 0
#define OsGate_device_reset_dtr 0x3E 0x67 0x9a 92 1 0 0 2 0

#define OsGate_wait_for_line_state 0x3E 0x67 0x9a 93 1 0 0 2 0
#define OsGate_get_line_state 0x3E 0x67 0x9a 94 1 0 0 2 0

#define OsGate_create_proc_handle 0x3E 0x67 0x9a 95 1 0 0 2 0
#define OsGate_deref_proc_handle 0x3E 0x67 0x9a 96 1 0 0 2 0

#define OsGate_notify_time_drift 0x3E 0x67 0x9a 97 1 0 0 2 0
#define OsGate_set_options 0x3E 0x67 0x9a 98 1 0 0 2 0

#define OsGate_clone_handle_mem 0x3E 0x67 0x9a 99 1 0 0 2 0
#define OsGate_clone_app 0x3E 0x67 0x9a 100 1 0 0 2 0

#define OsGate_get_apic_id 0x3E 0x67 0x9a 101 1 0 0 2 0

#define OsGate_debug_exception 0x3E 0x67 0x9a 104 1 0 0 2 0

#define OsGate_create_core 0x3E 0x67 0x9a 106 1 0 0 2 0
#define OsGate_get_core_num 0x3E 0x67 0x9a 107 1 0 0 2 0

#define OsGate_locked_debug_exception 0x3E 0x67 0x9a 108 1 0 0 2 0

#define OsGate_lock_task 0x3E 0x67 0x9a 109 1 0 0 2 0
#define OsGate_unlock_task 0x3E 0x67 0x9a 110 1 0 0 2 0

#define OsGate_get_core 0x3E 0x67 0x9a 112 1 0 0 2 0
#define OsGate_get_core_count 0x3E 0x67 0x9a 113 1 0 0 2 0
#define OsGate_run_ap_core 0x3E 0x67 0x9a 114 1 0 0 2 0

#define OsGate_preempt_timer_expired 0x3E 0x67 0x9a 115 1 0 0 2 0
#define OsGate_timer_expired 0x3E 0x67 0x9a 116 1 0 0 2 0

#define OsGate_reload_sys_timer 0x3E 0x67 0x9a 117 1 0 0 2 0
#define OsGate_start_sys_timer 0x3E 0x67 0x9a 118 1 0 0 2 0

#define OsGate_is_net_address_valid 0x3E 0x67 0x9a 119 1 0 0 2 0

#define OsGate_get_acpi_table 0x3E 0x67 0x9a 120 1 0 0 2 0

#define OsGate_allocate_debug_local_linear 0x3E 0x67 0x9a 121 1 0 0 2 0

#define OsGate_process_key_scan 0x3E 0x67 0x9a 122 1 0 0 2 0

#define OsGate_get_debug_thread_sel 0x3E 0x67 0x9a 123 1 0 0 2 0

#define OsGate_find_pci_class_all 0x3E 0x67 0x9a 124 1 0 0 2 0

#define OsGate_show_proc_debug 0x3E 0x67 0x9a 125 1 0 0 2 0

#define OsGate_get_pci_irq 0x3E 0x67 0x9a 126 1 0 0 2 0

#define OsGate_preempt_expired 0x3E 0x67 0x9a 127 1 0 0 2 0

#define OsGate_send_eoi 0x3E 0x67 0x9a 128 1 0 0 2 0
#define OsGate_send_int 0x3E 0x67 0x9a 129 1 0 0 2 0
#define OsGate_send_nmi 0x3E 0x67 0x9a 130 1 0 0 2 0

#define OsGate_add_printer 0x3E 0x67 0x9a 131 1 0 0 2 0

#define OsGate_crash_gate 0x3E 0x67 0x9a 132 1 0 0 2 0
#define OsGate_crash_tss 0x3E 0x67 0x9a 133 1 0 0 2 0
#define OsGate_crash_fault 0x3E 0x67 0x9a 134 1 0 0 2 0

#define OsGate_start_crash_core 0x3E 0x67 0x9a 135 1 0 0 2 0

#define OsGate_read_pnp_byte 0x3E 0x67 0x9a 137 1 0 0 2 0
#define OsGate_write_pnp_byte 0x3E 0x67 0x9a 138 1 0 0 2 0

#define OsGate_add_sys_env_var 0x3E 0x67 0x9a 139 1 0 0 2 0
#define OsGate_delete_sys_env_var 0x3E 0x67 0x9a 140 1 0 0 2 0
#define OsGate_find_sys_env_var 0x3E 0x67 0x9a 141 1 0 0 2 0

#define OsGate_start_preempt_timer 0x3E 0x67 0x9a 142 1 0 0 2 0
#define OsGate_reload_preempt_timer 0x3E 0x67 0x9a 143 1 0 0 2 0

#define OsGate_start_sys_preempt_timer 0x3E 0x67 0x9a 144 1 0 0 2 0
#define OsGate_reload_sys_preempt_timer 0x3E 0x67 0x9a 145 1 0 0 2 0

#define OsGate_flush_tlb 0x3E 0x67 0x9a 146 1 0 0 2 0

#define OsGate_is_valid_usb_pipe_sel 0x3E 0x67 0x9a 147 1 0 0 2 0

#define OsGate_hook_net_link_up 0x3E 0x67 0x9a 148 1 0 0 2 0
#define OsGate_req_arp 0x3E 0x67 0x9a 149 1 0 0 2 0

#define OsGate_begin_disc_handler 0x3E 0x67 0x9a 150 1 0 0 2 0
#define OsGate_end_disc_handler 0x3E 0x67 0x9a 151 1 0 0 2 0

#define OsGate_hook_init_pci 0x3E 0x67 0x9a 152 1 0 0 2 0

#define OsGate_get_msi_param 0x3E 0x67 0x9a 153 1 0 0 2 0

#define OsGate_allocate_ints 0x3E 0x67 0x9a 154 1 0 0 2 0
#define OsGate_free_int 0x3E 0x67 0x9a 155 1 0 0 2 0

#define OsGate_get_acpi_pci_device_name 0x3E 0x67 0x9a 156 1 0 0 2 0
#define OsGate_get_acpi_pci_device_info 0x3E 0x67 0x9a 157 1 0 0 2 0
#define OsGate_get_acpi_pci_device_irq 0x3E 0x67 0x9a 158 1 0 0 2 0

#define OsGate_start_core 0x3E 0x67 0x9a 159 1 0 0 2 0
#define OsGate_shutdown_core 0x3E 0x67 0x9a 160 1 0 0 2 0

#define OsGate_update_pstate 0x3E 0x67 0x9a 161 1 0 0 2 0

#define OsGate_enter_c3 0x3E 0x67 0x9a 162 1 0 0 2 0

#define OsGate_get_pci_msi 0x3E 0x67 0x9a 163 1 0 0 2 0
#define OsGate_setup_pci_msi 0x3E 0x67 0x9a 164 1 0 0 2 0
#define OsGate_request_msi_handler 0x3E 0x67 0x9a 165 1 0 0 2 0
#define OsGate_request_irq_handler 0x3E 0x67 0x9a 166 1 0 0 2 0
#define OsGate_request_pci_irq_handler 0x3E 0x67 0x9a 167 1 0 0 2 0

#define OsGate_register_syscall 0x3E 0x67 0x9a 168 1 0 0 2 0
#define OsGate_register_bimodal_syscall 0x3E 0x67 0x9a 169 1 0 0 2 0

#define OsGate_syscall_patch 0x3E 0x67 0x9a 170 1 0 0 2 0
#define OsGate_start_syscall 0x3E 0x67 0x9a 171 1 0 0 2 0
#define OsGate_setup_sysleave 0x3E 0x67 0x9a 172 1 0 0 2 0

#define OsGate_start_dev32 0x3E 0x67 0x9a 173 1 0 0 2 0

#define OsGate_get_ioapic_state 0x3E 0x67 0x9a 174 1 0 0 2 0

#define OsGate_lock_file 0x3E 0x67 0x9a 175 1 0 0 2 0
#define OsGate_unlock_file 0x3E 0x67 0x9a 176 1 0 0 2 0

