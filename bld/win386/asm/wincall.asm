;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


;****************************************************************************
;***                                                                      ***
;*** WINCALL.ASM - windows callback routines                              ***
;***                                                                      ***
;****************************************************************************
.386p

extrn   __32BitCallBackAddr : FWORD
extrn   _DataSelector:WORD
extrn   _StackSelector:WORD
extrn   _SaveSP:DWORD
extrn   _EntryStackSave:DWORD

Callback MACRO name
        public  CB&name
CB&name proc    far
        call    __CallBack_
        ret
CB&name endp
ENDM

DGROUP group _DATA
_DATA segment word public 'DATA' use16
_DATA ends

_TEXT segment word public 'CODE' use16
assume  cs:_TEXT

        Callback        512
        Callback        511
        Callback        510
        Callback        509
        Callback        508
        Callback        507
        Callback        506
        Callback        505
        Callback        504
        Callback        503
        Callback        502
        Callback        501
        Callback        500
        Callback        499
        Callback        498
        Callback        497
        Callback        496
        Callback        495
        Callback        494
        Callback        493
        Callback        492
        Callback        491
        Callback        490
        Callback        489
        Callback        488
        Callback        487
        Callback        486
        Callback        485
        Callback        484
        Callback        483
        Callback        482
        Callback        481
        Callback        480
        Callback        479
        Callback        478
        Callback        477
        Callback        476
        Callback        475
        Callback        474
        Callback        473
        Callback        472
        Callback        471
        Callback        470
        Callback        469
        Callback        468
        Callback        467
        Callback        466
        Callback        465
        Callback        464
        Callback        463
        Callback        462
        Callback        461
        Callback        460
        Callback        459
        Callback        458
        Callback        457
        Callback        456
        Callback        455
        Callback        454
        Callback        453
        Callback        452
        Callback        451
        Callback        450
        Callback        449
        Callback        448
        Callback        447
        Callback        446
        Callback        445
        Callback        444
        Callback        443
        Callback        442
        Callback        441
        Callback        440
        Callback        439
        Callback        438
        Callback        437
        Callback        436
        Callback        435
        Callback        434
        Callback        433
        Callback        432
        Callback        431
        Callback        430
        Callback        429
        Callback        428
        Callback        427
        Callback        426
        Callback        425
        Callback        424
        Callback        423
        Callback        422
        Callback        421
        Callback        420
        Callback        419
        Callback        418
        Callback        417
        Callback        416
        Callback        415
        Callback        414
        Callback        413
        Callback        412
        Callback        411
        Callback        410
        Callback        409
        Callback        408
        Callback        407
        Callback        406
        Callback        405
        Callback        404
        Callback        403
        Callback        402
        Callback        401
        Callback        400
        Callback        399
        Callback        398
        Callback        397
        Callback        396
        Callback        395
        Callback        394
        Callback        393
        Callback        392
        Callback        391
        Callback        390
        Callback        389
        Callback        388
        Callback        387
        Callback        386
        Callback        385
        Callback        384
        Callback        383
        Callback        382
        Callback        381
        Callback        380
        Callback        379
        Callback        378
        Callback        377
        Callback        376
        Callback        375
        Callback        374
        Callback        373
        Callback        372
        Callback        371
        Callback        370
        Callback        369
        Callback        368
        Callback        367
        Callback        366
        Callback        365
        Callback        364
        Callback        363
        Callback        362
        Callback        361
        Callback        360
        Callback        359
        Callback        358
        Callback        357
        Callback        356
        Callback        355
        Callback        354
        Callback        353
        Callback        352
        Callback        351
        Callback        350
        Callback        349
        Callback        348
        Callback        347
        Callback        346
        Callback        345
        Callback        344
        Callback        343
        Callback        342
        Callback        341
        Callback        340
        Callback        339
        Callback        338
        Callback        337
        Callback        336
        Callback        335
        Callback        334
        Callback        333
        Callback        332
        Callback        331
        Callback        330
        Callback        329
        Callback        328
        Callback        327
        Callback        326
        Callback        325
        Callback        324
        Callback        323
        Callback        322
        Callback        321
        Callback        320
        Callback        319
        Callback        318
        Callback        317
        Callback        316
        Callback        315
        Callback        314
        Callback        313
        Callback        312
        Callback        311
        Callback        310
        Callback        309
        Callback        308
        Callback        307
        Callback        306
        Callback        305
        Callback        304
        Callback        303
        Callback        302
        Callback        301
        Callback        300
        Callback        299
        Callback        298
        Callback        297
        Callback        296
        Callback        295
        Callback        294
        Callback        293
        Callback        292
        Callback        291
        Callback        290
        Callback        289
        Callback        288
        Callback        287
        Callback        286
        Callback        285
        Callback        284
        Callback        283
        Callback        282
        Callback        281
        Callback        280
        Callback        279
        Callback        278
        Callback        277
        Callback        276
        Callback        275
        Callback        274
        Callback        273
        Callback        272
        Callback        271
        Callback        270
        Callback        269
        Callback        268
        Callback        267
        Callback        266
        Callback        265
        Callback        264
        Callback        263
        Callback        262
        Callback        261
        Callback        260
        Callback        259
        Callback        258
        Callback        257
        Callback        256
        Callback        255
        Callback        254
        Callback        253
        Callback        252
        Callback        251
        Callback        250
        Callback        249
        Callback        248
        Callback        247
        Callback        246
        Callback        245
        Callback        244
        Callback        243
        Callback        242
        Callback        241
        Callback        240
        Callback        239
        Callback        238
        Callback        237
        Callback        236
        Callback        235
        Callback        234
        Callback        233
        Callback        232
        Callback        231
        Callback        230
        Callback        229
        Callback        228
        Callback        227
        Callback        226
        Callback        225
        Callback        224
        Callback        223
        Callback        222
        Callback        221
        Callback        220
        Callback        219
        Callback        218
        Callback        217
        Callback        216
        Callback        215
        Callback        214
        Callback        213
        Callback        212
        Callback        211
        Callback        210
        Callback        209
        Callback        208
        Callback        207
        Callback        206
        Callback        205
        Callback        204
        Callback        203
        Callback        202
        Callback        201
        Callback        200
        Callback        199
        Callback        198
        Callback        197
        Callback        196
        Callback        195
        Callback        194
        Callback        193
        Callback        192
        Callback        191
        Callback        190
        Callback        189
        Callback        188
        Callback        187
        Callback        186
        Callback        185
        Callback        184
        Callback        183
        Callback        182
        Callback        181
        Callback        180
        Callback        179
        Callback        178
        Callback        177
        Callback        176
        Callback        175
        Callback        174
        Callback        173
        Callback        172
        Callback        171
        Callback        170
        Callback        169
        Callback        168
        Callback        167
        Callback        166
        Callback        165
        Callback        164
        Callback        163
        Callback        162
        Callback        161
        Callback        160
        Callback        159
        Callback        158
        Callback        157
        Callback        156
        Callback        155
        Callback        154
        Callback        153
        Callback        152
        Callback        151
        Callback        150
        Callback        149
        Callback        148
        Callback        147
        Callback        146
        Callback        145
        Callback        144
        Callback        143
        Callback        142
        Callback        141
        Callback        140
        Callback        139
        Callback        138
        Callback        137
        Callback        136
        Callback        135
        Callback        134
        Callback        133
        Callback        132
        Callback        131
        Callback        130
        Callback        129
        Callback        128
        Callback        127
        Callback        126
        Callback        125
        Callback        124
        Callback        123
        Callback        122
        Callback        121
        Callback        120
        Callback        119
        Callback        118
        Callback        117
        Callback        116
        Callback        115
        Callback        114
        Callback        113
        Callback        112
        Callback        111
        Callback        110
        Callback        109
        Callback        108
        Callback        107
        Callback        106
        Callback        105
        Callback        104
        Callback        103
        Callback        102
        Callback        101
        Callback        100
        Callback        99
        Callback        98
        Callback        97
        Callback        96
        Callback        95
        Callback        94
        Callback        93
        Callback        92
        Callback        91
        Callback        90
        Callback        89
        Callback        88
        Callback        87
        Callback        86
        Callback        85
        Callback        84
        Callback        83
        Callback        82
        Callback        81
        Callback        80
        Callback        79
        Callback        78
        Callback        77
        Callback        76
        Callback        75
        Callback        74
        Callback        73
        Callback        72
        Callback        71
        Callback        70
        Callback        69
        Callback        68
        Callback        67
        Callback        66
        Callback        65
        Callback        64
        Callback        63
        Callback        62
        Callback        61
        Callback        60
        Callback        59
        Callback        58
        Callback        57
        Callback        56
        Callback        55
        Callback        54
        Callback        53
        Callback        52
        Callback        51
        Callback        50
        Callback        49
        Callback        48
        Callback        47
        Callback        46
        Callback        45
        Callback        44
        Callback        43
        Callback        42
        Callback        41
        Callback        40
        Callback        39
        Callback        38
        Callback        37
        Callback        36
        Callback        35
        Callback        34
        Callback        33
        Callback        32
        Callback        31
        Callback        30
        Callback        29
        Callback        28
        Callback        27
        Callback        26
        Callback        25
        Callback        24
        Callback        23
        Callback        22
        Callback        21
        Callback        20
        Callback        19
        Callback        18
        Callback        17
        Callback        16
        Callback        15
        Callback        14
        Callback        13
        Callback        12
        Callback        11
        Callback        10
        Callback        9
        Callback        8
        Callback        7
        Callback        6
        Callback        5
        Callback        4
        Callback        3
        Callback        2
        Callback        1

        public  __CallBack_
