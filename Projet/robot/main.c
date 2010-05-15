#include <stdlib.h>

#include "base.h"
#include "bt.h"
#include "move.h"

S8 main(void) {
  struct robot_struct *robot = malloc(sizeof(struct robot_struct));
  U8 **map = malloc(MAP_X_SIZE * MAP_Y_SIZE * sizeof(U8));
  robot->mode = MODE_AUTONOMOUS;

  while(1) {
    if(move_is_obstacle())
      move_handle_obstacle(robot, map);
    bt_check_connect(robot);
    switch(robot->mode) {
      case MODE_AUTONOMOUS:
        move_handle_autonomous(robot, map);
        break;
      case MODE_GUIDED:
        if(bt_is_active())
          move_handle_guided(robot);
        else
          move_handle_autonomous(robot, map);
        break;
    }
  }
}
