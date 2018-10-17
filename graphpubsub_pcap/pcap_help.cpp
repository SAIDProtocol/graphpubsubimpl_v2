//
// Created by jiachen on 10/16/18.
//
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <pcap/pcap.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pcap_help.hpp"

#define BUFFER_SIZE BUFSIZ

void print_all_devices() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevsp;

    int ret = pcap_findalldevs(&alldevsp, errbuf);
    printf("ret=%d\n", ret);

    for (auto dev = alldevsp; dev != nullptr; dev = dev->next) {
        printf("name = %s\n", dev->name);

        if (dev->description) printf("  description = %s\n", dev->description);
        printf("  flags =");
        if (dev->flags & PCAP_IF_LOOPBACK) printf(" PCAP_IF_LOOPBACK");
        if (dev->flags & PCAP_IF_UP) printf(" PCAP_IF_UP");
        if (dev->flags & PCAP_IF_RUNNING) printf(" PCAP_IF_RUNNING");
        printf("\n\n");
    }

    pcap_freealldevs(alldevsp);
}

int print_dev_net(const char *dev) {
    char errbuf[PCAP_ERRBUF_SIZE];
    int ret;
    char *net, *mask;
    bpf_u_int32 netp; /* ip          */
    bpf_u_int32 maskp;/* subnet mask */
    struct in_addr addr = in_addr();

    printf("DEV: %s\n", dev);

    ret = pcap_lookupnet(dev, &netp, &maskp, errbuf);

    if (ret == -1) {
        printf("Error: %s\n", errbuf);
        return -1;
    }

    /* get the network address in a human readable form */
    addr.s_addr = netp;
    net = inet_ntoa(addr);

    if (!net) {
        perror("inet_ntoa");
        return -1;
    }

    printf("NET: %s\n", net);

    /* do the same as above for the device's mask */
    addr.s_addr = maskp;
    mask = inet_ntoa(addr);

    if (!mask) {
        perror("inet_ntoa");
        return -1;
    }

    printf("MASK: %s\n", mask);
    return 0;
}

int capture_one_packet(const char *dev) {
    char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr hdr = pcap_pkthdr();
    struct ether_header *eptr;
    const uint8_t *packet;
    uint8_t *ptr;
    int i;
    int err = 0;

    printf("DEV: %s\n", dev);

    /* open the device for sniffing.
     *
     * pcap_t *pcap_open_live(char *device,int snaplen, int prmisc,int to_ms, char *ebuf)
     *
     * snaplen - maximum size of packets to capture in bytes
     * promisc - set card in promiscuous mode?
     * to_ms   - time to wait for packets in miliseconds before read times out
     * errbuf  - if something happens, place error string here
     *
     * Note if you change "prmisc" param to anything other than zero, you will get all packets your device sees,
     * whether they are intendeed for you or not!! Be sure you know the rules of the network you are running on before
     * you set your card in promiscuous mode!!
     */
    auto descr = pcap_open_live(dev, BUFFER_SIZE, 0, 2000, errbuf);

    if (!descr) {
        printf("pcap_open_live(): %s\n", errbuf);
        err = -1;
        goto cleanup;
    }

    printf("pcap data link type: %d\n", pcap_datalink(descr));

    /*
     * grab a packet from descr
     *
     * uint8_t *pcap_next(pcap_t *p,struct pcap_pkthdr *h)
     *
     * so just pass in the descriptor we got from our call to pcap_open_live and an allocated struct pcap_pkthdr
     *
     * struct pcap_pkthdr {
     *      struct timeval ts;   time stamp
     *      bpf_u_int32 caplen;  length of portion present
     *      bpf_u_int32;         lebgth this packet (off wire)
     * }
     */
    packet = pcap_next(descr, &hdr);

    if (!packet) {
        printf("Didn't grab packet\n");
        err = -1;
        goto cleanup;
    }

    printf("Grabbed packet of length %d, caplen %d\n", hdr.len, hdr.caplen);
    printf("Recieved at ..... %s\n", ctime((const time_t *) &hdr.ts.tv_sec));
    printf("Ethernet address length is %d\n", ETHER_HDR_LEN);

    /* lets start with the ether header... */
    eptr = (struct ether_header *) packet;

    /* Do a couple of checks to see what packet type we have..*/
    if (ntohs(eptr->ether_type) == ETHERTYPE_IP) {
        printf("Ethernet type hex:%04x dec:%d is an IP packet\n",
               ntohs(eptr->ether_type),
               ntohs(eptr->ether_type));
    } else if (ntohs(eptr->ether_type) == ETHERTYPE_ARP) {
        printf("Ethernet type hex:%04x dec:%d is an ARP packet\n",
               ntohs(eptr->ether_type),
               ntohs(eptr->ether_type));
    } else {
        printf("Ethernet type %04x dev:%d not IP\n",
               ntohs(eptr->ether_type),
               ntohs(eptr->ether_type));
    }

    ptr = eptr->ether_dhost;
    i = ETHER_ADDR_LEN;
    printf(" Destination Address:  ");
    do {
        printf("%s%02x", (i == ETHER_ADDR_LEN) ? " " : ":", *ptr++);
    } while (--i > 0);
    printf("\n");

    ptr = eptr->ether_shost;
    i = ETHER_ADDR_LEN;
    printf(" Source Address:  ");
    do {
        printf("%s%02x", (i == ETHER_ADDR_LEN) ? " " : ":", *ptr++);
    } while (--i > 0);
    printf("\n");

    cleanup:
    if (descr) pcap_close(descr);


    return err;
}

/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void print_hex_ascii_line(const uint8_t *payload, int len, int offset) {

    int i;
    int gap;
    const uint8_t *ch;

    /* offset */
    printf("%05d   ", offset);

    /* hex */
    ch = payload;
    for (i = 0; i < len; i++) {
        printf("%02x ", *ch);
        ch++;
        /* print extra space after 8th byte for visual aid */
        if (i == 7)
            printf(" ");
    }
    /* print space to handle line less than 8 bytes */
    if (len < 8)
        printf(" ");

    /* fill hex gap with spaces if not full line */
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            printf("   ");
        }
    }
    printf("   ");

    /* ascii (if printable) */
    ch = payload;
    for (i = 0; i < len; i++) {
        if (isprint(*ch))
            printf("%c", *ch);
        else
            printf(".");
        ch++;
    }

    printf("\n");
}

/*
 * print packet payload data (avoid printing binary data)
 */
void print_payload(const uint8_t *payload, int len) {

    int len_rem = len;
    int line_width = 16;            /* number of bytes per line */
    int line_len;
    int offset = 0;                    /* zero-based offset counter */
    const uint8_t *ch = payload;

    if (len <= 0)
        return;

    /* data fits on one line */
    if (len <= line_width) {
        print_hex_ascii_line(ch, len, offset);
        return;
    }

    /* data spans multiple lines */
    for (;;) {
        /* compute current line length */
        line_len = line_width % len_rem;
        /* print line */
        print_hex_ascii_line(ch, line_len, offset);
        /* compute total remaining */
        len_rem = len_rem - line_len;
        /* shift pointer to remaining bytes to print */
        ch = ch + line_len;
        /* add offset */
        offset = offset + line_width;
        /* check if we have line width chars or less */
        if (len_rem <= line_width) {
            /* print last line and get out */
            print_hex_ascii_line(ch, len_rem, offset);
            break;
        }
    }
}

void got_packet(uint8_t *, const struct pcap_pkthdr *header, const uint8_t *packet) {
    static int count = 0;   /* packet counter */

    const struct ether_header *ethernet;
    const uint8_t *payload, *ptr;

    int size_payload, i;

    count++;
    printf("\nPacket number %d:\n", count);
    printf("Grabbed packet of length %d, caplen %d\n", header->len, header->caplen);
    printf("Recieved at %s", ctime((const time_t *) &header->ts.tv_sec));

    /* define ethernet header */
    ethernet = reinterpret_cast<const struct ether_header *>(packet);

    printf("Ethernet type 0x%04x\n", ntohs(ethernet->ether_type));

    ptr = ethernet->ether_dhost;
    i = ETHER_ADDR_LEN;
    printf(" Destination Address:  ");
    do { printf("%s%02x", (i == ETHER_ADDR_LEN) ? " " : ":", *ptr++); } while (--i > 0);
    printf("\n");

    ptr = ethernet->ether_shost;
    i = ETHER_ADDR_LEN;
    printf(" Source Address:  ");
    do { printf("%s%02x", (i == ETHER_ADDR_LEN) ? " " : ":", *ptr++); } while (--i > 0);
    printf("\n");


    /* define/compute ip header offset */
    payload = packet + sizeof(struct ether_header);
    size_payload = header->len - sizeof(struct ether_header);

    /*
     * Print payload data; it might be binary, so don't just
     * treat it as a string.
     */
    if (size_payload > 0) {
        printf(" Payload (%d bytes):\n", size_payload);
        print_payload(payload, size_payload);
    }
}

