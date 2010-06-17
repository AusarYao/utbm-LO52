// Definitions of structures and associated functions
#ifndef _LO52_STRUCTURES_H
#define _LO52_STRUCTURES_H

struct robot_struct {
  U8 mode;
  U8 X, Y;
  U8 orientation;
};

void struct_map_init(U8[MAP_X_SIZE][MAP_Y_SIZE]);

#endif
