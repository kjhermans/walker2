#ifndef _GAME_WORLD_H_
#define _GAME_WORLD_H_

#include <stdint.h>

#define PILLAR_SIZE 128
#define FIELD_SIZE 32
//#define PILLAR_SIZE 16
//#define FIELD_SIZE 8

struct pillar
{
  unsigned block[ PILLAR_SIZE ];
  unsigned height;
};

void pillar_set_seed (uint32_t s);
void pillar_get (int x, int z, struct pillar* p);
void pillar_get_real (int x, int z, struct pillar* p);
void pillar_init ();

#endif
