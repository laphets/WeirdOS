/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/4
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "tcp.h"

uint8_t tcp_check_flag(tcp_t* tcp_packet, uint8_t flags) {
    if((tcp_packet->flags & flags) == flags ) {
        return 1;
    }
    return 0;
}

/*
 * Ref: https://locklessinc.com/articles/tcp_checksum/
 */
void tcp_fresh_checksum(tcp_t* tcp_packet, uint32_t size, uint8_t* target_ip) {
    uint8_t * buf;
    uint32_t sum = 0;
    int i;

    /* We should also include some pre-header fields */
    tcp_checksum_pre_header_t pre_header;
    memcpy(pre_header.src_ip, self_ip_addr, IPv4_ADDR_SIZE);
    memcpy(pre_header.dst_ip, target_ip, IPv4_ADDR_SIZE);
    pre_header.zeros = 0;
    pre_header.protocol = 6;
    pre_header.length = htons(size);
    buf = (uint8_t*)(&pre_header);
    for (i = 0; i < sizeof(tcp_checksum_pre_header_t); i += 2)
    {
        uint16_t word16 = *(unsigned short *) &buf[i];
        sum += word16;
    }

    /* Then calculate the read body */
    buf = (uint8_t*)tcp_packet;
    /* Accumulate checksum */
    for (i = 0; i < size - 1; i += 2)
    {
        uint16_t word16 = *(unsigned short *) &buf[i];
        sum += word16;
    }

    /* Handle odd-sized case */
    if (size & 1)
    {
        uint16_t word16 = (unsigned char) buf[i];
        sum += word16;
    }

    /* Fold to get the ones-complement result */
    while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);

    /* Invert to get the negative in ones-complement arithmetic */
    uint16_t checksum = (uint16_t)(~sum);
    tcp_packet->checksum = checksum;
}


//volatile uint32_t tcp_status = 0;
//uint32_t my_seq_num;
//uint32_t others_seq_num;
//uint32_t tcp_recv_buf;
//uint32_t tcp_recv_buf_start_ptr, tcp_recv_buf_end_ptr;
//uint32_t tcp_recv_buf_size;
//uint8_t host_ip[4] = {18, 220, 149, 166};
//uint16_t target_port = 80;

/* 0->init, 1->have send syn, 2 -> have received syn+ack */

//void tcp_test() {
//    tcp_status = 0;
//    my_seq_num = 23333;
//    others_seq_num = 0;
//    tcp_recv_buf_size = 300000;
//    tcp_recv_buf = kmalloc(tcp_recv_buf_size);
//    tcp_recv_buf_start_ptr = 0;
//    tcp_recv_buf_end_ptr = 0;
//    memset(tcp_recv_buf, 0, tcp_recv_buf_size);
////    uint8_t host_ip[4] = {216, 58, 192, 164};
//
////    uint8_t host_ip[4] = {10, 0, 2, 2};
//
//    /* First make syn handshake */
//    tcp_status = 1;
//    tcp_send(50032, target_port, host_ip, my_seq_num, 0, TCP_FLAG_SYN, NULL, 0);
//    my_seq_num++;
//    while(tcp_status != 2) {};
//    /* Then ack for that */
//    others_seq_num++;
//    tcp_send(50032, target_port, host_ip, 0, others_seq_num, TCP_FLAG_ACK, NULL, 0);
//    /* Then send http data */
//    tcp_status = 3;
//    char http_request[] = "GET / HTTP/1.1\r\nHost: lumetta.web.engr.illinois.edu\r\n\r\n";
//    tcp_send(50032, target_port, host_ip, my_seq_num, others_seq_num, TCP_FLAG_ACK | TCP_FLAG_PSH, http_request, strlen(http_request));
//    my_seq_num += strlen(http_request);
//
//}

void tcp_send(uint16_t src_port, uint16_t target_port, uint8_t* target_ip, uint32_t seq_num, uint32_t ack_num, uint8_t flags, uint8_t* data, uint32_t length) {
    /* Just first assume send without optional field */
    uint32_t total_length = sizeof(tcp_t) + length;
    /* kprintf("Begin malloc for the packet0\n"); */
    tcp_t* tcp_packet = kmalloc(total_length);
    /* kprintf("End malloc for the packet0\n"); */
    memset(tcp_packet, 0, total_length);
    /* kprintf("End1 malloc for the packet0\n"); */
    tcp_packet->src_port = htons(src_port);
    tcp_packet->dst_port = htons(target_port);
    tcp_packet->seq_num = htonl(seq_num);
    tcp_packet->ack_num = htonl(ack_num);
    tcp_packet->data_offset = 5;
    tcp_packet->reserved_ns = 0;
    tcp_packet->flags = flags;
    tcp_packet->window_size = htons(2048); /* Set to some magic value first */
    tcp_packet->checksum = 0;
    tcp_packet->urgent_pointer = 0;

    /* kprintf("Done for the packet0\n"); */

    /* Then do for the endian issue */
    *((uint8_t*)(&tcp_packet->data_reserved_ptr)) = htonb(*((uint8_t*)(&tcp_packet->data_reserved_ptr)), 4);

    /* kprintf("Done for the packet\n"); */

    /* Then copy the payload */
    if(length != 0) {
        memcpy(tcp_packet->data, data, length);
    }

    /* Then fresh checksum */
    tcp_fresh_checksum(tcp_packet, total_length, target_ip);

    ip_send(target_ip, IP_PROTOCOL_TCP, (uint8_t*)tcp_packet, total_length);
    kfree(tcp_packet);
}

