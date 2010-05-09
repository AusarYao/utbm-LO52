#include <stdlib.h>

#include "base.h"
#include "structures.h"
#include "bt.h"

int main(int argc, char* argv) {
  struct robot_struct *robot = malloc(sizeof(robot_struct));
  robot->mode = MODE_AUTONOMOUS;

  while(1) {
    bt_check_connect(robot);
    switch(robot->mode) {
      case MODE_AUTONOMOUS:
        handle_mode_autonomous(robot, map);
        break;
      case MODE_GUIDED:
        if(bt_is_active())
          handle_mode_guided(robot);
        else
          handle_mode_autonomous(robot, map);
        break;
    }
  }
}
