#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>

#include <sdbm_tree/td.h>

#include "world.h"
#include "md5.h"

static unsigned pillars[ FIELD_SIZE ][ FIELD_SIZE ][ PILLAR_SIZE ];

static int pillar_offset_x = -(FIELD_SIZE / 2);
static int pillar_offset_z = -(FIELD_SIZE / 2);

static
uint32_t seed = 0x590380fa;

void pillar_set_seed
  (uint32_t s)
{
  seed = s;
}

static
td_t pillardb;

int pillar_get_cached
  (int x, int z, unsigned pillar[ PILLAR_SIZE ])
{
  if (x >= pillar_offset_x && x < pillar_offset_x + FIELD_SIZE
      && z >= pillar_offset_z && z < pillar_offset_z + FIELD_SIZE)
  {
    memcpy(
      pillar,
      pillars[ x - pillar_offset_x ][ z - pillar_offset_z ],
      sizeof(unsigned) * PILLAR_SIZE
    );
    return 0;
  }
  //..  fetch it from the db and return 0
  return ~0;
}

void pillar_get_new
  (int x, int z, unsigned pillar[ PILLAR_SIZE ])
{
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
      for (; i < PILLAR_SIZE; i++) {
        pillar[ i ] = 0;
      }
      return;
    }
    pillar[ i ] = (rand() % 4) + 1;
  }
}

void pillar_get
  (int x, int z, unsigned pillar[ PILLAR_SIZE ])
{
  if (pillar_get_cached(x, z, pillar) == 0) {
    return;
  }
  pillar_get_new(x, z, pillar);
}

void pillar_init
  ()
{
  char path[ 256 ];

  snprintf(path, sizeof(path), "%s/.game.db", getenv("HOME"));
  if (td_open(&pillardb, path, 0, O_RDWR|O_CREAT, 0644)) {
    fprintf(stderr, "Error opening db at '%s'\n", path);
    abort();
  }
  for (int i=0; i < FIELD_SIZE; i++) {
    for (int j=0; j < FIELD_SIZE; j++) {
      pillar_get_new(
        pillar_offset_x + i,
        pillar_offset_z + j,
        pillars[ i ][ j ]
      );
    }
  }
}

