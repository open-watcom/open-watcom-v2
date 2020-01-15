/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2013 The Open Watcom Contributors. All Rights Reserved.
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
#define osgate_register_long_osgate 6
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
#define osgate_create_down_sel32 27
#define osgate_create_long_code_sel 28
#define osgate_selector_to_segment 29
#define osgate_segment_to_selector 30
#define osgate_free_selector 31
#define osgate_segment_not_present 32

#define osgate_get_page_entry 33
#define osgate_set_page_entry 34
#define osgate_get_thread_page_entry 35
#define osgate_set_thread_page_entry 36
#define osgate_read_thread_selector 37
#define osgate_write_thread_selector 38
#define osgate_read_thread_segment 39
#define osgate_write_thread_segment 40
#define osgate_set_page_emulate 43
#define osgate_invalid_os 44
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

#define osgate_allocate_physical32 70
#define osgate_allocate_physical64 71
#define osgate_allocate_dma_physical 72
#define osgate_free_physical 73

#define osgate_create_process 74

#define osgate_init_trap_gates 75
#define osgate_start_tasking 76

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
#define osgate_hook_init_tasking 97
#define osgate_hook_state 98

#define osgate_sim_sti 99
#define osgate_sim_cli 100
#define osgate_sim_set_flags 101
#define osgate_sim_get_flags 102
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

#define osgate_get_focus_thread 185

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
#define osgate_set_disc_use32 244

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

#define osgate_update_rtc 258

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

#define osgate_allocate_multiple_physical32 283
#define osgate_allocate_multiple_physical64 284

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

#define osgate_module_id_to_sel 340
#define osgate_alias_module_handle 341

#define osgate_thread_to_sel 342

#define osgate_stop_usb_req 344

#define osgate_reserve_com_line 346

#define osgate_device_set_dtr 347
#define osgate_device_reset_dtr 348

#define osgate_wait_for_line_state 349
#define osgate_get_line_state 350

#define osgate_notify_time_drift 353

#define osgate_get_apic_id 357

#define osgate_debug_exception 360

#define osgate_create_core 362
#define osgate_get_core_num 363

#define osgate_locked_debug_exception 364

#define osgate_lock_task 365
#define osgate_unlock_task 366
#define osgate_try_lock_task 367

#define osgate_get_core 368
#define osgate_get_core_count 369
#define osgate_run_ap_core 370

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

#define osgate_read_pnp_byte 393
#define osgate_write_pnp_byte 394

#define osgate_add_sys_env_var 395
#define osgate_delete_sys_env_var 396
#define osgate_find_sys_env_var 397

#define osgate_start_preempt_timer 398
#define osgate_reload_preempt_timer 399

#define osgate_start_sys_preempt_timer 400
#define osgate_reload_sys_preempt_timer 401

#define osgate_is_valid_usb_pipe_sel 403

#define osgate_hook_net_link_up 404
#define osgate_req_arp 405

#define osgate_begin_disc_handler 406
#define osgate_end_disc_handler 407

#define osgate_hook_init_pci 408

#define osgate_register_msi 409

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
#define osgate_notify_irq 423

#define osgate_register_syscall 424
#define osgate_register_bimodal_syscall 425

#define osgate_start_syscall 427

#define osgate_start_dev32 429

#define osgate_get_ioapic_state 430

#define osgate_lock_file 431
#define osgate_unlock_file 432

#define osgate_app_patch 435

#define osgate_copy_page_entries 436
#define osgate_move_page_entries 437

#define osgate_has_page_entry 438
#define osgate_free_page_entries 439
#define osgate_free_global_page_entries 439
#define osgate_allocate_page_entries 440
#define osgate_reserve_page_entries 441

#define osgate_add_carddev 442

#define osgate_get_thread_page_dir 443
#define osgate_set_page_dir 444
#define osgate_set_sys_page_dir 445
#define osgate_get_page_dir_attrib 446
#define osgate_get_page_dir 447
#define osgate_set_sys_page_entry 448
#define osgate_copy_sys_page_entries 449
#define osgate_get_sys_page_entry 450
#define osgate_get_sys_page_dir 451

#define osgate_notify_init_process 452
#define osgate_notify_create_process 453

#define osgate_get_scheduler_lock_counter 459
#define osgate_flush_tlb 460
#define osgate_setup_smp_patch 461
#define osgate_notify_thread_suspend 462

#define osgate_is_long_code_sel 463
#define osgate_setup_long_int_gate 464
#define osgate_setup_long_trap_gate 465

#define osgate_enter_long_int 466
#define osgate_leave_long_int 467

#define osgate_create_long_irq 468
#define osgate_add_long_irq 469
#define osgate_create_long_msi 470
#define osgate_add_long_msi 471

#define osgate_setup_long_spurious_int 472
#define osgate_setup_long_timer_int 473
#define osgate_setup_long_preempt_int 474
#define osgate_setup_long_hpet_int 477

#define osgate_setup_long_crash_gate 479

#define osgate_setup_long_nmi_core_dump 483

#define osgate_debug_block 484

#define osgate_switch_to_long_mode 485
#define osgate_switch_to_protected_mode 486

#define osgate_notify_create_long_process 487

#define osgate_load_long_regs 488
#define osgate_is_long_thread 489
#define osgate_long_mode_reset 490

#define osgate_create_sys_page_dir 491

#define osgate_init_long_exe 492
#define osgate_start_long_exe 493

#define osgate_read_thread64 494
#define osgate_write_thread64 495

#define osgate_allocate_long_buf 502
#define osgate_free_long_buf 503

#define osgate_handle_long_code_fault 504
#define osgate_load_long_breaks 505

#define osgate_set_futex_id 506
#define osgate_set_long_tls_linear 507

#define osgate_broadcast_driver_udp 509

#define osgate_get_net_driver_buffer 510
#define osgate_send_net_driver 511

