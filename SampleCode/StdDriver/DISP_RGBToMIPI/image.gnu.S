;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	ImageDataBase
	.globl	ImageDataLimit

	.align	4

	.text

ImageDataBase:
	.incbin "../../image_argb8_1920x1200.argb"

ImageDataLimit:
	.space   4

	.end

