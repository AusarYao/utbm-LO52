#include "base.h"
#include "structures.h"

void struct_map_init(U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {
  int i, j;
  for(i = 0; i < MAP_X_SIZE; i++)
    for(j = 0; j < MAP_Y_SIZE; j++)
      map[i][j] = 0;
  for(i = 0; i < MAP_X_SIZE; i++){
    map[i][0]+=BASE_DOWN;
    map[i][MAP_Y_SIZE-1]+=BASE_UP;
  }
  for(j = 0; j < MAP_Y_SIZE; j++){
    map[0][j] += BASE_LEFT;
    map[MAP_X_SIZE-1][j] += BASE_RIGHT;
  }
}