#define osgate_create_driver_ip 512
#define osgate_send_driver_ip 513

#define osgate_send_driver_udp 514

#define osgate_create_long_thread_info 515

#define osgate_has_long_mode 516

#define osgate_is_usb_hub_port_connected 517

#define osgate_wait_for_ehci 518
#define osgate_wait_for_ohci 519
#define osgate_wait_for_uhci 520

#define osgate_register_hid_input 522
#define osgate_get_signed_hid_input 523
#define osgate_get_unsigned_hid_input 524
#define osgate_set_hid_idle 525
#define osgate_find_hid_output_report 526
#define osgate_set_hid_output 527
#define osgate_update_hid_output 528
#define osgate_get_hid_log_min 529
#define osgate_get_hid_log_max 530
#define osgate_cond_begin_disc_handler 531
#define osgate_stop_disc_request 532

#define osgate_lock_usb 533
#define osgate_unlock_usb 534

#define osgate_restart_can_modules 535

#define osgate_send_can_bus_msg 536
#define osgate_hook_can_gen_bus_msg 537
#define osgate_create_can_id_hook 538
#define osgate_delete_can_id_hook 539

#define osgate_reset_usb_hub_port 540
#define osgate_add_lon_module 541

#define osgate_get_acpi_pnp_device_mem 542

#define osgate_force_level_irq 544

#define osgate_has_can_send_buf 545

#define osgate_fpu_exception 546

#define osgate_reset_can_buffers 547
#define osgate_send_can_bus_block 548

#define osgate_get_disc_vendor_info_buf 549

#define osgate_get_pci_msix 550
#define osgate_enable_pci_msix 551
#define osgate_setup_pci_msix_entry 552

#define osgate_update_tcp_mtu 553

#define osgate_notify_lon_data 554

#define osgate_set_thread_core 555

#define osgate_create_thread_id 556

#define osgate_init_freq 557
#define osgate_update_freq 558

#define osgate_add_thread_int 559

#define osgate_do_flush_tlb 561

#define osgate_get_sel_bitness 562

#define osgate_setup_long_schedule_int 563

#define osgate_long_timer_handler 564
#define osgate_long_hpet_handler 565

#define osgate_use_own_preempt_timer 566

#define osgate_fault_reset 567

#define osgate_disc_request_retry 568

#define osgate_get_thread_selector_page 569

#define osgate_notify_delete_process 573

#define osgate_add_video_mode 574
#define osgate_begin_get_video_modes 575
#define osgate_end_get_video_modes 576
#define osgate_switch_video_mode 577

#define osgate_start_smp_core_dump 578
#define osgate_start_core_dump 579
#define osgate_notify_core_dump 580
#define osgate_setup_nmi_core_dump 581

#define osgate_allocate_c_handle 584
#define osgate_open_c_file 585
#define osgate_close_c_file 586
#define osgate_ref_c_handle 587
#define osgate_read_c_file 588
#define osgate_write_c_file 589
#define osgate_get_c_file_size 590
#define osgate_set_c_file_size 591
#define osgate_get_c_file_time 592
#define osgate_set_c_file_time 593
#define osgate_read_c_console 594
#define osgate_write_c_console 595
#define osgate_open_kernel_file 596

#define osgate_fork_process 597

#define osgate_reset_process 601
#define osgate_exec_app 602

#define osgate_create_fork 610

#define osgate_dupl_c_file_to_file 611
#define osgate_c_handle_to_file_sel 612

#define osgate_add_serio_device 613

#define osgate_notify_can_offline 615

#define osgate_req_usb_data_no_copy 616
#define osgate_write_usb_data_no_copy 617

#define osgate_notify_can_modules_up 619
#define osgate_notify_can_online 620

#define osgate_lock_log 621
#define osgate_unlock_log 622
#define osgate_log_thread 623
#define osgate_log_memory 624
#define osgate_log_text 625
#define osgate_log_hex_byte 626
#define osgate_log_hex_word 627
#define osgate_log_hex_dword 628
#define osgate_log_small_mem 629
#define osgate_log_big_mem 630

#define osgate_create_pid 631

#define osgate_start_programs 632
#define osgate_register_loader 633

#define osgate_find_module_by_address 634
#define osgate_find_module_by_name 635
#define osgate_get_module_by_index 636

#define osgate_create_app_thread 637
#define osgate_terminate_app_thread 638

#define osgate_program_created 639
#define osgate_program_terminated 640
#define osgate_get_program_sel 641
#define osgate_get_program_id 642
#define osgate_module_loaded 643
#define osgate_module_unloaded 644
#define osgate_get_module_id 645

#define osgate_get_exe_start32 646

#define osgate_process_created 647
#define osgate_process_terminated 648
#define osgate_process_id_to_sel 649
#define osgate_get_process_id 650

#define osgate_create_ldt 651
#define osgate_destroy_ldt 652

#define osgate_create_c_handle 653
#define osgate_clone_c_handle 654
#define osgate_delete_c_handle 655

#define osgate_close_console 656
#define osgate_set_focus_console 657
#define osgate_get_focus_console 658
#define osgate_create_console 659

#define osgate_create_cur_dir 660
#define osgate_clone_cur_dir 661
#define osgate_delete_cur_dir 662

#define osgate_create_handle_data 663
#define osgate_destroy_handle_data 664

#define osgate_init_process_mem 665

#define osgate_start_fork 666
#define osgate_detach_fork 667
#define osgate_cleanup_fork 668
#define osgate_removed_process 669
#define osgate_delete_fork 670

#define osgate_create_env_sel 671
#define osgate_delete_env_sel 672
#define osgate_clone_env_sel 673

#define osgate_init_process 674

#define osgate_send_debug_event 675
#define osgate_kernel_debug_event 676

#define osgate_close_tcp_socket 677
#define osgate_read_tcp_socket 678
#define osgate_write_tcp_socket 679

