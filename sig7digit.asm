; Assembly file for seg7DigitUpdate() function
; Lab 3, ECE 367, fall 2016
; Created by Zhao Zhang
; Edited by Kenny Zhou

; This assembly file takes in 4 parameters from the main.c file and encodes them into
; four, 7 segment digits to display on the Grove 4-digit Display

; The 7-segment coding table
; There are 7 bits for the 7 segments, set the remaining MSB to 1 to have the colon on
					.data
g_seg7Coding		.byte 	10111111b			; digit 0
					.byte 	10000110b 			; digit 1
					.byte 	11011011b			; digit 2
					.byte	11001111b			; digit 3
					.byte	11100110b			; digit 4
					.byte	11101101b			; digit 5
					.byte	11111101b			; digit 6
					.byte	10000111b			; digit 7
					.byte	11111111b			; digit 8
					.byte	11101111b			; digit 9
					.byte 	10000000b			;blank

; Output code, equivalent to declare "uint8_t code[4]" as static variable
					.bss 	g_code, 4

; Function prototype: seg7DigitUpdate(uint8_t c1, uint8_t c2, uint8_t s1, uint8_t s2);
; The function encoded the digits and calls seg7Update() to display the digits on
; the 4-digit 7-segment display. The colon should always be on.
					.text
					.global seg7DigitUpdate		; make this symbol visible to outside
					.global seg7Update			; use this global symbol

; TI assembler requires that symbols in data/bss sections be re-declared in code section before use
seg7Coding			.field	g_seg7Coding
code				.field  g_code

seg7DigitUpdate		PUSH 	 {lr}
					; seems like colon turns off when first bit of the 4 digits is 0
					; r9 -> seg7Coding[i] <- r0

					; 4 digit number: d4,d3,d2,d1
					MOV 	r4, r0		; r4 = d1
					MOV 	r5, r1		; r5 = d2
					MOV 	r6, r2		; r6 = d3
					MOV 	r7, r3		; r7 = d4

					LDR		r0, code
					LDR 	r9, seg7Coding

;					CMP 	r7, #0				; skip storing 7 segment byte into memory if thousands digit is 0 (dont display)
;					BEQ		skip				; otherwise display thousands digit
					LDR		r7, [r9, r7]		; r7 = thousands digit of digitized number, offset by this to get 7 segment equivalent
					STRB 	r7, [r0, #3] 		; thousands digit
;					B 		continue			; must skip compare statement below if thousands digit is nonzero

;skip				LDR		r7, [r9, #10]		; load and store nothing (blank from segment table) into thousands digit
;					STRB	r7, [r0, #3]
;					CMP 	r6, #0				; then check if hundreds digit is 0
;					BEQ 	skip2
continue			LDR		r6, [r9, r6]		; r6 = hundreds digit of digitized number, offset by this to get 7 segment equivalent
					STRB 	r6, [r0, #2] 		; hundreds digit
;					B 		continue2			; must skip compare statement below if hundreds digit is nonzero

;skip2				LDR		r6, [r9, #10]		; load and store nothing (blank from segment table) into hundreds digit
;					STRB	r6, [r0, #2]
;					CMP 	r5, #0				; then check is tens digit is 0
;					BEQ		skip3
continue2			LDR		r5, [r9, r5]		; r5 has the tens digit of digitized number, offset by this to get 7 segment equivalent
					STRB 	r5, [r0, #1] 		; tens digit
;					B continue3

;skip3				LDR		r5, [r9, #10]		; load and store nothing (blank from segment table) into tens digit
;					STRB	r5, [r0, #1]
continue3			LDR		r4, [r9, r4]		; r4 has the ones digit of digitized number, offset by this to get 7 segment equivalent
					STRB 	r4, [r0, #0] 		; store 7 segment encoding of this digit into the ones digit of 7 segment display

					BL seg7Update

exit				POP		{lr}
					BX 		lr

					.end


