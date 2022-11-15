/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion del mapa
*/

#ifndef __MAPA_H__
#define __MAPA_H__
extern const char mapa_original[40][80];
#endif //  __MAPA_H__

#include "defines.h"
#include "types.h"
#include "screen.h"
#include "game.h"

void load_map(void);

char get_simbol(pos_t *pos);
void set_simbol(pos_t *pos, ca celda);
void break_wall(pos_t *pos);
void make_bridge(pos_t *pos);
ca get_celda(pos_t *pos);
void print_A_wins(void);
void print_B_wins(void);
void save_map(void);
void restor_map(void);