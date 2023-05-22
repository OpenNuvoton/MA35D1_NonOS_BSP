;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	VectorDataBase_HMAC
	.globl	VectorDataLimit_HMAC
	.globl	VectorDataBase_MD5
	.globl	VectorDataLimit_MD5

	.align	4

	.text

VectorDataBase_HMAC:
	.incbin "../../hmac_regression.rsp"

VectorDataLimit_HMAC:
	.space   4

VectorDataBase_MD5:
	.incbin "../../HMAC_MD5_RFC2202.dat"

VectorDataLimit_MD5:
	.space   4

	.end

