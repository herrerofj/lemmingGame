    w; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
;
; Definicion de rutinas de atencion de interrupciones

%include "print.mac"

BITS 32

sched_task_offset:     dd 0x0000
sched_task_selector:   dw 0x0000

%define TSS_IDLE 0x0070
%define scancode_Break  0x80

;; Debug
debug_mode: db 0x00
showing_debug: db 0x00

%define y_key 0x15
%define ON 0x01
%define OFF 0x00

extern imprimir_debugger
extern restor_map

;; prints
extern print_except
extern print_scanCode

;; PIC
extern pic_finish1

;; Sched
extern sched_next_task
extern sched_kill_task
extern sched_control_lemmings

;; Game
extern print_scores
extern game_tick
extern screen_fresh
extern game_cheack
;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;
%macro ISR 1
global _isr%1

_isr%1:
    
    xchg bx, bx
    cmp byte [debug_mode], OFF 
    jz .desalojar

    mov byte [showing_debug], ON
    pushad       ; Registros de proposito general (ver Manual)
    str ax       ; TR actual
    push eax 
    mov eax, cr0 ; CR0 actual
    push eax 
    mov eax, cr2 ; CR2 actual
    push eax
    mov eax, cr3 ; CR3 actual
    push eax
    mov eax, cr4 ; CR4 actual
    push eax
    push %1      ; Numero de Excepcion
    push ds      ; Segmento datos
    push es      ; Segmento es
    push fs      ; Segmento fs
    push gs      ; Segmento gs
    push esp     ; Tope de la pila actual
    call imprimir_debugger
    pop esp
    pop gs
    pop fs 
    pop es  
    pop ds 
    add esp, 4*6 
    popad 

    .desalojar:
        call sched_kill_task
        
        ;cmp byte [showing_debug], ON 
        ;jz .idle_jmp

    .idle_jmp:
        str ax 
        cmp ax, TSS_IDLE
        jz .end
        jmp TSS_IDLE:0x0
    .end:
    popad
    iret
    jmp $

%endmacro


;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1 
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8 
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19

;; Rutina de atención de PAGE FAULT
;; -------------------------------------------------------------------------- ;;
%define effective_map   1
extern mmu_map_compartida
global _isr14
_isr14:
    ;xchg bx, bx
    pushad 
    mov eax, cr2
    push eax
    call mmu_map_compartida
    add esp, 4
    cmp eax, effective_map
    jnz .desalojar
    
    popad
    add esp, 4  ;Elimino el error code
    ;   xchg bx, bx
    iret

    .desalojar:
        xchg bx, bx
        call sched_kill_task

        jmp TSS_IDLE:0x0
        jmp $


;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32
_isr32:
    pushad
    call pic_finish1   ;Indica que la interrupcion fue antendida
    cmp byte [showing_debug], ON
    jz .fin
    ;jmp .fin

    call next_clock    ;Imprimir el reloj del sistema

    call game_tick
    ;call screen_fresh
    call game_cheack

    call print_scores

    call sched_control_lemmings
    cmp eax, 1
    jz .idle_jmp
    call sched_next_task

    str bx 
    ;mov ax, TSS_IDLE
    cmp ax, bx 
    jz .fin 
    ;xchg bx, bx
    mov word [sched_task_selector], ax 
    jmp far [sched_task_offset]

    .fin:
        popad
        iret
    .idle_jmp:
        str ax
        cmp ax, TSS_IDLE
        jz .end
        jmp TSS_IDLE:0
    .end:
        popad 
        iret
        jmp $


;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
_isr33:
    xchg bx, bx
    pushad
    
    call pic_finish1   ;Indica que la interrupcion fue antendida

    in al, 0x60        ;Captura una tecla 

    cmp al, y_key
    jz .switch_debug
    jmp .fin
    xchg bx, bx
    .switch_debug:

        cmp byte[debug_mode], OFF
        jz .encender
        jmp .apagar
    
    .encender:
        xchg bx, bx
        mov byte[debug_mode], ON
        jmp .fin

    .apagar:
        cmp byte[showing_debug], ON 
        jz .reanudar
        mov byte[debug_mode], OFF
        jmp .fin

    .reanudar:
        mov byte[showing_debug], OFF
        call restor_map
        jmp .fin

    .fin:
    popad
    iret

;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
extern move_main
global _isr88
_isr88:
    pushad
    push eax
    call move_main
    ; Por alguna razon, lo retorna por eax, desconozco, pense que retornaba
    ;  por pila
    mov [esp+8*4], eax   ;esp+7*4 = eax_ret, pero se pusheo el parametro,
    add esp, 4           ; entonces esp+8*4 = eax_ret
    jmp TSS_IDLE:0
    popad
    iret

extern explode_main
global _isr98
_isr98:
    pushad
    call explode_main
    call sched_kill_task
    jmp TSS_IDLE:0
    popad
    iret

extern bridge_main
global _isr108
_isr108:
    pushad
    push eax
    call bridge_main
    add esp, 4
    call sched_kill_task
    jmp TSS_IDLE:0
    popad
    iret

;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:           dd 0x00000000
isrClock:            db '|/-\'
next_clock:
        pushad
        inc DWORD [isrNumber]
        mov ebx, [isrNumber]
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrNumber], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock
                print_text_pm ebx, 1, 0x0f, 49, 79
                popad
        ret

global next_simbol

res: db " "

next_simbol:
        ;xchg bx, bx
        pushad 
        mov eax, [esp+9*4] ; esp+7*4 = eax, esp+8*4 = eip_3, esp+9*4 = parametro
        add eax, isrClock
        xor ebx, ebx
        mov bl, [eax]
        mov [esp+9*4], ebx
        popad
        mov eax, [esp+4]
        ret

global game_end_ASM

game_end_ASM:
    jmp $
