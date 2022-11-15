/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones de impresion por pantalla.
*/

#include "screen.h"
#include "mapa.h"
#include "i386.h"
#include "defines.h"

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  for (i = 0; text[i] != 0; i++) {
    p[y][x].c = (uint8_t)text[i];
    p[y][x].a = (uint8_t)attr;
    x++;
    if (x == VIDEO_COLS) {
      x = 0;
      y++;
    }
  }
}

void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  uint32_t i;
  uint8_t letras[16] = "0123456789";

  for (i = 0; i < size; i++) {
    uint32_t resto = numero % 10;
    numero = numero / 10;
    p[y][x + size - i - 1].c = letras[resto];
    p[y][x + size - i - 1].a = attr;
  }
}

void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  uint8_t hexa[8];
  uint8_t letras[16] = "0123456789ABCDEF";
  hexa[0] = letras[(numero & 0x0000000F) >> 0];
  hexa[1] = letras[(numero & 0x000000F0) >> 4];
  hexa[2] = letras[(numero & 0x00000F00) >> 8];
  hexa[3] = letras[(numero & 0x0000F000) >> 12];
  hexa[4] = letras[(numero & 0x000F0000) >> 16];
  hexa[5] = letras[(numero & 0x00F00000) >> 20];
  hexa[6] = letras[(numero & 0x0F000000) >> 24];
  hexa[7] = letras[(numero & 0xF0000000) >> 28];
  for (i = 0; i < size; i++) {
    p[y][x + size - i - 1].c = hexa[i];
    p[y][x + size - i - 1].a = attr;
  }
}

void print_except(uint32_t exceptCode){
  short x = 10;
  short y = 10;
  switch (exceptCode){
    case 0: 
      print("#DE Error de division. [0]", x,y, 0x02);
    break;
    case 1:
      print("#DB Error reservado. [1]", x, y, 0x02);
    break;
    case 2:
      print("Interrupcion NMI. [2]", x, y, 0x02);
    break;
    case 3:
      print("#BP BreakPoint. [3]", x, y, 0x02);
    break;
    case 4:
      print("#OF Error de Overflow. [4]", x, y, 0x02);
    break;
    case 5:
      print("#BR Error: Rango limite excedido. [5]", x, y, 0x02);
    break;
    case 6:
      print("#UD Codigo de operacion invalida. [6]", x, y, 0x02);
    break;
    case 7:
      print("#NM Coprocesador no disponible. [7]", x, y, 0x02);
    break;
    case 8:
      print("#DF Falla doble. [8]", x, y, 0x02);
    break;
    case 9:
      print("Segmento de coprocesador excedido. [9]", x, y, 0x02);
    break;
    case 10:
      print("#TS TSS invalido. [10]", x, y, 0x02);
    break;
    case 11:
      print("#NP Segmento no presente. [11]", x, y, 0x02);
    break;
    case 12:
      print("#SS Falla en el segmento de stack. [12]", x, y, 0x02);
    break;
    case 13:
      print("#GP Error de proteccion general. [13]", x, y, 0x02);
    break;
    case 14:
      print("#PF Error de pagina. [14]", x, y, 0x02);
    break;
    case 16:
      print("#MF Error en la FPU. [16]", x, y, 0x02);
    break;
    case 17:
      print("#AC Error de alineacion. [17]", x, y, 0x02);
    break;
    case 18:
      print("#MC Machine check. [18]", x, y, 0x02);
    break;    
    case 19:
      print("#XF Excepcion del punto flotante en SIMD. [19]", x, y, 0x02);
    break;
  }
}  

void print_scanCode(uint8_t scanCode){
  if (scanCode<=11){
    print("Tecla ",  10, 10, 0x0A);
    if (scanCode == 11){
      print("0", 20, 10, 0x0A);
    }else{
      print_hex(scanCode-1, 1, 20, 10, 0x0A);
    }
  }else{
    print_hex(scanCode, 1, 15, 10, 0x0A);
    }
}

void print_teams(void){
    // Amalin 
    /* Numero de leming y clock*/
    for(uint8_t i = 0; i < 5 ; i++){
        print_dec(i+1, 2, 19+i*4, 43, 0x04);
        print("x", 19+i*4, 45, 0x0F);
    }
    // Betarote 
    /* Numero de leming y clock*/
    for(uint8_t i = 0; i < 5 ; i++){
        print_dec(i+1, 2, 43+i*4, 43, 0x01);
        print("x", 43+i*4, 45, 0x0F);
    }    
}

void print_clock(char simbol, uint32_t team, uint32_t task){
    //breakpoint();
    if(team == AMALIN){
        print(&simbol, 19+task*4, 45, 0x0F);
    }else{
        print(&simbol, 43+task*4, 45, 0x0F);
    }
}

void print_ground(pos_t *pos){
    ca celda = get_celda(pos);
    set_simbol(pos, celda);
}

void print_leming(pos_t *pos, char team){
    if(team == AMALIN){
        set_simbol(pos,(ca ){*"A",0x24});
    }else{
        set_simbol(pos,(ca ){*"B",0x21});
    }
    return;
}

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  uint32_t f;
  uint32_t c;
  for (f = fInit; f < fInit + fSize; f++) {
    for (c = cInit; c < cInit + cSize; c++) {
      p[f][c].c = character;
      p[f][c].a = attr;
    }
  }
}

