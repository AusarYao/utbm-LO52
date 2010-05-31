#include <stdlib.h>
#include "base.h"
#include "bt.h"
#include "move.h"
#include "sensors.h"

S8 main(void) {
  struct robot_struct robot = {MODE_AUTONOMOUS, 0, 0, 0};
  U8 map[MAP_X_SIZE][MAP_Y_SIZE];

  struct_map_init(map);
  base_init();

  while(1) {
    bt_check_connect(&robot);

    switch(robot.mode) {
      case MODE_AUTONOMOUS:
        move_autonomous(&robot, map);
        break;

      case MODE_GUIDED:
        if(bt_is_active())
          move_guided(&robot, &robot);
        else
          move_autonomous(&robot, map);
        break;
    }
  }

}
