/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "types.h"

void mmu_init(void);

paddr_t mmu_next_free_kernel_page(void);

paddr_t mmu_next_free_user_page(void);

void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs);

paddr_t mmu_unmap_page(uint32_t cr3, vaddr_t virt);

paddr_t mmu_init_kernel_dir(void);

paddr_t mmu_init_task_dir(paddr_t phy, uint8_t team);

uint8_t mmu_map_compartida(vaddr_t virt);

void mmu_unmap_shared(uint32_t cr3);


typedef struct str_page_directory_entry {
	uint8_t present: 1;
	uint8_t read_write: 1;
    uint8_t user_supervisor: 1;
	uint8_t page_write_through: 1;
	uint8_t page_cache_disable: 1;
	uint8_t accessed: 1;
	uint8_t x: 1;
	uint8_t page_size: 1;
	uint8_t ignored: 1;
	uint8_t available:3 ;
	uint32_t page_table_base: 20;	
} __attribute__((__packed__)) page_directory_entry;

typedef struct str_page_table_entry {
	uint8_t present: 1;
	uint8_t read_write: 1;
        uint8_t user_supervisor: 1;
	uint8_t page_write_through: 1;
	uint8_t page_cache_disable: 1;
	uint8_t accessed: 1;
	uint8_t dirty: 1;	
	uint8_t x: 1;
	uint8_t global: 1;
	uint8_t available:3 ;
	uint32_t physical_address_base: 20;	
} __attribute__((__packed__)) page_table_entry;

void mmu_init_page_table_entry(page_table_entry *pt, vaddr_t virt, paddr_t phy, uint32_t attrs, uint32_t pages);

void mmu_init_page_directory_entry(page_directory_entry *pd, page_table_entry *pt, vaddr_t virt, uint32_t attrs);
#endif //  __MMU_H__