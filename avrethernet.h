#ifndef AVRETHERNETH
#define AVRETHERNETH

unsigned char OpenSocket(unsigned char  sock, unsigned char  eth_protocol, unsigned int  tcp_port);
void CloseSocket(unsigned char  sock);
void DisconnectSocket(unsigned char  sock);
unsigned char Listen(unsigned char  sock);
unsigned char Send(unsigned char  sock, const unsigned char  *buf, unsigned int  buflen);
unsigned int Receive(unsigned char  sock, unsigned char  *buf, unsigned int  buflen);
unsigned int ReceivedSize(unsigned char  sock);

void my_select(void);
void my_deselect(void);
unsigned char my_xchg(unsigned char  val);
void my_reset(void);

#endif
