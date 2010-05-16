// Main definitions of constants
#ifndef _LO52_BASE_H
#define _LO52_BASE_H

// Include the systems' headers
#include "base/core.h"
#include "base/types.h"

// Definitions of the different modes used by the robot.
#define MODE_AUTONOMOUS       1
#define MODE_GUIDED           2

// Size of the map
#define MAP_X_SIZE            3
#define MAP_Y_SIZE            3
#define MAP_SUB_SIZE          30

#include "structures.h"

void base_init(void);

#endif
