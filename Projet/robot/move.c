#include <math.h>

#include "move.h"
#include "bt.h"
#include "sensors.h"

static U32 move_tach_right, move_tach_left;

// Make the robot move backward on the given distance in centimeters.
// Start gradually and stop gradually if stop flag is set.
static void move_backward(struct robot_struct*, U32, bool);
// Compute the angle to rotate the wheels for the given distance
static double move_compute_angle(double);
// Compute the distance for the given angle.
static double move_compute_distance(double);
static void move_escape_wall(struct robot_struct*);
// Make the robot move forward on the given distance in centimeters.
// Start gradually and stop gradually if the stop flag is set.
static void move_forward(struct robot_struct*, U32, bool);
// Get the difference between current tach count and last recorded.
static S32 move_get_tach_diff(void);
// Handle an obstacle by notifying the application and escaping from it.
static void move_handle_obstacle(struct robot_struct*,
    U8[MAP_X_SIZE][MAP_Y_SIZE]);
static U8 move_log(U8);
static U8 move_pow(U8, U8);
static void move_refresh_tach(void);
static bool move_retry(void);
// Make the robot turn by the given angle, in degrees.
static void move_rotate_angle(struct robot_struct*, S32);
static void move_stop(struct robot_struct*);
static void move_update_position(struct robot_struct*);
//to get the coordinates of an adjacent square
static void adjacent_square(struct robot_struct *robot, U8 direction,\
    U8 *Wall);
//return TRUE if the bot can go to the square, FALSE otherwise
static bool no_wall_to_go(struct robot_struct *robot, U8 direction,\
    U8 map[MAP_X_SIZE][MAP_Y_SIZE]);

// Move in autonomous mode, exploring the field.
void move_autonomous(struct robot_struct *robot,
    U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {

  move_update_position(robot);

  if(sensors_flag() && \
      !(map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] & BASE_FLAG)) {

    move_stop(robot);
    nx_systick_wait_ms(MOVE_FLAG_FREEZE * 1000);
    move_forward(robot, BASE_FLAG_SIZE, FALSE);
    map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] += BASE_FLAG;
  }
  else if (sensors_contact()) {
    move_handle_obstacle(robot, map);
  }
  else {
    move_forward(robot, MAP_SUB_SIZE, FALSE);
  }
}

