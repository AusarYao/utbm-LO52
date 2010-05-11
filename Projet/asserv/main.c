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
#include "base/drivers/bt.h"

#include "bt.h"

#define MSG_SIZE 2

#define HIGH_SPEED 90 // Vitesse Rapide
#define MEDIUM_SPEED 80 // Vitesse Lente
#define LOW_SPEED 70 // Vitesse Lente
#define TURN_SPEED 80//vitesse pour tourner	

#define LEFT_MOTOR 2
#define RIGHT_MOTOR 0

#define WHEEL_SPACING 116
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

#define MOVE_TIME 110

volatile bool	quit = FALSE;

//ne sert à rien mais si je l'enlève le programme de boote pas... je ne sais pas pourquoi (dans le main)
int	uouih = 0;



// Déclaration des différentes fonctions
void vTurnRight(void);
void vForward(U32 distance);
void vTurn(void);
void vHalfTurn(void);


void init(void);
void die(void);
void send_data(void);
void detect_obstacle(void);
void handle_cmd(void);
void vPlaySond(void);
void vDisplaySensorsValues(void);
void vTurnLeft(U32 *last_left_counter, U32 *last_right_counter);
void vForwardUntilWall(U32 *last_left_counter, U32 *last_right_counter);
void vForwardUntilWhite(U32 *last_left_counter, U32 *last_right_counter);
void vDetectMurDroit(void);
void vGetPosition(U32 *last_left_counter, U32 *last_right_counter, int *left, int *right);
void vForwardStop(U32 *last_left_counter, U32 *last_right_counter, int distance);
void vBackStop(U32 *last_left_counter, U32 *last_right_counter, int distance);


//Arrete les moteurs et le robot avec CANCEL
static void watchdog(void) {
			S8 data[MSG_SIZE];

	switch (nx_avr_get_button()) {
		case BUTTON_CANCEL:
			nx_motors_stop(RIGHT_MOTOR, TRUE);
			nx_motors_stop(LEFT_MOTOR, TRUE);
			nx_core_halt();
		case BUTTON_OK:
			break;
		case BUTTON_LEFT:
			data[0]=98;	
			data[1]=98;
			nx_bt_stream_write((U8 *)data, 2);
			nx_systick_wait_ms(1000);
			break;
		case BUTTON_RIGHT:
			data[0]=99;	
			data[1]=99;
			nx_bt_stream_write((U8 *)data, 2);
			nx_systick_wait_ms(1000);
			break;
		case BUTTON_NONE:
			break;
	}

}





