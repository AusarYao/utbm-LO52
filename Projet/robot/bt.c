#include "bt.h"

// Bluetooth initialisation. Return port handle or -1 in case of errors.
int bt_init(void) {
  nx_bt_init();
  nx_bt_set_friendly_name(BT_DEVICE_NAME);
  nx_bt_set_discoverable(TRUE);

  return nx_bt_open_port();
}
