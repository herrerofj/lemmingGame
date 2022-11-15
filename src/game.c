/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "mapa.h"
#include "types.h"
#include "screen.h"
#include "i386.h"
#include "sched.h"

extern char next_simbol(int32_t simbol);
extern void game_end_ASM(void);

uint32_t lemSize_A = 0;
uint32_t lemSize_B = 0;

int8_t clocks_A[5] = {-1, -1, -1, -1, -1};
int8_t clocks_B[5] = {-1, -1, -1, -1, -1};

pos_t pos_A[5] = {0};
pos_t pos_B[5] = {0};

void print_scores(void){
    // Amalin 
    
    /* Nombre equipo*/
    print("Amalin", 5, 41, 0x04);
    /* Caja puntajes */
    screen_draw_box(43,5,3,9,0,0x47);
    print_dec(lemSize_A, 7, 6, 44, 0x47);

    // Betarote

    /* Nombre equipo*/
    print("Betarote", 66, 41, 0x01);

    /* Caja puntajes */
    screen_draw_box(43,66,3,9,0,0x17);
    print_dec(lemSize_B, 7, 67, 44, 0x17);
}

void game_tick(void) {
    int8_t team = sched_actual_team(); 
    if(team != IDLE){
        //breakpoint();
        uint32_t task = sched_actual_task(team);
        char simbol;
        if(team == AMALIN){
            if(clocks_A[task]<3){
                clocks_A[task]++;
            }else{clocks_A[task] = 0;}
            simbol = next_simbol(clocks_A[task]); 
        }else{
            if(clocks_B[task]<3){
                clocks_B[task]++;
            }else{clocks_B[task] = 0;}
            simbol = next_simbol(clocks_B[task]);
        }
        print_clock(simbol, team, task);
    }
}

void kill_clock(uint32_t team, int8_t task){
    if(team == AMALIN){
        clocks_A[task] = -1;
        print("x", 19+task*4, 45, 0x0F);
    }else{
        clocks_B[task] = -1;
        print("x", 43+task*4, 45, 0x0F);
    }
}

void pos_init(pos_t *pos, uint8_t team, int8_t task){
    //breakpoint();
    print_leming(pos, team);
    if(team == AMALIN){
        pos_A[task].x = pos->x;
        pos_A[task].y = pos->y;
        clocks_A[task] = 0;
        //print_clock(*"|", team, task);
        lemSize_A++;
    }
    if(team == BETAROTE){
        pos_B[task].x = pos->x;
        pos_B[task].y = pos->y;
        clocks_B[task] = 0;
        /*breakpoint();
        print_clock(*"|", team, task);
        breakpoint();*/
        lemSize_B++;
    }
}

void game_init(void) {
    load_map();
    print_scores();
    print_teams();
}

void game_end(int8_t team){
    if(team == AMALIN){
        print_A_wins();
    }else{
        print_B_wins();
    }
    game_end_ASM();
}

void game_cheack(void){
    int8_t res = -1;
    for(int i = 0; i < 5; i++){
        if(clocks_A[i]!=-1 && pos_A[i].y ==79){
            res = AMALIN;
        }
        if(clocks_B[i]!=-1 && pos_B[i].y ==0){
            res = BETAROTE;
        }
    }
    if(res != -1){
        game_end(res);
    }
}

void screen_fresh(void){
    int8_t team = sched_actual_team();
    if(team != IDLE){
        uint32_t task = sched_actual_task(team);
        if(team == AMALIN){
            if(clocks_A[task] != -1){
                set_simbol(&pos_A[task],(ca ){*"A",0x24});
            }
        }
        if(team == BETAROTE){
            if(clocks_B[task]!=-1){
                set_simbol(&pos_B[task],(ca ){*"B",0x21});
            }
        }
    }
}

move_result_e move_up(pos_t *pos){
    ca (*screen)[ROW_SIZE] = (ca (*)[ROW_SIZE])VIDEO;
    move_result_e res = SUCCESS;
    if(pos->x == 0){
        res = BORDER;
    }else{
        char simbol = screen[pos->x-1][pos->y].c;
        if(simbol == *"P"){
            res = WALL;
        }else{
            if(simbol == *"L"){
                res = LAKE;
            }else{
                if(simbol == *"A" || simbol == *"B"){
                    res = LEMMING;
                }
            }
        }
    }
    return res;
}