uint32_t tienen_codigo[20] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0};
/*
  tienen_codigo[8] = 1;
  tienen_codigo[10] = 1;
  tienen_codigo[11] = 1;
  tienen_codigo[12] = 1;
  tienen_codigo[13] = 1;
  tienen_codigo[14] = 1;
  tienen_codigo[17] = 1;
*/  
void imprimir_debugger(uint32_t *p_stack){
  breakpoint();
  save_map();
  uint8_t i = 0;
  uint16_t gs = (uint16_t)p_stack[i++];
  uint16_t fs = (uint16_t)p_stack[i++];
  uint16_t es = (uint16_t)p_stack[i++];
  uint16_t ds = (uint16_t)p_stack[i++];
  uint32_t int_number = (uint32_t)p_stack[i++];
  uint32_t cr4 = (uint32_t)p_stack[i++];
  uint32_t cr3 = (uint32_t)p_stack[i++];
  uint32_t cr2 = (uint32_t)p_stack[i++];
  uint32_t cr0 = (uint32_t)p_stack[i++];
  uint16_t task_selector = (uint16_t)p_stack[i++];
  uint32_t edi = (uint32_t)p_stack[i++];
  uint32_t esi = (uint32_t)p_stack[i++];
  uint32_t ebp = (uint32_t)p_stack[i++];
  i++;
  uint32_t ebx = (uint32_t)p_stack[i++];
  uint32_t edx = (uint32_t)p_stack[i++];
  uint32_t ecx = (uint32_t)p_stack[i++];
  uint32_t eax = (uint32_t)p_stack[i++];
  uint16_t stack_error = 0xDEAD;
  if(tienen_codigo[int_number] == 1){
    stack_error = (uint16_t)p_stack[i++];
  }
  uint32_t eip = (uint32_t)p_stack[i++];
  uint16_t cs = (uint16_t)p_stack[i++];
  uint32_t eflags = (uint32_t)p_stack[i++];
  uint32_t esp = (uint32_t)p_stack[i++];
  uint16_t ss = (uint16_t)p_stack[i++];

  task_selector += 0;
  /*
  uint32_t stack1;
  uint32_t stack2;
  uint32_t stack3;
  uint32_t *p_esp = (uint32_t *)esp;

  // breakpoint();
  // uint32_t modulo_stack = esp%PAGE_SIZE;
  if((TASK_STACK_BASE-0x1000) < esp && esp < TASK_STACK_BASE){
    stack1 = 0xDEADBEEF;
    stack2 = 0xDEADBEEF;
    stack3 = 0xDEADBEEF;
  }else{
    stack1 = p_esp[0];
    stack2 = 0xDEADBEEF;
    stack3 = 0xDEADBEEF;
  }*/


  //Cuadro negro
  screen_draw_box(0, 10, 39, 60, 0, 0x07);

  //Numero de interrupcion
  print_dec(int_number, 2, 20, 5, 0x07);

  i = 10;
  print("eax", 15,i++,0x07);
  print("ebx", 15,i++,0x07);
  print("ecx", 15,i++,0x07);
  print("edx", 15,i++,0x07);
  print("esi", 15,i++,0x07);
  print("edi", 15,i++,0x07);
  print("ebp", 15,i++,0x07);
  print("esp", 15,i++,0x07);
  print("eip", 15,i++,0x07);
  print("cs", 16,i++,0x07);
  print("ds", 16,i++,0x07);
  print("es", 16,i++,0x07);
  print("fs", 16,i++,0x07);
  print("gs", 16,i++,0x07);
  print("ss", 16,i++,0x07);
  i++;
  print("eflags", 15,i++,0x07);

  i = 10;

  print_hex(eax, 8, 19, i++, 0x02);
  print_hex(ebx, 8, 19, i++, 0x02);
  print_hex(ecx, 8, 19, i++, 0x02);
  print_hex(edx, 8, 19, i++, 0x02);
  print_hex(esi, 8, 19, i++, 0x02);
  print_hex(edi, 8, 19, i++, 0x02);
  print_hex(ebp, 8, 19, i++, 0x02);
  print_hex(esp, 8, 19, i++, 0x02);
  print_hex(eip, 8, 19, i++, 0x02);
  print_hex(cs, 8, 19, i++, 0x02);
  print_hex(ds, 8, 19, i++, 0x02);
  print_hex(es, 8, 19, i++, 0x02);
  print_hex(fs, 8, 19, i++, 0x02);
  print_hex(gs, 8, 19, i++, 0x02);
  print_hex(ss, 8, 19, i++, 0x02);
  i++;
  print_hex(eflags, 8, 22, i++, 0x02);

  i = 10;

  print("cr0", 30,i++,0x07);
  print("cr2", 30,i++,0x07);
  print("cr3", 30,i++,0x07);
  print("cr4", 30,i++,0x07);
  if(tienen_codigo[int_number] == 1){
    print("err", 30,i++,0x07);
  }
  i++;
  print("stack", 30,i++,0x07);

  i = 10;

  print_hex(cr0, 8, 34, i++, 0x02);
  print_hex(cr2, 8, 34, i++, 0x02);
  print_hex(cr3, 8, 34, i++, 0x02);
  print_hex(cr4, 8, 34, i++, 0x02);

  if(tienen_codigo[int_number] == 1){
    print_hex(stack_error, 8, 34, i++, 0x02);
  }
  /*
  i+=2;
  print_hex(stack1, 8, 30, i++, 0x02);
  print_hex(stack2, 8, 30, i++, 0x02);
  print_hex(stack3, 8, 30, i++, 0x02);

  i = 10;
  print("backtrace", 45, i++, 0x07);
  // breakpoint();
  uint32_t *p_ebp = (uint32_t *)ebp; 
  while((uint32_t)p_ebp >= task_mem_start && (uint32_t)p_ebp < task_mem_end - 4){
    uint32_t iret = p_ebp[1];
    print_hex(iret, 8, 45, i++, 0x02);
    p_ebp = (uint32_t *)p_ebp[0];
  }
  */
}