#define osgate_close_udp_socket 680
#define osgate_read_udp_socket 681
#define osgate_write_udp_socket 682

#define osgate_allocate_c_proc_handle 683

#define osgate_connect_udp_socket 684
#define osgate_connect_tcp_socket 685

#define osgate_tcp_socket_read_count 686
#define osgate_udp_socket_read_count 687

#define osgate_tcp_socket_write_space 688
#define osgate_has_tcp_socket_exc 689

#define osgate_start_read_tcp_socket 690
#define osgate_stop_read_tcp_socket 691
#define osgate_start_read_udp_socket 692
#define osgate_stop_read_udp_socket 693

#define osgate_start_write_tcp_socket 694
#define osgate_stop_write_tcp_socket 695

#define osgate_start_exc_tcp_socket 696
#define osgate_stop_exc_tcp_socket 697

#define osgate_signal_read_handle 698
#define osgate_signal_write_handle 699
#define osgate_signal_exc_handle 700

#define osgate_start_read_stdin 701
#define osgate_stop_read_stdin 702

#define osgate_start_read_c_file 703
#define osgate_stop_read_c_file 704

#define osgate_poll_udp_socket 705
#define osgate_poll_tcp_socket 706

#define osgate_register_custom_hid 707

#define osgate_find_hid_feature_report 708
#define osgate_get_hid_report_size 709
#define osgate_get_hid_report_buf 710
#define osgate_read_hid_feature 711
#define osgate_write_hid_feature 712




#define OsGate_register_osgate 0x3E 0x67 0x9a 0 0 0 0 2 0
#define OsGate_is_valid_osgate 0x3E 0x67 0x9a 1 0 0 0 2 0
#define OsGate_register_usergate 0x3E 0x67 0x9a 2 0 0 0 2 0
#define OsGate_register_bimodal_usergate 0x3E 0x67 0x9a 3 0 0 0 2 0
#define OsGate_register_usergate16 0x3E 0x67 0x9a 4 0 0 0 2 0
#define OsGate_register_usergate32 0x3E 0x67 0x9a 5 0 0 0 2 0
#define OsGate_register_long_osgate 0x3E 0x67 0x9a 6 0 0 0 2 0
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
#define OsGate_create_down_sel32 0x3E 0x67 0x9a 27 0 0 0 2 0
#define OsGate_create_long_code_sel 0x3E 0x67 0x9a 28 0 0 0 2 0
#define OsGate_selector_to_segment 0x3E 0x67 0x9a 29 0 0 0 2 0
#define OsGate_segment_to_selector 0x3E 0x67 0x9a 30 0 0 0 2 0
#define OsGate_free_selector 0x3E 0x67 0x9a 31 0 0 0 2 0
#define OsGate_segment_not_present 0x3E 0x67 0x9a 32 0 0 0 2 0

#define OsGate_get_page_entry 0x3E 0x67 0x9a 33 0 0 0 2 0
#define OsGate_set_page_entry 0x3E 0x67 0x9a 34 0 0 0 2 0
#define OsGate_get_thread_page_entry 0x3E 0x67 0x9a 35 0 0 0 2 0
#define OsGate_set_thread_page_entry 0x3E 0x67 0x9a 36 0 0 0 2 0
#define OsGate_read_thread_selector 0x3E 0x67 0x9a 37 0 0 0 2 0
#define OsGate_write_thread_selector 0x3E 0x67 0x9a 38 0 0 0 2 0
#define OsGate_read_thread_segment 0x3E 0x67 0x9a 39 0 0 0 2 0
#define OsGate_write_thread_segment 0x3E 0x67 0x9a 40 0 0 0 2 0
#define OsGate_set_page_emulate 0x3E 0x67 0x9a 43 0 0 0 2 0
#define OsGate_invalid_os 0x3E 0x67 0x9a 44 0 0 0 2 0
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

#define OsGate_allocate_physical32 0x3E 0x67 0x9a 70 0 0 0 2 0
#define OsGate_allocate_physical64 0x3E 0x67 0x9a 71 0 0 0 2 0
#define OsGate_allocate_dma_physical 0x3E 0x67 0x9a 72 0 0 0 2 0
#define OsGate_free_physical 0x3E 0x67 0x9a 73 0 0 0 2 0

#define OsGate_create_process 0x3E 0x67 0x9a 74 0 0 0 2 0

#define OsGate_init_trap_gates 0x3E 0x67 0x9a 75 0 0 0 2 0
#define OsGate_start_tasking 0x3E 0x67 0x9a 76 0 0 0 2 0

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
#define OsGate_hook_init_tasking 0x3E 0x67 0x9a 97 0 0 0 2 0
#define OsGate_hook_state 0x3E 0x67 0x9a 98 0 0 0 2 0

#define OsGate_sim_sti 0x3E 0x67 0x9a 99 0 0 0 2 0
#define OsGate_sim_cli 0x3E 0x67 0x9a 100 0 0 0 2 0
#define OsGate_sim_set_flags 0x3E 0x67 0x9a 101 0 0 0 2 0
#define OsGate_sim_get_flags 0x3E 0x67 0x9a 102 0 0 0 2 0
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

#define OsGate_get_focus_thread 0x3E 0x67 0x9a 185 0 0 0 2 0

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
#define OsGate_set_disc_use32 0x3E 0x67 0x9a 244 0 0 0 2 0

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

#define OsGate_update_rtc 0x3E 0x67 0x9a 2 1 0 0 2 0

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

#define OsGate_allocate_multiple_physical32 0x3E 0x67 0x9a 27 1 0 0 2 0
#define OsGate_allocate_multiple_physical64 0x3E 0x67 0x9a 28 1 0 0 2 0

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

#define OsGate_module_id_to_sel 0x3E 0x67 0x9a 84 1 0 0 2 0
#define OsGate_alias_module_handle 0x3E 0x67 0x9a 85 1 0 0 2 0

