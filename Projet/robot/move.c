#include <math.h>

#include "move.h"
#include "bt.h"
#include "sensors.h"

static U32 move_tach_right, move_tach_left;

//add the walls on the right to the map
static void move_add_wall_to_map(struct robot_struct*,
    U8[MAP_X_SIZE][MAP_Y_SIZE], U8);
// Return the mask of an adjacent wall
static U8 move_adjacent_square(struct robot_struct*, U8);
// Make the robot move backward on the given distance in millimeters.
// Start gradually and stop gradually if stop flag is set.
static void move_backward(struct robot_struct*, U32, bool);
// Compute the angle to rotate the wheels for the given distance
static double move_compute_angle(double);
// Compute the distance for the given angle.
static double move_compute_distance(double);
//compute the last position from the last Wall
//static void move_compute_position_from_wall(struct robot_struct*);
static void move_escape_wall(struct robot_struct*);
// Make the robot move forward on the given distance in millimeters.
// Start gradually and stop gradually if the stop flag is set.
static void move_forward(struct robot_struct*, U32, bool,
    U8[MAP_X_SIZE][MAP_Y_SIZE]);
static bool move_get_coordinates(struct robot_struct*, U8, U8*, U8*);
// Get the difference between current tach count and last recorded.
static S32 move_get_tach_diff(void);
// Handle an obstacle by notifying the application and escaping from it.
static void move_handle_obstacle(struct robot_struct*,
    U8[MAP_X_SIZE][MAP_Y_SIZE]);
static U8 move_log(U8);
// Return TRUE if the bot can go to the square, FALSE otherwise
static bool move_no_wall_to_go(struct robot_struct*, U8,
    U8[MAP_X_SIZE][MAP_Y_SIZE]);
static U8 move_pow(U8, U8);
static void move_refresh_tach(void);
static bool move_retry(void);
// Make the robot turn by the given angle, in degrees.
static void move_rotate_angle(struct robot_struct*, S32);
//Return TRUE if the adjacent square has never been visited, FALSE otherwise
static bool move_square_unknown(struct robot_struct*,
    U8[MAP_X_SIZE][MAP_Y_SIZE], U8);
static void move_start(S8, S8);
static void move_stop(struct robot_struct*);
static void move_update_position(struct robot_struct*);



//add the walls on the right/top to the map
static void move_add_wall_to_map(struct robot_struct* robot,
    U8 map[MAP_X_SIZE][MAP_Y_SIZE], U8 side) {
  U8 X_adjacent_square;
  U8 Y_adjacent_square;
  int power;

  power = (move_log(robot->orientation) + move_log(side)) % 4;
  map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] |= move_pow(2, power);

  X_adjacent_square = robot->X / MAP_SUB_SIZE;
  Y_adjacent_square = robot->Y / MAP_SUB_SIZE;

  bt_msg_send_wall(robot->X / MAP_SUB_SIZE, robot->Y / MAP_SUB_SIZE,\
      move_pow(2, power));


  //if the square on the side is in the map
  if(move_get_coordinates(robot, side, &X_adjacent_square,\
                                       &Y_adjacent_square)) {
    power = (power + 2) % 4;
    map[X_adjacent_square][Y_adjacent_square] |= move_pow(2, power);
  }
}

//to get the mask of an adjacent Wall
static U8 move_adjacent_square(struct robot_struct *robot, U8 direction) {
  int power = (move_log(direction) + move_log(robot->orientation)) % 4;
  return move_pow(2, power);
}

// Move in autonomous mode, exploring the field.
void move_autonomous(struct robot_struct *robot,
    U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {

    map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] |= BASE_VISITED;
  if(sensors_wall()) {
    move_add_wall_to_map(robot, map, BASE_RIGHT);
  }

  if(sensors_flag() && \
      !(map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] & BASE_FLAG)) {
    nx_sound_freq_async(523,200);
    move_stop(robot);
    bt_msg_send_flag(robot->X / MAP_SUB_SIZE, robot->Y / MAP_SUB_SIZE, 1);
    nx_systick_wait_ms(MOVE_FLAG_FREEZE * 1000);
    move_forward(robot, BASE_FLAG_SIZE, FALSE, map);
    map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] |= BASE_FLAG;
  }
  else if (sensors_contact()) {
    move_handle_obstacle(robot, map);
    move_add_wall_to_map(robot, map, BASE_UP);
  }
  //if the up case has never been visited and there is no walls
  else if(move_square_unknown(robot, map, BASE_UP) && \
         (move_no_wall_to_go(robot, BASE_UP, map))) {
    move_forward(robot, MAP_SUB_SIZE, TRUE, map);
  }
  //if the right case has never been visited and there is no walls
  else if(move_square_unknown(robot, map, BASE_RIGHT) && \
         (move_no_wall_to_go(robot, BASE_RIGHT, map))) {
    move_rotate_angle(robot, 90);
    move_forward(robot, MAP_SUB_SIZE - 10, TRUE, map);
  }
  //if the left case has never been visited and there is no walls
  else if(move_square_unknown(robot, map, BASE_LEFT) && \
         (move_no_wall_to_go(robot, BASE_LEFT, map))) {
    move_rotate_angle(robot, -90);
    move_forward(robot, MAP_SUB_SIZE - 10, TRUE, map);
  }
  //if we can't go straight on
  else if(move_no_wall_to_go(robot, BASE_UP, map)) {
    move_forward(robot, MAP_SUB_SIZE, TRUE, map);
  }
  //if there is no walls to go on the right
  else if((move_no_wall_to_go(robot, BASE_RIGHT, map))) {
    move_rotate_angle(robot, 90);
    move_forward(robot, MAP_SUB_SIZE - 10, TRUE, map);
  }
  //if there is no walls on the left
  else if((move_no_wall_to_go(robot, BASE_LEFT, map))) {
    move_rotate_angle(robot, -90);
    move_forward(robot, MAP_SUB_SIZE - 10, TRUE, map);
  }
  else {
    move_rotate_angle(robot, 180);
    move_forward(robot, MAP_SUB_SIZE - 10, TRUE, map);
  }
}

// Make the robot move backward on the given distance in millimeters.
// Start gradually and stop gradually if stop flag is set.
static void move_backward(struct robot_struct *robot, U32 distance,
    bool stop) {
  double wheel_angle = move_compute_angle(distance);
  U32 init_tach[2];


  // Get the initial state of the motors.
  init_tach[0] = nx_motors_get_tach_count(MOVE_LEFT_MOTOR);
  init_tach[1] = nx_motors_get_tach_count(MOVE_RIGHT_MOTOR);

  // Make the motors turn at full speed, backward.
  move_start(-MOVE_HIGH_SPEED,-MOVE_HIGH_SPEED);

  if(stop) {
    // Wait until we reach the final part.
    while((init_tach[0] - nx_motors_get_tach_count(MOVE_LEFT_MOTOR)) <
        wheel_angle);
    move_stop(robot);
    move_update_position(robot);
  }
}

// Compute the angle to rotate the wheels for the given distance
static double move_compute_angle(double distance) {
  double wheel_rotations = distance / (M_PI * MOVE_WHEEL_DIAMETER / 10.);
  return wheel_rotations * 360;
}

// Compute the distance for the given angle.
static double move_compute_distance(double angle) {
  double wheel_rotations = angle / 360;
  return wheel_rotations * (M_PI * MOVE_WHEEL_DIAMETER / 10.);
}

//compute the last position from the last Wall
/*
static void move_compute_position_from_wall(struct robot_struct *robot) {
  switch(robot->orientation) {
    case BASE_UP:
      robot->Y=(robot->Y / MAP_SUB_SIZE) * MAP_SUB_SIZE + 14;
    break;
    case BASE_DOWN:
      robot->Y=(robot->Y / MAP_SUB_SIZE) * MAP_SUB_SIZE + 11;
    break;
    case BASE_RIGHT:
      robot->X=(robot->Y / MAP_SUB_SIZE) * MAP_SUB_SIZE + 14;
    break;
    case BASE_LEFT:
      robot->X=(robot->Y / MAP_SUB_SIZE) * MAP_SUB_SIZE + 11;
    break;
  }
}
*/

static void move_escape_wall(struct robot_struct *robot) {
  move_stop(robot);
  nx_systick_wait_ms(100);
  move_backward(robot, 3, TRUE);
}

