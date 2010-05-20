#include "bt.h"


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
}

// Send a bt_message structure
int bt_msg_send(struct bt_message *msg) {

  U8 data[MSG_SIZE];
  data[0]=msg->type;
  data[1]=msg->X;
  data[2]=msg->Y;
  data[3]=msg->mask;

  nx_bt_stream_write((U8 *)data, MSG_SIZE);
  return 1;
}