move_result_e move_right(pos_t *pos){
    ca (*screen)[ROW_SIZE] = (ca (*)[ROW_SIZE])VIDEO;
    move_result_e res = SUCCESS;
    if(pos->y == 79){
        res = BORDER;
    }else{
        char simbol = screen[pos->x][pos->y+1].c;
        if(simbol == *"P"){
            res = WALL;
        }else{
            if(simbol == *"L"){
                res = LAKE;
            }else{
                if(simbol == *"A" || simbol == *"B"){
                    res = LEMMING;
                }
            }
        }
    }
    return res;
}

move_result_e move_down(pos_t *pos){
    ca (*screen)[ROW_SIZE] = (ca (*)[ROW_SIZE])VIDEO;
    move_result_e res = SUCCESS;
    if(pos->x == 39){
        res = BORDER;
    }else{
        char simbol = screen[pos->x+1][pos->y].c;
        if(simbol == *"P"){
            res = WALL;
        }else{
            if(simbol == *"L"){
                res = LAKE;
            }else{
                if(simbol == *"A" || simbol == *"B"){
                    res = LEMMING;
                }
            }
        }
    }
    return res;
}

move_result_e move_left(pos_t *pos){
    ca (*screen)[ROW_SIZE] = (ca (*)[ROW_SIZE])VIDEO;
    move_result_e res = SUCCESS;
    if(pos->y == 0){
        res = BORDER;
    }else{
        char simbol = screen[pos->x][pos->y-1].c;
        if(simbol == *"P"){
            res = WALL;
        }else{
            if(simbol == *"L"){
                res = LAKE;
            }else{
                if(simbol == *"A" || simbol == *"B"){
                    res = LEMMING;
                }
            }
        }
    }
    return res;
}

move_result_e move_valido(direction_e direction, pos_t *pos){
    move_result_e res = SUCCESS;
    switch(direction){
        case UP:
            res = move_up(pos);
            break;
        case RIGHT:
            res = move_right(pos);
            break;
        case DOWN:
            res = move_down(pos);
            break;
        case LEFT:
            res = move_left(pos);
            break;
    }
    return res;
}

move_result_e move_main(direction_e direction){
    move_result_e res = SUCCESS;
    int8_t team = sched_actual_team();
    int8_t task = sched_actual_task(team);
    pos_t *pos;
    if(team == AMALIN){
        pos = &pos_A[task];
    }else{
        pos = &pos_B[task];
    }
    res = move_valido(direction, pos);
    if(res == SUCCESS){

        print_ground(pos);
        switch (direction){
            case UP:
                pos->x=pos->x-1;
                break;
            case RIGHT:
                pos->y=pos->y+1;
                break;
            case DOWN:
                pos->x=pos->x+1;
                break;
            case LEFT:
                pos->y=pos->y-1;
                break;
        }
        print_leming(pos, team);   
    }
    return res;
}

pos_t effective_pos(pos_t *pos, int8_t i){
    int8_t offset_x = 0;
    int8_t offset_y = 0;
    if(i == 0 || i == 3 || i == 5){
        offset_y = -1;
    }else{
        if(i == 2 || i == 4 || i == 7){
            offset_y = 1;
        }
    }
    if (i == 0 || i == 1 || i == 2){
        offset_x = -1;
    }else{
        if(i == 5 || i == 6 || i ==7){
            offset_x = 1;
        }
    }
    pos_t aux_pos = {0};
    aux_pos.x = pos->x + offset_x;
    aux_pos.y = pos->y + offset_y;
    return aux_pos;
}

void explode_wall(pos_t *pos, int8_t i){
    pos_t ef_pos = effective_pos(pos, i);
    break_wall(&ef_pos);
    print_ground(&ef_pos);
    return;
} 

