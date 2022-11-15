#include "syscall.h"
#include "types.h"

void task(void) {
  while (true) {
    __asm__ volatile("pause");
  }
}
