#include <stdlib.h>

#include "base.h"
#include "bt.h"
#include "move.h"

S8 main(void) {
  struct robot_struct robot = {MODE_AUTONOMOUS, 0, 0, 0};
  U8 **map = malloc(MAP_X_SIZE * MAP_Y_SIZE * sizeof(U8));

  base_init();

  while(1) {
    if(move_is_obstacle())
      move_handle_obstacle(&robot, map);

    bt_check_connect(&robot);

    switch(robot.mode) {
      case MODE_AUTONOMOUS:
        move_autonomous(&robot, map);
        break;

      case MODE_GUIDED:
        if(bt_is_active())
          move_guided(&robot);
        else
          move_autonomous(&robot, map);
        break;
    }
  }
}
