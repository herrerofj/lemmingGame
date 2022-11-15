/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de las rutinas asociadas al juego.
*/

#ifndef __GAME_H__
#define __GAME_H__

#include "types.h"

typedef struct str_pos {
  uint8_t x;
  uint8_t y;
}__attribute__((__packed__)) pos_t;


void game_tick(void);
void game_init(void); 
void screen_fresh(void);
void kill_clock(uint32_t team, int8_t task);
void pos_init(pos_t *pos, uint8_t team, int8_t task);
pos_t *get_position(int8_t team, int8_t task);

typedef enum direction_e {
  DIR_UP = 0,
  DIR_RIGHT = 1,
  DIR_DOWN = 2,
  DIR_LEFT = 3
} direction_e;

typedef enum move_result_e {
  MOVE_RESULT_SUCCESS = 0,
  MOVE_RESULT_WALL = 1,
  MOVE_RESULT_LAKE = 2,
  MOVE_RESULT_BORDER = 3,
  MOVE_RESULT_LEMMING = 4
} move_result_e;




#endif //  __GAME_H__
