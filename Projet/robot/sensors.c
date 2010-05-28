#include "sensors.h"

// Return TRUE if an obstacle has been encountered, FALSE otherwise.
bool sensors_contact(void) {
  if (nx_sensors_analog_get(SENSORS_TOUCH) < SENSORS_TOUCH_THRESHOLD)
    return TRUE;
  return FALSE;
}

// Return TRUE if a wall is detected on the right of the robot.
bool sensors_wall(void) {
  if (nx_radar_read_distance(SENSORS_RADAR, 0) <= SENSORS_RADAR_THRESHOLD)
    return TRUE;
  return FALSE;
}
