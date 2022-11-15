	/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "kassert.h"
#include "sched.h"
#include "tss.h"

uint32_t next_page_kernel;
uint32_t next_page_user; 
uint8_t (*virt_amalin_compartida)[4096];
uint8_t (*virt_betarote_compartida)[4096];

void mmu_init(void) {
	next_page_kernel = 0x102000;
	next_page_user = 0x2400000;
	virt_amalin_compartida = (uint8_t (*)[4096]) 0x100000;
	virt_betarote_compartida = (uint8_t (*)[4096]) 0x101000;
}

paddr_t mmu_next_free_kernel_page(void) { 
	uint32_t r = next_page_kernel;
	next_page_kernel += 0x1000;
  return r;
}

paddr_t mmu_next_free_user_page(void) {
	uint32_t r = next_page_user;
	next_page_user += 0x1000;
  return r;
}

paddr_t mmu_init_kernel_dir(void) {
	// Creo directorio del kernel
	page_directory_entry *pd_k = (page_directory_entry *) KERNEL_PAGE_DIR;	
	page_table_entry *pt_0 = (page_table_entry *) KERNEL_PAGE_TABLE_0;
	// Limpio las tablas
	for (int i = 0; i < 1024; i++) {
		pd_k[i] = (page_directory_entry){0};
		pt_0[i]= (page_table_entry){0};
	}	
	// Cargo la page table en el directorio
	//void mmu_init_page_directory_entry(page_directory_entry *pd, page_table_entry *pt, vaddr_t virt, uint32_t attrs)
	mmu_init_page_directory_entry(pd_k, pt_0, 0, MMU_P | MMU_S | MMU_W);
	/* Mapeo Identity maping los primeros 4MB
	void mmu_init_page_table_entry(page_table_entry *pt, vaddr_t virt, paddr_t phy, uint32_t attrs, uint32_t pages)*/
	mmu_init_page_table_entry(pt_0, 0, 0, MMU_P | MMU_S | MMU_W, 1024);
 // Retorno el page directory para interpretarlo como CR3
 return (paddr_t) pd_k; 
}

void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs) {
  
  page_directory_entry *pd = (page_directory_entry*) (cr3 & RM_ATTRS); //Filtro los attrs
  vaddr_t directory_index = virt>>22; //dirct_indx
 
  if(pd[directory_index].present!=1){  
  	page_table_entry *new_pt = (page_table_entry *) mmu_next_free_kernel_page();
	for(int i = 0; i < 1024; i++){
 		new_pt[i] = (page_table_entry){0};
  	}
  	//void mmu_init_page_directory_entry(page_directory_entry *pd, page_table_entry *pt, vaddr_t virt, uint32_t attrs)
  	mmu_init_page_directory_entry(pd, new_pt, virt, attrs);
  }
  page_table_entry *pt = (page_table_entry *)(pd[directory_index].page_table_base <<12);	
  //void mmu_init_page_table_entry(page_table_entry *pt, vaddr_t virt, paddr_t phy, uint32_t attrs, uint32_t pages)
  mmu_init_page_table_entry(pt, virt, phy, attrs, 1);
  tlbflush();
}

paddr_t mmu_unmap_page(uint32_t cr3, vaddr_t virt) {
	paddr_t r;
	page_directory_entry *direct_base = (page_directory_entry *) (cr3 & RM_ATTRS);
	vaddr_t direct_indx = virt>>22;

	page_table_entry *table_base = (page_table_entry *) (direct_base[direct_indx].page_table_base << 12);
	uint32_t table_indx = ((uint32_t) (virt>>12)) & 0x3FF; //table_indx
	r = (paddr_t) (table_base[table_indx].physical_address_base << 12);
	table_base[table_indx].present = 0;
	
	tlbflush();
	return r;
}

