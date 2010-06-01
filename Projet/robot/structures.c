#include "base.h"
#include "structures.h"

void struct_map_init(U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {
  int i, j;
  for(i = 0; i < MAP_X_SIZE; i++) {
    for(j = 0; j < MAP_Y_SIZE; j++) {
      map[i][j] = 0;
      // Draw the walls over the outside frame.
      if(!i)
        map[i][j] += BASE_LEFT;
      if (i == (MAP_X_SIZE - 1))
        map[i][j] += BASE_RIGHT;
      if (!j)
        map[i][j] += BASE_UP;
      if (j == (MAP_Y_SIZE - 1))
        map[i][j] += BASE_DOWN;
    }
  }
}
