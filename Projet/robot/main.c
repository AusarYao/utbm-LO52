#include <stdlib.h>
#include "base/display.h"
#include "base.h"
#include "base/core.h"
#include "base/drivers/systick.h"
#include "base/drivers/avr.h"
#include "bt.h"
#include "move.h"
#include "sensors.h"


//add the possibility to use the buttons
static void watchdog(void) {

	switch (nx_avr_get_button()) {
		case BUTTON_CANCEL:
			nx_core_halt();
      break;
		case BUTTON_OK:
			break;
		case BUTTON_LEFT:
			break;
		case BUTTON_RIGHT:
			break;
		case BUTTON_NONE:
			break;
  }
}


S8 main(void) {
  struct robot_struct robot = {MODE_AUTONOMOUS, 0, 0, 0};
  U8 map[MAP_X_SIZE][MAP_Y_SIZE];

  struct_map_init(map);
  nx_display_clear();
	nx_display_cursor_set_pos(0, 0);
	nx_display_string("Lo52 project\n");
	nx_display_cursor_set_pos(0, 2);

	//display battery level
	nx_display_uint(nx_avr_get_battery_voltage());
	nx_display_cursor_set_pos(4, 2);
	nx_display_string("/4000\n");
	nx_systick_install_scheduler(watchdog);
  base_init();

  while(1) {
    bt_check_connect(&robot);

    switch(robot.mode) {
      case MODE_AUTONOMOUS:
        move_autonomous(&robot, map);
        break;

      case MODE_GUIDED:
        if(bt_is_active())
          move_guided(&robot,&robot);
        else
          move_autonomous(&robot, map);
        break;
    }
  }

}
