// Bluetooth API and structures
#ifndef _LO52_BT_H
#define _LO52_BT_H

#include "base/drivers/bt.h"

#include "base.h"

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
  S32 X,
      Y;
  // If a wall's concerned, here is the relevant mask giving its position.
  // If not, we hereby give the robot orientation.
  U8 mask;
};

// Check if the connection is not down.
bool bt_is_active(void);

// Process the pending messages on the Bluetooth stack.
void bt_check_connect(struct robot_struct*);

#endif
