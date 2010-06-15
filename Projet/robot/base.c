#include "base.h"
#include "bt.h"
#include "move.h"
#include "sensors.h"

//add the possibility to use the buttons
static void watchdog(void) {
  switch (nx_avr_get_button()) {
    case BUTTON_CANCEL:
      base_kill();
      break;
    case BUTTON_OK:
      break;
    case BUTTON_LEFT:
      break;
    case BUTTON_RIGHT:
      break;
    case BUTTON_NONE:
      break;
  }
}

void base_kill(void) {
  nx_display_clear();
  nx_display_cursor_set_pos(0, 0);
  nx_display_string("End...\n");
  nx_systick_wait_ms(2000);
  nx_motors_stop(MOVE_LEFT_MOTOR, TRUE);
  nx_motors_stop(MOVE_RIGHT_MOTOR, TRUE);
  nx_radar_close(SENSORS_RADAR);
  bt_die();
  nx_systick_wait_ms(2000);
  nx_core_halt();
}

void base_init(struct robot_struct *robot) {
  // Initialisation de l'ecran
  nx_display_clear();
  nx_display_cursor_set_pos(0, 0);
  nx_display_string("Lo52 project\n");
  nx_display_cursor_set_pos(0, 2);

  // Display battery level.
  nx_display_uint(nx_avr_get_battery_voltage());
  nx_display_cursor_set_pos(4, 2);
  nx_display_string("/4000\n");

  // Mise en place du Wachdog
  nx_systick_install_scheduler(watchdog);

  // Initialisation du BlueTooth
  bt_init();

  //initialisation du son
  nx__sound_init();

  // Initialisation des capteurs
  nx_radar_init(SENSORS_RADAR);
  sensors_wall();
  nx_sensors_analog_enable(SENSORS_TOUCH);
  nx_sensors_analog_enable(SENSORS_LIGHT);
  nx_sensors_analog_digi_set(SENSORS_LIGHT, DIGI0);

  // Recuperation de la valeur de luminosite courante : on est sur un
  // drapeau
  robot->light_threshold = nx_sensors_analog_get(SENSORS_LIGHT) - 4;

  // Initialisation de la partie déplacement.
  move_init();
}
