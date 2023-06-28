;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	ImageDataBase
	.globl	ImageDataLimit
	.globl	OverlayImageDataBase
	.globl	OverlayImageDataLimit

	.align	4

	.text

ImageDataBase:
	.incbin "../../framebuffer_image_1024x600.argb"
ImageDataLimit:
	.space   4

OverlayImageDataBase:
    .incbin "../../overlay_image_640x480.argb"
OverlayImageDataLimit:
	.space   4

	.end

