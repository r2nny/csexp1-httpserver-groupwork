/**
 * prototype 
 */

#ifndef EXP1LIB_H
#define EXP1LIB_H

int exp1_tcp_listen(int port);
int exp1_tcp_connect(const char *hostname, int port);
int exp1_udp_listen(int port);
int exp1_udp_connect(const char *hostname, int port);
double gettimeofday_sec(void);
int exp1_do_talk(int sock);

// for handling HTTP request
void exp1_send_404(int sock);

#endif