#define OsGate_thread_to_sel 0x3E 0x67 0x9a 86 1 0 0 2 0

#define OsGate_stop_usb_req 0x3E 0x67 0x9a 88 1 0 0 2 0

#define OsGate_reserve_com_line 0x3E 0x67 0x9a 90 1 0 0 2 0

#define OsGate_device_set_dtr 0x3E 0x67 0x9a 91 1 0 0 2 0
#define OsGate_device_reset_dtr 0x3E 0x67 0x9a 92 1 0 0 2 0

#define OsGate_wait_for_line_state 0x3E 0x67 0x9a 93 1 0 0 2 0
#define OsGate_get_line_state 0x3E 0x67 0x9a 94 1 0 0 2 0

#define OsGate_notify_time_drift 0x3E 0x67 0x9a 97 1 0 0 2 0

#define OsGate_get_apic_id 0x3E 0x67 0x9a 101 1 0 0 2 0

#define OsGate_debug_exception 0x3E 0x67 0x9a 104 1 0 0 2 0

#define OsGate_create_core 0x3E 0x67 0x9a 106 1 0 0 2 0
#define OsGate_get_core_num 0x3E 0x67 0x9a 107 1 0 0 2 0

#define OsGate_locked_debug_exception 0x3E 0x67 0x9a 108 1 0 0 2 0

#define OsGate_lock_task 0x3E 0x67 0x9a 109 1 0 0 2 0
#define OsGate_unlock_task 0x3E 0x67 0x9a 110 1 0 0 2 0
#define OsGate_try_lock_task 0x3E 0x67 0x9a 111 1 0 0 2 0

#define OsGate_get_core 0x3E 0x67 0x9a 112 1 0 0 2 0
#define OsGate_get_core_count 0x3E 0x67 0x9a 113 1 0 0 2 0
#define OsGate_run_ap_core 0x3E 0x67 0x9a 114 1 0 0 2 0

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

#define OsGate_read_pnp_byte 0x3E 0x67 0x9a 137 1 0 0 2 0
#define OsGate_write_pnp_byte 0x3E 0x67 0x9a 138 1 0 0 2 0

#define OsGate_add_sys_env_var 0x3E 0x67 0x9a 139 1 0 0 2 0
#define OsGate_delete_sys_env_var 0x3E 0x67 0x9a 140 1 0 0 2 0
#define OsGate_find_sys_env_var 0x3E 0x67 0x9a 141 1 0 0 2 0

#define OsGate_start_preempt_timer 0x3E 0x67 0x9a 142 1 0 0 2 0
#define OsGate_reload_preempt_timer 0x3E 0x67 0x9a 143 1 0 0 2 0

#define OsGate_start_sys_preempt_timer 0x3E 0x67 0x9a 144 1 0 0 2 0
#define OsGate_reload_sys_preempt_timer 0x3E 0x67 0x9a 145 1 0 0 2 0

#define OsGate_is_valid_usb_pipe_sel 0x3E 0x67 0x9a 147 1 0 0 2 0

#define OsGate_hook_net_link_up 0x3E 0x67 0x9a 148 1 0 0 2 0
#define OsGate_req_arp 0x3E 0x67 0x9a 149 1 0 0 2 0

#define OsGate_begin_disc_handler 0x3E 0x67 0x9a 150 1 0 0 2 0
#define OsGate_end_disc_handler 0x3E 0x67 0x9a 151 1 0 0 2 0

#define OsGate_hook_init_pci 0x3E 0x67 0x9a 152 1 0 0 2 0

#define OsGate_register_msi 0x3E 0x67 0x9a 153 1 0 0 2 0

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
#define OsGate_notify_irq 0x3E 0x67 0x9a 167 1 0 0 2 0

#define OsGate_register_syscall 0x3E 0x67 0x9a 168 1 0 0 2 0
#define OsGate_register_bimodal_syscall 0x3E 0x67 0x9a 169 1 0 0 2 0

#define OsGate_start_syscall 0x3E 0x67 0x9a 171 1 0 0 2 0

#define OsGate_start_dev32 0x3E 0x67 0x9a 173 1 0 0 2 0

#define OsGate_get_ioapic_state 0x3E 0x67 0x9a 174 1 0 0 2 0

#define OsGate_lock_file 0x3E 0x67 0x9a 175 1 0 0 2 0
#define OsGate_unlock_file 0x3E 0x67 0x9a 176 1 0 0 2 0

#define OsGate_app_patch 0x3E 0x67 0x9a 179 1 0 0 2 0

#define OsGate_copy_page_entries 0x3E 0x67 0x9a 180 1 0 0 2 0
#define OsGate_move_page_entries 0x3E 0x67 0x9a 181 1 0 0 2 0

#define OsGate_has_page_entry 0x3E 0x67 0x9a 182 1 0 0 2 0
#define OsGate_free_page_entries 0x3E 0x67 0x9a 183 1 0 0 2 0
#define OsGate_free_global_page_entries 0x3E 0x67 0x9a 183 1 0 0 2 0
#define OsGate_allocate_page_entries 0x3E 0x67 0x9a 184 1 0 0 2 0
#define OsGate_reserve_page_entries 0x3E 0x67 0x9a 185 1 0 0 2 0

#define OsGate_add_carddev 0x3E 0x67 0x9a 186 1 0 0 2 0

#define OsGate_get_thread_page_dir 0x3E 0x67 0x9a 187 1 0 0 2 0
#define OsGate_set_page_dir 0x3E 0x67 0x9a 188 1 0 0 2 0
#define OsGate_set_sys_page_dir 0x3E 0x67 0x9a 189 1 0 0 2 0
#define OsGate_get_page_dir_attrib 0x3E 0x67 0x9a 190 1 0 0 2 0
#define OsGate_get_page_dir 0x3E 0x67 0x9a 191 1 0 0 2 0
#define OsGate_set_sys_page_entry 0x3E 0x67 0x9a 192 1 0 0 2 0
#define OsGate_copy_sys_page_entries 0x3E 0x67 0x9a 193 1 0 0 2 0
#define OsGate_get_sys_page_entry 0x3E 0x67 0x9a 194 1 0 0 2 0
#define OsGate_get_sys_page_dir 0x3E 0x67 0x9a 195 1 0 0 2 0

