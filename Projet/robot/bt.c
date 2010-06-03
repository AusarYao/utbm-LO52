#include "bt.h"
#include "move.h"

// Bluetooth initialisation. Return port handle or -1 in case of errors.
int bt_init(void) {
  nx_bt_init();
  nx_bt_set_friendly_name(BT_DEVICE_NAME);
  nx_bt_set_discoverable(TRUE);

  return nx_bt_open_port();
}

//TODO
// Check if the connection is not down.
bool bt_is_active(void) {
  return TRUE;
}

//TODO
// Process the pending messages on the Bluetooth stack.
void bt_check_connect(struct robot_struct *robot) {

  S8 data[BT_MSG_SIZE];
  //recept message
  nx_bt_stream_read((U8*)data, BT_MSG_SIZE);

  //if we received data
  if(nx_bt_stream_data_read()>1){
    struct robot_struct robot_next = {MODE_GUIDED, data[1], data[2], data[3]};
    switch(data[0]){
      case BT_MSG_POSITION:
        move_guided(robot, robot_next.X, robot_next.Y);
      break;
      // Last wall's position request.
      case BT_MSG_LST_WALL:
      //not used for the bot
      break;
      // Last captured flag position request.
      case BT_MSG_CPT_FLAG:
      
      break;
      // Position recalibration request.
      case BT_MSG_RECAL_POS:

      break;
      // Abort last instruction.
      case BT_MSG_ABORT_LST:

      break;
      // Ping the other end.
      case BT_MSG_PING:
        bt_msg_send_ack();
      break;
      // ACK
      case BT_MSG_ACK:

      break;
    }
  }
}

// Send a bt_message structure
int bt_msg_send(struct bt_message *msg) {

  U8 data[BT_MSG_SIZE];
  data[0]=msg->type;
  data[1]=msg->X;
  data[2]=msg->Y;
  data[3]=msg->mask;

  nx_bt_stream_write((U8 *)data, BT_MSG_SIZE);
  return 1;
}

//send a message to send the position
void bt_msg_send_position(U8 X, U8 Y, U8 orientation) {
  U8 data[BT_MSG_SIZE];
  data[0]=BT_MSG_POSITION;
  data[1]=X;
  data[2]=Y;
  data[3]=orientation;
  nx_bt_stream_write((U8 *)data, BT_MSG_SIZE);
}

//send a message with a wall
void bt_msg_send_wall(U8 X, U8 Y, U8 side) {
  U8 data[BT_MSG_SIZE];
  data[0]=BT_MSG_LST_WALL;
  data[1]=X;
  data[2]=Y;
  data[3]=side;
  nx_bt_stream_write((U8 *)data, BT_MSG_SIZE);
}

//send a message with a flag
void bt_msg_send_flag(U8 X, U8 Y, U8 side) {
  U8 data[BT_MSG_SIZE];
  data[0]=BT_MSG_CPT_FLAG;
  data[1]=X;
  data[2]=Y;
  data[3]=side;
  nx_bt_stream_write((U8 *)data, BT_MSG_SIZE);
}

//send a ping
void bt_msg_send_ping() {
  U8 data[BT_MSG_SIZE];
  data[0]=BT_MSG_PING;
  data[1]=1;
  data[2]=1;
  data[3]=1;
  nx_bt_stream_write((U8 *)data, BT_MSG_SIZE);
}

//send ACK
void bt_msg_send_ack() {
  U8 data[BT_MSG_SIZE];
  data[0]=BT_MSG_ACK;
  data[1]=1;
  data[2]=1;
  data[3]=1;
  nx_bt_stream_write((U8 *)data, BT_MSG_SIZE);
}
