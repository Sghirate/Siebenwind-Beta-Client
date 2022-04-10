#pragma once

extern bool g_DisablePing;

bool socket_init();
void socket_shutdown();
u32 socket_localaddress();

typedef void* tcp_socket;

tcp_socket tcp_open();
bool tcp_connect(tcp_socket socket, const char *address, u16 port);
int tcp_select(tcp_socket socket);
int tcp_recv(tcp_socket socket, unsigned char *data, size_t max_size);
int tcp_send(tcp_socket socket, unsigned char *data, size_t size);
void tcp_close(tcp_socket socket);

typedef void *icmp_handle;

icmp_handle icmp_open();
int icmp_query(icmp_handle handle, const char *ip, u32 *timems);
void icmp_close(icmp_handle handle);