#define OsGate_notify_init_process 0x3E 0x67 0x9a 196 1 0 0 2 0
#define OsGate_notify_create_process 0x3E 0x67 0x9a 197 1 0 0 2 0

#define OsGate_get_scheduler_lock_counter 0x3E 0x67 0x9a 203 1 0 0 2 0
#define OsGate_flush_tlb 0x3E 0x67 0x9a 204 1 0 0 2 0
#define OsGate_setup_smp_patch 0x3E 0x67 0x9a 205 1 0 0 2 0
#define OsGate_notify_thread_suspend 0x3E 0x67 0x9a 206 1 0 0 2 0

#define OsGate_is_long_code_sel 0x3E 0x67 0x9a 207 1 0 0 2 0
#define OsGate_setup_long_int_gate 0x3E 0x67 0x9a 208 1 0 0 2 0
#define OsGate_setup_long_trap_gate 0x3E 0x67 0x9a 209 1 0 0 2 0

#define OsGate_enter_long_int 0x3E 0x67 0x9a 210 1 0 0 2 0
#define OsGate_leave_long_int 0x3E 0x67 0x9a 211 1 0 0 2 0

#define OsGate_create_long_irq 0x3E 0x67 0x9a 212 1 0 0 2 0
#define OsGate_add_long_irq 0x3E 0x67 0x9a 213 1 0 0 2 0
#define OsGate_create_long_msi 0x3E 0x67 0x9a 214 1 0 0 2 0
#define OsGate_add_long_msi 0x3E 0x67 0x9a 215 1 0 0 2 0

#define OsGate_setup_long_spurious_int 0x3E 0x67 0x9a 216 1 0 0 2 0
#define OsGate_setup_long_timer_int 0x3E 0x67 0x9a 217 1 0 0 2 0
#define OsGate_setup_long_preempt_int 0x3E 0x67 0x9a 218 1 0 0 2 0
#define OsGate_setup_long_hpet_int 0x3E 0x67 0x9a 221 1 0 0 2 0

#define OsGate_setup_long_crash_gate 0x3E 0x67 0x9a 223 1 0 0 2 0

#define OsGate_setup_long_nmi_core_dump 0x3E 0x67 0x9a 227 1 0 0 2 0

#define OsGate_debug_block 0x3E 0x67 0x9a 228 1 0 0 2 0

#define OsGate_switch_to_long_mode 0x3E 0x67 0x9a 229 1 0 0 2 0
#define OsGate_switch_to_protected_mode 0x3E 0x67 0x9a 230 1 0 0 2 0

#define OsGate_notify_create_long_process 0x3E 0x67 0x9a 231 1 0 0 2 0

#define OsGate_load_long_regs 0x3E 0x67 0x9a 232 1 0 0 2 0
#define OsGate_is_long_thread 0x3E 0x67 0x9a 233 1 0 0 2 0
#define OsGate_long_mode_reset 0x3E 0x67 0x9a 234 1 0 0 2 0

#define OsGate_create_sys_page_dir 0x3E 0x67 0x9a 235 1 0 0 2 0

#define OsGate_init_long_exe 0x3E 0x67 0x9a 236 1 0 0 2 0
#define OsGate_start_long_exe 0x3E 0x67 0x9a 237 1 0 0 2 0

#define OsGate_read_thread64 0x3E 0x67 0x9a 238 1 0 0 2 0
#define OsGate_write_thread64 0x3E 0x67 0x9a 239 1 0 0 2 0

#define OsGate_allocate_long_buf 0x3E 0x67 0x9a 246 1 0 0 2 0
#define OsGate_free_long_buf 0x3E 0x67 0x9a 247 1 0 0 2 0

#define OsGate_handle_long_code_fault 0x3E 0x67 0x9a 248 1 0 0 2 0
#define OsGate_load_long_breaks 0x3E 0x67 0x9a 249 1 0 0 2 0

#define OsGate_set_futex_id 0x3E 0x67 0x9a 250 1 0 0 2 0
#define OsGate_set_long_tls_linear 0x3E 0x67 0x9a 251 1 0 0 2 0

#define OsGate_broadcast_driver_udp 0x3E 0x67 0x9a 253 1 0 0 2 0

#define OsGate_get_net_driver_buffer 0x3E 0x67 0x9a 254 1 0 0 2 0
#define OsGate_send_net_driver 0x3E 0x67 0x9a 255 1 0 0 2 0

#define OsGate_create_driver_ip 0x3E 0x67 0x9a 0 2 0 0 2 0
#define OsGate_send_driver_ip 0x3E 0x67 0x9a 1 2 0 0 2 0

#define OsGate_send_driver_udp 0x3E 0x67 0x9a 2 2 0 0 2 0

#define OsGate_create_long_thread_info 0x3E 0x67 0x9a 3 2 0 0 2 0

#define OsGate_has_long_mode 0x3E 0x67 0x9a 4 2 0 0 2 0

#define OsGate_is_usb_hub_port_connected 0x3E 0x67 0x9a 5 2 0 0 2 0

#define OsGate_wait_for_ehci 0x3E 0x67 0x9a 6 2 0 0 2 0
#define OsGate_wait_for_ohci 0x3E 0x67 0x9a 7 2 0 0 2 0
#define OsGate_wait_for_uhci 0x3E 0x67 0x9a 8 2 0 0 2 0

