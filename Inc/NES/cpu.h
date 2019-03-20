#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "mapper.h"
#include "controller.h"
#include "typedef.h"

typedef u16 (*mode)();

void cpu_init();
void cpu_run();
void cpu_setNMI(u8 value);
void cpu_setIRQ(u8 value);
void cpu_log(char* s, int length);

#endif // CPU_H_
