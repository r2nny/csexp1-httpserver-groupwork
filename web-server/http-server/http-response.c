#include "exp1.h"
#include "exp1lib.h"
#define HTTP_RESPONSE_BUFFER_SIZE 16384


void exp1_send_404(int sock) {
    char buf[HTTP_RESPONSE_BUFFER_SIZE];
    int ret;

    sprintf(buf, "HTTP/1.0 404 Not Found\r\n\r\n");
    printf("%s", buf);
    ret = send(sock, buf, strlen(buf), 0);

    if (ret < 0) {
        shutdown(sock, SHUT_RDWR);
        close(sock);
    }
}

void send_response(int sock, int statusCode) {
    char buf[HTTP_RESPONSE_BUFFER_SIZE];
    int ret;
    const char *message;

    switch(statusCode) {
        case 200:
            message = "HTTP/1.0 200 OK\r\n\r\n";
            break;
        case 201:
            message = "HTTP/1.0 201 Created\r\n\r\n";
            break;
        case 400:
            message = "HTTP/1.0 400 Bad Request\r\n\r\n";
            break;
        case 401:
            message = "HTTP/1.0 401 Unauthorized\r\n\r\n";
            break;
        case 403:
            message = "HTTP/1.0 403 Forbidden\r\n\r\n";
            break;
        case 404:
            message = "HTTP/1.0 404 Not Found\r\n\r\n";
            break;
        case 500:
            message = "HTTP/1.0 500 Internal Server Error\r\n\r\n";
            break;
        default:
            message = "HTTP/1.0 400 Bad Request\r\n\r\n";
            break;
    }

    sprintf(buf, "%s", message);
    printf("%s", buf);
    ret = send(sock, buf, strlen(buf), 0);

    if (ret < 0) {
        shutdown(sock, SHUT_RDWR);
        close(sock);
    }
}

