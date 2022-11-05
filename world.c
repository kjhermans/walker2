#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

//#include <sdbm_tree/td.h>

#include "world.h"
#include "md5.h"

static struct pillar pillars[ FIELD_SIZE ][ FIELD_SIZE ];

static int pillar_offset_x = -(FIELD_SIZE / 2);
static int pillar_offset_z = -(FIELD_SIZE / 2);

static
uint32_t seed = 0x590380fa;

void pillar_set_seed
  (uint32_t s)
{
  seed = s;
}

//static
//td_t pillardb;

int pillar_get_cached
  (int x, int z, struct pillar* p)
{
  if (x >= pillar_offset_x && x < pillar_offset_x + FIELD_SIZE
      && z >= pillar_offset_z && z < pillar_offset_z + FIELD_SIZE)
  {
    *p = pillars[ x - pillar_offset_x ][ z - pillar_offset_z ];
    return 0;
  }
  //..  fetch it from the db and return 0
  return ~0;
}

void pillar_get_new
  (int x, int z, struct pillar* p)
{

/*
  if ((x + z) % 2) {
    *p = (struct pillar){ .block[ 0 ] = 1, .height = 1 };
  } else {
    *p = (struct pillar){ .block[ 0 ] = 0, .height = 0 };
  }
  return;
*/

  unsigned char pillarseed[ 12 ];
  int32_t xx = x, zz = z;
  unsigned char rnd[ 16 ];

  memcpy(pillarseed, &xx, 4);
  memcpy(pillarseed + 4, &zz, 4);
  memcpy(pillarseed + 8, &seed, 4);
  md5(pillarseed, sizeof(pillarseed), rnd);

  srand(*((unsigned*)rnd));
  for (unsigned i=0; i < PILLAR_SIZE; i++) {
    if ((rand() % 4) == 0) {
      p->height = i;
      for (; i < PILLAR_SIZE; i++) {
        p->block[ i ] = 0;
      }
      return;
    }
    p->block[ i ] = (rand() % 4) + 1;
  }
  p->height = PILLAR_SIZE;
}

void pillar_get
  (int x, int z, struct pillar* p)
{
  if (pillar_get_cached(x, z, p) == 0) {
    return;
  }
  pillar_get_new(x, z, p);
}

void pillar_init
  ()
{
/*
  char path[ 256 ];

  snprintf(path, sizeof(path), "%s/.walker2.db", getenv("HOME"));
  if (td_open(&pillardb, path, 0, O_RDWR|O_CREAT, 0644)) {
    fprintf(stderr, "Error opening db at '%s'\n", path);
    abort();
  }
*/
  for (int i=0; i < FIELD_SIZE; i++) {
    for (int j=0; j < FIELD_SIZE; j++) {
      pillar_get_new(
        pillar_offset_x + i,
        pillar_offset_z + j,
        &(pillars[ i ][ j ])
      );
    }
  }
}

