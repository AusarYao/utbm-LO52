#include "bt.h"
#include "base/display.h"
#include "base/types.h"
#include "base/core.h"
#include "base/drivers/bt.h"
#include "base/drivers/systick.h"
#include "base/util.h"



#define BT_NAME			"NXT-TOS"
#define BT_INQUIRY_MAXDEV	255
#define BT_INQUIRY_TIMEOUT	0x20
#define BT_INQUIRY_CLASS	{ 0, 0, 0, 0 }
#define BT_PIN "1234"

//port handle or -1 if failure
static int btp;

//handle
static int bth;


int bt_init(void)
{
	bth = -1;
	nx_bt_init();
	nx_bt_set_friendly_name(BT_NAME);
	nx_bt_set_discoverable(TRUE);
	if ((btp = nx_bt_open_port()) == -1)
		return -1;




	return btp;
}

void bt_close(void)
{
	if (nx_bt_stream_opened())
		nx_bt_stream_close();
	if (bth != -1) {
		nx_bt_close_connection(bth);
		bth = -1;
	}
}

void bt_die(void)
{
	bt_close();
	if (btp != -1) {
		nx_bt_close_port(btp);
		btp = -1;
	}
}

void bt_wait_connection(void)
{
	bt_close();
	while (!nx_bt_stream_opened()) {
		if (nx_bt_connection_pending()) {
			nx_bt_accept_connection(TRUE);
			while ((bth = nx_bt_connection_established()) == -1)
				nx_systick_wait_ms(100);
			nx_bt_stream_open(bth);
			nx_display_cursor_set_pos(0,5);
			nx_display_string("connected\n");
		}
		nx_systick_wait_ms(100);
	}
}
