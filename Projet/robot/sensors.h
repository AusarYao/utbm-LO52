// All sensors' related constants and functions
#ifndef _LO52_SENSORS_H
#define _LO52_SENSORS_H

// Include the systems' headers
#include "base/drivers/radar.h"
#include "base/drivers/sensors.h"

// Constants
#define SENSORS_TOUCH     0
#define SENSORS_RADAR     1
#define SENSORS_LIGHT     2

#define SENSORS_TOUCH_THRESHOLD   500
#define SENSORS_RADAR_THRESHOLD   20
#define SENSORS_LIGHT_THRESHOLD   500

// Return TRUE if an obstacle has been encountered, FALSE otherwise.
bool sensors_contact(void);

// Return TRUE if a wall is detected on the right of the robot.
bool sensors_wall(void);

// Return TRUE if we are on a flag.
bool sensors_flag(void);

#endif
