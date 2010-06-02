// Main definitions of constants
#ifndef _LO52_BASE_H
#define _LO52_BASE_H

#include <stdlib.h>

// Include the systems' headers
#include "base/core.h"
#include "base/types.h"
#include "base/display.h"
#include "base/drivers/systick.h"
#include "base/drivers/avr.h"

// Definitions of the different modes used by the robot.
#define MODE_AUTONOMOUS       1
#define MODE_GUIDED           2

// Size of the map
#define MAP_X_SIZE            8
#define MAP_Y_SIZE            4
#define MAP_SUB_SIZE          250

#define BASE_FLAG_SIZE        150

// Robot's different orientations.
#define BASE_UP             0b000001
#define BASE_RIGHT          0b000010
#define BASE_DOWN           0b000100
#define BASE_LEFT           0b001000

#define BASE_FLAG           0b010000
#define BASE_VISITED        0b100000

#include "structures.h"

void base_init(void);

#endif