//Cette fonction permet de donner la position du robot
void vGetPosition(U32 *last_left_counter, U32 *last_right_counter, int *left, int *right) {
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


//avance en reduisant la vitesse des moteurs au départ et à l'arrivée vers le point choisi
void vForwardStop(U32 *last_left_counter, U32 *last_right_counter, int distance) {

	U32 pos=0;
	U32 pos1=0;
	U32 pos2=0;

	float fAngle;
	fAngle=360*distance/(PI*55);
	U32 uAngle=(U32)fAngle;

	nx_motors_rotate(LEFT_MOTOR, 70);
	nx_motors_rotate(RIGHT_MOTOR, 70);
	nx_systick_wait_ms(50);
	nx_motors_rotate(LEFT_MOTOR, 80);
	nx_motors_rotate(RIGHT_MOTOR, 80);
	nx_systick_wait_ms(50);
	nx_motors_rotate(LEFT_MOTOR, 90);
	nx_motors_rotate(RIGHT_MOTOR, 90);

	while(pos<*last_left_counter+uAngle-100) {
		pos=nx_motors_get_tach_count(LEFT_MOTOR);
		nx_systick_wait_ms(1);
	}
	nx_motors_rotate(LEFT_MOTOR, 80);
	nx_motors_rotate(RIGHT_MOTOR, 80);

	while(pos1<*last_left_counter+uAngle && pos2<*last_right_counter+uAngle) {
		pos1=nx_motors_get_tach_count(LEFT_MOTOR);
		pos2=nx_motors_get_tach_count(RIGHT_MOTOR);
		nx_systick_wait_ms(1);
	}

	if (pos1>=*last_left_counter+uAngle) {
		nx_motors_stop(LEFT_MOTOR, TRUE);
		while(pos2<*last_right_counter+uAngle) {			
			pos2=nx_motors_get_tach_count(RIGHT_MOTOR);
			nx_systick_wait_ms(1);
		}
		nx_motors_stop(RIGHT_MOTOR, TRUE);
	}
	else{
		nx_motors_stop(RIGHT_MOTOR, TRUE);
		while(pos1<*last_left_counter+uAngle) {			
			pos1=nx_motors_get_tach_count(LEFT_MOTOR);
			nx_systick_wait_ms(1);
		}
		nx_motors_stop(LEFT_MOTOR, TRUE);
	}
		*last_left_counter=nx_motors_get_tach_count(LEFT_MOTOR);
		*last_right_counter=nx_motors_get_tach_count(RIGHT_MOTOR);

}


//recule tant que la position voulue n'est pas atteinte
void vBackStop(U32 *last_left_counter, U32 *last_right_counter, int distance) {


	U32 pos1=*last_left_counter;
	U32 pos2=*last_right_counter;

	float fAngle;
	fAngle=360*distance/(PI*55);
	U32 uAngle=(U32)fAngle;

	nx_motors_rotate(LEFT_MOTOR, -80);
	nx_motors_rotate(RIGHT_MOTOR, -80);



	while(pos1>*last_left_counter-uAngle && pos2>*last_right_counter-uAngle) {
		pos1=nx_motors_get_tach_count(LEFT_MOTOR);
		pos2=nx_motors_get_tach_count(RIGHT_MOTOR);
		nx_systick_wait_ms(1);
	}

	if (pos1<=*last_left_counter-uAngle) {
		nx_motors_stop(LEFT_MOTOR, TRUE);
		while(pos2>*last_right_counter-uAngle) {			
			pos2=nx_motors_get_tach_count(RIGHT_MOTOR);
			nx_systick_wait_ms(1);
		}
		nx_motors_stop(RIGHT_MOTOR, TRUE);
	}
	else{
		nx_motors_stop(RIGHT_MOTOR, TRUE);
		while(pos1>*last_left_counter-uAngle) {			
			pos1=nx_motors_get_tach_count(LEFT_MOTOR);
			nx_systick_wait_ms(1);
		}
		nx_motors_stop(LEFT_MOTOR, TRUE);
	}
		*last_left_counter=nx_motors_get_tach_count(LEFT_MOTOR);
		*last_right_counter=nx_motors_get_tach_count(RIGHT_MOTOR);

}

/**
 * Cette fonction permet de faire avancer le robot d'une certaine distance
 */
//est moins precise que vForwardStop
void vForward(U32 distance) {
	float angle;
	angle=360*distance/(PI*55);

	nx_motors_rotate_angle(LEFT_MOTOR, HIGH_SPEED, (U32)angle, TRUE);
	nx_motors_rotate_angle(RIGHT_MOTOR, HIGH_SPEED, (U32)angle, TRUE);
	nx_systick_wait_ms(500+distance/200*1000);
}


/**
 * Cette fonction permet de faire tourner le robot vers la gauche
 */
void vTurnRight() {
	float angle;
	angle=360*PI*WHEEL_SPACING/(4*PI*55);



	nx_motors_rotate_angle(LEFT_MOTOR, TURN_SPEED, (U32)angle, TRUE);
	nx_motors_rotate_angle(RIGHT_MOTOR, -TURN_SPEED, (U32)angle, TRUE);	
	nx_systick_wait_ms(600);	
}


/**
 * Cette fonction permet de faire tourner le robot vers la gauche
 */
void vTurnLeft(U32 *last_left_counter, U32 *last_right_counter) {


	U32 pos1=*last_left_counter;
	U32 pos2=*last_right_counter;

	float fAngle;
	fAngle=360*PI*WHEEL_SPACING/(4*PI*55);
	U32 uAngle=(U32)fAngle;


	nx_motors_rotate(LEFT_MOTOR, -80);
	nx_motors_rotate(RIGHT_MOTOR, 80);

	while(pos1>*(last_left_counter-uAngle) && pos2<(*last_right_counter+uAngle)) {
		pos1=nx_motors_get_tach_count(LEFT_MOTOR);
		pos2=nx_motors_get_tach_count(RIGHT_MOTOR);
		nx_systick_wait_ms(1);
	}

	if (pos1>=*last_left_counter-uAngle) {
		nx_motors_stop(LEFT_MOTOR, TRUE);
		while(pos2<*last_right_counter+uAngle) {			
			pos2=nx_motors_get_tach_count(RIGHT_MOTOR);
			nx_systick_wait_ms(1);
		}
		nx_motors_stop(RIGHT_MOTOR, TRUE);
	}
	else{
		nx_motors_stop(RIGHT_MOTOR, TRUE);
		while(pos1>*last_left_counter-uAngle) {			
			pos1=nx_motors_get_tach_count(LEFT_MOTOR);
			nx_systick_wait_ms(1);
		}
		nx_motors_stop(LEFT_MOTOR, TRUE);
	}
		*last_left_counter=nx_motors_get_tach_count(LEFT_MOTOR);
		*last_right_counter=nx_motors_get_tach_count(RIGHT_MOTOR);
}

void vHalfTurn() {		nx_systick_wait_ms(1);
	float angle;
	angle=360*PI*WHEEL_SPACING/(2*PI*55);

	nx_motors_rotate_angle(LEFT_MOTOR, -TURN_SPEED, (U32)angle, TRUE);
	nx_motors_rotate_angle(RIGHT_MOTOR, TURN_SPEED, (U32)angle, TRUE);
	nx_systick_wait_ms(1200);
}

void vTurn() {
	float angle;
	angle=360*PI*WHEEL_SPACING/(PI*55);

	nx_motors_rotate_angle(LEFT_MOTOR, -TURN_SPEED, (U32)angle, TRUE);
	nx_motors_rotate_angle(RIGHT_MOTOR, TURN_SPEED, (U32)angle, TRUE);
	nx_systick_wait_ms(2400);
}

void vPlaySond() {
	//Play sound
	nx_sound_freq(DO, 200);
	nx_sound_freq(DO, 200);
	nx_sound_freq(DO, 200);
	nx_sound_freq(RE, 200);
	nx_sound_freq(MI, 200);
	nx_sound_freq(RE, 200);
	nx_sound_freq(DO, 200);
	nx_sound_freq(MI, 200);
	nx_sound_freq(RE, 200);
	nx_sound_freq(RE, 200);
	nx_sound_freq(DO, 200);
}

/**
 * Cette fonction affiche les valeurs des capteurs
 */
void vDisplaySensorsValues() {
	U8 radar;
	U32 contact;
	U32 light;

	while(TRUE) {
		nx_display_clear();
		nx_display_cursor_set_pos(0, 5);
		nx_display_string("Touch");
		contact=nx_sensors_analog_get(TOUCH_SENSOR);
		nx_display_cursor_set_pos(7, 5);
		nx_display_uint(contact);

		nx_display_cursor_set_pos(0, 6);
		nx_display_string("Radar");
		radar=nx_radar_read_distance(RADAR_SENSOR, 0);
		nx_display_cursor_set_pos(7, 6);
		nx_display_uint(radar);

		nx_display_cursor_set_pos(0, 7);
		nx_display_string("Light");
		light=nx_sensors_analog_get(LIGHT_SENSOR);
		nx_display_cursor_set_pos(7, 7);
		nx_display_uint(light);
		nx_systick_wait_ms(50);
	}
}


/**
 * Teste si le robot n'est pas en contact avec un mur
 */
void vForwardUntilWall(U32 *last_left_counter, U32 *last_right_counter) {
	//si on touche un obstacle
	if (nx_sensors_analog_get(TOUCH_SENSOR)<500) {
			//arrete les moteurs
			nx_motors_stop(RIGHT_MOTOR, TRUE);
			nx_motors_stop(LEFT_MOTOR, TRUE);

			*last_left_counter=nx_motors_get_tach_count(LEFT_MOTOR);
			*last_right_counter=nx_motors_get_tach_count(RIGHT_MOTOR);

			//recule
			vBackStop(last_left_counter, last_right_counter,50);

			//tourne à gauche
			vTurnLeft(last_left_counter, last_right_counter);

			//avance
			nx_motors_rotate(LEFT_MOTOR,MEDIUM_SPEED);
			nx_motors_rotate(RIGHT_MOTOR, MEDIUM_SPEED);
	}
}

//teste si le robot est sur un drapeau
void vForwardUntilWhite(U32 *last_left_counter, U32 *last_right_counter) {
	if (nx_sensors_analog_get(LIGHT_SENSOR)<500) {
			//arrete les moteurs
			nx_motors_stop(RIGHT_MOTOR, TRUE);
			nx_motors_stop(LEFT_MOTOR, TRUE);

			*last_left_counter=nx_motors_get_tach_count(LEFT_MOTOR);
			*last_right_counter=nx_motors_get_tach_count(RIGHT_MOTOR);

			vPlaySond();
			//attends 3 secondes
			nx_systick_wait_ms(3000);

			//avance pour sortir du drapeau
			vForwardStop(last_left_counter, last_right_counter,150); //changer ici en focntion de la taille du drapeau

			//avance
			nx_motors_rotate(LEFT_MOTOR,MEDIUM_SPEED);
			nx_motors_rotate(RIGHT_MOTOR, MEDIUM_SPEED);
	}
}

/**
 * Detecte s'il y'a un mur à droite
 */
void vDetectMurDroit() {
	if (nx_radar_read_distance(RADAR_SENSOR, 0)<20) {
		nx_display_cursor_set_pos(0, 6);
		nx_display_string("Mur droit");
	}
	else{
		nx_display_cursor_set_pos(0, 6);
		nx_display_string("         ");
	}
}


/**
 * Initialisation des differents capteurs et du bluetooth
 */
void init(void)
{
	nx_display_clear();
	nx_display_cursor_set_pos(0, 0);
	nx_display_string("Lo52 project\n");
	nx_display_cursor_set_pos(0, 2);

	//on affiche le niveau de la batterie
	nx_display_uint(nx_avr_get_battery_voltage());
	nx_display_cursor_set_pos(4, 2);
	nx_display_string("/4000\n");
	nx_systick_install_scheduler(watchdog);

	// initialise le radar ultrason
	nx_radar_init(RADAR_SENSOR);

	//initialise le bluetooth
	bt_init();
	nx_display_string("bluetooth...OK");

	//initialise le son
	nx__sound_init();

	// initialise le capteur de contact
	nx_sensors_analog_enable(TOUCH_SENSOR);

	//initialise le capteur de luminosité
	nx_sensors_analog_enable(LIGHT_SENSOR);
	nx_sensors_analog_digi_set(LIGHT_SENSOR, DIGI0);

}


/**
 * Arret du robot
 */
void die(void)
{
	nx_display_string("dying...\n");
	nx_motors_stop(LEFT_MOTOR, TRUE);
	nx_motors_stop(RIGHT_MOTOR, TRUE);
	nx_radar_close(RADAR_SENSOR);
	bt_die();
	nx_systick_wait_ms(2000);
	nx_core_halt();
}


/**
 * Envoi des donnée en bluetooth
 */
void send_data(void)
{
	S8 data[MSG_SIZE];
	data[0]=100;	
	data[1]=100;

	nx_bt_stream_write((U8 *)data, 2);
}


/**
 * Detecte si la valeur est inferieure à 25 et envoi un message
 */
void detect_obstacle(void)
{
	if (nx_radar_read_distance(RADAR_SENSOR, 0) <= 25) {
		send_data();

	}
}

void handle_cmd(void)
{
	S8 speed[MSG_SIZE];
	//reception du message
	nx_bt_stream_read((U8 *)speed, MSG_SIZE);
	while (!quit && nx_bt_stream_data_read() < 1) {
		detect_obstacle();
		nx_systick_wait_ms(100);
	}		
	

	if (speed[0] > 100 || speed[0] < -100
	 || speed[1] > 100 || speed[1] < -100)
		speed[0] = speed[1] = 0;

	

	nx_motors_rotate_time(LEFT_MOTOR, speed[0], MOVE_TIME, FALSE);
	nx_motors_rotate_time(RIGHT_MOTOR, speed[1], MOVE_TIME, FALSE);

return;
}

void main(void)
{
	uouih=0; //ne sert à rien mais si je l'enlève le programme de boote pas... je ne sais pas pourquoi
	init();
	while (!quit) {
		if (!nx_bt_stream_opened() || nx_bt_connection_pending())
			bt_wait_connection();
		else
			handle_cmd();
	}
	die();
}
