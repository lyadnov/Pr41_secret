;
; file: flash_asm.s
;

;----constants-----
WR = 15;

.text
.global _flash_read_word_asm
.global _flash_erase_block_asm
.global _flash_write_word_asm
.global _flash_write_row_asm
.global _flash_write_block_asm

;------------------------------
_flash_read_word_asm:
	mov     #_flash_result,w1
	mov     _flash_addr,w0
	tblrdl  [w0],[w1++]
	tblrdh [w0],[w1]
	return

;------------------------------
_flash_erase_block_asm:
	mov     _flash_addr,w0
	tblwtl  w0, [w0]
	;записываем теперь Program Latch в Program Memory
	disi    #5          ; Block all interrupts with priority <7 for next 5 instructions
	mov     #0x55,w0    ; 
	mov     w0, NVMKEY  ; Write the 55 key
	mov     #0xAA, w1   ;
	mov     w1, NVMKEY  ; Write the AA key
	bset    NVMCON, #WR ; Start the erase sequence
	nop                 ; Insert two NOPs after the
	nop                 ; erase command is asserted
	btsc    NVMCON,#WR
	bra     $-2    
	return

;------------------------------
_flash_write_word_asm:
	 mov    #_flash_result,w0
	mov     [w0++],w1
	mov     _flash_addr,w2
	tblwtl  w1, [w2]
	mov     [w0],w1   
	tblwth  w1, [w2]
	;записываем теперь Program Latch в Program Memory
	disi    #5          ; Block all interrupts with priority <7 for next 5 instructions
	mov     #0x55,w0    ; 
	mov     w0, NVMKEY  ; Write the 55 key
	mov     #0xAA, w1   ;
	mov     w1, NVMKEY  ; Write the AA key
	bset    NVMCON, #WR ; Start the erase sequence
	nop                 ; Insert two NOPs after the
	nop                 ; erase command is asserted
	btsc    NVMCON,#WR
	bra     $-2    
	return

;------------------------------
_flash_write_row_asm:
	mov    _flash_result,w0
	mov     _flash_addr,w2
	mov     #64,w3

flash_write_row_loop:
	;записываем cлово 3байта
	mov.b   [w0++],w1 
	tblwtl.b w1, [w2++]
	mov.b   [w0++],w1 
	tblwtl.b w1, [w2--]
	mov.b   [w0++],w1 
	tblwth.b w1, [w2]
	add     #2,w2
	sub     #1,w3
	bra     nz,flash_write_row_loop 
	
	;записываем теперь Program Latch в Program Memory
	disi    #5          ; Block all interrupts with priority <7 for next 5 instructions
	mov     #0x55,w0    ; 
	mov     w0, NVMKEY  ; Write the 55 key
	mov     #0xAA, w1   ;
	mov     w1, NVMKEY  ; Write the AA key
	bset    NVMCON, #WR ; Start the erase sequence
	nop                 ; Insert two NOPs after the
	nop                 ; erase command is asserted    
	btsc    NVMCON,#WR
	bra     $-2
		
	return

;------------------------------
_flash_write_block_asm:
	mov    _flash_result,w0 ;source
	mov     _flash_addr,w2  ;destination
	mov     #8,w4           ;block loop counter

flash_write_block_loop:
	mov     #64,w3          ;row loop couner

flash_write_row_loop2:
	;записываем cлово 3байта
	mov.b   [w0++],w1       ;w1-промежуточный буфер 
	tblwtl.b w1, [w2++]
	mov.b   [w0++],w1 
	tblwtl.b w1, [w2--]
	mov.b   [w0++],w1 
	tblwth.b w1, [w2]
	add     #2,w2
	sub     #1,w3
	bra     nz,flash_write_row_loop2 
	;записываем теперь Program Latch в Program Memory
	disi    #5          ; Block all interrupts with priority <7 for next 5 instructions
	mov     #0x55,w3    ; 
	mov     w3, NVMKEY  ; Write the 55 key
	mov     #0xAA, w1   ;
	mov     w1, NVMKEY  ; Write the AA key
	bset    NVMCON, #WR ; Start the erase sequence
	nop                 ; Insert two NOPs after the
	nop                 ; erase command is asserted    
	btsc    NVMCON,#WR
	bra     $-2
	
	sub     #1,w4
	bra     nz,flash_write_block_loop 
	
	return

.end

