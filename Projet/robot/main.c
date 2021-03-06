#include <stdlib.h>
#include "base.h"
#include "bt.h"
#include "move.h"
#include "sensors.h"
#include "base/drivers/sound.h"

S8 main(void) {
  struct robot_struct robot = {MODE_AUTONOMOUS, 12, 8, BASE_RIGHT};
  U8 map[MAP_X_SIZE][MAP_Y_SIZE];

  struct_map_init(map);
  base_init();


  //Wait for a bluetooth connection

  if (!nx_bt_stream_opened() || nx_bt_connection_pending())
      bt_wait_connection();

  while(nx_avr_get_button() != BUTTON_OK) {
    nx_systick_wait_ms(10);
  }

  sensors_light_calibrate();

  while(1) {
    //On a desactivaté la reception de message bletooth car le robot bug
    //La communication bluetooth ne fonctionne que dans
    //le sens robot->aplication
    // bt_check_connect(&robot, map);

    switch(robot.mode) {
      case MODE_AUTONOMOUS:
        move_autonomous(&robot, map);
        break;
/*
      case MODE_GUIDED:
        if(bt_is_active())
        //  move_guided(&robot, robot.X, robot.Y, map);
        else
          move_autonomous(&robot, map);
        break;
*/
    }
  }

}
