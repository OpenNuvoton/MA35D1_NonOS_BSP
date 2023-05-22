;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	VectorDataBase_H264BS
	.globl	VectorDataLimit_H264BS

	.align	4

	.text

VectorDataBase_H264BS:
	.incbin "../../bubles_1080p_base.264"

VectorDataLimit_H264BS:
	.space   4

	.end

