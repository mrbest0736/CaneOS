; CaneOS Bootloader
; Minimal bootloader: prints a message and halts

ORG 0x7C00                ; Boot sector load address

MOV AH, 0x0E              ; BIOS teletype function
MOV BX, 0x0007            ; Page number and text attribute
MOV SI, msg               ; Point to message
call print_string

JMP $                     ; Infinite loop (halt)

print_string:
	LODSB                 ; Load byte at SI into AL, increment SI
	OR  AL, AL            ; Check for null terminator
	JZ  done              ; If zero, end
	INT 0x10              ; BIOS: print character in AL
	JMP print_string      ; Repeat
done:
	RET

msg DB 'CaneOS Bootloader', 0

TIMES 510-($-$$) DB 0     ; Pad to 510 bytes
DW 0xAA55                 ; Boot sector signature
