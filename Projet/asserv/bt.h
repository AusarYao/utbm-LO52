#ifndef __NXOS_NXPLORER_BT_H__
#define __NXOS_NXPLORER_BT_H__

void	bt_close(void);
int	bt_init(void);
void	bt_die(void);
void	bt_wait_connection(void);

#endif /* __NXOS_NXPLORER_BT_H__ */
