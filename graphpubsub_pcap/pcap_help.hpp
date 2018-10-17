//
// Created by jiachen on 10/16/18.
//

#ifndef GRAPHPUBSUB_PCAP_PCAP_HELP_HPP
#define GRAPHPUBSUB_PCAP_PCAP_HELP_HPP

#include <cstdint>

void print_all_devices();

int print_dev_net(const char *dev);

int capture_one_packet(const char *dev);

void print_hex_ascii_line(const uint8_t *payload, int len, int offset);

void print_payload(const uint8_t *payload, int len);

int capture_dev(const char *dev, int num_packets, const char *filter_exp);

int relay_dev(const char *src_dev, int num_packets, const char *filter_exp, const char *dst_dev);

#endif //GRAPHPUBSUB_PCAP_PCAP_HELP_HPP
