;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;// .syntax unified

    .globl  VectorDataBase_GCM_Encrypt
    .globl  VectorDataLimit_GCM_Encrypt

    .globl  VectorDataBase_GCM_Decrypt
    .globl  VectorDataLimit_GCM_Decrypt

    .align  4

    .text

VectorDataBase_GCM_Encrypt:
    .incbin "../../gcm_encrypt_regression.rsp"

VectorDataLimit_GCM_Encrypt:
    .space   4

VectorDataBase_GCM_Decrypt:
    .incbin "../../gcm_decrypt_regression.rsp"  // OK

VectorDataLimit_GCM_Decrypt:
    .space   4

    .end

