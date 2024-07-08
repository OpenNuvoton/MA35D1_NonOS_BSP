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
	.globl	ImageYUV422DataBase
	.globl	ImageYUV422DataLimit
	.globl	ImageNV12DataBase
	.globl	ImageNV12DataLimit
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

ImageYUV422DataBase:
	.incbin "../../image_yuv422_1024x600.dat"

ImageYUV422DataLimit:
	.space   4

ImageNV12DataBase:
	.incbin "../../image_nv12_1024x600.yuv"

ImageNV12DataLimit:
	.space   4



	.end

