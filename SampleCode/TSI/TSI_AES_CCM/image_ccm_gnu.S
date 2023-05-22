;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

;//	.syntax	unified

	.globl	VectorDataBase_CCM
	.globl	VectorDataLimit_CCM

	.align	4
 
	.text
        
VectorDataBase_CCM:
    .incbin "../../regression.dat"

VectorDataLimit_CCM:
    .space   4
    
    .end
