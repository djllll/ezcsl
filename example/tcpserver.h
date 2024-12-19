#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

extern int tcp_server_open(void);
extern void tcp_server_close(void);
extern void tcp_server_send_char(char *c,int len);
extern void tcp_server_recv_char_in_thread(void);
extern void tcp_server_start_receving_thread(void);

#endif