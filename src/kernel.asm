    ; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "print.mac"

%define CS_RING_0 0x0040
%define CS_RING_3 0x004B 
%define DS_RING_0 0x0050
%define DS_RING_3 0x005B
%define VS_RING_0 0x0060

%define TSS_INIT 0x0068
%define TSS_IDLE 0x0070
%define TSS_A_0  0x0078
%define TSS_A_1  0x0080
%define TSS_A_2  0x0090
%define TSS_A_3  0x0098
%define TSS_A_4  0x00A0
%define TSS_B_0  0x00A8
%define TSS_B_1  0x00B0
%define TSS_B_2  0x00B8
%define TSS_B_3  0x00C0
%define TSS_B_4  0x00C8


%define ESP_INIT 0x25000
%define EBP_INIT 0x25000

%define FLAG_PG 0x80000000

%define MMU_P 0x1
%define MMU_W 0x2
%define MMU_R 0x0
%define MMU_U 0x4
%define MMU_S 0x0

global start

extern idt_init
extern pic_reset
extern pic_enable
extern GDT_DESC
extern IDT_DESC
extern mmu_init_kernel_dir
extern mmu_init
extern mmu_map_page
extern mmu_unmap_page
extern mmu_init_task_dir   
extern tss_init 
extern tss_task_init
extern tss_tasks_init
extern sched_init
extern game_init

BITS 16
;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db     'Iniciando kernel en Modo Real'
start_rm_len equ    $ - start_rm_msg

start_pm_msg db     'Iniciando kernel en Modo Protegido'
start_pm_len equ    $ - start_pm_msg

fondo db  0x22

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

BITS 32
pintar_mapa:
    push    eax
    push    ebx
    push    ecx
    
    mov 	ax,	VS_RING_0
	mov 	gs, ax
    mov     ecx, 6400               ;ecx = Pixel * 80 * 40 (pixel = 2 byte) 
    mov 	ebx, 0					;ebx = contador
    mov     al, [fondo]        ;al  = byte color del mapa

    .ciclo_mapa:
        mov     byte [gs:ebx], 0    ;Seteo el caracter nulo
        inc     ebx                 ;Avanzo al color
        mov     [gs:ebx], al        ;Pinto la casilla
        inc     ebx                 ;Avanza a proxima casilla
        cmp 	ecx, ebx			;Chequeo si termine
        jnz    .ciclo_mapa

    pop     ecx 
    pop     ebx
    pop     eax
    ret

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli
    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0

    ; Habilitar A20
    call A20_disable
    call A20_check
    call A20_enable
    call A20_check
    
    ; Cargar la GDT
    lgdt [GDT_DESC]

    ; Setear el bit PE del registro CR0
    mov eax, cr0
    or  eax, 0x1
    mov cr0, eax
    
    ; Saltar a modo protegido
    jmp CS_RING_0:modo_protegido

BITS 32
modo_protegido:
    ; Establecer selectores de segmentos
    mov ax, DS_RING_0
    mov ds, ax           
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Establecer la base de la pila
    mov ebp, EBP_INIT
    mov esp, ESP_INIT

    ; Imprimir mensaje de bienvenida
    print_text_pm start_pm_msg, start_pm_len, 0x07, 0, 0
    
    ; Inicializar pantalla
    call pintar_mapa
    
    ; Inicializar el manejador de memoria
    call mmu_init
 
    ; Inicializar el directorio de paginas
    call  mmu_init_kernel_dir

    ; Cargar directorio de paginas
    mov cr3, eax

    ; Habilitar paginacion
    mov eax, cr0
    mov edx, FLAG_PG
    or  eax, edx
    mov cr0, eax
    
    ; Inicializar tss
    call tss_init

    ; Inicializar el scheduler
    call sched_init

    ; Inicializar la IDT
    call idt_init
    
    ; Cargar IDT
    lidt [IDT_DESC]

    ; Configurar controlador de interrupciones
    call pic_reset
    call pic_enable
    
    ; Cargar tarea inicial
    mov ax, TSS_INIT
    ltr ax

    call game_init
    
    ; Activar interrupciones
    sti


    ; Saltar a la primera tarea: Idle
    xchg bx, bx
    jmp TSS_IDLE:0
    
    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
