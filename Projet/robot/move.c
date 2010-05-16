#include "move.h"
#include "bt.h"
#include "sensors.h"

//TODO
static bool move_retry(void) {
  return TRUE;
}

//TODO
static void move_escape_wall(void) {
}

// Handle an obstacle by notifying the application and escaping from it.
void move_handle_obstacle(struct robot_struct *robot, U8 **map) {
  struct bt_message msg = {BT_MSG_LST_WALL, robot->X, robot->Y,
                    robot->orientation};
  bt_msg_send(&msg);
  map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] += \
      robot->orientation;

  if(move_retry()) {
    move_escape_wall();
  }
  else {
    struct bt_message abort = {BT_MSG_ABORT_LST, robot->X, robot->Y,
                      robot->orientation};
    bt_msg_send(&abort);
    map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] -= \
        robot->orientation;
  }
}

//TODO
// Move following the instructions given by the application.
void move_guided(struct robot_struct *robot) {
}

//TODO
// Move in autonomous mode, exploring the field.
void move_autonomous(struct robot_struct *robot, U8 **map) {
}

// Return TRUE if an obstacle has been encountered, FALSE otherwise.
bool move_is_obstacle(void) {
  if (nx_sensors_analog_get(SENSORS_TOUCH) < SENSORS_TOUCH_THRESHOLD)
    return TRUE;

  return FALSE;
}
