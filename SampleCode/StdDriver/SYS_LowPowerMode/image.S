;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2020 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/


;// .syntax unified

    .globl  Core1DataBase
    .globl  Core1DataLimit

    .align  4

    .text

    .align  4
Core1DataBase:
    .incbin "../../pd1.bin"
Core1DataLimit:


    .end


