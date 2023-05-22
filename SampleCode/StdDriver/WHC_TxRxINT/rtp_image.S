;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

.section .data
	.align 4
	.global rtp_file_data
	.global rtp_file_end
rtp_file_data:
	.incbin "../../rtp_whctxrx.bin"
rtp_file_end:
