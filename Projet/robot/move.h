// Movement functions and constants

#ifndef _LO52_MOVE_H
#define _LO52_MOVE_H

#include "structures.h"

#define MOVE_SPEED          80
#define MOVE_HIGH_SPEED     (SPEED + 10)
#define MOVE_LOW_SPEED      (SPEED - 10)
#define MOVE_TURN_SPEED     (SPEED)

#define MOVE_LEFT_MOTOR     2
#define MOVE_RIGHT_MOTOR    0

#define MOVE_WHEEL_SPACING  116
#define MOVE_WHEEL_DIAMETER 55

// Handle an obstacle by notifying the application and escaping from it.
void move_handle_obstacle(robot_struct*, U8**);
// Move following the instructions given by the application.
void handle_mode_guided(robot_struct*);
// Move in autonomous mode, exploring the field.
void move_handle_autonomous(robot_struct*, U8**);

#endif
