;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	VectorDataBase
	.globl	VectorDataLimit

	.align	4

	.text

VectorDataBase:
	.incbin "../../sha3_regression.rsp"

VectorDataLimit:
	.space   4

	.end

