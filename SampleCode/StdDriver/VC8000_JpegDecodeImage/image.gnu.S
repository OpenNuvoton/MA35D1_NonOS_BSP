;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	VectorDataBase_JPEG
	.globl	VectorDataLimit_JPEG

	.align	4

	.text

VectorDataBase_JPEG:
	.incbin "../../Ball_1440x900.jpg"

VectorDataLimit_JPEG:
	.space   4

	.end

