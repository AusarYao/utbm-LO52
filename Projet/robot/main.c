#include <stdlib.h>

#include "base.h"
#include "structures.h"

int main(int argc, char* argv) {
  struct robot_struct *robot = malloc(sizeof(robot_struct));
  robot->mode = MODE_AUTONOMOUS;

  while(1) {
    switch(robot->mode) {
      case MODE_AUTONOMOUS:
        handle_mode_autonomous(robot, map);
        break;
      case MODE_GUIDED:
        handle_mode_guided(robot);
        break;
    }
  }
  bt_check_connect(robot);
}
