#include "sensors.h"

static U32 light_threshold = 500;

// Return TRUE if an obstacle has been encountered, FALSE otherwise.
bool sensors_contact(void) {
  if (nx_sensors_analog_get(SENSORS_TOUCH) < SENSORS_TOUCH_THRESHOLD)
    return TRUE;
  return FALSE;
}

// Return TRUE if a wall is detected on the right of the robot.
bool sensors_wall(void) {
  if (nx_radar_read_distance(SENSORS_RADAR, 0) < SENSORS_RADAR_THRESHOLD)
    return TRUE;
  return FALSE;
}

// Return TRUE if we are on a flag.
bool sensors_flag(void) {
  if (nx_sensors_analog_get(SENSORS_LIGHT) < light_threshold)
    return TRUE;
  return FALSE;
}

void sensors_init(void) {
  nx_radar_init(SENSORS_RADAR);
  sensors_wall();
  nx_sensors_analog_enable(SENSORS_TOUCH);
  nx_sensors_analog_enable(SENSORS_LIGHT);
  nx_sensors_analog_digi_set(SENSORS_LIGHT, DIGI0);

  // Recuperation de la valeur de luminosite courante : on est sur un
  // drapeau
  light_threshold = nx_sensors_analog_get(SENSORS_LIGHT) - 6;
}