void explode_leming(pos_t *pos, int8_t i){
    uint8_t team = -1;
    uint8_t task = -1;
    pos_t ef_pos = effective_pos(pos, i);
    for(int j = 0; j < 5; j++){
        if(pos_A[j].x == ef_pos.x && pos_A[j].y == ef_pos.y){
            team = AMALIN;
            task = j;
        }
    }
    for(int j = 0; j < 5; j++){
        if(pos_B[j].x == ef_pos.x && pos_B[j].y == ef_pos.y){
            team =BETAROTE;
            task = j;
        }
    }
    sched_explode(team, task);
    print_ground(&ef_pos);
    return;
}

char simbols[8] = {0};

void load_simbols(pos_t *pos){
    ca (*screen)[ROW_SIZE] = (ca (*)[ROW_SIZE])VIDEO;
    if(0 < pos->x && pos->x < 39){
        simbols[1] = screen[pos->x-1][pos->y].c;
        simbols[6] = screen[pos->x+1][pos->y].c;
    }else{
        if(pos->x == 39){
            simbols[1] = screen[pos->x-1][pos->y].c;
        }
        if(pos->x == 0){
            simbols[6] = screen[pos->x-1][pos->y].c;
        }
    }
    if(0 < pos->y && pos->y < 79){
        simbols[3] = screen[pos->x][pos->y-1].c;
        simbols[4] = screen[pos->x][pos->y+1].c;
    }else{
        if(pos->y == 0){
            simbols[4] = screen[pos->x][pos->y+1].c;
        }
        if(pos->y == 79){
            simbols[3] = screen[pos->x][pos->y-1].c;
        }
    }
    if(simbols[1] != 0){
        if(simbols[3] != 0){
            simbols[0] = screen[pos->x-1][pos->y-1].c;
        }
        if(simbols[4] != 0){
            simbols[2] = screen[pos->x-1][pos->y+1].c;
        }
    }
    if(simbols[6] != 0){
        if(simbols[3] != 0){
            simbols[5] = screen[pos->x+1][pos->y-1].c;
        }
        if(simbols[4] != 0){
            simbols[7] = screen[pos->x+1][pos->y+1].c;
        }
    }
    return;
}
/*
|0 |1 |2 |
|3 |EX|4 |
|5 |6 |7 |
*/
void explode_main(void){
    int8_t team = sched_actual_team();
    int8_t task = sched_actual_task(team);
    pos_t *pos;
    if(team == AMALIN){
        pos = &pos_A[task];
    }else{
        pos = &pos_B[task];
    }
    load_simbols(pos);
    for(int i = 0; i < 8; i++){
        if(simbols[i] == *"P"){
            explode_wall(pos, i);
        }
        if(simbols[i] == *"A" || simbols[i] == *"B"){
            explode_leming(pos, i); 
        }
    }
    print_ground(pos);
    return;
}

pos_t *get_position(int8_t team, int8_t task){
   if(team == AMALIN){
        return &pos_A[task];
    }else{
        return &pos_B[task];
    }
}

pos_t effct_pos_lake(direction_e direction, pos_t* pos){
    pos_t effct_pos;
    int8_t offset_x = 0;
    int8_t offset_y = 0;
    switch(direction){
        case UP:
            offset_x = -1;
            break;
        case RIGHT:
            offset_y = 1;
            break;
        case DOWN:
            offset_x = 1;
            break;
        case LEFT:
            offset_y = -1;
            break;
    }
    effct_pos.x = pos->x + offset_x;
    effct_pos.y = pos->y + offset_y;
    return effct_pos;
}

move_result_e bridge_valido(direction_e direction, pos_t *pos){
    return move_valido(direction, pos);
}

void bridge_main(direction_e direction){
    pos_t *pos = get_position(sched_actual_team(), sched_actual_task(sched_actual_team()));
    //char simbol = get_simbol(pos);
    move_result_e lake = bridge_valido(direction, pos);
    if(lake == LAKE){
        pos_t effct_pos = effct_pos_lake(direction, pos);
        set_simbol(&effct_pos,(ca ){*"+",0x1F});
        make_bridge(&effct_pos);
    }
    print_ground(pos);
}