/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

/* Definicion de la pantalla */
#define VIDEO_FILS 50
#define VIDEO_COLS 80

#include "colors.h"
#include "defines.h"
#include "stdint.h"
#include "game.h"
#include "mapa.h"



void print(const char* text, uint32_t x, uint32_t y, uint16_t attr);
void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr);
void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr);

void print_except(uint32_t exceptCode);

void print_scanCode(uint8_t scanCode);

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr);

void screen_draw_layout(void);

void print_teams(void);

void print_clock(char simbol, uint32_t team, uint32_t task);  

void print_ground(pos_t *pos);

void print_leming(pos_t *pos, char team);

void imprimir_debugger(uint32_t *p_stack);

#endif //  __SCREEN_H__
