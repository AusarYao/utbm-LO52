// Bluetooth API and structures
#ifndef _LO52_BT_H
#define _LO52_BT_H

#include "base/drivers/bt.h"

#include "base.h"

// BlueTooth device name
#define BT_DEVICE_NAME      "NXT-GROUPE-E"
#define BT_PIN              "1234"
// Bluetooth message size, in bytes.
#define BT_MSG_SIZE         4

// Different message types sent over the BT connection.
// Position request from the app.
#define BT_MSG_POSITION     1
// Last wall's position request.
#define BT_MSG_LST_WALL     2
// Last captured flag position request.
#define BT_MSG_CPT_FLAG     3
// Position recalibration request.
#define BT_MSG_RECAL_POS    4
// Abort last instruction.
#define BT_MSG_ABORT_LST    5
// Ping the other end.
#define BT_MSG_PING         6
// ACK.
#define BT_MSG_ACK          7

struct bt_message {
  // Message type.
  U8 type;
  // Relevant positions
  U8 X,
     Y;
  // If a wall's concerned, here is the relevant mask giving its position.
  // If not, we hereby give the robot orientation.
  U8 mask;
};

// Bluetooth initialisation
int bt_init(void);

//close bluetooth connection 
void bt_die(void);

// Check if the connection is not down.
bool bt_is_active(void);

//Wait for a connection
void bt_wait_connection(void);

// Process the pending messages on the Bluetooth stack.
void bt_check_connect(struct robot_struct*, U8[MAP_X_SIZE][MAP_Y_SIZE]);

// Send a bt_message structure
int bt_msg_send(struct bt_message*);

//send a message with the position
void bt_msg_send_position(U8, U8, U8);
//send a message with a wall
void bt_msg_send_wall(U8, U8, U8);
//send a message with a flag
void bt_msg_send_flag(U8, U8, U8);
//send ping
void bt_msg_send_ping(void);
//send ack
void bt_msg_send_ack(void);

#endif
