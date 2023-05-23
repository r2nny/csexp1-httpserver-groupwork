#include "exp1.h"
#include "exp1lib.h"

typedef struct{
  char cmd[64];
  char path[256];
  char real_path[256];
  char type[64];
  int code;
  int size;
} exp1_info_type;

int exp1_http_session(int sock);
int exp1_parse_header(char* buf, int size, exp1_info_type* info);
void exp1_parse_status(char* status, exp1_info_type *pinfo);
void exp1_check_file(exp1_info_type *info);
void exp1_http_reply(int sock, exp1_info_type *info);
void exp1_send_404(int sock);
void exp1_send_file(int sock, char* filename);

#define MAX_CLIENTS 10
#define BUFFER_SIZE 2048

int main(int argc, char **argv){
  int sock_listen;
  int client_sockets[MAX_CLIENTS]; // クライアントのソケット記憶用
  fd_set readfds; // 読み込み可能なソケットを監視するセット
  int max_sd; // 最大ソケットディスクリプタ

  sock_listen = exp1_tcp_listen(10090);

  // サーバの待機状態に入る
  if (listen(sock_listen, MAX_CLIENTS) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  // クライアントソケットの初期化
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    client_sockets[i] = 0;
  }

  char buf[BUFFER_SIZE]; // 受信データのバッファ

  while(1){
    struct sockaddr addr;
    int sock_client;
    int len;
    // ソケットセットを初期化
    FD_ZERO(&readfds);

    // サーバソケットをセットに追加
    FD_SET(sock_listen, &readfds);
    max_sd = sock_listen;

    // クライアントソケットをセットに追加
    for (int i = 0; i < MAX_CLIENTS; ++i) {
      int sd = client_sockets[i];

      // 有効なソケットディスクリプタであればセットに追加
      if (sd > 0) {
        FD_SET(sd, &readfds);
      }

      // 最大ソケットディスクリプタを更新
      if (sd > max_sd) {
        max_sd = sd;
      }
    }

    // ソケットの状態を監視
    int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

    if (activity < 0) {
      perror("select");
      exit(EXIT_FAILURE);
    }

    // サーバソケットに新しい接続があるか確認
    if (FD_ISSET(sock_listen, &readfds)) {
      struct sockaddr_in client_addr;
      socklen_t addr_len = sizeof(client_addr);

      // 新しい接続を受け入れる
      int new_sd = accept(sock_listen, (struct sockaddr*)&client_addr, &addr_len);
      if (new_sd < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      // クライアントソケットを記憶
      for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (client_sockets[i] == 0) {
          client_sockets[i] = new_sd;
          break;
        }
      }
    }

    // クライアントソケットでデータが受信可能か確認し、処理
    for (int i = 0; i < MAX_CLIENTS; ++i) {
      int sd = client_sockets[i];

      if (FD_ISSET(sd, &readfds)) {
        // クライアントからデータを受信
        int recv_size = recv(sd, buf, sizeof(buf), 0);
        if (recv_size == 0) {
          // 接続のクローズ
          close(sd);
          client_sockets[i] = 0;
        } else if (recv_size < 0) {
          perror("recv");
        } else {
          sock_client = accept(sock_listen, &addr, (socklen_t*) &len);
          exp1_http_session(sock_client);

          shutdown(sock_client, SHUT_RDWR);
          close(sock_client);
        }
      }
    }
  }

  // サーバソケットのクローズ
  //close(sock_listen);
}


int exp1_http_session(int sock){
  char buf[2048];
  int recv_size = 0;
  exp1_info_type info;
  int ret = 0;

  while(ret == 0){
    int size = recv(sock, buf + recv_size, 2048, 0);

    if(size == -1){
      return -1;
    }

    recv_size += size;
    ret = exp1_parse_header(buf, recv_size, &info);
  }

  exp1_http_reply(sock, &info);

  return 0;
}

