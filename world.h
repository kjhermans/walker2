#ifndef _GAME_WORLD_H_
#define _GAME_WORLD_H_

#include <stdint.h>

#define PILLAR_SIZE 128
#define FIELD_SIZE 32
//#define PILLAR_SIZE 16
//#define FIELD_SIZE 8

void pillar_set_seed (uint32_t s);
void pillar_get (int x, int z, unsigned pillar[ PILLAR_SIZE ]);
void pillar_init ();

#endif