int capture_dev(const char *dev, int num_packets, const char *filter_exp) {
    printf("Device: %s\n", dev);
    printf("Number of packets: %d\n", num_packets);
    printf("Filter expression: %s\n", filter_exp);
    int err = 0;

    char errbuf[PCAP_ERRBUF_SIZE];        /* error buffer */
    struct bpf_program fp;            /* compiled filter program (expression) */
    pcap_t *handle;                /* packet capture handle */
    bpf_u_int32 net;            /* ip */
    memset(&fp, 0, sizeof(struct bpf_program));

    handle = pcap_open_live(dev, BUFFER_SIZE, 1, 0, errbuf);
    if (!handle) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        err = -1;
        goto cleanup;
    }

    if (pcap_datalink(handle) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", dev);
        err = -2;
        goto cleanup;
    }

    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        err = -3;
        goto cleanup;
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        err = -4;
        goto cleanup;
    }

    /* now we can set our callback function */
    pcap_loop(handle, num_packets, got_packet, nullptr);


    cleanup:
    if (handle) pcap_close(handle);
    pcap_freecode(&fp);

    return err;
}

void relay_packet(uint8_t *user, const struct pcap_pkthdr *header, const uint8_t *packet) {
    struct timeval tv1, tv2;
    gettimeofday(&tv1, nullptr);
    pcap_t *handle_dst = reinterpret_cast<pcap_t *>(user);
    pcap_inject(handle_dst, packet, header->caplen);
    gettimeofday(&tv2, nullptr);
    printf("diff1: %'ld us\n", (tv1.tv_sec - header->ts.tv_sec) * 1000000 + (tv1.tv_usec - header->ts.tv_usec));
    printf("diff2: %'ld us\n", (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec));
//    printf("Recieved at %s.%06ld\n", ctime(&header->ts.tv_sec), header->ts.tv_usec);
//    printf("In handler %s.%06ld\n", ctime(&tv1.tv_sec), tv1.tv_usec);
//    printf("After inject %s.%06ld\n", ctime(&tv2.tv_sec), tv2.tv_usec);
}

int relay_dev(const char *src_dev, int num_packets, const char *filter_exp, const char *dst_dev) {
    printf("Device: %s -> %s\n", src_dev, dst_dev);
    printf("Number of packets: %d\n", num_packets);
    printf("Filter expression: %s\n", filter_exp);
    int err = 0;

    char errbuf[PCAP_ERRBUF_SIZE];        /* error buffer */
    struct bpf_program fp;            /* compiled filter program (expression) */
    pcap_t *handle_src, *handle_dst;                /* packet capture handle_src */
    bpf_u_int32 net;            /* ip */
    memset(&fp, 0, sizeof(struct bpf_program));

    handle_src = pcap_open_live(src_dev, BUFFER_SIZE, 1, 0, errbuf);
    if (!handle_src) {
        fprintf(stderr, "Couldn't open device %s: %s\n", src_dev, errbuf);
        err = -1;
        goto cleanup;
    }

    if (pcap_datalink(handle_src) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", src_dev);
        err = -2;
        goto cleanup;
    }

    handle_dst = pcap_open_live(dst_dev, BUFFER_SIZE, 1, 0, errbuf);
    if (!handle_dst) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dst_dev, errbuf);
        err = -3;
        goto cleanup;
    }

    if (pcap_datalink(handle_dst) != DLT_EN10MB) {
        fprintf(stderr, "%s is not an Ethernet\n", dst_dev);
        err = -4;
        goto cleanup;
    }

//    /* compile the filter expression */
//    if (pcap_compile(handle_src, &fp, filter_exp, 0, net) == -1) {
//        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle_src));
//        err = -5;
//        goto cleanup;
//    }
//
//    if (pcap_setfilter(handle_src, &fp) == -1) {
//        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle_src));
//        err = -6;
//        goto cleanup;
//    }
    printf("handle_dst=%p\n", handle_dst);

    /* now we can set our callback function */
    pcap_loop(handle_src, num_packets, relay_packet, reinterpret_cast<u_char *>(handle_dst));


    cleanup:
    if (handle_src) pcap_close(handle_src);
    pcap_freecode(&fp);

    return err;
}