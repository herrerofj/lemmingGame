	/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "defines.h"
#include "kassert.h"
#include "mmu.h"
#include "i386.h"

tss_t tss_initial = {0};
tss_t tss_idle = {0};

tss_t tss_task[10] = {0};
uint16_t gdt_tss[10] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24};

#define CS_RING_0 0x0040
#define CS_RING_3 0x004B 
#define DS_RING_0 0x0050
#define DS_RING_3 0x005B
#define VS_RING_0 0x0060
#define EFLAGS_I 0x202
#define IOMAP_a	 0x0000

void tss_initial_init(void){
  gdt[GDT_TSS_INIT].base_15_0  = (uint32_t )(&tss_initial) & 0xFFFF;
  gdt[GDT_TSS_INIT].base_23_16 = ((uint32_t )(&tss_initial) >> 16) & 0xFF;
  gdt[GDT_TSS_INIT].base_31_24 = ((uint32_t )(&tss_initial) >> 24) & 0xFF;
  }

void tss_idle_init(void){

  tss_idle.eip = TASK_IDLE_START;
  tss_idle.cr3 = KERNEL_PAGE_DIR;
  tss_idle.esp = KERNEL_STACK;
  tss_idle.ebp = KERNEL_STACK;
  tss_idle.cs  = CS_RING_0;
  tss_idle.ds  = DS_RING_0;
  tss_idle.es  = DS_RING_0;
  tss_idle.fs  = DS_RING_0;
  tss_idle.gs  = DS_RING_0;
  tss_idle.ss  = DS_RING_0;
  tss_idle.eflags = EFLAGS_I;

  gdt[GDT_TSS_IDLE].base_15_0  = (uint32_t )(&tss_idle) & 0xFFFF;
  gdt[GDT_TSS_IDLE].base_23_16 = ((uint32_t )(&tss_idle) >> 16) & 0xFF;
  gdt[GDT_TSS_IDLE].base_31_24 = ((uint32_t )(&tss_idle) >> 24) & 0xFF;
  }

void tss_task_init(paddr_t cr3, uint32_t gdt_idx, tss_t *tss_paux){
	paddr_t stack_0  = mmu_next_free_kernel_page();

	tss_paux->eip = TASK_CODE_VIRTUAL;
	tss_paux->cr3 = cr3;
	tss_paux->esp = TASK_STACK_BASE;
	tss_paux->ebp = TASK_STACK_BASE;
	tss_paux->cs  = CS_RING_3;
	tss_paux->ds  = DS_RING_3;
	tss_paux->es  = DS_RING_3;
	tss_paux->fs  = DS_RING_3;
	tss_paux->gs  = DS_RING_3;
	tss_paux->ss  = DS_RING_3;
	tss_paux->eflags = EFLAGS_I;
	tss_paux->iomap = IOMAP_a;

	tss_paux->ss0  = DS_RING_0;
	tss_paux->esp0 = stack_0 + PAGE_SIZE;

	gdt[gdt_idx].base_15_0 = (uint32_t )(tss_paux) & 0xFFFF;
	gdt[gdt_idx].base_23_16 = ((uint32_t )(tss_paux) >> 16) & 0xFF;
	gdt[gdt_idx].base_31_24 = ((uint32_t )(tss_paux) >> 24) & 0xFF;
	}

void tss_tasks_init(void){
	for(int i = 0; i < 5; i++){
		tss_task_init(mmu_init_task_dir(0x18000, AMALIN), gdt_tss[i], &tss_task[i]);
	}	
	for(int i = 5; i < 10; i++){
		tss_task_init(mmu_init_task_dir(0x1A000, BETAROTE), gdt_tss[i], &tss_task[i]);
	}
}

void tss_init(void) {
	tss_idle_init();
	tss_initial_init();
	tss_tasks_init();
}

void tss_task_reset(uint8_t team, uint8_t task){
	uint8_t idx = task;
	if(team == BETAROTE){
		idx += 5;
	}
	tss_task[idx].eip = TASK_CODE_VIRTUAL;
	tss_task[idx].esp = TASK_STACK_BASE;
	tss_task[idx].cs = CS_RING_3;
	tss_task[idx].ss = DS_RING_3;
	tss_task[idx].eflags = EFLAGS_I;
	mmu_unmap_shared(tss_task[idx].cr3);
}

uint32_t tss_task_cr3(uint8_t team, uint8_t task){
	uint8_t idx = task;
	if(team == BETAROTE){
		idx += 5;
	}
	return tss_task[idx].cr3;
}

