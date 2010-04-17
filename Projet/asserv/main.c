#include "base/display.h"
#include "base/drivers/systick.h"
#include "base/drivers/avr.h"
#include "base/core.h"
#include "base/drivers/motors.h"
#include "base/drivers/radar.h"
#include "base/drivers/sensors.h"
#include "base/drivers/sound.h"
#include "base/drivers/_sound.h"
#include "base/types.h"

#define HIGH_SPEED 80 // Vitesse Rapide
#define MEDIUM_SPEED 70 // Vitesse Lente
#define LOW_SPEED 70 // Vitesse Lente
#define TURN_SPEED 80//vitesse pour tourner	

#define LEFT_MOTOR 2
#define RIGHT_MOTOR 0

#define WHEEL_SPACING 110
#define WHEEL_HEIGHT 55
#define PI 3.141593

#define DO 523
#define RE 587
#define MI 659
#define FA 698
#define SOL 783
#define LA 880
#define SI 987

#define TOUCH_SENSOR 0
#define LIGHT_SENSOR 2
#define RADAR_SENSOR 1


/**
 * Déclaration des différentes fonctions
 */

void vPlaySond(void);
void vDisplaySensorsValues(void);
void vTurnLeft(void);
void vTurnRight(void);
void vTurn(void);
void vHalfTurn(void);
void vForward(U32 distance);
void vGetPosition(U32 *last_left_counter, U32 *last_right_counter, int *left, int *right );

/**
 * Fonction permettant d'arrêter le robot lors d'un appui sur le bouton CANCEL
 */
static void watchdog(void){
	if(nx_avr_get_button()==BUTTON_CANCEL)
		nx_core_halt();	
}




/**
 * Cette fonction permet de donner la position du robot
 */
void vGetPosition(U32 *last_left_counter, U32 *last_right_counter, int *left, int *right )
{
	float left_position;
	float right_position;
	U32 left_counter;
	U32 right_counter;
	U32 current_left_counter;
	U32 current_right_counter;

	current_left_counter=nx_motors_get_tach_count(LEFT_MOTOR);
	current_right_counter=nx_motors_get_tach_count(RIGHT_MOTOR);

	left_counter=current_left_counter-*last_left_counter;
	right_counter=current_right_counter-*last_right_counter;

	*last_left_counter=current_left_counter;
	*last_right_counter=current_right_counter;

	left_position=(left_counter*1.0/360)*PI*WHEEL_HEIGHT;
	right_position=(right_counter*1.0/360)*PI*WHEEL_HEIGHT;

	*left=(int)left_position;
	*right=(int)right_position;
	
}

/**
 * Cette fonction permet de faire avancer le robot d'une certaine distance
 */
void vForward(U32 distance)
{
	float angle;
	angle=360*distance/(PI*55);

	nx_motors_rotate_angle(LEFT_MOTOR,HIGH_SPEED,(U32)angle,TRUE);
	nx_motors_rotate_angle(RIGHT_MOTOR,HIGH_SPEED,(U32)angle,TRUE); 	
	nx_systick_wait_ms(500+distance/200*1000); 	
}


/**
 * Cette fonction permet de faire tourner le robot vers la gauche
 */
void vTurnRight()
{
	float angle;
	angle=360*PI*WHEEL_SPACING/(4*PI*55);

	nx_motors_rotate_angle(LEFT_MOTOR,TURN_SPEED,(U32)angle,TRUE);
	nx_motors_rotate_angle(RIGHT_MOTOR,-TURN_SPEED,(U32)angle,TRUE); 	 	
	nx_systick_wait_ms(600); 	 	
}


/**
 * Cette fonction permet de faire tourner le robot vers la gauche
 */
void vTurnLeft()
{
	float angle;
	angle=360*PI*WHEEL_SPACING/(4*PI*55);

	nx_motors_rotate_angle(LEFT_MOTOR,-TURN_SPEED,(U32)angle,TRUE);
	nx_motors_rotate_angle(RIGHT_MOTOR,TURN_SPEED,(U32)angle,TRUE);
	nx_systick_wait_ms(600); 	 	
}

void vHalfTurn()
{
	float angle;
	angle=360*PI*WHEEL_SPACING/(2*PI*55);

	nx_motors_rotate_angle(LEFT_MOTOR,-TURN_SPEED,(U32)angle,TRUE);
	nx_motors_rotate_angle(RIGHT_MOTOR,TURN_SPEED,(U32)angle,TRUE); 	 
	nx_systick_wait_ms(1200); 	 	
}

