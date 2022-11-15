/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de tipos comunes en el kernel.
*/

#ifndef __TYPES_H__
#define __TYPES_H__

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

typedef uint32_t vaddr_t; // direccion virtual.
typedef uint32_t paddr_t; // direccion fisica.

/* Estructura de para acceder a memoria de video */
typedef struct ca_s {
  uint8_t c;
  uint8_t a;
} ca; 

#endif //  __TYPES_H__
