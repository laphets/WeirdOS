/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/6
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "socket.h"


/**
 * We will generate some rand port in [1024, 65536)
 * @return
 */
uint16_t rand_port() {
    uint16_t port = 0;
    int32_t i = 0;
    for(i = 0; i < 20 /* MAX TRYING NUMBER */; i++) {
        port = rand(SOCKET_MAGIC_RAND_SEED, SOCKET_MAX_PORT);
        if(port < 1024) {
            continue;
        }
        int32_t j = 0;
        uint8_t is_present = 0;
        for(j = 0; j < SOCKET_MAX_NUM; j++) {
            if(socket_list[j].present == 1 && socket_list[j].port == port) {
                is_present = 1;
                break;
            }
        }
        if(is_present == 1)
            continue;
        return port;
    }
    return port;
}

uint32_t rand_seq_num() {
    return rand(SOCKET_MAGIC_RAND_SEED, 100000000);
}

void init_socket() {
    int32_t i = 0;
    for(i = 0; i < SOCKET_MAX_NUM; i++) {
        socket_t* socket = &socket_list[i];
        socket->present = 0;
        socket->sid = i;
    }
}

int32_t socket_open() {
    int32_t i = 0;
    socket_t* socket = NULL;
    for(i = 0; i < SOCKET_MAX_NUM; i++) {
        if(socket_list[i].present == 0) {
            socket = &socket_list[i];
            break;
        }
    }
    if (socket == NULL) {
        /* The socket is full, we just throw error */
        kprintf("Socket is full\n");
        return -1;
    }

    socket->present = 1;
    socket->port = rand_port();
    socket->recv_buffer = NULL;
    socket->tcp_status = SOCKET_TCP_STATUS_INIT;
    socket->dns_status = 0;

    return socket->sid;
}

void socket_close(int32_t sid) {
    socket_t* socket = &socket_list[sid];
    if(socket->present != 1)
        return;

    socket->present = 0;
    socket->port = 0;
}

socket_t* get_socket_by_port(uint16_t port) {
    int32_t i = 0;
    for(i = 0; i < SOCKET_MAX_NUM; i++) {
        if(socket_list[i].present == 1 && socket_list[i].port == port) {
            return &socket_list[i];
        }
    }
    return NULL;
}

uint32_t socket_dns_send(int32_t sid, uint8_t* domain, uint8_t* res_ip) {
    socket_t* socket = &socket_list[sid];
    if(socket->present != 1)
        return NULL;

    /* First send the request */
    socket->dns_status = 1;
    dns_send(socket->port, domain);
    /* Wait for response */
    while(socket->dns_status != 2) {};
    /* Return the addr */
    socket->dns_status = 0;
    /* kprintf("socket_dns_res_ip:\n"); */
    /* print_ip(socket->dns_res_ip); */
    memcpy(res_ip, socket->dns_res_ip, IPv4_ADDR_SIZE);
    return 0;
}

/**
 * Send a HTTP request to target and take back the chunked response
 * @param sid
 * @param target_ip
 */
void* socket_http_send(int32_t sid, uint8_t* target_ip, uint16_t target_port, uint8_t* data, uint32_t length, uint32_t* res_length) {
    kprintf("Begin send HTTP request...\n");
    socket_t* socket = &socket_list[sid];
    if(socket->present != 1)
        return NULL;

    memcpy(socket->target_ip, target_ip, IPv4_ADDR_SIZE);
    socket->has_sent_fin = 0;
    socket->tcp_common_length = 0;
    socket->data_packet_num = 0;
    socket->target_port = target_port;
    socket->tcp_status = SOCKET_TCP_STATUS_INIT;
    socket->my_seq_num = rand_seq_num();
    socket->others_seq_num = 0;
    socket->recv_buffer_size = 300000;
    socket->recv_buffer = kmalloc(socket->recv_buffer_size);
    memset(socket->recv_buffer, 0, socket->recv_buffer_size);
    socket->recv_buf_end_ptr = 0;

    /* Then we begin make connection */
    socket->tcp_status = SOCKET_TCP_STATUS_SENT_SYN;
    tcp_send(socket->port, target_port, target_ip, socket->my_seq_num, 0, TCP_FLAG_SYN, NULL, 0);
    socket->my_seq_num++;
    /* Wait for SYN/ACK */
    while(socket->tcp_status != SOCKET_TCP_STATUS_RECV_SYNACK) {};
    /* ACK for that SYN */
    socket->others_seq_num++;
    socket->first_data_packet_seq_num = socket->others_seq_num;
    tcp_send(socket->port, target_port, target_ip, 0, socket->others_seq_num, TCP_FLAG_ACK, NULL, 0);
    /* Then send http data */
    socket->tcp_status = SOCKET_TCP_STATUS_SENT_REQUEST;
    /* char http_request[] = "GET / HTTP/1.1\r\nHost: lumetta.web.engr.illinois.edu\r\n\r\n"; */
    tcp_send(socket->port, target_port, target_ip, socket->my_seq_num, socket->others_seq_num, TCP_FLAG_ACK | TCP_FLAG_PSH, data, length);
    socket->my_seq_num += length;
    /* Wait for the full data accept */
    while(socket->tcp_status != SOCKET_TCP_STATUS_SENT_FIN) {};

    if(res_length != NULL)
        *res_length = socket->recv_buf_end_ptr;

    /* kprintf("socket->data_packet_num: %d\n", socket->data_packet_num); */

    return socket->recv_buffer;
}
