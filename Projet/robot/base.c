#include "base.h"
#include "sensors.h"
#include "bt.h"

void base_init(void) {
  // Initialisation du BlueTooth
  bt_init();

  // Initialisation des capteurs
  nx_radar_init(SENSORS_RADAR);
  nx_sensors_analog_enable(SENSORS_TOUCH);
  nx_sensors_analog_enable(SENSORS_LIGHT);
}