#define OsGate_register_hid_input 0x3E 0x67 0x9a 10 2 0 0 2 0
#define OsGate_get_signed_hid_input 0x3E 0x67 0x9a 11 2 0 0 2 0
#define OsGate_get_unsigned_hid_input 0x3E 0x67 0x9a 12 2 0 0 2 0
#define OsGate_set_hid_idle 0x3E 0x67 0x9a 13 2 0 0 2 0
#define OsGate_find_hid_output_report 0x3E 0x67 0x9a 14 2 0 0 2 0
#define OsGate_set_hid_output 0x3E 0x67 0x9a 15 2 0 0 2 0
#define OsGate_update_hid_output 0x3E 0x67 0x9a 16 2 0 0 2 0
#define OsGate_get_hid_log_min 0x3E 0x67 0x9a 17 2 0 0 2 0
#define OsGate_get_hid_log_max 0x3E 0x67 0x9a 18 2 0 0 2 0
#define OsGate_cond_begin_disc_handler 0x3E 0x67 0x9a 19 2 0 0 2 0
#define OsGate_stop_disc_request 0x3E 0x67 0x9a 20 2 0 0 2 0

#define OsGate_lock_usb 0x3E 0x67 0x9a 21 2 0 0 2 0
#define OsGate_unlock_usb 0x3E 0x67 0x9a 22 2 0 0 2 0

#define OsGate_restart_can_modules 0x3E 0x67 0x9a 23 2 0 0 2 0

#define OsGate_send_can_bus_msg 0x3E 0x67 0x9a 24 2 0 0 2 0
#define OsGate_hook_can_gen_bus_msg 0x3E 0x67 0x9a 25 2 0 0 2 0
#define OsGate_create_can_id_hook 0x3E 0x67 0x9a 26 2 0 0 2 0
#define OsGate_delete_can_id_hook 0x3E 0x67 0x9a 27 2 0 0 2 0

#define OsGate_reset_usb_hub_port 0x3E 0x67 0x9a 28 2 0 0 2 0
#define OsGate_add_lon_module 0x3E 0x67 0x9a 29 2 0 0 2 0

#define OsGate_get_acpi_pnp_device_mem 0x3E 0x67 0x9a 30 2 0 0 2 0

#define OsGate_force_level_irq 0x3E 0x67 0x9a 32 2 0 0 2 0

#define OsGate_has_can_send_buf 0x3E 0x67 0x9a 33 2 0 0 2 0

#define OsGate_fpu_exception 0x3E 0x67 0x9a 34 2 0 0 2 0

#define OsGate_reset_can_buffers 0x3E 0x67 0x9a 35 2 0 0 2 0
#define OsGate_send_can_bus_block 0x3E 0x67 0x9a 36 2 0 0 2 0

#define OsGate_get_disc_vendor_info_buf 0x3E 0x67 0x9a 37 2 0 0 2 0

#define OsGate_get_pci_msix 0x3E 0x67 0x9a 38 2 0 0 2 0
#define OsGate_enable_pci_msix 0x3E 0x67 0x9a 39 2 0 0 2 0
#define OsGate_setup_pci_msix_entry 0x3E 0x67 0x9a 40 2 0 0 2 0

#define OsGate_update_tcp_mtu 0x3E 0x67 0x9a 41 2 0 0 2 0

#define OsGate_notify_lon_data 0x3E 0x67 0x9a 42 2 0 0 2 0

#define OsGate_set_thread_core 0x3E 0x67 0x9a 43 2 0 0 2 0

#define OsGate_create_thread_id 0x3E 0x67 0x9a 44 2 0 0 2 0

#define OsGate_init_freq 0x3E 0x67 0x9a 45 2 0 0 2 0
#define OsGate_update_freq 0x3E 0x67 0x9a 46 2 0 0 2 0

#define OsGate_add_thread_int 0x3E 0x67 0x9a 47 2 0 0 2 0

#define OsGate_do_flush_tlb 0x3E 0x67 0x9a 49 2 0 0 2 0

#define OsGate_get_sel_bitness 0x3E 0x67 0x9a 50 2 0 0 2 0

#define OsGate_setup_long_schedule_int 0x3E 0x67 0x9a 51 2 0 0 2 0

#define OsGate_long_timer_handler 0x3E 0x67 0x9a 52 2 0 0 2 0
#define OsGate_long_hpet_handler 0x3E 0x67 0x9a 53 2 0 0 2 0

#define OsGate_use_own_preempt_timer 0x3E 0x67 0x9a 54 2 0 0 2 0

#define OsGate_fault_reset 0x3E 0x67 0x9a 55 2 0 0 2 0

#define OsGate_disc_request_retry 0x3E 0x67 0x9a 56 2 0 0 2 0

#define OsGate_get_thread_selector_page 0x3E 0x67 0x9a 57 2 0 0 2 0

#define OsGate_notify_delete_process 0x3E 0x67 0x9a 61 2 0 0 2 0

#define OsGate_add_video_mode 0x3E 0x67 0x9a 62 2 0 0 2 0
#define OsGate_begin_get_video_modes 0x3E 0x67 0x9a 63 2 0 0 2 0
#define OsGate_end_get_video_modes 0x3E 0x67 0x9a 64 2 0 0 2 0
#define OsGate_switch_video_mode 0x3E 0x67 0x9a 65 2 0 0 2 0

#define OsGate_start_smp_core_dump 0x3E 0x67 0x9a 66 2 0 0 2 0
#define OsGate_start_core_dump 0x3E 0x67 0x9a 67 2 0 0 2 0
#define OsGate_notify_core_dump 0x3E 0x67 0x9a 68 2 0 0 2 0
#define OsGate_setup_nmi_core_dump 0x3E 0x67 0x9a 69 2 0 0 2 0