__CallBack_ proc        near
        pop     DX                      ; get return address
        inc     BP                      ; standard Windows prolog
        push    BP                      ; ...
        mov     BP,SP                   ; ... get access to parms
        push    DS                      ; save DS
ifdef DLL32
        mov     AX,DGROUP
        mov     DS,AX                   ; get access to DGROUP
else
        push    SS                      ; SS points to DGROUP
        pop     DS                      ; set DS to DGROUP
endif
        mov     AX,offset __CallBack_-1 ; calc. callback index
        sub     AX,DX                   ; ...
        mov     DX,AX                   ; ...
        mov     AX,SS                   ; get 16-bit stack selector
        mov     word ptr _EntryStackSave,SP     ; save current sp
        mov     word ptr _EntryStackSave+2,SS   ; save stack segment
        mov     ES,_DataSelector        ; get 32-bit data selector
        mov     SS,_StackSelector       ; switch to 32-bit stack
        mov     ESP,_SaveSP             ; ...
        push    _EntryStackSave         ; save 16-bit SS:SP on 32-bit stack
        call    __32BitCallBackAddr     ; go over to 32-bit side
        lea     ECX,4[ESP]              ; calc. value for 32-bit stack pointer
        mov     _SaveSP,ECX             ; save 32-bit stack pointer 22-nov-93
        lss     SP,[ESP]                ; switch back to 16-bit stack
        pop     DS                      ; restore DS
        pop     BP                      ; restore BP
        dec     BP                      ; ...
        pop     BX                      ; get return address
        pop     CX                      ; ...
        xor     DH,DH                   ; zero high byte of stack adjustment
        add     SP,DX                   ; adjust stack pointer
        push    CX                      ; push return address
        push    BX                      ; ...
        push    EAX                     ; push return value
        pop     AX                      ; pop return value into DX:AX
        pop     DX                      ; ...
        retf                            ; return
__CallBack_ endp

_TEXT   ends
        end
