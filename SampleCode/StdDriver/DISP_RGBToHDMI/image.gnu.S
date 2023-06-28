;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	ImageRGB565DataBase
	.globl	ImageRGB565DataLimit
	.align	4

	.text

ImageRGB565DataBase:
	.incbin "../../image_rgb565_1920x1080.argb"

ImageRGB565DataLimit:
	.space   4
	.end

