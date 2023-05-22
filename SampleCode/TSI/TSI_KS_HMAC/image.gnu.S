;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	VectorDataBase_HMAC
	.globl	VectorDataLimit_HMAC

	.align	4

	.text

VectorDataBase_HMAC:
	.incbin "../../HMAC_NIST_SHA2.rsp"

VectorDataLimit_HMAC:
	.space   4

	.end

