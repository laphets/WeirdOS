/*
 * @Author:  Laphets<wenqing4@illinois.edu>
 * @Date:  2019/12/6
 * Copyright (c) 2019 ECE391 G42. All rights reserved.
 */
#include "dns.h"

/**
 * Init DNS server address
 */
void init_dns() {
    dns_record_head = NULL;
    default_dns_server[0] = 10;
    default_dns_server[1] = 0;
    default_dns_server[2] = 2;
    default_dns_server[3] = 3;
}

/**
 * Parse domain str into dns style str
 * @param domain
 * @return
 */
uint8_t* gen_parsed_domain(uint8_t* domain) {
    uint32_t origin_domain_length = strlen((const int8_t*)domain);
    uint32_t domain_length = origin_domain_length + 1; /* 1 for the front mark */
    uint8_t* parsed_domain = kmalloc(domain_length);
    memcpy(parsed_domain + 1, domain, origin_domain_length);

    int32_t mark_pos = 0, i = 1, cnt = 0;
    for(i = 1; i < domain_length; i++) {
        if(parsed_domain[i] == '.') {
            parsed_domain[mark_pos] = cnt;
            mark_pos = i;
            cnt = 0;
        } else {
            cnt++;
        }
    }
    parsed_domain[mark_pos] = cnt;
    return parsed_domain;
}

dns_record_t* dns_cache_allocate(uint8_t* domain, uint8_t* ip) {
    dns_record_t* record = kmalloc(sizeof(dns_record_t));
    strcpy((int8_t*)record->domain, (int8_t*)domain);
    memcpy(record->ip, ip, sizeof(ip_wrapper_t));
    record->next = NULL;
    return record;
}

dns_record_t* dns_cache_find(uint8_t* domain) {
    dns_record_t* cur_node = dns_record_head;
    while(cur_node != NULL) {
        if(strlen((const int8_t*)domain) != strlen((const int8_t*)cur_node->domain)) {
            continue;
        }
        if(strncmp((const int8_t*)cur_node->domain, (const int8_t*)domain, strlen((const int8_t*)domain)) == 0) {
            return cur_node;
        }
    }

    return NULL;
}

void dns_cache_update(uint8_t* domain, uint8_t* ip) {
    dns_record_t* record = dns_cache_find(domain);
    if(record != NULL) {
        memcpy(record->ip, ip, IPv4_ADDR_SIZE);
        return;
    }
    record = dns_cache_allocate(domain, ip);
    record->next = dns_record_head;
    dns_record_head = record;
}

/**
 * Query DNS for some certain domain
 * @param domain
 * @return
 */
ip_wrapper_t dns_query(uint8_t* domain) {
    if(dns_cache_find(domain) == NULL) {
        uint32_t sid = socket_open();
        uint8_t ip[IPv4_ADDR_SIZE];
        socket_dns_send(sid, domain, ip);
        kprintf("dns_ip: \n");
        print_ip(ip);
        socket_close(sid);
        dns_cache_update(domain, ip);
    }
    ip_wrapper_t result;
    dns_record_t* record = dns_cache_find(domain);
    memcpy(result.ip_addr, record->ip, IPv4_ADDR_SIZE);
    kprintf("result_dns_ip: \n");
    print_ip(result.ip_addr);
    result.valid = 1;
    return result;
}

/**
 * We will use recursive way to achieve the record
 * The Query format should be:
 * QueryName(use \0 to terminate) + uint16(type) + uint16(Class)
 */
void dns_send(uint16_t src_port, uint8_t* domain) {
    uint8_t* parsed_domain = gen_parsed_domain(domain);
    uint32_t domain_length = strlen((const int8_t*)domain) + 1;

    uint32_t query_length = domain_length + 1 + sizeof(uint16_t) + sizeof(uint16_t);
    uint32_t packet_length = sizeof(dns_t) + query_length;
    dns_t* dns_packet = kmalloc(packet_length);
    uint16_t id = (uint16_t)rand(233, 50032);
    dns_packet->id = htons(id);
    dns_packet->control = htons(0x0100); /* Enable recursive query */
    dns_packet->question_count = htons(1);
    dns_packet->answer_count = htons(0);
    dns_packet->authority_count = htons(0);
    dns_packet->additional_count = htons(0);

    *(uint8_t*)((uint32_t)dns_packet->payload) = 0x07;
    memcpy(dns_packet->payload, parsed_domain, domain_length);
    *(uint8_t*)((uint32_t)dns_packet->payload + domain_length) = '\0';
    uint16_t* type_ptr = (uint16_t*)((uint32_t)dns_packet->payload + domain_length + 1);
    uint16_t* class_ptr = (uint16_t*)((uint32_t)dns_packet->payload + domain_length + 3);
    *type_ptr = htons(0x0001);
    *class_ptr = htons(0x0001);


    udp_send(src_port, DNS_PORT, default_dns_server, (uint8_t*)dns_packet, packet_length);
    kfree(parsed_domain);
    kfree(dns_packet);
}

/**
 * Receiver handler for DNS feedback
 * @param dns_packet
 * @param length
 * @param res_ip
 */
void dns_recv(dns_t* dns_packet, uint32_t length, uint8_t* res_ip) {
    /* To simpify the parse, we just try to find two consecutive 0x0001 0x0001 as the A record */
    uint32_t data_ptr = 0, cnt = 0;
    for(data_ptr = (uint32_t)(dns_packet->payload); (data_ptr + 3) < ((uint32_t)dns_packet + length); data_ptr++) {
        /* We find 0001 0001 */
        uint8_t* byte_array = (uint8_t*)data_ptr;
        if(byte_array[0] == 0 && byte_array[1] == 1 && byte_array[2] == 0 && byte_array[3] == 1) {
            cnt++;
            if(cnt == 2 /* count to 2 */) {
                /* The first one is in Query, so the second one is the answer */
                dns_ans_t* ans = (dns_ans_t*)byte_array;
                ans->type = ntohs(ans->type);
                ans->class = ntohs(ans->class);
                ans->ttl = ntohl(ans->ttl);
                ans->data_length = ntohs(ans->data_length);
                /* kprintf("dns_res: res_ip: 0x%x ttl: %d, data_length: %d\n", res_ip, ans->ttl, ans->data_length); */
                /* print_ip(ans->addr); */
                if(res_ip != NULL) {
                    memcpy(res_ip, ans->addr, IPv4_ADDR_SIZE);
                }
                break;
            }
        }
    }
}