#define OsGate_allocate_c_handle 0x3E 0x67 0x9a 72 2 0 0 2 0
#define OsGate_open_c_file 0x3E 0x67 0x9a 73 2 0 0 2 0
#define OsGate_close_c_file 0x3E 0x67 0x9a 74 2 0 0 2 0
#define OsGate_ref_c_handle 0x3E 0x67 0x9a 75 2 0 0 2 0
#define OsGate_read_c_file 0x3E 0x67 0x9a 76 2 0 0 2 0
#define OsGate_write_c_file 0x3E 0x67 0x9a 77 2 0 0 2 0
#define OsGate_get_c_file_size 0x3E 0x67 0x9a 78 2 0 0 2 0
#define OsGate_set_c_file_size 0x3E 0x67 0x9a 79 2 0 0 2 0
#define OsGate_get_c_file_time 0x3E 0x67 0x9a 80 2 0 0 2 0
#define OsGate_set_c_file_time 0x3E 0x67 0x9a 81 2 0 0 2 0
#define OsGate_read_c_console 0x3E 0x67 0x9a 82 2 0 0 2 0
#define OsGate_write_c_console 0x3E 0x67 0x9a 83 2 0 0 2 0
#define OsGate_open_kernel_file 0x3E 0x67 0x9a 84 2 0 0 2 0

#define OsGate_fork_process 0x3E 0x67 0x9a 85 2 0 0 2 0

#define OsGate_reset_process 0x3E 0x67 0x9a 89 2 0 0 2 0
#define OsGate_exec_app 0x3E 0x67 0x9a 90 2 0 0 2 0

#define OsGate_create_fork 0x3E 0x67 0x9a 98 2 0 0 2 0

#define OsGate_dupl_c_file_to_file 0x3E 0x67 0x9a 99 2 0 0 2 0
#define OsGate_c_handle_to_file_sel 0x3E 0x67 0x9a 100 2 0 0 2 0

#define OsGate_add_serio_device 0x3E 0x67 0x9a 101 2 0 0 2 0

#define OsGate_notify_can_offline 0x3E 0x67 0x9a 103 2 0 0 2 0

#define OsGate_req_usb_data_no_copy 0x3E 0x67 0x9a 104 2 0 0 2 0
#define OsGate_write_usb_data_no_copy 0x3E 0x67 0x9a 105 2 0 0 2 0

#define OsGate_notify_can_modules_up 0x3E 0x67 0x9a 107 2 0 0 2 0
#define OsGate_notify_can_online 0x3E 0x67 0x9a 108 2 0 0 2 0

#define OsGate_lock_log 0x3E 0x67 0x9a 109 2 0 0 2 0
#define OsGate_unlock_log 0x3E 0x67 0x9a 110 2 0 0 2 0
#define OsGate_log_thread 0x3E 0x67 0x9a 111 2 0 0 2 0
#define OsGate_log_memory 0x3E 0x67 0x9a 112 2 0 0 2 0
#define OsGate_log_text 0x3E 0x67 0x9a 113 2 0 0 2 0
#define OsGate_log_hex_byte 0x3E 0x67 0x9a 114 2 0 0 2 0
#define OsGate_log_hex_word 0x3E 0x67 0x9a 115 2 0 0 2 0
#define OsGate_log_hex_dword 0x3E 0x67 0x9a 116 2 0 0 2 0
#define OsGate_log_small_mem 0x3E 0x67 0x9a 117 2 0 0 2 0
#define OsGate_log_big_mem 0x3E 0x67 0x9a 118 2 0 0 2 0

#define OsGate_create_pid 0x3E 0x67 0x9a 119 2 0 0 2 0

#define OsGate_start_programs 0x3E 0x67 0x9a 120 2 0 0 2 0
#define OsGate_register_loader 0x3E 0x67 0x9a 121 2 0 0 2 0

#define OsGate_find_module_by_address 0x3E 0x67 0x9a 122 2 0 0 2 0
#define OsGate_find_module_by_name 0x3E 0x67 0x9a 123 2 0 0 2 0
#define OsGate_get_module_by_index 0x3E 0x67 0x9a 124 2 0 0 2 0

#define OsGate_create_app_thread 0x3E 0x67 0x9a 125 2 0 0 2 0
#define OsGate_terminate_app_thread 0x3E 0x67 0x9a 126 2 0 0 2 0

#define OsGate_program_created 0x3E 0x67 0x9a 127 2 0 0 2 0
#define OsGate_program_terminated 0x3E 0x67 0x9a 128 2 0 0 2 0
#define OsGate_get_program_sel 0x3E 0x67 0x9a 129 2 0 0 2 0
#define OsGate_get_program_id 0x3E 0x67 0x9a 130 2 0 0 2 0
#define OsGate_module_loaded 0x3E 0x67 0x9a 131 2 0 0 2 0
#define OsGate_module_unloaded 0x3E 0x67 0x9a 132 2 0 0 2 0
#define OsGate_get_module_id 0x3E 0x67 0x9a 133 2 0 0 2 0

#define OsGate_get_exe_start32 0x3E 0x67 0x9a 134 2 0 0 2 0

#define OsGate_process_created 0x3E 0x67 0x9a 135 2 0 0 2 0
#define OsGate_process_terminated 0x3E 0x67 0x9a 136 2 0 0 2 0
#define OsGate_process_id_to_sel 0x3E 0x67 0x9a 137 2 0 0 2 0
#define OsGate_get_process_id 0x3E 0x67 0x9a 138 2 0 0 2 0

#define OsGate_create_ldt 0x3E 0x67 0x9a 139 2 0 0 2 0
#define OsGate_destroy_ldt 0x3E 0x67 0x9a 140 2 0 0 2 0

#define OsGate_create_c_handle 0x3E 0x67 0x9a 141 2 0 0 2 0
#define OsGate_clone_c_handle 0x3E 0x67 0x9a 142 2 0 0 2 0
#define OsGate_delete_c_handle 0x3E 0x67 0x9a 143 2 0 0 2 0

