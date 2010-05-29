#include <math.h>

#include "move.h"
#include "bt.h"
#include "sensors.h"

//TODO
// Move in autonomous mode, exploring the field.
void move_autonomous(struct robot_struct *robot,
    U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {
}

// Make the robot move backward on the given distance in centimeters.
// Start and stop gradually.
static void move_backward(struct robot_struct *robot, U32 distance) {
  double wheel_angle = move_compute_angle(distance);
  U32 init_tach[2];

  // Change the position of the robot.
  switch(robot->orientation) {
    case BASE_UP:
      robot->Y -= (U8)distance;
      break;
    case BASE_DOWN:
      robot->Y += (U8)distance;
      break;
    case BASE_LEFT:
      robot->X += (U8)distance;
      break;
    case BASE_RIGHT:
      robot->X -= (U8)distance;
      break;
  }

  // Get the initial state of the motors.
  init_tach[0] = nx_motors_get_tach_count(MOVE_LEFT_MOTOR);
  init_tach[1] = nx_motors_get_tach_count(MOVE_RIGHT_MOTOR);

  // Make the motors turn at a low speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, -MOVE_LOW_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, -MOVE_LOW_SPEED);

  // Wait until the first step is reached.
  while((init_tach[0] - nx_motors_get_tach_count(MOVE_LEFT_MOTOR)) <
      (MOVE_STEP * wheel_angle));

  // Make the motors turn at normal speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, -MOVE_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, -MOVE_SPEED);

  // Wait until the second step is reached.
  while((init_tach[0] - nx_motors_get_tach_count(MOVE_LEFT_MOTOR)) <
      (2 * MOVE_STEP * wheel_angle));

  // Make the motors turn at high speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, -MOVE_HIGH_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, -MOVE_HIGH_SPEED);

  // Wait until we reach the final part.
  while((init_tach[0] - nx_motors_get_tach_count(MOVE_LEFT_MOTOR)) <
      ((1 - (2 * MOVE_STEP)) * wheel_angle));

  // Make the motors turn at normal speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, -MOVE_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, -MOVE_SPEED);

  // Wait until we reach the final step.
  while((init_tach[0] - nx_motors_get_tach_count(MOVE_LEFT_MOTOR)) <
      ((1 - MOVE_STEP) * wheel_angle));

  // Make the motors turn at low speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, -MOVE_LOW_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, -MOVE_LOW_SPEED);

  // Wait until we reach the end of our move.
  while((init_tach[0] - nx_motors_get_tach_count(MOVE_LEFT_MOTOR)) <
      wheel_angle);

  // We stop the motors.
  nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
  nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
}

// Compute the angle to rotate the wheels for the given distance
static double move_compute_angle(double distance) {
  double wheel_rotations = distance / (2. * M_PI * MOVE_WHEEL_DIAMETER);
  return wheel_rotations * 360;
}

static void move_escape_wall(struct robot_struct *robot) {
  move_backward(robot, 10);
  move_rotate_angle(robot, -90);
}

// Make the robot move forward on the given distance in centimeters.
// Start and stop gradually.
static void move_forward(struct robot_struct *robot, U32 distance) {
  double wheel_angle = move_compute_angle(distance);
  U32 init_tach[2];

  // Change the position of the robot.
  switch(robot->orientation) {
    case BASE_UP:
      robot->Y += (U8)distance;
      break;
    case BASE_DOWN:
      robot->Y -= (U8)distance;
      break;
    case BASE_LEFT:
      robot->X -= (U8)distance;
      break;
    case BASE_RIGHT:
      robot->X += (U8)distance;
      break;
  }

  // Get the initial state of the motors.
  init_tach[0] = nx_motors_get_tach_count(MOVE_LEFT_MOTOR);
  init_tach[1] = nx_motors_get_tach_count(MOVE_RIGHT_MOTOR);

  // Make the motors turn at a low speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, MOVE_LOW_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, MOVE_LOW_SPEED);

  // Wait until the first step is reached.
  while((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
      (MOVE_STEP * wheel_angle));

  // Make the motors turn at normal speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, MOVE_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, MOVE_SPEED);

  // Wait until the second step is reached.
  while((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
      (2 * MOVE_STEP * wheel_angle));

  // Make the motors turn at high speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, MOVE_HIGH_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, MOVE_HIGH_SPEED);

  // Wait until we reach the final part.
  while((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
      ((1 - (2 * MOVE_STEP)) * wheel_angle));

  // Make the motors turn at normal speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, MOVE_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, MOVE_SPEED);

  // Wait until we reach the final step.
  while((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
      ((1 - MOVE_STEP) * wheel_angle));

  // Make the motors turn at low speed.
  nx_motors_rotate(MOVE_LEFT_MOTOR, MOVE_LOW_SPEED);
  nx_motors_rotate(MOVE_RIGHT_MOTOR, MOVE_LOW_SPEED);

  // Wait until we reach the end of our move.
  while((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
      wheel_angle);

  // We stop the motors.
  nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
  nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
}

//TODO
// Move following the instructions given by the application.
void move_guided(struct robot_struct *robot) {
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
    move_escape_wall(robot);
  }
  else {
    struct bt_message abort_msg = {BT_MSG_ABORT_LST, robot->X, robot->Y,
                      robot->orientation};
    bt_msg_send(&abort_msg);
    map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] -= \
        robot->orientation;
  }
}

static U8 move_log(U8 a) {
  U8 count = 0;
  for(; a > 1; count++)
    a >> 1;
  return count;
}

static U8 move_pow(U8 a, U8 b) {
  return a << (b-1);
}

//TODO
static bool move_retry(void) {
  return TRUE;
}

// Make the robot turn by the given angle, in degrees.
static void move_rotate_angle(struct robot_struct *robot, U32 angle) {
  double distance = 2. * M_PI * angle * MOVE_WHEEL_SPACING / 360.;
  double angle_rotation = move_compute_angle(distance);
  int power;

  if(angle_rotation > 0) {
    nx_motors_rotate_angle(MOVE_LEFT_MOTOR, MOVE_TURN_SPEED, \
        (U32)angle_rotation, TRUE);
    nx_motors_rotate_angle(MOVE_RIGHT_MOTOR, -MOVE_TURN_SPEED, \
        (U32)angle_rotation, TRUE);
    nx_systick_wait_ms(600 * angle / 90);
  }
  else {
    nx_motors_rotate_angle(MOVE_RIGHT_MOTOR, MOVE_TURN_SPEED, \
        -angle_rotation, TRUE);
    nx_motors_rotate_angle(MOVE_LEFT_MOTOR, -MOVE_TURN_SPEED, \
        -angle_rotation, TRUE);
    nx_systick_wait_ms(-(600 * angle / 90));
  }


  // Modify the robot's orientation
  power = (move_log(robot->orientation) + angle / 90) % 4;
  robot->orientation = move_pow(2, power);
}
