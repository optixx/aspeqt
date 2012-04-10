;=================================================;
;=          AspeQt Client utility v1.0            ;
;=        Copyright 2010 Ray. N. Ataergin		  ;
;=================================================;
; This program and its source is released under:  ;
;           GNU GENERAL PUBLIC LICENSE            ;
; Please read the details in the file LICENCE.TXT ;
; included with AspeQT distribution archive       ;
;=================================================;
; This version of the AspeQT Client will only     ;
; work with Spartados versions > 2.5 and with all ;
; versions of SpartaDos X.                        ;
;=================================================;

; Various OS Vectors
dosvec		=	$0A			
ciov		= 	$E456
siov		=	$E459

; Device Control Block
ddevic		=	$0300
dunit		=	$0301
dcomnd		=	$0302
dstats		=	$0303
dbuflo		=	$0304
dbufhi		=	$0305
dbytlo		=	$0308
dbythi		=   $0309
daux1		=	$030A
daux2		=	$030B

; I/O Control Block for Msg Display
iocb0		=	$0340
iccom		=	$0002
icbal		=	$0004
icbah		=	$0005
icbll		=	$0008
icblh		=	$0009

; Hardware Registers
portb		=	$D301

; SpartaDos Kernel Area (Page 7)
sparta		=	$700
spver		=	$701
skernel		=	$703
sdevic		=	$761
sdate		=	$77B

; SpartaDos User Area
comfnam		= 	33

; SpartaDos Vectors
vsettd		=	$FFC3
vtdon		=	$FFC6

*			= 	$6000

aStart
			jsr aEol
			
			ldx #<aID		
			ldy #>aID							
			jsr aPrint
						
			ldx #<aCpyl		
			ldy #>aCpyl							
			jsr aPrint
			jsr aEol			
			
			lda sparta			
			cmp #83				
			beq aVers			

			ldx #<aNotSP			
			ldy #>aNotSP			
			jsr aPrint
			pla
			pla
			jmp	aExit		
				
aVers
			lda spver			
			ldx #0			
aLoo1
			cmp aSpvI,x				
			beq aFound
			inx
			cpx #6
			bne	aLoo1
			jmp	adispV
			 
aFound
			lda aSpvo,x
			sta aVerno
						
adispV
			ldx #<aSparta		
			ldy #>aSparta		
			jsr aPrint

			ldy	#5
			ldx	#2
aLoo2			
			lda (dosvec),y
			sta zcrname,x
			dey
			dex
			bne aLoo2
aLoo5		jsr zcrname
			bne aAna
			lda #1
			cmp aIfSw			
			bne aNoSw					
			rts							

aAna
			lda #1
			sta aIfSw					
			ldy #comfnam+3				
			lda (dosvec),y
			cmp aTime					
			beq aDT1
			
aNoSw
			jsr aEol
			ldx #<aHlp01
			ldy #>aHlp01
			jsr aPrint
			jsr aEol						
			ldx #<aHlp02
			ldy #>aHlp02
			jsr aPrint						
			ldx #<aHlp03
			ldy #>aHlp03
			jsr aPrint
			ldx #<aHlp04
			ldy #>aHlp04
			jsr aPrint															
			rts																						

aDT1
			iny
			ldx #0
			stx aSlSo
aLoo6
			lda (dosvec),y
			cmp aTopt,x
			bne aLoo7 
			lda aTopt,x			
			sta aSlSo					
			jmp	aDT2
			
aLoo7
			inx
			lda #155
			cmp aTopt,x
			bne aLoo6
			jmp aNoSw					

aDT2						
			ldx #<aRecvn			
			ldy #>aRecvn		
			jsr aPrint	
	
			lda aDevic					
			sta ddevic
			lda aUnit
			sta dunit
			lda aComnd
			sta dcomnd
			lda aSend
			sta dstats
			lda #<aBuf
			sta dbuflo
			lda #>aBuf
			sta dbufhi
			lda abytlo
			sta dbytlo
			lda #0
			sta dbythi
			lda #0
			sta daux1
			lda aReq
			sta daux2
			jsr siov
					
			lda dstats
			cmp #1
			beq aSucc1
			
