;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

.section .data
	.align 8
	.global rtp_file_data
	.global rtp_file_end
rtp_file_data:
	.incbin "../../share_memory_sdram.bin"
rtp_file_end:
