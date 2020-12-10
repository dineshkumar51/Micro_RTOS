				AREA |.text|,CODE,READONLY,ALIGN=2
			    THUMB
				PRESERVE8
			    EXTERN currentPt
			    EXPORT PendSV_Handler 
				EXPORT osSchedulerLaunch
				IMPORT choose_next
					
PendSV_Handler                 ;save r0,r1,r2,r3,r12,lr,pc,psr
 	CPSID	    F               ;Disable interrupts	I
	PUSH		{R4-R11}       ;save r4,r5,r6,r7,r8,r9,r10,r11
	LDR			R0,=currentPt  ;r0 points to currentPt
	LDR			R1,[R0]        ;r1=r0
	STR			SP,[R1]        ;updated stack point is saved in currentPt->stack
	;LDR			R1,[R1,#4]     ;r1 = currentPt->next
	;STR			R1,[R0]        ;currentPt = r1
	PUSH        {R0,LR}
	BL          choose_next
	pop         {R0,LR}
	LDR			R1,[R0]        ;r1=r0
	LDR 		SP,[R1]        ;sp = currentPt->stackPt
	POP         {R4-R11}       ;restoring registers r4,r5,r6,r7,r8,r9,r10,r11
	CPSIE       I               ;enable interrupts
	BX			LR	;restoring r0,r1,r2,r3,r12,lr,pc,psr
	
	
	
osSchedulerLaunch              ;To start the first task
    LDR			R0,=currentPt  ;r0 points to cuurentPt
	LDR			R2,[R0]        ;r2 = r0
	LDR         SP,[R2]        ;core SP = currentPt->stack
	POP         {R4-R11}
	POP         {R0-R3}
	POP         {R12}
	ADD         SP,SP,#4
	POP         {LR}
	ADD         SP,SP,#4
	CPSIE       I
	BX          LR
	
    ALIGN
    END
	
		