int exp1_parse_header(char* buf, int size, exp1_info_type* info){
  char status[1024];
  int i, j;

  enum state_type{
    PARSE_STATUS,
    PARSE_END
  }state;

  state = PARSE_STATUS;
  j = 0;
  for(i = 0; i < size; i++){
    switch(state){
      case PARSE_STATUS:
      if(buf[i] == '\r'){
        status[j] = '\0';
        j = 0;
        state = PARSE_END;
        exp1_parse_status(status, info);
        exp1_check_file(info);
      }else{
        status[j] = buf[i];
        j++;
      }
      break;

      default:
      {}
      break;
    }

    if(state == PARSE_END){
      return 1;
    }
  }

  return 0;
}

void exp1_parse_status(char* status, exp1_info_type *pinfo){
  char cmd[1024];
  char path[1024];
  //char* pext;
  int i, j;

  enum state_type{
    SEARCH_CMD,
    SEARCH_PATH,
    SEARCH_END
  }state;

  state = SEARCH_CMD;
  j = 0;
  for(i = 0; i < strlen(status); i++){
    switch(state){
      case SEARCH_CMD:
      if(status[i] == ' '){
        cmd[j] = '\0';
        j = 0;
        state = SEARCH_PATH;
      }else{
        cmd[j] = status[i];
        j++;
      }
      break;

      case SEARCH_PATH:
      if(status[i] == ' '){
        path[j] = '\0';
        j = 0;
        state = SEARCH_END;
      }else{
        path[j] = status[i];
        j++;
      }
      break;

      default:
      {}
      break;
    }
  }

  strcpy(pinfo->cmd, cmd);
  strcpy(pinfo->path, path);
}

void exp1_check_file(exp1_info_type *info){
  struct stat s;
  int ret;
  char* pext;

  sprintf(info->real_path, "html%s", info->path);
  ret = stat(info->real_path, &s);

  if((s.st_mode & S_IFMT) == S_IFDIR){
    sprintf(info->real_path, "%s/index.html", info->real_path);
  }

  ret = stat(info->real_path, &s);

  if(ret == -1){
    info->code = 404;
  }else{
    info->code = 200;
    info->size = (int) s.st_size;
  }

  pext = strstr(info->path, ".");
  if(pext != NULL && strcmp(pext, ".html") == 0){
    strcpy(info->type, "text/html");
  }else if(pext != NULL && strcmp(pext, ".jpg") == 0){
    strcpy(info->type, "image/jpeg");
  }
}

void exp1_http_reply(int sock, exp1_info_type *info){
  char buf[16384];
  int len;
  int ret;

  if(info->code == 404){
    exp1_send_404(sock);
    printf("404 not found %s\n", info->path);
    return;
  }

  len = sprintf(buf, "HTTP/1.0 200 OK\r\n");
  len += sprintf(buf + len, "Content-Length: %d\r\n", info->size);
  len += sprintf(buf + len, "Content-Type: %s\r\n", info->type);
  len += sprintf(buf + len, "\r\n");

  ret = send(sock, buf, len, 0);
  if(ret < 0){
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return;
  }

  exp1_send_file(sock, info->real_path);
}

void exp1_send_404(int sock){
  char buf[16384];
  int ret;

  sprintf(buf, "HTTP/1.0 404 Not Found\r\n\r\n");
  printf("%s", buf);
  ret = send(sock, buf, strlen(buf), 0);

  if(ret < 0){
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}

void exp1_send_file(int sock, char* filename){
  FILE *fp;
  int len;
  char buf[16384];

  fp = fopen(filename, "r");
  if(fp == NULL){
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return;
  }

  len = fread(buf, sizeof(char), 16384, fp);
  while(len > 0){
    int ret = send(sock, buf, len, 0);
    if(ret < 0){
      shutdown(sock, SHUT_RDWR);
      close(sock);
      break;
    }
    len = fread(buf, sizeof(char), 1460, fp);
  }

  fclose(fp);
}