// Make the robot move backward on the given distance in centimeters.
// Start gradually and stop gradually if stop flag is set.
static void move_backward(struct robot_struct *robot, U32 distance,
    bool stop) {
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

  if(stop) {
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
}

// Compute the angle to rotate the wheels for the given distance
static double move_compute_angle(double distance) {
  double wheel_rotations = distance / (2. * M_PI * MOVE_WHEEL_DIAMETER);
  return wheel_rotations * 360;
}

// Compute the distance for the given angle.
static double move_compute_distance(double angle) {
  double wheel_rotations = angle / 360;
  return wheel_rotations * (2 * M_PI * MOVE_WHEEL_DIAMETER);
}

static void move_escape_wall(struct robot_struct *robot) {
  move_backward(robot, 10, TRUE);
  move_rotate_angle(robot, -90);
}

// Make the robot move forward on the given distance in centimeters.
// Start gradually and stop gradually if the stop flag is set.
static void move_forward(struct robot_struct *robot, U32 distance,
    bool stop) {
  double wheel_angle = move_compute_angle(distance);
  U32 init_tach[2];

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

  if(stop) {
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
}

static S32 move_get_tach_diff(void) {
  U32 tach_left = nx_motors_get_tach_count(MOVE_LEFT_MOTOR),
      tach_right = nx_motors_get_tach_count(MOVE_RIGHT_MOTOR);
  return (tach_left - move_tach_left + tach_right - move_tach_right) / 2;
}

//TODO
// Move following the instructions given by the application.
void move_guided(struct robot_struct *current_robot,\
                 struct robot_struct *next_robot) {

  U8 orientation = move_log(current_robot->orientation);

  // We have to move on X
  if(current_robot->X != next_robot->X) {
    // We have to go on the left
    if(current_robot->X == next_robot->X + 1) {
      move_rotate_angle(current_robot, -(orientation - 1) * 90);
    }
    //we have to go on the right
    else {
      move_rotate_angle(current_robot, -(orientation + 1) * 90);
    }
  }
  else {
    //we have to go down
    if(current_robot->Y == (next_robot->Y + 1)) {
      move_rotate_angle(current_robot, -orientation * 90);
    }
    //we have to go up
    else {
      move_rotate_angle(current_robot, -(orientation - 2) * 90);
    }
  }
  move_forward(current_robot, 25, TRUE);
}


// Handle an obstacle by notifying the application and escaping from it.
static void move_handle_obstacle(struct robot_struct *robot,
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

void move_init(void) {
  move_tach_left = nx_motors_get_tach_count(MOVE_LEFT_MOTOR);
  move_tach_right = nx_motors_get_tach_count(MOVE_RIGHT_MOTOR);
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

static void move_refresh_tach(void) {
  move_tach_left = nx_motors_get_tach_count(MOVE_LEFT_MOTOR);
  move_tach_right = nx_motors_get_tach_count(MOVE_RIGHT_MOTOR);
}

//TODO
static bool move_retry(void) {
  return TRUE;
}

// Make the robot turn by the given angle, in degrees.
static void move_rotate_angle(struct robot_struct *robot, S32 angle) {
  double distance;
  double angle_rotation;
  int power;

  move_update_position(robot);

  // Ensure that our angle is in the [-180, 180] range.
  if(angle > 180)
    angle -= 360;
  else if(angle < -180)
    angle += 360;

  // Compute the number of rotations, and its associated angle.
  distance = 2. * M_PI * angle * MOVE_WHEEL_SPACING / 360.;
  angle_rotation = move_compute_angle(distance);

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

  move_refresh_tach();
}

static void move_stop(struct robot_struct *robot) {

  nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
  nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);

  // Compute the new position.
  move_update_position(robot);
}

static void move_update_position(struct robot_struct *robot) {
  S32 tach_diff, dist_diff;

  tach_diff = move_get_tach_diff();
  move_refresh_tach();
  dist_diff = move_compute_distance(tach_diff);

  // Change the position of the robot.
  switch(robot->orientation) {
    case BASE_UP:
      robot->Y += (U8)dist_diff;
      break;
    case BASE_DOWN:
      robot->Y -= (U8)dist_diff;
      break;
    case BASE_LEFT:
      robot->X -= (U8)dist_diff;
      break;
    case BASE_RIGHT:
      robot->X += (U8)dist_diff;
      break;
  }
}


//to get the coordinates of an adjacent square
static void adjacent_square(struct robot_struct *robot, U8 direction,
    U8 *Wall){
  if((robot->orientation==1 && direction==2)|| \
     (robot->orientation==2 && direction==1)|| \
     (robot->orientation==4 && direction==8)|| \
     (robot->orientation==8 && direction==4)){
    *Wall=BASE_RIGHT;
  }
  if((robot->orientation==1 && direction==8)|| \
     (robot->orientation==2 && direction==4)|| \
     (robot->orientation==4 && direction==2)|| \
     (robot->orientation==8 && direction==1)){
    *Wall=BASE_LEFT;
  }
  if((robot->orientation==1 && direction==1)|| \
     (robot->orientation==2 && direction==8)|| \
     (robot->orientation==4 && direction==4)|| \
     (robot->orientation==8 && direction==2)){
    *Wall=BASE_UP;
  }
  if((robot->orientation==1 && direction==4)|| \
     (robot->orientation==2 && direction==2)|| \
     (robot->orientation==4 && direction==1)|| \
     (robot->orientation==8 && direction==8)){
    *Wall=BASE_DOWN;
  }
}

//return TRUE if the bot can go to the square, FALSE otherwise
static bool no_wall_to_go(struct robot_struct *robot, U8 direction,
    U8 map[MAP_X_SIZE][MAP_Y_SIZE]){
  U8 Wall=0;
  adjacent_square(robot, direction, &Wall);

  //if there is no wall to go on the next case
  if(!(map[robot->X][robot->Y] & Wall)){
    return TRUE;
  }
  return FALSE;
}
