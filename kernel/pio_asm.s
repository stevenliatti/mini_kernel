global outb
global outw
global inb
global inw

section .txt

outb:
	mov		word dx, [esp+4]      ; Port
	mov     byte al, [esp+8]      ; Data
	out     dx, al
	ret

outw:
	mov     word dx, [esp+4]
	mov     word ax, [esp+8]
	out     dx, ax
	ret

inb:
	mov     word dx, [esp+4]      ; Port
	in      byte al, dx
	ret

inw:
	mov     word dx, [esp+4]      ; Port
	in      word ax, dx
	ret
