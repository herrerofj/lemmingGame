/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del scheduler
*/

#include "sched.h"
#include "defines.h"
#include "i386.h"
#include "game.h"
#include "mapa.h"
#include "tss.h"

uint16_t tareas_amalin[PLAYER_TASKS];
uint16_t tareas_betarote[PLAYER_TASKS];

uint8_t  tareas_vivas_amalin[PLAYER_TASKS];
uint8_t  tareas_vivas_betarote[PLAYER_TASKS];

uint8_t  vivas_amalin;
uint8_t  vivas_betarote;

uint32_t turn_amalin_created[PLAYER_TASKS];
uint32_t turn_betarote_created[PLAYER_TASKS];


uint32_t last_player = BETAROTE;
uint32_t last_amalin_task = -1;
uint32_t last_betarote_task = -1;
  
uint32_t clock_actual;


void sched_init(void) {

  tareas_amalin[0] = TSS_A_0;
  tareas_amalin[1] = TSS_A_1;
  tareas_amalin[2] = TSS_A_2;
  tareas_amalin[3] = TSS_A_3;
  tareas_amalin[4] = TSS_A_4;

  tareas_betarote[0] = TSS_B_0;
  tareas_betarote[1] = TSS_B_1;
  tareas_betarote[2] = TSS_B_2;
  tareas_betarote[3] = TSS_B_3;
  tareas_betarote[4] = TSS_B_4;

  for(int i = 0; i < PLAYER_TASKS; i++){
    tareas_vivas_amalin[i] = 0;
    tareas_vivas_betarote[i] = 0;
  }
  clock_actual = 0;
  vivas_amalin = 0;
  vivas_betarote = 0;
  for(int i = 0; i < PLAYER_TASKS; i++){
    turn_amalin_created[i] = 0;
    turn_betarote_created[i] = 0;
  }
} 

uint8_t sched_kill_old(uint8_t team){
  uint8_t res = 0;
  int8_t old_lemming = -1;
  uint32_t max_turns = 0;
  uint32_t *turns_lemming; 
  if(team == AMALIN){
    turns_lemming = turn_amalin_created;
  }else{
    turns_lemming = turn_betarote_created;
  }
  for(int i = 0; i<PLAYER_TASKS; i++){
    if(max_turns < turns_lemming[i]){
      old_lemming = i;
      max_turns = turns_lemming[i];
    }
  }
  if(old_lemming != -1){
    if(old_lemming == sched_actual_task(team) && team == sched_actual_team()){
      //breakpoint();
      res = 1;
    }
    sched_explode(team, old_lemming);
    print_ground(get_position(team, old_lemming));
  }
  return res;
}

uint8_t sched_control_lemmings(void){
    pos_t pos;
    uint8_t res = 0;
    if(clock_actual%50/*401*/ == 0){
      pos.x = BASE_AMALIN_X;
      pos.y = BASE_AMALIN_Y;
      if(vivas_amalin < PLAYER_TASKS && get_simbol(&pos)  == *"."){
          uint8_t idx = 0;
          while(tareas_vivas_amalin[idx] != 0){
            idx++;
          } 
          tareas_vivas_amalin[idx] = 1;
          pos_init(&pos, AMALIN, idx);
          vivas_amalin++;
          turn_amalin_created[idx] = 0;
          tss_task_reset(AMALIN, idx);
          print("|", 19+idx*4, 45, 0x0F);
      }
      pos.x = BASE_BETAROTE_X;
      pos.y = BASE_BETAROTE_Y;
      if(vivas_betarote < PLAYER_TASKS && get_simbol(&pos) == *"."){
          uint8_t idx = 0;
          while(tareas_vivas_betarote[idx] != 0){
            idx++;
          } 
          tareas_vivas_betarote[idx] = 1;
          pos_init(&pos, BETAROTE, idx);
          vivas_betarote++;
          turn_betarote_created[idx] = 0;
          tss_task_reset(BETAROTE, idx);
          print("|", 43+idx*4, 45, 0x0F);
      }
    } 
    clock_actual++; 
    if(clock_actual%2001 == 0){
      if(vivas_amalin == PLAYER_TASKS){
        res += sched_kill_old(AMALIN);
      }
      if(vivas_betarote == PLAYER_TASKS){
        res += sched_kill_old(BETAROTE);
      }
    }
    return res; 
}

uint16_t sched_next_task(void) {
  if(last_player == AMALIN){
    last_player = BETAROTE;
    last_betarote_task++;
    if(!(last_betarote_task < PLAYER_TASKS)){
      last_betarote_task = 0;
    }
    turn_betarote_created[last_betarote_task]++;
    if(tareas_vivas_betarote[last_betarote_task] == 0){
      return TSS_IDLE;
    }else{
      return tareas_betarote[last_betarote_task];
    }
  }else{
    last_player = AMALIN;
    last_amalin_task++;
    if(!(last_amalin_task < 5)){
      last_amalin_task = 0;
    }
    turn_amalin_created[last_amalin_task]++;
    if(tareas_vivas_amalin[last_amalin_task] == 0){
      return TSS_IDLE;
    }else{
      return tareas_amalin[last_amalin_task];
    }
  }
}

void sched_kill_task(void){
  if(last_player == AMALIN){
    tareas_vivas_amalin[last_amalin_task] = 0;
    vivas_amalin += -1;
    kill_clock(AMALIN, last_amalin_task);
    print_ground(get_position(AMALIN, last_amalin_task));
  }else {
    tareas_vivas_betarote[last_betarote_task] = 0;
    vivas_betarote += -1;
    kill_clock(BETAROTE, last_betarote_task);
    print_ground(get_position(BETAROTE, last_betarote_task));
  }
  return;
}

int8_t sched_actual_team(void){
  if(last_player == AMALIN){
    if(tareas_vivas_amalin[last_amalin_task] == 1){
      return AMALIN;
    }
  }else{
    if(tareas_vivas_betarote[last_betarote_task] == 1){
      return BETAROTE;
    }
  }
  return IDLE;
}

int8_t sched_actual_task(uint32_t team){
  int8_t res = -1;
  if(team == AMALIN){
    res = last_amalin_task;
  }else{
    res = last_betarote_task;
  }
  return res;
}

void sched_explode(uint8_t team, uint8_t task){
  if(team == AMALIN){
    tareas_vivas_amalin[task] = 0;
    vivas_amalin += -1;
    kill_clock(AMALIN, task);
    print_ground(get_position(team, task));
  }else{
    tareas_vivas_betarote[task] = 0;
    vivas_betarote += -1;
    kill_clock(BETAROTE, task);
    print_ground(get_position(team, task));
  }
  return;
}

