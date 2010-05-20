#include <math.h>

#include "move.h"
#include "bt.h"
#include "sensors.h"

// Make the robot turn by the given angle, in degrees.
static void rotate_angle(struct robot_struct *robot, U32 angle) {
  float distance = 2. * M_PI * angle * MOVE_WHEEL_SPACING / 360.;
  float wheel_rotations = distance / (2. * M_PI * MOVE_WHEEL_DIAMETER);
  float angle_rotation = wheel_rotations * 360;
  int power;

  if(angle_rotation > 0) {
    nx_motors_rotate_angle(MOVE_LEFT_MOTOR, MOVE_TURN_SPEED, \
        (U32)angle_rotation, TRUE);
    nx_motors_rotate_angle(MOVE_RIGHT_MOTOR, -MOVE_TURN_SPEED, \
        (U32)angle_rotation, TRUE);
  }
  else {
    nx_motors_rotate_angle(MOVE_RIGHT_MOTOR, MOVE_TURN_SPEED, \
        abs((U32)angle_rotation), TRUE);
    nx_motors_rotate_angle(MOVE_LEFT_MOTOR, -MOVE_TURN_SPEED, \
        abs((U32)angle_rotation), TRUE);
  }

  nx_systick_wait_ms(abs(600 * angle / 90));

  // Modify the robot's orientation
  power = (int)(log(robot->orientation) + angle / 90.) % 4;
  robot->orientation = pow(2., power);
}

//TODO
static bool move_retry(void) {
  return TRUE;
}

//TODO
static void move_escape_wall(void) {
}

// Handle an obstacle by notifying the application and escaping from it.
void move_handle_obstacle(struct robot_struct *robot,
    U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {
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
void move_autonomous(struct robot_struct *robot,
    U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {
}

// Return TRUE if an obstacle has been encountered, FALSE otherwise.
bool move_is_obstacle(void) {
  if (nx_sensors_analog_get(SENSORS_TOUCH) < SENSORS_TOUCH_THRESHOLD)
    return TRUE;

  return FALSE;
}
