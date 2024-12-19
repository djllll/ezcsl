#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>
#include "ezcsl_port.h"

#pragma comment(lib, "Ws2_32.lib")

#define PORT 3333

static WSADATA wsaData;
static int server_fd, new_socket;
static struct sockaddr_in address;
static int addrlen = sizeof(address);
static pthread_t thread;
static int thread_exit_signal = 0;

static int set_socket_timeout(int sockfd, int sec, int usec);
int tcp_server_open(void);
void tcp_server_close(void);
void tcp_server_send_char(char *c,int len);
void tcp_server_recv_char_in_thread(void);
void tcp_server_start_receving_thread(void);


static int set_socket_timeout(int sockfd, int sec, int usec) {
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
 
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt");
        return -1;
    }
    return 0;
}
 
 void tcp_server_send_char(char *c,int len)
{
    send(new_socket, c, len, 0);
}


void tcp_server_recv_char_in_thread(void)
{
    char buf;
    thread_exit_signal = 0;
    while(!thread_exit_signal){
        int valread = recv(new_socket, &buf, 1, 0);
        if(valread>=0){
            ezport_receive_a_char(buf);
        }else{
            if(send(new_socket,"",0,0)<0){
                fprintf(stderr, "Client Lost ! Restarting Server..\n");
                closesocket(new_socket);
                closesocket(server_fd);
                WSACleanup();
                Sleep(500);
                tcp_server_open();
            }
        }
    }
}


int tcp_server_open(void)
{
    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }

    // 创建套接字文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        fprintf(stderr, "Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // 将套接字绑定到端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        fprintf(stderr, "bind failed: %ld\n", WSAGetLastError());
        tcp_server_close();
        return 1;
    }

    // 设置超时
    if (set_socket_timeout(server_fd, 1, 0) < 0) {
        fprintf(stderr, "Set timeout failed: %ld\n", WSAGetLastError());
        tcp_server_close();
        return 1;
    } 

    // 监听连接
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        fprintf(stderr, "Listen failed: %ld\n", WSAGetLastError());
        tcp_server_close();
        return 1;
    }

    printf("Listener on port %d \n", PORT);

    // 接受客户端连接
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) == INVALID_SOCKET) {
        fprintf(stderr, "accept failed: %ld\n", WSAGetLastError());
        tcp_server_close();
        return 1;
    }

    printf("Client Arrived!\r\n");


    return 0;
}

void tcp_server_start_receving_thread(void){
    pthread_create(&thread,NULL,(void * )tcp_server_recv_char_in_thread,NULL);
}

void tcp_server_close(void)
{
    printf("TCP Server Exit! \r\n");
    thread_exit_signal = 1;
    pthread_join(thread,NULL);
    thread_exit_signal = 0;
    closesocket(new_socket);
    closesocket(server_fd);
    WSACleanup();
}

