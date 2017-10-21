global outb
global outw
global inb
global inw

section .txt

; void outb(uint16_t port, uint8_t data)
outb:
	enter	0, 0
	mov		dx,[ebp+8]      ; Port
	mov     al,[ebp+12]     ; Data
	out     dx, al
	leave
	ret

; void outw(uint16_t port, uint16_t data)
outw:
	enter 	0, 0
	mov     dx,[ebp+8]
	mov     ax,[ebp+12]
	out     dx,ax
	leave
	ret

; uint8_t inb(uint16_t port)
inb:
	enter 0, 0
	mov     dx,[ebp+8]      ; Port
	in      al, dx
	leave
	ret

; uint16_t inw(uint16_t port)
inw:
	enter 0, 0
	mov     dx,[ebp+8]      ; Port
	in      ax,dx
	leave
	ret