void tcp_recv(tcp_t* tcp_packet, uint32_t length) {
    /* Fix some endian issue */
    *((uint8_t*)(&tcp_packet->data_reserved_ptr)) = ntohb(*((uint8_t*)(&tcp_packet->data_reserved_ptr)), 4);
    tcp_packet->src_port = ntohs(tcp_packet->src_port);
    tcp_packet->dst_port = ntohs(tcp_packet->dst_port);
    tcp_packet->seq_num = ntohl(tcp_packet->seq_num);
    tcp_packet->ack_num = ntohl(tcp_packet->ack_num);

    uint32_t data_length = length - tcp_packet->data_offset * 4;
    uint32_t data_ptr = (uint32_t)tcp_packet + tcp_packet->data_offset * 4;

    gui_debug("TCP Packet Comes!!!!");

    kprintf("TCP Packet Comes!!!!, data_length: %d, flags: 0x%x, seq_num: %d, ack_num: %d\n", data_length, tcp_packet->flags, tcp_packet->seq_num, tcp_packet->ack_num);

    /* Check for whether we care for that packet */
    socket_t* socket = get_socket_by_port(tcp_packet->dst_port);
    if (socket == NULL) {
        /* We don't care for that */
        return;
    }

    /* First check for the handshake */
    if(tcp_check_flag(tcp_packet, TCP_FLAG_SYN | TCP_FLAG_ACK) == 1) {
        if(socket->tcp_status == SOCKET_TCP_STATUS_SENT_SYN) {
            socket->others_seq_num = tcp_packet->seq_num;
            socket->tcp_status = SOCKET_TCP_STATUS_RECV_SYNACK;
        }
    }

    if(tcp_check_flag(tcp_packet, TCP_FLAG_FIN) == 1 && data_length == 0) {
        /* Send fin */
        kprintf("WE ARE GOING TO SEND FIN!!\n");
        socket->others_seq_num++;
        tcp_send(socket->port, socket->target_port, socket->target_ip, 0, socket->others_seq_num, TCP_FLAG_ACK, NULL, 0);
        if(socket->has_sent_fin != 1) {
            tcp_send(socket->port, socket->target_port, socket->target_ip, socket->my_seq_num, socket->others_seq_num, TCP_FLAG_FIN, NULL, 0);
        }
        socket->tcp_status = SOCKET_TCP_STATUS_SENT_FIN;
        return;
    }

    /* Then check for the data */
    if(socket->tcp_status == SOCKET_TCP_STATUS_SENT_REQUEST && data_length > 0) {
        if(socket->tcp_common_length == 0)
            socket->tcp_common_length = data_length;
        if(tcp_packet->seq_num + data_length > socket->others_seq_num) {
            if(socket->content_length == 0) {
                static char content_match_str[] = "Content-Length: ";
                char* content_length_ptr =  strstr((char*)data_ptr, (char*)content_match_str);
                if(content_length_ptr != NULL) {
                    content_length_ptr += strlen(content_match_str);
                    char* content_length_ptr_end = strstr(content_length_ptr, "\r\n");
                    char content_length_str[20];
                    uint32_t content_length_str_length = (uint32_t)content_length_ptr_end - (uint32_t)content_length_ptr;
                    memcpy(content_length_str, content_length_ptr, content_length_str_length);
                    content_length_str[content_length_str_length] = '\0';
                    socket->content_length = atoi(content_length_str);

                    char* header_end_ptr =  strstr((char*)data_ptr, "\r\n\r\n");
                    if(header_end_ptr != NULL) {
                        socket->header_length = 4 + (uint32_t)header_end_ptr - (uint32_t)data_ptr;
                        socket->target_length = socket->content_length + socket->header_length;
                    }
                }
            }

            /* That data is useful, we then copy */
            uint32_t begin_buffer_delta = 0;
            if(tcp_packet->seq_num < socket->first_data_packet_seq_num) {
                /* Some overflow occurrs */
                begin_buffer_delta = tcp_packet->seq_num + (TCP_MAC_UINT32 - socket->first_data_packet_seq_num) + 1;
            } else {
                begin_buffer_delta = (tcp_packet->seq_num - socket->first_data_packet_seq_num);
            }
            memcpy((void*)((uint32_t)socket->recv_buffer + begin_buffer_delta), (const void*)data_ptr, data_length);

            socket->recv_buf_end_ptr = begin_buffer_delta + data_length;
            socket->others_seq_num = tcp_packet->seq_num + data_length;
            socket->data_packet_num++;

            if((socket->target_length) != 0 && socket->recv_buf_end_ptr >= socket->target_length) {
                /* Then we send FIN */
                tcp_send(socket->port, socket->target_port, socket->target_ip, socket->my_seq_num, socket->others_seq_num, TCP_FLAG_ACK | TCP_FLAG_FIN, NULL, 0);
                socket->my_seq_num++;
                socket->has_sent_fin = 1;
                return;
            }
        }
        /* Anyway, we will send an ACK for that packet */
        tcp_send(socket->port, socket->target_port, socket->target_ip, 0, socket->others_seq_num, TCP_FLAG_ACK, NULL, 0);
    }

    if(tcp_check_flag(tcp_packet, TCP_FLAG_FIN) == 1) {
        /* Send fin */
        kprintf("WE ARE GOING TO SEND FIN!!\n");
        socket->others_seq_num++;
        tcp_send(socket->port, socket->target_port, socket->target_ip, 0, socket->others_seq_num, TCP_FLAG_ACK, NULL, 0);
        if(socket->has_sent_fin != 1) {
            tcp_send(socket->port, socket->target_port, socket->target_ip, socket->my_seq_num, socket->others_seq_num, TCP_FLAG_FIN, NULL, 0);
        }
        socket->tcp_status = SOCKET_TCP_STATUS_SENT_FIN;
    }

}
