#include "base.h"
#include "structures.h"

void struct_map_init(U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {
  int i, j;
  for(i = 0; i < MAP_X_SIZE; i++)
    for(j = 0; j < MAP_Y_SIZE; j++)
      map[i][j] = 0;
}
