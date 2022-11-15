/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler.
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "types.h"
#include "defines.h"

void sched_init();
uint16_t sched_next_task();
void sched_kill_task();
int8_t sched_actual_team(void);
int8_t sched_actual_task(uint32_t team);
void sched_explode(uint8_t team, uint8_t task);
uint8_t sched_control_lemmings(void);


#endif //  __SCHED_H__
