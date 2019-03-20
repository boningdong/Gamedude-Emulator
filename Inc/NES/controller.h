#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "typedef.h"

typedef uint8_t u8;

u8 controller_rd(u8 controller);
void controller_wr(u8 data);

#endif // CONTROLLER_H_