void vTurn()
{
	float angle;
	angle=360*PI*WHEEL_SPACING/(PI*55);

	nx_motors_rotate_angle(LEFT_MOTOR,-TURN_SPEED,(U32)angle,TRUE);
	nx_motors_rotate_angle(RIGHT_MOTOR,TURN_SPEED,(U32)angle,TRUE); 	 
	nx_systick_wait_ms(2400); 	 	
}

void vPlaySond()
{
	//Play sound
	nx_sound_freq(DO,200);
	nx_sound_freq(DO,200);
	nx_sound_freq(DO,200);
	nx_sound_freq(RE,200);
	nx_sound_freq(MI,200);
	nx_sound_freq(RE,200);
	nx_sound_freq(DO,200);
	nx_sound_freq(MI,200);
	nx_sound_freq(RE,200);
	nx_sound_freq(RE,200);
	nx_sound_freq(DO,200);
}

void vDisplaySensorsValues()
{
	U8 radar;
	U32 contact;
	U32 light;

	while(TRUE){
		nx_display_clear();
		nx_display_cursor_set_pos(0,5);
		nx_display_string("Touch");
		contact=nx_sensors_analog_get(TOUCH_SENSOR);
		nx_display_cursor_set_pos(7,5);
		nx_display_uint(contact);

		nx_display_cursor_set_pos(0,6);
		nx_display_string("Radar");
		radar=nx_radar_read_distance(RADAR_SENSOR,0);
		nx_display_cursor_set_pos(7,6);
		nx_display_uint(radar);

		nx_display_cursor_set_pos(0,7);
		nx_display_string("Light");
		light=nx_sensors_analog_get(LIGHT_SENSOR);
		nx_display_cursor_set_pos(7,7);
		nx_display_uint(light);
		nx_systick_wait_ms(50);
	}
}

/**
 * Fonction principale
 */
void main(void) {
/*
	U32 last_left_counter=0;
	U32 last_right_counter=0;
	int left=0;
	int right=0;
*/
	nx_systick_install_scheduler(watchdog);

	// initialise le radar ultrason
	nx_radar_init(RADAR_SENSOR);
	nx_radar_read_distance(RADAR_SENSOR,0);

	//initialise le son
	nx__sound_init();

	// initialise le capteur de contact
	nx_sensors_analog_enable(TOUCH_SENSOR);

	//initialise le capteur de luminosité
	nx_sensors_analog_enable(LIGHT_SENSOR);
	nx_sensors_analog_digi_set(LIGHT_SENSOR, DIGI0); 

	nx_display_clear();
	nx_display_cursor_set_pos(0,0);
	nx_display_string("Projet LO52");
	nx_display_cursor_set_pos(0,1);
	nx_display_string("Test asservissement");
	nx_display_cursor_set_pos(0,2);
	nx_display_uint(nx_avr_get_battery_voltage());
	nx_display_cursor_set_pos(4,2);
	nx_display_string("/4000");
	nx_display_cursor_set_pos(0,3);
	nx_display_string("WAIT OK button");

	// On attend un appui sur le bouton ok pour commencer l'exécution
	while(nx_avr_get_button()!=BUTTON_OK)
	{
		nx_systick_wait_ms(10);
	}
	nx_systick_wait_ms(2000);



	/**
	 * Phase d'initialisation test avance et affiche position
	 */
/*
	// Met les moteurs en marche
	nx_motors_rotate (LEFT_MOTOR,HIGH_SPEED);
	nx_motors_rotate (RIGHT_MOTOR,HIGH_SPEED);
	nx_systick_wait_ms(2000);
	nx_motors_stop(LEFT_MOTOR,TRUE);
	nx_motors_stop(RIGHT_MOTOR,TRUE);

	vGetPosition(&last_left_counter, &last_right_counter, &left, &right);

	nx_display_cursor_set_pos(0,0);
	nx_display_uint(left);
	//distance moteur gauche en mm	
	nx_display_cursor_set_pos(0,1);
	nx_display_uint(right);
*/
/*	vTurnLeft();
	nx_systick_wait_ms(1000);
	vTurnRight();
	nx_systick_wait_ms(1000);
	vForward(500);
	nx_systick_wait_ms(2000);
*/

	//afiche les valeurs des capteurs
	//vDisplaySensorsValues();

//	vHalfTurn();
//	vHalfTurn();
//	vForward(1000);

/*
	vTurn();
*/



	vForward(500);

	vTurnLeft();
	vForward(500);
	vTurnLeft();
	vForward(500);
	vTurnLeft();
	vForward(500);
	vTurnLeft();


	nx_systick_wait_ms(2000);
}


