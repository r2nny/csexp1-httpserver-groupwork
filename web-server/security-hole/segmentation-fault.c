#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <float.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>

void test_func(char *msg)
{
  uint32_t y = 0x88776655;
  char buf[16];
  uint32_t z = 0xccbbaa99;
  int i, j;
  uint32_t base = 0;

  printf("0x%016x: buf\n", buf);
  printf("0x%016x: &y\n", &y);
  printf("0x%016x: msg\n", msg);
  printf("0x%016x: &z\n", &z);


  printf("\n");
  printf("start dump memory\n");
  base = (uint32_t)&z;
  for(i = 0; i < 18; i++){
    printf("0x%016x: ", base);
    for(j = 0; j < 8; j++){
      uint8_t *p = (uint8_t*) base;
      printf("%02x", *(p + j));
    }
    printf("\n");
    base += 8;
  }
  printf("end dump memory\n");
  printf("\n");

  strcpy(buf, msg);
}

int main()
{
  uint32_t x = 0x44332211;
  char data[] = "abcd" "efgh" "ijkl" "mnop" "qrst" "uvwx"
                "abcd" "efgh" "ijkl" "mnop" "qrst" "uvwx";
  printf("0x%016x: data\n", data);
  printf("0x%016x: &x\n", &x);
  printf("0x%016x: main\n", main);

  test_func(data);
}