#define OsGate_close_console 0x3E 0x67 0x9a 144 2 0 0 2 0
#define OsGate_set_focus_console 0x3E 0x67 0x9a 145 2 0 0 2 0
#define OsGate_get_focus_console 0x3E 0x67 0x9a 146 2 0 0 2 0
#define OsGate_create_console 0x3E 0x67 0x9a 147 2 0 0 2 0

#define OsGate_create_cur_dir 0x3E 0x67 0x9a 148 2 0 0 2 0
#define OsGate_clone_cur_dir 0x3E 0x67 0x9a 149 2 0 0 2 0
#define OsGate_delete_cur_dir 0x3E 0x67 0x9a 150 2 0 0 2 0

#define OsGate_create_handle_data 0x3E 0x67 0x9a 151 2 0 0 2 0
#define OsGate_destroy_handle_data 0x3E 0x67 0x9a 152 2 0 0 2 0

#define OsGate_init_process_mem 0x3E 0x67 0x9a 153 2 0 0 2 0

#define OsGate_start_fork 0x3E 0x67 0x9a 154 2 0 0 2 0
#define OsGate_detach_fork 0x3E 0x67 0x9a 155 2 0 0 2 0
#define OsGate_cleanup_fork 0x3E 0x67 0x9a 156 2 0 0 2 0
#define OsGate_removed_process 0x3E 0x67 0x9a 157 2 0 0 2 0
#define OsGate_delete_fork 0x3E 0x67 0x9a 158 2 0 0 2 0

#define OsGate_create_env_sel 0x3E 0x67 0x9a 159 2 0 0 2 0
#define OsGate_delete_env_sel 0x3E 0x67 0x9a 160 2 0 0 2 0
#define OsGate_clone_env_sel 0x3E 0x67 0x9a 161 2 0 0 2 0

#define OsGate_init_process 0x3E 0x67 0x9a 162 2 0 0 2 0

#define OsGate_send_debug_event 0x3E 0x67 0x9a 163 2 0 0 2 0
#define OsGate_kernel_debug_event 0x3E 0x67 0x9a 164 2 0 0 2 0

#define OsGate_close_tcp_socket 0x3E 0x67 0x9a 165 2 0 0 2 0
#define OsGate_read_tcp_socket 0x3E 0x67 0x9a 166 2 0 0 2 0
#define OsGate_write_tcp_socket 0x3E 0x67 0x9a 167 2 0 0 2 0

#define OsGate_close_udp_socket 0x3E 0x67 0x9a 168 2 0 0 2 0
#define OsGate_read_udp_socket 0x3E 0x67 0x9a 169 2 0 0 2 0
#define OsGate_write_udp_socket 0x3E 0x67 0x9a 170 2 0 0 2 0

#define OsGate_allocate_c_proc_handle 0x3E 0x67 0x9a 171 2 0 0 2 0

#define OsGate_connect_udp_socket 0x3E 0x67 0x9a 172 2 0 0 2 0
#define OsGate_connect_tcp_socket 0x3E 0x67 0x9a 173 2 0 0 2 0

#define OsGate_tcp_socket_read_count 0x3E 0x67 0x9a 174 2 0 0 2 0
#define OsGate_udp_socket_read_count 0x3E 0x67 0x9a 175 2 0 0 2 0

#define OsGate_tcp_socket_write_space 0x3E 0x67 0x9a 176 2 0 0 2 0
#define OsGate_has_tcp_socket_exc 0x3E 0x67 0x9a 177 2 0 0 2 0

#define OsGate_start_read_tcp_socket 0x3E 0x67 0x9a 178 2 0 0 2 0
#define OsGate_stop_read_tcp_socket 0x3E 0x67 0x9a 179 2 0 0 2 0
#define OsGate_start_read_udp_socket 0x3E 0x67 0x9a 180 2 0 0 2 0
#define OsGate_stop_read_udp_socket 0x3E 0x67 0x9a 181 2 0 0 2 0

#define OsGate_start_write_tcp_socket 0x3E 0x67 0x9a 182 2 0 0 2 0
#define OsGate_stop_write_tcp_socket 0x3E 0x67 0x9a 183 2 0 0 2 0

#define OsGate_start_exc_tcp_socket 0x3E 0x67 0x9a 184 2 0 0 2 0
#define OsGate_stop_exc_tcp_socket 0x3E 0x67 0x9a 185 2 0 0 2 0

#define OsGate_signal_read_handle 0x3E 0x67 0x9a 186 2 0 0 2 0
#define OsGate_signal_write_handle 0x3E 0x67 0x9a 187 2 0 0 2 0
#define OsGate_signal_exc_handle 0x3E 0x67 0x9a 188 2 0 0 2 0

#define OsGate_start_read_stdin 0x3E 0x67 0x9a 189 2 0 0 2 0
#define OsGate_stop_read_stdin 0x3E 0x67 0x9a 190 2 0 0 2 0

#define OsGate_start_read_c_file 0x3E 0x67 0x9a 191 2 0 0 2 0
#define OsGate_stop_read_c_file 0x3E 0x67 0x9a 192 2 0 0 2 0

#define OsGate_poll_udp_socket 0x3E 0x67 0x9a 193 2 0 0 2 0
#define OsGate_poll_tcp_socket 0x3E 0x67 0x9a 194 2 0 0 2 0

#define OsGate_register_custom_hid 0x3E 0x67 0x9a 195 2 0 0 2 0

#define OsGate_find_hid_feature_report 0x3E 0x67 0x9a 196 2 0 0 2 0
#define OsGate_get_hid_report_size 0x3E 0x67 0x9a 197 2 0 0 2 0
#define OsGate_get_hid_report_buf 0x3E 0x67 0x9a 198 2 0 0 2 0
#define OsGate_read_hid_feature 0x3E 0x67 0x9a 199 2 0 0 2 0
#define OsGate_write_hid_feature 0x3E 0x67 0x9a 200 2 0 0 2 0