paddr_t mmu_init_task_dir(paddr_t phy, uint8_t team) {
	page_directory_entry *pd_task = (page_directory_entry *) mmu_next_free_kernel_page();
	page_table_entry *pt_0 = (page_table_entry *) mmu_next_free_kernel_page();

	for (int i = 0; i < 1024; i++) {
		pd_task[i] = (page_directory_entry){0};
		pt_0[i]= (page_table_entry){0};
	}

	mmu_init_page_directory_entry(pd_task, pt_0, 0, MMU_P | MMU_S | MMU_W);
	/* Mapeo Identity maping los primeros 4MB */
	mmu_init_page_table_entry(pt_0, 0, 0, MMU_P | MMU_S | MMU_W, 1024);

	// Pido una pagina de 4Kb para mapear las direcciones de codigo del usuario y su stack
	page_table_entry *pt_cdst = (page_table_entry *) mmu_next_free_kernel_page();
	//pt_cdst+=0;
	mmu_init_page_directory_entry(pd_task, pt_cdst, TASK_CODE_VIRTUAL, MMU_P | MMU_U | MMU_W);
	// Mapeo las dos paginas para el codigo de la tarea
	mmu_init_page_table_entry(pt_cdst, TASK_CODE_VIRTUAL, phy, MMU_P | MMU_U | MMU_R, 2);
	// Mapeo la pagina para el stack de la pila
	mmu_init_page_table_entry(pt_cdst, TASK_STACK_BASE-0x1000, mmu_next_free_user_page(), MMU_P | MMU_U | MMU_W, 1);
	
	// Mapeo el area compartida, con MMU_P = 0
	paddr_t phy_shared;
	if(team == AMALIN){
		phy_shared = (paddr_t) (0x400000);
	}else{
		phy_shared = (paddr_t) (0x1400000);
	}
	for(int i = 0; i < 4; i++){
		page_table_entry *pt_shared = (page_table_entry *) mmu_next_free_kernel_page();
		phy_shared += (0x400000 * i); 
		vaddr_t virt = INICIO_AREA_COMPARTIDA+0x400000 * i;
		mmu_init_page_directory_entry(pd_task, pt_shared, virt, MMU_U | MMU_W);
		// Mapeo las dos paginas para el codigo de la tarea
		mmu_init_page_table_entry(pt_shared, virt, phy_shared, MMU_U | MMU_W, 1024);
	}

	return (paddr_t) pd_task;
}

void mmu_init_page_directory_entry(page_directory_entry *pd, page_table_entry *pt, vaddr_t virt, uint32_t attrs){
	vaddr_t directory_index = virt>>22;
	pd[directory_index] = (page_directory_entry){0};
	pd[directory_index].present = attrs & 0x1;
	pd[directory_index].read_write = (attrs>>1) & 0x1;
	pd[directory_index].user_supervisor = (attrs>>2) & 0x1;
	pd[directory_index].page_table_base = ((uint32_t) pt>>12);
}

void mmu_init_page_table_entry(page_table_entry *pt, vaddr_t virt, paddr_t phy, uint32_t attrs, uint32_t pages){
	uint32_t table_index = ((uint32_t) (virt>>12)) & 0x3FF; //table_indx
	for(uint32_t i = 0; i < pages; i++){
		pt[table_index + i] = (page_table_entry){0};			
		pt[table_index + i].present = attrs & 0x1;
	  pt[table_index + i].read_write = (attrs >> 1) & 0x1;
		pt[table_index + i].user_supervisor = (attrs >> 2) & 0x1;
		pt[table_index + i].physical_address_base = (uint32_t)(phy >> 12) + i;
	}
}


uint8_t mmu_map_compartida(vaddr_t virt){
	uint8_t res;
	if(INICIO_AREA_COMPARTIDA<=virt && virt<FIN_AREA_COMPARTIDA){
		uint32_t virt_base = virt & 0xFFFFF000;
		uint32_t virt_idx = (virt_base %0x400000)>>12;
		uint8_t team = sched_actual_team();
		if(team == AMALIN){
			if(*virt_amalin_compartida[virt_idx] == 0){
				*virt_amalin_compartida[virt_idx] = 1;
			}
			page_directory_entry *pd_task = (page_directory_entry *) tss_task_cr3(team, sched_actual_task(team));
			vaddr_t directory_index = virt>>22;
			if(pd_task[directory_index].present != 1){
				pd_task[directory_index].present = 1;
			}
			page_table_entry *pt_task = (page_table_entry *) (pd_task[directory_index].page_table_base <<12);
			uint32_t table_index = ((uint32_t) (virt>>12)) & 0x3FF; //table_indx
			pt_task[table_index].present = 0x1;
			tlbflush();
		}
		if(team == BETAROTE){
			if(*virt_betarote_compartida[virt_idx] == 0){
				*virt_betarote_compartida[virt_idx] = 1;
			}
			page_directory_entry *pd_task = (page_directory_entry *) tss_task_cr3(team, sched_actual_task(team));
			vaddr_t directory_index = virt>>22;
			if(pd_task[directory_index].present != 1){
				pd_task[directory_index].present = 1;
			}
			page_table_entry *pt_task = (page_table_entry *) (pd_task[directory_index].page_table_base <<12);
			uint32_t table_index = ((uint32_t) (virt>>12)) & 0x3FF; //table_indx
			pt_task[table_index].present = 0x1;
			tlbflush();
		}
		res = 1;
	}else{
		res = 0;
	}
	return res;
}

void mmu_unmap_shared(uint32_t cr3){
	page_directory_entry * pd = (page_directory_entry *) cr3;
	for(int i = 0; i < 4; i++){
		vaddr_t virt = (vaddr_t )(INICIO_AREA_COMPARTIDA+0x400000 * i);
		vaddr_t directory_index = virt >>22;
		if(pd[directory_index].present != 0){
			page_table_entry * pt = (page_table_entry *) (pd[directory_index].page_table_base <<12);
			for(int j = 0; j < 1024; j++){
				if(pt[j].present != 0){
					pt[j].present = 0;
				}
			}
			pd[directory_index].present = 0;
		}
	}
	tlbflush();
}