aErr1
			ldx #<aFail			
			ldy #>aFail			
			jsr aPrint			
			rts							

aSucc1				
			ldx #<aSucc			
			ldy #>aSucc				
			jsr aPrint
			
			lda aVerno
			cmp #4
			beq aKernel
						
aComtab
			ldx	#0
			ldy	#13
aLoo3
			lda aBuf,x
			sta (dosvec),y
			inx
			iny
			cpx #6
			bne aLoo3
aPortb
			lda	portb
			pha
			and #254
			sta	portb
			clc
			jsr vsettd
			pla
			sta portb												
			bcc aOK
			
aNOk
 			ldx #<aFailS			
			ldy #>aFailS				
			jsr aPrint		
			rts							
			
aOk
			ldx #<aSetd			
			ldy #>aSetd					
			jsr aPrint
			
			ldy #0
			lda aSlSo
			cmp #83						
			bne	aNotS
			rts							
aNotS
			cmp #79						
			bne aNotO
			ldy #1
aNotO
			lda	portb
			pha
			and #254
			sta	portb
			clc
			jsr vtdon
			pla
			sta portb
			bcs aErr2
			rts							
			
aErr2
			jsr aEol
			ldx #<aFailT1			
			ldy #>aFailT1			
			jsr aPrint			
			ldx #<aFailT2			
			ldy #>aFailT2			
			jsr aPrint			
			rts							
						
aKernel								
			ldx	#0
aLoo4
			lda aBuf,x
			sta sdate,x
			inx
			cpx #6
			bne aLoo4
			ldy	#$65
			lda #$10
			sta sdevic
			jmp skernel
			
			cmp #0
			beq aOK
			jmp aNOK
			
aEol
			ldx #<aCr
			ldy #>aCr
			jsr aPrint
            rts
			
aPrint
			txa
			ldx #$00		
			sta	iocb0+icbal,x
			tya			
			sta iocb0+icbah,x
			lda aCmd
			sta iocb0+iccom,x						
			lda aBlen				
			sta iocb0+icbll,x
			jsr ciov
			rts
			
aExit		
			ldx #<aExi		
			ldy #>aExi				
			jsr aPrint
			lda #$05
			sta iocb0+iccom,x
			lda #$01
			sta iocb0+icbll,x
			jsr ciov
			jmp (dosvec)					
			rts
zcrname
			jmp $FFFF

aID		.byte "AspeQt Client",155
aCpyl	.byte "Copyright (C) 2012 Ray N Ataergin", 155
aNotSP	.byte "Only SpartaDOS v2.5 and up supported!.",155
aSparta	.byte "SpartaDos v"
aVerNo	.byte "?.xx"
aRunin  .byte " detected", 155
aRecvn	.byte "Polling server for Date/Time..", 155
aSucc	.byte "Data received from server..", 155
aSetd	.byte "Time/Date is set.", 155
aFail	.byte "Server offline or did not respond!", 155
aFailS	.byte "Failed to set Time/Date!", 155
aFailT1	.byte "Failed to turn TD line ON/OFF", 155
aFailT2 .byte "Check if TD driver is loaded", 155
aExi	.byte "Press RETURN to exit", 155
aHlp01	.byte "Usage: ASPECL TS|TO|TF",155
aHlp02	.byte "Where  TS: Set Time/Date",155
aHlp03	.byte "       TO: Set Time/Date, TD ON", 155
aHlp04  .byte "       TF: Set Time/Date, TD OFF", 155
aSpvI	.byte "%23@CD"
aSpvO   .byte "233444"
aIfSw   .byte $0									
aSlSo	.byte $0																		
aTime	.byte "T"									
aTopt	.byte "SOF", 155							
aDevic	.byte $46									
aUnit	.byte $01									
aComnd	.byte $93									
aSend	.byte $40									
aRecv	.byte $20									
aBytlo	.byte $06									
aCmd	.byte $09									
aBlen	.byte $FF									
aBuf	.byte $FF, $FF, $FF, $FF, $FF, $FF 			
aReq    .byte $01									
													
aCr		.byte 155
		
*			=	$02E0								
		.word	aStart

