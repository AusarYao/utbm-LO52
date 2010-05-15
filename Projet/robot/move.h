// Movement functions and constants

#ifndef _LO52_MOVE_H
#define _LO52_MOVE_H

#include "structures.h"

// Handle an obstacle by notifying the application and escaping from it.
void move_handle_obstacle(robot_struct*, unsigned short**);
// Move following the instructions given by the application.
void handle_mode_guided(robot_struct*);
// Move in autonomous mode, exploring the field.
void move_handle_autonomous(robot_struct*, unsigned short**);

#endif