// Make the robot move forward on the given distance in millimeters.
// Start gradually and stop gradually if the stop flag is set.
static void move_forward(struct robot_struct *robot, U32 distance,
    bool stop, U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {
  double wheel_angle = move_compute_angle(distance);
  U32 init_tach[2];

  // Get the initial state of the motors.
  init_tach[0] = nx_motors_get_tach_count(MOVE_LEFT_MOTOR);
  init_tach[1] = nx_motors_get_tach_count(MOVE_RIGHT_MOTOR);

  // Make the motors turn at full speed.
  move_start(MOVE_HIGH_SPEED, MOVE_HIGH_SPEED);

  if(stop) {
   // Wait until we reach the final distance.
    while((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
        wheel_angle && (nx_motors_get_tach_count(MOVE_RIGHT_MOTOR)
        - init_tach[1]) < wheel_angle && !(sensors_contact())) {
      nx_systick_wait_ms(1);
    }

    if(!(sensors_contact())) {
      if((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
          wheel_angle) {
        nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
        while((nx_motors_get_tach_count(MOVE_RIGHT_MOTOR) - init_tach[1]) <
        wheel_angle) {
          nx_systick_wait_ms(1);
        }
        nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
      }
      else {
        nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
        while((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
            wheel_angle) {
          nx_systick_wait_ms(1);
        }
        nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
      }
    }
    else {
      move_stop(robot);
    }

    move_update_position(robot);

    if(sensors_contact()) {
      move_handle_obstacle(robot, map);
      move_add_wall_to_map(robot, map, BASE_UP);
    }

    if(sensors_flag() && \
        !(map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] & BASE_FLAG)) {
       nx_sound_freq_async(523,200);
      bt_msg_send_flag(robot->X / MAP_SUB_SIZE, robot->Y / MAP_SUB_SIZE, 1);
      nx_systick_wait_ms(MOVE_FLAG_FREEZE * 1000);

      move_forward(robot, BASE_FLAG_SIZE, FALSE, map);
      map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] |= BASE_FLAG;
    }
  }
}

static bool move_get_coordinates(struct robot_struct *robot, U8 side,
    U8 *X_adjacent_square, U8 *Y_adjacent_square) {
 //get the mask of the square on the right of the bot
  U8 mask_adjacent_wall=move_adjacent_square(robot, side);
  int power = (move_log(robot->orientation) + move_log(side)) % 4;

  if(!(((move_pow(2, power) == BASE_RIGHT) && (robot->X == MAP_X_SIZE-1)) || \
          ((move_pow(2, power) == BASE_LEFT) && (robot->X == 0)) || \
          ((move_pow(2, power) == BASE_DOWN) && (robot->Y == MAP_Y_SIZE-1)) || \
          ((move_pow(2, power) == BASE_UP) && (robot->Y == 0)))) {

    switch(mask_adjacent_wall) {
      case BASE_RIGHT:
        *X_adjacent_square += 1;
      break;
      case BASE_DOWN:
        *Y_adjacent_square -= 1;
      break;
      case BASE_LEFT:
        *X_adjacent_square -= 1;
      break;
      case BASE_UP:
        *Y_adjacent_square += 1;
      break;
    }
    return TRUE;
  }
  return FALSE;
}

static S32 move_get_tach_diff(void) {
  U32 tach_left = nx_motors_get_tach_count(MOVE_LEFT_MOTOR),
      tach_right = nx_motors_get_tach_count(MOVE_RIGHT_MOTOR);
  return ((S32)tach_left - (S32)move_tach_left + (S32)tach_right -\
      (S32)move_tach_right) / 2;
}

//TODO
// Move following the instructions given by the application.
void move_guided(struct robot_struct *current_robot,\
                 U8 next_X, U8 next_Y, U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {

  U8 orientation = move_log(current_robot->orientation);
  U32 dist = 0;
  // We have to move on X
  if(current_robot->X / MAP_SUB_SIZE != next_X / MAP_SUB_SIZE) {
    // We have to go on the left
    if(current_robot->X > next_X) {
      dist = current_robot->X - next_X;
      move_rotate_angle(current_robot, (3 - orientation) * 90);
    }
    //we have to go on the right
    else {
      dist = next_X - current_robot->X;
      move_rotate_angle(current_robot, (1 - orientation) * 90);
    }
  }
  else {
    //we have to go down
    if(current_robot->Y / MAP_SUB_SIZE > next_Y / MAP_SUB_SIZE) {
      dist = current_robot->Y - next_Y;
      move_rotate_angle(current_robot, (2 - orientation) * 90);
    }
    //we have to go up
    else {
      dist = next_Y - current_robot->Y;
      move_rotate_angle(current_robot, (-orientation) * 90);
    }
  }
  move_forward(current_robot, dist, TRUE, map);
}


// Handle an obstacle by notifying the application and escaping from it.
static void move_handle_obstacle(struct robot_struct *robot,
    U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {
  map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] |= \
      robot->orientation;

  if(move_retry()) {
    move_escape_wall(robot);
  }
  else {
    map[robot->X / MAP_SUB_SIZE][robot->Y / MAP_SUB_SIZE] ^= \
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
    a >>= 1;
  return count;
}

//return TRUE if the bot can go to the square, FALSE otherwise
static bool move_no_wall_to_go(struct robot_struct *robot, U8 direction,
    U8 map[MAP_X_SIZE][MAP_Y_SIZE]) {
  U8 Wall = move_adjacent_square(robot, direction);

  //if there is no wall to go on the next case
  if(map[robot->X / MAP_SUB_SIZE ][robot->Y / MAP_SUB_SIZE] & Wall)
    return FALSE;
  return TRUE;
}

static U8 move_pow(U8 a, U8 b) {
  if(!b)
    return 1;
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
  U32 init_tach[2];
  double distance, angle_rotation;
  int power;

  if(angle != 0) {

    move_update_position(robot);

    nx_systick_wait_ms(100);

    // Get the initial state of the motors.
    init_tach[0] = nx_motors_get_tach_count(MOVE_LEFT_MOTOR);
    init_tach[1] = nx_motors_get_tach_count(MOVE_RIGHT_MOTOR);

    // Ensure that our angle is in the [-180, 180] range.
    if(angle > 180)
      angle -= 360;
    else if(angle < -180)
      angle += 360;

    // Compute the number of rotations, and its associated angle.
    distance = M_PI * angle * (MOVE_WHEEL_SPACING / 10. )/ 360.;
    angle_rotation = move_compute_angle(distance);

    if(angle_rotation > 0) {
      move_start(-MOVE_TURN_SPEED, MOVE_TURN_SPEED);

      // We wait for one motor to have reached the target angle.
      while(((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
            angle_rotation) && ((init_tach[1] -
            nx_motors_get_tach_count(MOVE_RIGHT_MOTOR)) > -angle_rotation)) {
        nx_systick_wait_ms(1);
      }
      if((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
          angle_rotation) {
        nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
        while((init_tach[1] - nx_motors_get_tach_count(MOVE_RIGHT_MOTOR)) >
            -angle_rotation) {
          nx_systick_wait_ms(1);
        }
        nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
      }
      else {
        nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
        while((nx_motors_get_tach_count(MOVE_LEFT_MOTOR) - init_tach[0]) <
            angle_rotation) {
          nx_systick_wait_ms(1);
        }
        nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
      }

    }
    else {
      move_start(MOVE_TURN_SPEED, -MOVE_TURN_SPEED);

      // We wait for one motor to have reached the target angle.
      while(((nx_motors_get_tach_count(MOVE_RIGHT_MOTOR) - init_tach[1]) >
            angle_rotation) && ((init_tach[0] -
            nx_motors_get_tach_count(MOVE_LEFT_MOTOR)) < -angle_rotation)) {
        nx_systick_wait_ms(1);
      }

      if((nx_motors_get_tach_count(MOVE_RIGHT_MOTOR) - init_tach[1]) >
          angle_rotation) {
        nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
        while((init_tach[0] - nx_motors_get_tach_count(MOVE_LEFT_MOTOR)) <
            -angle_rotation) {
          nx_systick_wait_ms(1);
        }
        nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
      }
      else {
        nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
        while((nx_motors_get_tach_count(MOVE_RIGHT_MOTOR) - init_tach[1]) >
            angle_rotation) {
          nx_systick_wait_ms(1);
        }
        nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
      }
    }

    // Modify the robot's orientation
    power = (move_log(robot->orientation) + angle / 90) % 4;
    if(power<0)
      power+=4;
    robot->orientation = move_pow(2, power);


    move_refresh_tach();
  }
}

//Return TRUE if the adjacent square has never been visited, FALSE otherwise
static bool move_square_unknown(struct robot_struct *robot,
  U8 map[MAP_X_SIZE][MAP_Y_SIZE], U8 side) {
  U8 X_adjacent_square = robot->X / MAP_SUB_SIZE;
  U8 Y_adjacent_square = robot->Y / MAP_SUB_SIZE;
  if(move_get_coordinates(robot, side, &X_adjacent_square, &Y_adjacent_square))
    //if the adjacent case has already been visited
    if(map[X_adjacent_square][Y_adjacent_square] & BASE_VISITED)
      return FALSE;
  return TRUE;
}

static void move_start(S8 right_speed, S8 left_speed) {
  static bool state = FALSE;
  if(state) {
    nx_motors_rotate(MOVE_LEFT_MOTOR, left_speed);
    nx_motors_rotate(MOVE_RIGHT_MOTOR, right_speed);
    state = FALSE;
  }
  else {
    nx_motors_rotate(MOVE_RIGHT_MOTOR, right_speed);
    nx_motors_rotate(MOVE_LEFT_MOTOR, left_speed);
    state = TRUE;
  }
}

static void move_stop(struct robot_struct *robot) {
  static bool state = FALSE;
  if(state) {
    nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
    nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
  }
  else {
    nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
    nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
  }
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
      robot->Y += dist_diff;
      break;
    case BASE_DOWN:
      robot->Y -= dist_diff;
      break;
    case BASE_LEFT:
      robot->X -= dist_diff;
      break;
    case BASE_RIGHT:
      robot->X += dist_diff;
      break;
  }

  bt_msg_send_position((U8)robot->X / MAP_SUB_SIZE,
      (U8) robot->Y / MAP_SUB_SIZE,(U8) robot->orientation);
}


