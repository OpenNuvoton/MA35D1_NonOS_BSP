;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	ImageARGB8DataBase
	.globl	ImageARGB8DataLimit
	.globl	ImageRGB565DataBase
	.globl	ImageRGB565DataLimit
	.align	4

	.text

ImageARGB8DataBase:
	.incbin "../../image_argb8_1024x600.argb"

ImageARGB8DataLimit:
	.space   4

ImageRGB565DataBase:
	.incbin "../../image_rgb565_1024x600.argb"

ImageRGB565DataLimit:
	.space   4
	.end

